#ifndef SYNC_SHIFT_FINDER_H
#define SYNC_SHIFT_FINDER_H

#include "sync_global.h"

#include <QQueue>
#include <QObject>

/*!
 * \brief The FindChannelForShift class
 */


class Sync2D : public QObject
{
    Q_OBJECT

  public:
    Sync2D(const ShPtrPacketBuffer& inBuffer1,
           const ShPtrPacketBuffer& inBuffer2,
           const SyncData& data);

    ~Sync2D();
    void start();
    void stop();
  signals:
    void finished();
  private:
    bool calcShift();
  private:
    struct Impl;
    std::unique_ptr<Impl>d;
};
#endif // SYNC_SHIFT_FINDER_H
