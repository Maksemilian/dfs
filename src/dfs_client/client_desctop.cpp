#include "client_desctop.h"
#include "desctop.pb.h"

DesctopClient::DesctopClient(): Client (ConnectData())
{

}

void DesctopClient::sendCommand()
{

}

void DesctopClient::onMessageReceived(const QByteArray& buffer)
{
    proto::desctop::DesctopInfo desctopInfo;
    if(!desctopInfo.ParseFromArray(buffer.constData(), buffer.size()))
    {
        qDebug() << "ERROR PARSE DESCTOP INFO";
        return;
    }
    emit desctopInfoReceived(desctopInfo);
}

