#include "file_loader.h"

#include "device_set_client.h"

#include "interface/i_device_set_settings.h"

#include "peer_wire_client.h"

#include "packet.pb.h"
#include "device_set_info.pb.h"
#include "command.pb.h"

#include "ring_packet_buffer.h"

#include <string>

#include <QByteArray>
#include <QDataStream>
#include <QTimer>

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QHostAddress>


struct FileLoader::Impl
{

    Impl(){}
    Impl(const QHostAddress &address,quint16 port,
         const QStringList &fileNames):
        address(address),
        port(port),
        fileNames(fileNames),
        quit(true)
    { }
    QHostAddress address;
    quint16 port;
    QStringList fileNames;
    ShPtrPeerWireClient stream;

    QList<std::shared_ptr<QFile > >files;
    QList<qint64>fileSizes;

    QFutureWatcher<void>fw;
    std::atomic<bool>quit;
};

FileLoader::FileLoader(const QHostAddress &address,quint16 port,
                       const QStringList &fileNames):
    d(std::make_unique<Impl>(address,port,fileNames))
{}

FileLoader::~FileLoader()=default;

bool FileLoader::createDirectory(const QString &name)
{
    QDir dir(QCoreApplication::applicationDirPath()+"/"+name);
    if(!dir.exists()){
        if(!dir.mkpath(QCoreApplication::applicationDirPath()+"/"+name)){
            qDebug()<<"ERROR MKPATH:"<<QCoreApplication::applicationDirPath()+"/"+name;
            return false;
        }
    }else {
        qDebug()<<"DIR EXIST"<<dir.dirName();
    }
    return true;
}

bool FileLoader::createDirectoriesForFiles()
{
    for(const QString&fileName:d->fileNames)
    {
        //work dir is ds_name/date
        QString workDir=fileName.left(fileName.lastIndexOf('/')+1);
        qDebug()<<"CreateDIR::workDir"<<workDir<<fileName<<fileName.lastIndexOf('/');
        if(!createDirectory(workDir)){
            qDebug()<<"ERROR CREATE DIR"<<workDir;
            return false;
        }
        QString saveFilePath=QCoreApplication::applicationDirPath()+"/"+fileName;
        d->files<<std::make_shared<QFile>(saveFilePath);
        qDebug()<<"createDirectoriesForFiles::saveFilePath"<<saveFilePath;
        if(!d->files.last()->open(QIODevice::WriteOnly)){
            qDebug()<<"createDirectoriesForFiles::ERROR OPEN FILE-WriteOnly";
            return false;
        }
    }
    return true;
}

void FileLoader::sendRequestToStream(const QString &fileName)
{
    FileRequest request;

    request.set_file_name(fileName.toStdString());
    //TODO ADD FILE POSITION
    request.set_file_position(0);

    qDebug()<<"Request"
           <<QString(fileName.data())
             //          <<requestData.filePosition
             ;

    int len=request.ByteSize();
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<len;

    d->stream->writeDataToBuffer(baSize.constData(),baSize.size());
    d->stream->writeToSocket(baSize.size());

    std::vector<char>buf(static_cast<size_t>(request.ByteSize()));
    request.SerializeToArray(buf.data(),request.ByteSize());
    d->stream->writeDataToBuffer(buf.data(),request.ByteSize());
    d->stream->writeToSocket(request.ByteSize());
    d->stream->flush();
}

void FileLoader::writeAnswer(int answer)
{
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<answer;

    qDebug()<<"Begin writeAnswer";
    d->stream->writeDataToBuffer(baSize.constData(),baSize.size());
    d->stream->writeToSocket(baSize.size());
    d->stream->flush();
    qDebug()<<"End writeAnswer";
}

int FileLoader::readAnswerFromStream()
{
    if(d->stream->waitForReadyRead(WAIT_TIME_MSEC)){
        char buf[sizeof(int)];
        d->stream->readFromSocket(sizeof(int));
        d->stream->readDataFromBuffer(buf,sizeof(int));

        bool ok;
        int num=QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        return num;
    }
    return 0;
}

bool FileLoader::connectToServer()
{
    d->stream=std::make_shared<PeerWireClient>();
    d->stream->connectToHost(d->address,d->port);

    if(!d->stream->waitForConnected(WAIT_TIME_MSEC)){
        qWarning()<<"ERROR connected for"<<d->address<<d->port;
        return false;
    }

    return true;
}

