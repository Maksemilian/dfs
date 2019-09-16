#ifndef TIME_READER_H
#define TIME_READER_H

#include <QObject>
#include <QFutureWatcher>
#include <QReadWriteLock>

class TimeReader :public QObject{
    Q_OBJECT
public:
    TimeReader();
    void setPortName(QString &portName);
    void start();
    void stop();
    void getTimeOfWeek(quint32 &timeOfWeek);
    void getWeekNumber(quint16 &weekNumber);

    void getTime(quint16 &weekNumber,quint32 &timeOfWeek);

    Q_SIGNAL void stopedTrimble();
private:
   void process();
   inline void reset(){
        weekNumber=0;
        timeOfWeek=0;
    }
private:
    QReadWriteLock rwLock;
    QFutureWatcher<void> processFutureWatcher;

    std::atomic<bool> startedAtomWatch;
    quint16 weekNumber;
    quint32 timeOfWeek;
};
#endif // TIME_READER_H
