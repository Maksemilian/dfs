#include "sync_controller.h"

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include <QDebug>

struct SyncController::Impl
{
    Impl():
        syncBuffer1(std::make_shared<RingPacketBuffer>(16)),
        syncBuffer2(std::make_shared<RingPacketBuffer>(16)),
        sumDivBuffer(std::make_shared<RingIpp32fcBuffer>(16))
    {}
    ShPtrPacketBuffer syncBuffer1;
    ShPtrPacketBuffer syncBuffer2;
    ShPtrIpp32fcBuffer sumDivBuffer;

    QFutureWatcher<void> fw;
    Sync2D* shiftFinder = nullptr;
};

SyncController::SyncController():
    d(std::make_unique<Impl>())
{}

ShPtrPacketBuffer SyncController::syncBuffer1()
{
    return d->syncBuffer1;
}

ShPtrPacketBuffer SyncController::syncBuffer2()
{
    return d->syncBuffer2;
}

ShPtrIpp32fcBuffer SyncController::sumDivMethodBuffer()
{
    return d->sumDivBuffer;
}

SyncController::~SyncController() = default;


//************* START / STOP SYNC *************************************

/*!
 * \brief SyncPairChannel::start
 */
void SyncController::start(const ShPtrPacketBuffer& mainBuffer,
                           const ShPtrPacketBuffer& buffer,
                           const SyncData& data)
{
    qDebug() << "******SyncPairChannel::start();";

    d->shiftFinder = new Sync2D(d->syncBuffer1,
                                d->syncBuffer2,
                                d->sumDivBuffer);

    QObject::connect(d->shiftFinder, &Sync2D::finished,
                     d->shiftFinder, &Sync2D::deleteLater);

    d->fw.setFuture(QtConcurrent::run(d->shiftFinder, &Sync2D::start,
                                      mainBuffer, buffer, data));

}

/*!
 * \brief SyncPairChannel::stop
 */
void SyncController::stop()
{
    d->shiftFinder->stop();
    qDebug() << "SyncPairChannel::stop();";
}

