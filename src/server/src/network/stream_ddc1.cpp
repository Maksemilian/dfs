#include "stream_ddc1.h"
#include "ring_packet_buffer.h"
#include "peer_wire_client.h"
#include "command.pb.h"

#include <QDataStream>

StreamDDC1::StreamDDC1(PeerWireClient*streamSocket, std::shared_ptr<RingPacketBuffer>buffer)
    :
      streamSocket(streamSocket),
      buffer(buffer)
{

}

void StreamDDC1::process()
{
    //********************
    Packet packet;
    while(!quit){
        if(buffer->pop(packet)){
            int packetByteSize=packet.ByteSize();
            QByteArray ba;
            QDataStream out(&ba,QIODevice::WriteOnly);

            out<<packetByteSize;
            if(!streamSocket->isOpen()){
                qDebug("Socket Close");
                break;
            }

            streamSocket->writeDataToBuffer(ba.constData(),ba.size());
            streamSocket->writeToSocket(ba.size());
            streamSocket->flush();
            char buf[packetByteSize];
            packet.SerializeToArray(buf,packetByteSize);

            streamSocket->writeDataToBuffer(buf,packetByteSize);

            qint64 bytesWriten=0;
            while (!quit&&bytesWriten != packetByteSize ){
                if(!streamSocket->isWritable()){
                    quit=true;
                    break;
                }
                qint64 bytes=streamSocket->writeToSocket(packetByteSize);
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
               <<"ADC_C"<<packet.adc_period_counter();
        }
    }
    emit finished();
    qDebug("SIGNAL FINISHED STREAM DDC WRITER");
}

void StreamDDC1::start(){
    quit=false;
    qDebug("START STREAM WRITER");
}

void StreamDDC1::stop(){
    quit=true;
    qDebug("STOP STREAM WRITER");
}
