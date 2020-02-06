#ifndef SYNC_SHIFT_FINDER_H
#define SYNC_SHIFT_FINDER_H

#include "radio_channel.h"
#include <QObject>
#include "custom_thread.h"
/*!
 * \brief The FindChannelForShift class
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
    struct Impl;
    std::unique_ptr<Impl>d;
};
#endif // SYNC_SHIFT_FINDER_H
