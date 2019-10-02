#ifndef STREAM_FILE_H
#define STREAM_FILE_H
#include <QTcpServer>
#include <QRunnable>

class QThreadPool;
class PeerWireClient ;
class QFile;

class StreamFile :public QObject,public QRunnable
{
    Q_OBJECT
    static const int WAIT_TIME_FOR_READ_REQUEST=5000;
    static const int WAIT_TIME_FOR_READ_ANSWER=50000;
    static const int BYTES_READ=200000;
    static const QString DATE_FORMAT;
public:
    StreamFile(PeerWireClient *streamSoccket);
    StreamFile(qintptr socketDescriptor);
    void run() override;
    inline void setQuit(bool state){quit=state;}
    Q_SIGNAL void finished();
private:
    QPair<QString,qint64> readRequest(PeerWireClient &peer);
    void writeAnswer(PeerWireClient &streamSocket,int answer);
    int readAnswer(PeerWireClient &stream);
    bool writeFile(QFile &file,PeerWireClient &peer);
private:
    qintptr socketDescriptor;
    PeerWireClient * streamSocket;
    std::atomic_bool quit;
};
#endif // STREAM_FILE_H
