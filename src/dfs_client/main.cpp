#include "ui/main_window.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QScreen>
#include <QDebug>

#include "ipp.h"

void messageLogger(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString txt;
    static long long uid = 0;
    QRegExp rx("([\\w-]+::[\\w-]+)");

    if(rx.indexIn(context.function) == -1)return;
    QString function = rx.cap(1);
    QString msgSep = (msg.length() > 0) ? ">> " : "";

    switch (type)
    {
        case QtInfoMsg:
            break;
        case QtDebugMsg:
            txt = QString("Debug:%1%2%3").arg(function).arg(msgSep).arg(msg);
            break;
        case QtWarningMsg:
            txt = QString("Warning:%1%2%3").arg(function).arg(msgSep).arg(msg);
            break;
        case QtCriticalMsg:
            txt = QString("Critical:%1%2%3").arg(function).arg(msgSep).arg(msg);
            break;
        case QtFatalMsg:
            txt = QString("Fatal:%1%2%3").arg(function).arg(msgSep).arg(msg);

            abort();
    }
    uid++;
    QFile file("C:/Qt/log.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream ts(&file);
        ts << txt << "\r\n";
        file.close();
    }
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    //***ipp lib
    ippInit(); //Initialize Intel IPP library

    //***log file
    QFile file("C:/Qt/log.txt");
    qDebug() << sizeof (size_t) << sizeof (int) << sizeof (unsigned int);
    if(file.exists() && file.size() > 0)
        file.remove();

    //qInstallMessageHandler(messageLogger);

    //***app
    const quint16 INCREASE_MAIN_WINDOW_WIDTH = 600;
    const quint16 INCREASE_MAIN_WINDOW_HEIGH = 100;

    MainWindow mainWindow;
    QScreen* firstScreen = QGuiApplication::screens().first();

    mainWindow.setFixedSize(firstScreen->size().width() - INCREASE_MAIN_WINDOW_WIDTH,
                            firstScreen->size().height() - INCREASE_MAIN_WINDOW_HEIGH);
    mainWindow.show();
    return a.exec();
}