bool FileLoader::sendTypeConnetion()
{
    //        sendRequestToStream(d->streams[i],d->requests[i]);
    ConnectRequest request;
    request.set_type(ConnectRequest::CT_FILE);

    int len=request.ByteSize();
    QByteArray baSize;
    QDataStream out(&baSize,QIODevice::WriteOnly);
    out<<len;

    d->stream->writeDataToBuffer(baSize.constData(),baSize.size());
    d->stream->writeToSocket(baSize.size());

    char buf[request.ByteSize()];
    request.SerializeToArray(buf,request.ByteSize());
    d->stream->writeDataToBuffer(buf,request.ByteSize());
    d->stream->writeToSocket(request.ByteSize());
    if(d->stream->waitForReadyRead(5000)){
        char buf[sizeof(int)];
        d->stream->readFromSocket(sizeof(int));
        d->stream->readDataFromBuffer(buf,sizeof(int));
        bool ok;
        int num=QByteArray(buf,sizeof(int)).toHex().toInt(&ok,16);
        if(!num){
            return false;
        }
    }else{
        qDebug("Error FILE Reader");
        return false;
    }
    return true;
}

bool FileLoader::sendRequestedFileName()
{
    for(const QString&fileName:d->fileNames){
        sendRequestToStream(fileName);
        if(!readAnswerFromStream())
            return false;
    }
    return true;
}

void FileLoader::process()
{
    qDebug()<<"BEGIN PROC";

    if(!connectToServer()){
        qDebug()<<"ERROR CONNECT TO STREAM";
        emit finished();
        return;
    }

    if(!sendTypeConnetion()){
        qDebug()<<"ERROR SEND TYPE CONNECTION";
        emit finished();
        return;
    }

    if(!createDirectoriesForFiles()){
        qDebug()<<"ERROR CREATION DIR FOR FILES";
        emit finished();
        return;
    }

    qDebug()<<"======CONNECT TO STREAM";

    if(!sendRequestedFileName()){
        qDebug()<<"ERROR SEND REQUEST TO STREAM";
        emit finished();
        return;
    }

    qDebug()<<"======SEND TO STREAM";

    if(!readFilesFromStreams())
        qDebug("ERROR PROCESS TASK");

    //    qDebug()<<"1";
    for(auto sPtrFile:d->files)
        sPtrFile->close();
    //qDebug()<<"*************2";

    d->stream->disconnectFromHost();
    //qDebug()<<"************3";
    if( d->stream->state()!=QAbstractSocket::UnconnectedState)
        d->stream->waitForDisconnected(WAIT_TIME_MSEC);
    //    qDebug()<<"end";
    emit finished();
    qDebug()<<"END PROC FILE LOADER";
}

qint64 FileLoader::readFullSizeFromStreams()
{
    qint64 fileSize=0;
    qint64 fullSize=0;
    //TODO СДЕЛАТЬ ЦИКЛ ПОЛУЧЕНИЯ СПИСКА РАЗМЕРОВ ЗАПРАШИВАЕМЫХ ФАЙЛОВ
    //    for(const ShPtrPeerWireClient&stream:d->streams){
    if(d->stream->waitForReadyRead(WAIT_TIME_MSEC)){
        char buf[sizeof(qint64)];
        qDebug()<<"******1"<<d->stream->readFromSocket(sizeof(qint64));
        qDebug()<<"******2"<<d->stream->readDataFromBuffer(buf,sizeof(qint64));

        bool ok;
        fileSize=QByteArray(buf,sizeof(qint64)).toHex().toLongLong(&ok,16);
        fullSize+=fileSize;
        qDebug()<<"FILE SIZE"<<fileSize<<buf;
        d->fileSizes<<fileSize;
    }else{
        qDebug("Error FILE SIZE");
        return -1;
    }
    //    }
    qDebug()<<"Full Task SIZE"<<fullSize<<"bytes";
    return fullSize;
}


qint64 FileLoader::currentTaskSize()
{
    qint64 currentSize=0;
    //TODO CURRENT TASK SIZE
    //    for(TaskRequest&req:d->requests)
    //        currentSize+=req.file->size();

    return currentSize;
}

