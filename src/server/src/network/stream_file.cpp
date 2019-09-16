#include "stream_file.h"

#include "peer_wire_client.h"

#include "command.pb.h"

#include <QCoreApplication>
#include <QDate>
#include <QDataStream>
#include <QFile>

const QString StreamFile::DATE_FORMAT="yyyy-MM-dd";

StreamFile::StreamFile(PeerWireClient *streamSocket):
    streamSocket(streamSocket),quit(true){
}

StreamFile::StreamFile(qintptr socketDescriptor):
    socketDescriptor(socketDescriptor){
}

QPair<QString,qint64> StreamFile::readRequest(PeerWireClient &streamSocket)
{
    QString requestedFilePath;
    qint64 filePosition=-1;
    if(streamSocket.waitForReadyRead(WAIT_TIME_FOR_READ_REQUEST)){
        char len[sizeof(int)];
        streamSocket.readFromSocket(sizeof(int));
        streamSocket.readDataFromBuffer(len,sizeof(int));
        bool ok;
        int num=QByteArray(len,sizeof(int)).toHex().toInt(&ok,16);
        //        char buf[num];
        std::vector<char>buf(static_cast<size_t>(num));
        streamSocket.readFromSocket(num);
        streamSocket.readDataFromBuffer(buf.data(),num);

        FileRequest request;
        request.ParseFromArray(buf.data(),num);
//        FileRequest::Date &fileDate=const_cast<FileRequest::Date&>(request.file_date());
//        QDate date(static_cast<int>(fileDate.year()),
//                   static_cast<int>(fileDate.month()),
//                   static_cast<int>(fileDate.day()));

//        QString deviceSetName=request.device_set_name().data();

        QString fileName=request.file_name().data();
        filePosition=request.file_position();
        fileName+=".bin";
        requestedFilePath=QCoreApplication::applicationDirPath()+"/"+
//                deviceSetName+"/"+date.toString(DATE_FORMAT)+"/"+
                fileName;
        qDebug()<<"READ_REQUEST::FileName"<<fileName<<filePosition;
    }
    return {requestedFilePath,filePosition};
}

bool StreamFile::writeFile(QFile &file, PeerWireClient &streamSocket){
    char buf[BYTES_READ];

    qint64 fileSize=file.size()/*-file.pos()*/;
    qint64 bytesWritten=0;
    qDebug()<<"File Size"<<fileSize;
    QByteArray ba;
    QDataStream out(&ba,QIODevice::WriteOnly);

    out<<fileSize;

    fileSize=file.size()-file.pos();
    if(!streamSocket.isOpen()){
        qDebug("Socket Close");
        return false;
    }
//    qDebug()<<"******0"<<sizeof (fileSize)<<ba.size();
    /*qDebug()<<"******1"<<*/streamSocket.writeDataToBuffer(ba.constData(),ba.size());
    /*qDebug()<<"******2"<<*/streamSocket.writeToSocket(ba.size());
    streamSocket.flush();
    try {//WARNING ЕСЛИ НА КЛИЕНТЕ  ПОСТАВИТ ЗАДЕРЖКУ 1МС ТО BAD ALLOC

    while(!quit&&!file.atEnd()){
        qint64 bytesReadFromFile=file.read(buf,BYTES_READ);
        //QThread::sleep(1);
        if(!streamSocket.isWritable()){
            qDebug()<<"streamSocket: isn't writable"<<file.fileName();
            return false;
        }
        qint64 bytesWriteToSocket=streamSocket.writeDataToBuffer(buf,qMin<qint64>(fileSize-bytesWritten,sizeof(buf)));

        if(bytesReadFromFile!=bytesWriteToSocket){
            qDebug()<<"ERROR READ/WRIE FILE:"<<bytesReadFromFile<<bytesWriteToSocket;
            return false;
        }
        bytesWritten+=bytesWriteToSocket;


        streamSocket.writeToSocket(bytesWriteToSocket);
        streamSocket.flush();
        //WARNING ЕСЛИ ПОСТАВИТЬ ДАННЫЙ МЕТОДВ ТО BAD ALLOC НЕ ПРОИСХОДИТ
        streamSocket.waitForBytesWritten(5000);
//        qDebug()<<bytesWritten<<bytesWriteToSocket;
    }
    } catch (std::exception &err) {
        qDebug()<<err.what();
    }

    if(!readAnswer(streamSocket)){
        qDebug()<<"Read Answer BAD"<<file.fileName();
        return false;
    }

    return true;
}

void StreamFile::writeAnswer(PeerWireClient &streamSocket,int answer){
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<answer;

    streamSocket.writeDataToBuffer(baSize.constData(),baSize.size());
    streamSocket.writeToSocket(baSize.size());
    streamSocket.flush();
}

int StreamFile::readAnswer(PeerWireClient &stream){
    if(stream.waitForReadyRead(WAIT_TIME_FOR_READ_ANSWER)){
        char buf[sizeof(int)];
        stream.readFromSocket(sizeof(int));
        stream.readDataFromBuffer(buf,sizeof(int));

        bool ok;
        int num=QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        return num;
    }
    return 0;
}

void StreamFile::run(){
    //TODO ИСПРАВИТЬ
    PeerWireClient &streamSocket=*(this->streamSocket);

    QPair<QString,qint64> requestedFilePath=readRequest(streamSocket);
    if(!requestedFilePath.first.isEmpty()){
        qDebug("STREAM MESSAGE");

        QFile file(requestedFilePath.first);

        if(!file.exists()){
            qDebug()<<"FILE ISN'T EXISTS:"<<requestedFilePath;
            return;
        }

        writeAnswer(streamSocket,true);

        if(!file.open(QIODevice::ReadOnly)){
            qDebug()<<"FILE ERROR OPEN";
            writeAnswer(streamSocket,false);
            return;
        }

        if(!file.seek(requestedFilePath.second)){
            qDebug()<<"=========SET FILE POS BAD"<<requestedFilePath.second;
            return;
        }

        if(!writeFile(file,streamSocket))
            qDebug()<<"ERROR WHILE FILE WRITING"<<file.fileName();

        file.close();

    }else{
        qDebug("BAD FILE REQUEST");
    }
    emit finished();
    qDebug("SIGNAL FINISHED STREAM WRITER");
}
