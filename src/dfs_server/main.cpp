#include "wrd_ds_selector.h"
#include "host_server.h"

#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>
#include <QHostAddress>
#include <QCommandLineParser>

#define PATH_TO_LIBRARY "G35DDCAPI"

G3XDDCAPI_CREATE_INSTANCE createInstance;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineOption portOption("p","port","value","value");
    QCommandLineParser parser;

    parser.addOption(portOption);
    parser.process(app.arguments());

    quint16 listenPort=parser.value(portOption).toUShort()>0?
                parser.value(portOption).toUShort():9000;

    QLibrary library(PATH_TO_LIBRARY);
    if(!library.load()){
        qDebug()<<"Failed to load G35DDCAPI.dll!";
        return -1;
    }

    createInstance=reinterpret_cast<G3XDDCAPI_CREATE_INSTANCE>
            (library.resolve("CreateInstance"));

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

    StreamServer *streamServer=new StreamServer();
    streamServer->listen(QHostAddress::Any,listenPort);
    qDebug()<<"Listen"<<listenPort;
    return app.exec();
}

