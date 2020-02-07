#ifndef SYNC_SHIFT_FINDER_H
#define SYNC_SHIFT_FINDER_H


#include <QObject>
#include "radio_channel.h"
#include "custom_thread.h"

/*! \defgroup sync Sync
 * \brief Модуль синхронизации каналов
 *
 * На данный момент модуль поддерживает синхронизацию двух каналов
*/

///@{

/*! \brief Класс синхронизации двух каналов
 *
 * Данный класс синхронизует канал channel2
 * с главным каналом channel1
 */
class Sync2D : public QObject
{
    Q_OBJECT
  public:
    Sync2D(const ShPtrRadioChannel& channel1,
           const ShPtrRadioChannel& channel2,
           const ChannelData& data);
    ~Sync2D();
    void start();
    void stop();
  signals:
    void finished();
  private:
    void process();
  private:
    struct Impl;
    std::unique_ptr<Impl>d;
};
/// @}

#endif // SYNC_SHIFT_FINDER_H
