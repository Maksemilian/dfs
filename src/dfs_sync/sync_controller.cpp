#include "sync_controller.h"

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QDebug>

struct SyncController::Impl
{
    Impl():
        syncBuffer1(std::make_shared<RingPacketBuffer>(16)),
        syncBuffer2(std::make_shared<RingPacketBuffer>(16))
        //sumDivBuffer(std::make_shared<RingIpp32fcBuffer>(16))

    { }
    ShPtrPacketBuffer syncBuffer1;
    ShPtrPacketBuffer syncBuffer2;
//    ShPtrIpp32fcBuffer sumDivBuffer;

    QFutureWatcher<void> fw;
    std::shared_ptr<Sync2D> _sync = nullptr;
};


SyncController::SyncController():
    d(std::make_unique<Impl>())
{}

SyncController::~SyncController() = default;


//************* START / STOP SYNC *************************************
void SyncController::start(const ShPtrRadioChannel& channel1,
                           const ShPtrRadioChannel& channel2,
                           const ChannelData& data)
{
    qDebug() << "******SyncPairChannel::start();";

    d->_sync = std::make_shared<Sync2D>(channel1, channel2, data);

    QObject::connect(d->_sync.get(), &Sync2D::finished,
                     d->_sync.get(), &Sync2D::deleteLater);

    d->fw.setFuture(QtConcurrent::run(d->_sync.get(), &Sync2D::start));
}


/*!
 * \brief SyncPairChannel::stop
 */
void SyncController::stop()
{
    d->_sync->stop();
    qDebug() << "SyncPairChannel::stop();";
}

/*
void SyncController::start(const ShPtrPacketBuffer& mainBuffer,
                           const ShPtrPacketBuffer& buffer,
                           const ChannelData& data)
{
    qDebug() << "******SyncPairChannel::start();";

    d->_sync = std::make_shared<Sync2D>(mainBuffer, buffer, data);

    QObject::connect(d->_sync.get(), &Sync2D::finished,
                     d->_sync.get(), &Sync2D::deleteLater);

    d->fw.setFuture(QtConcurrent::run(d->_sync.get(), &Sync2D::start));
}
*/
