#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <QFutureWatcher>

class RingPacketBuffer;

class SignalFileWriter
{
     enum FileExtension{
       FE_BIN,
       FE_TXT,
       FE_DOC
     };
     static const QString DATE_FORMAT;
public:
    inline SignalFileWriter(){}

    void createMainDirectory(const QString &name);

    void start(RingPacketBuffer *ddc1Buffer,const QString &fileName,FileExtension fileExtension=FE_BIN);
    void stop();

private:
    void createWorkDirectory();
    void run(RingPacketBuffer *ddc1Buffer,const QString &fileName);
private:
    QString mainDirName;
    QString workDirName;

    QFutureWatcher<void> fw;
    std::atomic<bool> quit;
};
#endif // FILE_WRITER_H
