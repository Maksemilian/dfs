#include "sync_2d.h"

#include "sync_channel_equalizer.h"
#include "sync_calc_delta_pps.h"

#include <QDebug>
#include <QtConcurrent/QtConcurrent>
//******************************* FindChannelForShift ******************************************

struct Sync2D::Impl
{
    Impl(const ShPtrRadioChannel& channel1,
         const ShPtrRadioChannel& channel2,
         const ChannelData& data):
        _channel1(channel1),
        _channel2(channel2),
        _data(data)    {    }

    ShPtrRadioChannel _channel1;
    ShPtrRadioChannel _channel2;
    ChannelData _data;

    QFutureWatcher<void> fw;
    std::atomic_bool quit;
};

/*! \addtogroup sync Sync
 */
///@{
Sync2D::Sync2D(const ShPtrRadioChannel& channel1,
               const ShPtrRadioChannel& channel2,
               const ChannelData& data):
    d(std::make_unique<Impl>(channel1, channel2, data)) {}

Sync2D::~Sync2D()
{
    qDebug() << "SYNC_PROCESS_DESTR";
};

void Sync2D::start()
{
    d->quit = false;
    d->fw.setFuture(QtConcurrent::run(this, &Sync2D::process));
}

void Sync2D::stop()
{
    d->quit = true;
    d->fw.waitForFinished();
}
/*!
 * \brief Метод синхронизации двух каналов
 *
 * Из каналов последовательно считываются блоки ...
 * \code
 * \endcode
 */
void Sync2D::process()
{
    qDebug() << "THREAD_SYNC_BEGIN";
    d->quit = false;
    //FIND DELTA PPS
//    Q_ASSERT_X(d->_channel1 != d->_channel2,
//               "Sync2D::start", "channels must not equal");
    CalcDeltaPPS c(d->_channel1, d->_channel2, d->_data);
    double deltaPPS = c.findDeltaPPS();

    std::unique_ptr<ChannelEqualizer>blockEqualizer =
        std::make_unique<ChannelEqualizer>(d->_channel2, deltaPPS);

    bool isRead1 = false;
    bool isRead2 = false;
    //FIND SHIFT BUFFER
    while (true && !d->quit)
    {
        if(d->_channel1->readIn())
        {
            isRead1 = true;
        }

        if(d->_channel2->readIn())
        {
            isRead2 = true;
        }

        if(isRead1 && isRead2)
        {
            blockEqualizer->initShiftBuffer();
            qDebug() << "ShiftBuffer init";
            break;
        }
    }
    isRead1 = false;
    isRead2 = false;

    //READ CHANNEL1 AND SHIFT CHANNEL2
    while(!d->quit)
    {
        if(d->_channel1->readIn())
        {
            isRead1 = true;
        }

        if(d->_channel2->readIn())
        {
            isRead2 = true;
        }

        if(isRead1 && isRead2)
        {

            d->_channel1->skip();
            blockEqualizer->shiftChannel();

            isRead1 = false;
            isRead2 = false;
        }
    }

    qDebug() << "THREAD_SYNC_END";
}
///@}
