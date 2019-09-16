#include "packet.pb.h"
#include "ring_packet_buffer.h"

#include "signal_file_writer.h"

#include <QFile>
#include <QtConcurrent/QtConcurrent>
const QString SignalFileWriter::DATE_FORMAT="yyyy-MM-dd";

void SignalFileWriter::createMainDirectory(const QString &name){
    QDir dir(QCoreApplication::applicationDirPath());
    qDebug()<<"Main Dir:"<<dir.path();
    if(dir.isEmpty()||!dir.exists(name)){
        if(dir.mkpath(dir.path()+"/"+name)){
            //qDebug()<<PATH_TO_MAIN_DIR+"/"+signalFilesDirName;
            mainDirName=dir.path()+"/"+name;
            createWorkDirectory();
        }else{
            qDebug()<<"ERROR MAIN DIR:"<<dir.path()+"/"+name;
        }
    }else {
        mainDirName=dir.path()+"/"+name;
        createWorkDirectory();
        //qDebug()<<"Main Dir exist"<<mainDirName;
    }
}

void SignalFileWriter::createWorkDirectory(){
    QString name=QDate::currentDate().toString(DATE_FORMAT);
    if(!mainDirName.isEmpty()){
        QDir dir(mainDirName);
        if(!dir.exists(name)){
            if(dir.mkdir(name)){
                //qDebug()<<PATH_TO_MAIN_DIR+"/"+name;
                workDirName=mainDirName+"/"+name;
            }else{
                qDebug()<<"ERROR WORK DIR:"<<dir.dirName()+"/"+name;
            }
        }else {
            qDebug()<<"Dir already exists:"<<dir.dirName()<<name;
            workDirName=mainDirName+"/"+name;
            //qDebug()<<"Work Dir exist"<<workDirName;
        }
    }
}

void SignalFileWriter::run(RingPacketBuffer *ddc1Buffer,const QString &fileName){
    QIODevice::OpenMode mode;

    QString name=workDirName+"/"+fileName;
    qDebug()<<"File Name"<<name;
    if(QFile::exists(name)){
        mode=QIODevice::Append;
    }else {
        mode=QIODevice::WriteOnly;
    }

    QFile file(name);
    if(file.open(mode)){
        //qDebug()<<"File Name"<<file.fileName();
        Packet packet;
        int bufferIndex=0;
        while(!quit){
            if(ddc1Buffer->get(bufferIndex,packet)){
                int packetByteSize=packet.ByteSize();
                char buf[packetByteSize];
                packet.SerializeToArray(buf,packetByteSize);

                QByteArray ba;
                QDataStream out(&ba,QIODevice::WriteOnly);

                out<<packetByteSize;
                file.write(ba);

                qint64 writeRes=0,bytesWriten=0;

                while (!quit&&bytesWriten != packetByteSize ){
                    if(!file.isWritable()){
                        quit=true;
                        break;
                    }
                    writeRes = file.write(buf + bytesWriten, packetByteSize - bytesWriten );
                    if(writeRes<0){
                        quit=true;
                        break;
                    }
                    bytesWriten += writeRes;
                }
            }
        }
        file.close();
    }
    else
    {
        qDebug()<<"FILE ERROR : File"<<workDirName+"/"+fileName<<"isn't opened"<<file.error();
    }
}

void SignalFileWriter::start(RingPacketBuffer *ddc1Buffer,const QString &fileName,FileExtension fileExtension){
    if(fw.isRunning())return;
    QString fullName;
    switch (fileExtension) {
    case FE_BIN:
        fullName=(fileName+".bin");
        break;
    case FE_TXT:
         fullName=(fileName+".txt");
        break;
    case FE_DOC:
         fullName=(fileName+".docx");
        break;
    }
    quit=false;
    fw.setFuture(QtConcurrent::run(this,&SignalFileWriter::run,ddc1Buffer,fullName));
}

void SignalFileWriter::stop(){
    quit=true;
    fw.waitForFinished();
}
