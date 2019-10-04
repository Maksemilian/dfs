#include "host_stream_ddc1.h"
#include "peer_wire_client.h"
#include "receiver.pb.h"
#include "channel.h"

#include <QDataStream>
#include <QtConcurrent/QtConcurrent>

StreamDDC1::StreamDDC1(net::ChannelHost*streamSocket,
                       std::shared_ptr<RingBuffer>buffer)
    :
      _streamSocket(streamSocket),
      buffer(buffer)

{}

void StreamDDC1::process()
{
    qDebug("BEGIN PROCESS STREAM");
    quit=false;
    //********************
    proto::receiver::Packet packet;
    proto::receiver::HostToClient hostToClient;
    while(!quit){
        if(buffer->pop(packet))
        {
            hostToClient.mutable_packet()->CopyFrom(packet);
            int packetByteSize= hostToClient.ByteSize();

            QByteArray ba;
            QDataStream out(&ba,QIODevice::WriteOnly);

            out<<packetByteSize;
            if(!_streamSocket->isOpen()){
                qDebug("Socket Close");
                break;
            }

            _streamSocket->writeDataToBuffer(ba.constData(),ba.size());
            _streamSocket->writeToSocket(ba.size());
            _streamSocket->flush();
            char buf[packetByteSize];
            hostToClient.SerializeToArray(buf,packetByteSize);

            _streamSocket->writeDataToBuffer(buf,packetByteSize);

            qint64 bytesWriten=0;
            while (!quit&&bytesWriten != packetByteSize ){
                if(!_streamSocket->isWritable()){
                    quit=true;
                    break;
                }
                qint64 bytes=_streamSocket->writeToSocket(packetByteSize);
                if(bytes==-1){
                    quit=true;
                    break;
                }
                bytesWriten+=bytes;
            }
            qDebug()<<"WRITE_PACKET:"
                   <<packet.block_number()
                  <<packet.sample_rate()
                 <<"TOW:"<<packet.time_of_week()
                <<"DDC_C"<<packet.ddc_sample_counter()
               <<"ADC_C"<<packet.adc_period_counter()
              <<hostToClient.ByteSize();
        }
    }
    emit finished();
    qDebug("SIGNAL FINISHED STREAM DDC WRITER");
}

void StreamDDC1::start(){
    quit=false;
    qDebug("START STREAM WRITER");
    //        QtConcurrent::run(this,&StreamDDC1::process);
}

void StreamDDC1::stop(){
    quit=true;
    qDebug("STOP STREAM WRITER");
}
