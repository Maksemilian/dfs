#include "src/network/receiver_station_client.h"

#include "src/receiver/device_set_selector.h"

#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>
#include <QHostAddress>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTime>
#include <QTimer>

#define PATH_TO_LIBRARY "G35DDCAPI"

G3XDDCAPI_CREATE_INSTANCE createInstance;

void reconnect(ReceiverStationClient *client,
               const QHostAddress &address,
               quint16 port,int reconnectTime=10000);

void tryReconnectByTimer(const QTime &time,
                         ReceiverStationClient *client,
                         const QHostAddress &address, quint16 port,int reconnectTime);

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineOption addressOption("a", "address","value","value");

    QCommandLineOption portOption("m","main-port","value","value");
    QCommandLineOption countStationOption("c","count-station","value","value");

    QCommandLineOption streamPortOption("s","stream-port","value","value");
    QCommandLineOption filePortOption("f","file-port","value","value");

    QCommandLineOption reconnectTimeOption("t","reconnect-time","value","value");

    QCommandLineParser parser;

    parser.addOption(addressOption);
    parser.addOption(portOption);
    parser.addOption(countStationOption);
    parser.addOption(filePortOption);
    parser.addOption(streamPortOption);
    parser.addOption(reconnectTimeOption);

    parser.process(app.arguments());

    QHostAddress serverAddress(parser.value(addressOption));
    quint16 serverPort=parser.value(portOption).toUShort();
    quint16 countStation=parser.value(countStationOption).toUShort();
    quint16 filePort=parser.value(filePortOption).toUShort();
    quint16 streamPort=parser.value(streamPortOption).toUShort();
    //    int reconnectTimeSec=parser.value(reconnectTimeOption).toInt();
    int reconnectTimeInMs=(parser.value(reconnectTimeOption).toInt()*60)*1000;
    QLibrary library(PATH_TO_LIBRARY);
    if(!library.load()){
        qDebug()<<"Failed to load G35DDCAPI.dll!";
        return -1;
    }

    qDebug()<<"ReconnectTime"
           <<parser.value(reconnectTimeOption).toInt()<<" sec"
          <<reconnectTimeInMs<<"ms";
    createInstance=reinterpret_cast<G3XDDCAPI_CREATE_INSTANCE>(library.resolve("CreateInstance"));

    if(!createInstance){
        qDebug()<<"Failed to load G35DDCAPI.dll!";
        library.unload();
        return -1;
    }

    if(DeviceSetSelector::numberAvailableDeviceSet()==0
            //||countStation!=DeviceSetSelector::numberAvailableDeviceSet()
            ){
        qDebug()<<"Failed geting Device Set!";
        library.unload();
        return -1;
    }

    quint32 numberDeviceSet=0;
    for(quint16 i=0;i<countStation;i++){
        ReceiverStationClient *station=new ReceiverStationClient(streamPort+i,filePort+i);
        if(station->setupNewDeviceSet(numberDeviceSet)){
            QObject::connect(station,&ReceiverStationClient::stationDisconnected,
                             [station,serverAddress,serverPort,reconnectTimeInMs]{
                reconnect(station,serverAddress,serverPort,reconnectTimeInMs);
            });

            if(!station->connectToHost(serverAddress,serverPort)){
                qDebug()<<"Client don't connect"<<serverAddress<<serverPort;
                reconnect(station,serverAddress,serverPort,reconnectTimeInMs);
            }
        }else {
            qDebug()<<"Device Set "<<numberDeviceSet<<" not avaiable";
        }
    }

    return app.exec();
}

void reconnect(ReceiverStationClient *client,const QHostAddress &address, quint16 port,int reconnectTime){
    std::shared_ptr<QTime> sptTimer(new QTime);
    (*sptTimer).start();
    tryReconnectByTimer(*sptTimer,client,address, port,reconnectTime);
}

void tryReconnectByTimer(const QTime &time,ReceiverStationClient *client,
                         const QHostAddress &address, quint16 port,
                         int reconnectTime){
    qDebug("Reconnection");
    if(time.elapsed()<=reconnectTime){
        if(!client->connectToHost(address,port)){
            QTimer::singleShot(200,[&time,client,address, port ,reconnectTime](){
                qDebug("Attempt reconect");
                tryReconnectByTimer(time,client,address, port,reconnectTime);
            });
        }
    }
}
