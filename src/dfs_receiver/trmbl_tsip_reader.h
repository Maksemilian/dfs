#ifndef TIME_READER_H
#define TIME_READER_H

#include <QObject>
#include <QFutureWatcher>
#include <QReadWriteLock>

/*! \addtogroup receiver
 */

///@{
/*!
 * \brief читает временные данные полученные от спутника
 *
 * \attention данный класс реализует паттерн Singlton
 */
class TimeReader : public QObject
{
    Q_OBJECT
    TimeReader();
  public:
    static TimeReader& instance();
    inline bool isStarted()
    {
        return startedAtomWatch;
    }
    /*!
     * \brief запускает получение временных данных
     * в отдельном потоке
     */
    void start();
    /*!
     * \brief останавливает поток получения временных данных
     */
    void stop();
    //TODO сделать возвращаемые значения
    // и убрать соответсвующие данные из полей класса
    /*!
     * \brief получает секунду в неделе
     */
    void getTimeOfWeek(quint32& timeOfWeek);
    /*!
     * \brief получает номер недели
     */
    void getWeekNumber(quint16& weekNumber);

    void getTime(quint16& weekNumber, quint32& timeOfWeek);

    Q_SIGNAL void stopedTrimble();
  private:
    void process();
    inline void reset()
    {
        weekNumber = 0;
        timeOfWeek = 0;
    }
  private:
    QReadWriteLock rwLock;
    QFutureWatcher<void> processFutureWatcher;

    std::atomic<bool> startedAtomWatch;
    quint16 weekNumber;
    quint32 timeOfWeek;
};
///@}
#endif // TIME_READER_H