bool FileLoader::readFilesFromStreams()
{
    qint64 fullSize=readFullSizeFromStreams();
    if(fullSize<0){
        qDebug()<<"ERROR READ FILE SIZES FROM STREAM";
        return false;
    }
    qint64 bytesReaded=currentTaskSize();
    if(bytesReaded==fullSize){
        emit bytesProgress(bytesReaded,fullSize);
        qDebug()<<"EQU bytes and full size:"<<bytesReaded<<fullSize;
        return true;
    }

    qDebug()<<"Start"<<d->quit;
    //    qint64 fs=fullSize;
    int fileIndex=0;
    emit fullSizeFile(fileIndex,fullSize);
    while(!d->quit/*&&fullSize!=0!d->streams.isEmpty()*/){
            if(!d->quit&&d->fileSizes[fileIndex]!=0&&
                    (d->stream->waitForReadyRead(WAIT_TIME_MSEC)||
                     d->stream->canTransferMore())){
                qint64 available = qMin<qint64>(d->stream->socketBytesAvailable(), d->fileSizes[fileIndex]);
                //                qDebug()<<"Aval"<<available;
                if(available>0){
                    qint64 bytes=d->stream->readFromSocket(qMin<qint64>(available, BYTES_TO_READ));
                    if(bytes>0){
                        d->fileSizes[fileIndex]-=bytes;
                        //                        fullSize-=bytes;
                        std::vector<char>buf(static_cast<size_t>(bytes));
                        qint64 bytesReadedFromSocket=d->stream->readDataFromBuffer(buf.data(),bytes);
                        //                        qint64 bytesWrittenToFile=d->files[i]->write(buf.data(),bytes);
                        qint64 bytesWrittenToFile=d->files[fileIndex]->write(buf.data(),bytes);
                        bytesReaded+=bytes;
                        emit bytesProgress(bytesReaded,fullSize);
                        emit bytesProgressFile(fileIndex,bytesReaded,fullSize);

                        if(bytesWrittenToFile!=bytesReadedFromSocket)
                            //                            qDebug()<<"ERROR:"<<d->files[i]->error()<<bytesReadedFromSocket;
                            qDebug()<<"ERROR:"<<d->files[fileIndex]->error()<<bytesReadedFromSocket;

                        if(d->fileSizes[fileIndex]==0){
                            writeAnswer(1);
                            qDebug()<<"FILE read"<<d->fileSizes[fileIndex];
//                            emit finished();
                            return true;
                        }
                    }
                }
            }else if(d->fileSizes[fileIndex]>0){
                writeAnswer(0);
                qDebug()<<"FILE not read"<<d->fileSizes[fileIndex];
//                emit finished();
                return false;
            }

        //WARNING ЕСЛИ ПОСТАВИТЬ ЗАДЕРЖКУ 1МС ТО BAD ALLOC НА СЕРВЕРЕ
        //        QThread::msleep(1);
    }
    qDebug()<<"Stop"<<d->quit;
    return true;
}

void FileLoader::start()
{
    d->quit=false;
    d->fw.setFuture(QtConcurrent::run(this,&FileLoader::process));
}

void FileLoader::stop()
{
    d->quit=true;
    d->fw.waitForFinished();
}

//***********************************

//if(fileSize!=0){

//    int ret= QMessageBox::warning(nullptr,request.fileName,"reload file:"+saveFilePath
//                                  ,QMessageBox::Yes,QMessageBox::No);
//    switch (ret) {
//    case QMessageBox::Yes:
//        const_cast<qint64&>(fileName.filePosition)=0;
//        if(!fileName.file->open(QIODevice::WriteOnly)){
//            qDebug()<<"createDirectoriesForFiles::ERROR OPEN FILE-WriteOnly";
//            return false;
//        }
//        break;
//    case QMessageBox::No:
//        const_cast<qint64&>(fileName.filePosition)=fileSize;
//        if(!fileName.file->open(QIODevice::Append)){
//            qDebug()<<"createDirectoriesForFiles::ERROR OPEN FILE-Append";
//            return false;
//        }
//        break;
//    };
//    qDebug()<<"CreateDIR::FILE POS:"<<fileName.filePosition<<fileSize;
//}else {
//    const_cast<qint64&>(fileName.filePosition)=0;
//    if(!fileName.file->open(QIODevice::WriteOnly)){
//        qDebug()<<"CreateDIR::ERROR OPEN FILE"<<fileName.filePosition<<fileSize;;
//        return false;
//    }
//}
