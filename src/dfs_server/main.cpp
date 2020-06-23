#include "host_server.h"
#include "wrd_coh_g35_ds.h"
#include "trmbl_tsip_reader.h"

#include "G35DDCAPI.h"

#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>
#include <QHostAddress>
#include <QCommandLineParser>

#define PATH_TO_LIBRARY "G35DDCAPI"

G3XDDCAPI_CREATE_INSTANCE createInstance;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    TimeReader& inst = TimeReader::instance();

    QObject::connect(&app, &QCoreApplication::aboutToQuit,
                     &inst, &TimeReader::stop);

    QCommandLineOption portOption("p", "port", "value", "value");
    QCommandLineParser parser;

    parser.addOption(portOption);
    parser.process(app.arguments());

    quint16 listenPort = parser.value(portOption).toUShort() > 0 ?
                         parser.value(portOption).toUShort() : 9000;

    QLibrary library(PATH_TO_LIBRARY);
    if(!library.load())
    {
        qDebug() << "Failed to load G35DDCAPI.dll!";
        return -1;
    }

    createInstance = reinterpret_cast<G3XDDCAPI_CREATE_INSTANCE>
                     (library.resolve("CreateInstance"));

    if(!createInstance)
    {
        qDebug() << "Failed to load G35DDCAPI.dll!";
        library.unload();
        return -1;
    }

    if(numberAvailableDeviceSet() == 0)
    {
        qDebug() << " Number Available Devise Set is zero";
        return  -1;
    }

    TimeReader::instance().start();
    HostServer* srv = new HostServer();
    srv->listen(QHostAddress::Any, listenPort);

    qDebug() << "Server Ready:" << listenPort;

    return app.exec();
}

