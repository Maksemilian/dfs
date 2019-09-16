#ifndef FILE_LOADER_H
#define FILE_LOADER_H


#include <memory>

#include <QObject>
#include <QDate>
#include <QFile>
#include <QRunnable>

class PeerWireClient;
class QFile;
class QHostAddress;

using ShPtrPeerWireClient=std::shared_ptr<PeerWireClient>;

struct TaskRequest
{
    QString stationAddress;
    quint16 stationPort;

    QString fileName;
    std::shared_ptr<QFile >file;
    qint64 filePosition;
};

class FileLoader:public QObject
{
     Q_OBJECT
     friend class TaskLoader;
     static const int WAIT_TIME_MSEC=5000;
     static const qint64 BYTES_TO_READ=200000;

public:
    FileLoader(const QHostAddress &address,quint16 port,
               const QStringList &fileNames);
    ~FileLoader();
    Q_SIGNAL void bytesProgress(qint64 bytesReaded,qint64 bitesSize);

    Q_SIGNAL void fullSizeFile(int fileIndex,qint64 fullSize);
    Q_SIGNAL void bytesProgressFile(int fileIndex, qint64 bytesReaded,qint64 bitesSize);

    Q_SIGNAL void finished();
    Q_SIGNAL void fileReaded();
    void start();
    void stop();
    void process();
private:

    bool createDirectory(const QString &dirName);

    bool createDirectoriesForFiles();

    bool connectToServer();
    bool sendRequestedFileName();
    bool sendTypeConnetion();

    qint64 currentTaskSize();
    void sendRequestToStream(const QString &fileName);
    int  readAnswerFromStream();

    qint64 readFullSizeFromStreams();
    void writeAnswer(int answer);
    bool readFilesFromStreams();

private:
    struct Impl;
    std::unique_ptr<Impl> d;
};
#endif // FILE_LOADER_H
