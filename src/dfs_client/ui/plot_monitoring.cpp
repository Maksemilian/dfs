#include "plot_monitoring.h"
#include "sync_pair_channel.h"
#include "client_ds_ui.h"
#include "i_device_settings.h"
#include "plot_elipse.h"
#include "plot_channel.h"
#include <QDebug>
#include <QtConcurrent/QtConcurrent>

PlotMonitoring::PlotMonitoring(QWidget* paret)
    : QWidget (paret),
      channelPlot(new ChannelPlot(2, 8192)),
      elipsPlot(new ElipsPlot(this)),
      sync(std::make_unique<SyncController>()),
      quit(true)
{
    QGridLayout* gl = new QGridLayout();
    setLayout(gl);
    gl->addWidget(channelPlot, 0, 0, 1, 2);
    gl->addWidget(elipsPlot, 1, 0);

//    sync->setSumDivUpdater(elipsPlot);
//    sync->setSyncSignalUpdater(channelPlot);
}

void PlotMonitoring::onDeviceSetListReady(const QList<DeviceSetWidget*>& dsList)
{
    quit = false;
    qDebug() << "START SYNC" << ds->getDDC1Frequency()
             << ds->getSampleRateForBandwith()
             << ds->getSamplesPerBuffer();

    ShPtrPacketBufferPair bufferPair;
    bufferPair.first = dsList.first()->ddc1Buffer();
    bufferPair.second = dsList.last()->ddc1Buffer();
    sync->start(bufferPair,
                ds->getDDC1Frequency(),
                ds->getSampleRateForBandwith(),
                ds->getSamplesPerBuffer());

    QtConcurrent::run([this]()
    {
        qDebug() << "START UPDATE";
        quint32 blockSize = 8192;
        quint32 COUNT_SIGNAL_COMPONENT = 2;
        std::unique_ptr<float[]>dataPairSingal(new float[blockSize * COUNT_SIGNAL_COMPONENT]);
        std::unique_ptr<float[]>sumSubData(new float[blockSize * COUNT_SIGNAL_COMPONENT]);
        proto::receiver::Packet packet[CHANNEL_SIZE];
        std::vector<Ipp32fc>v;
        bool isFirstStationReadedPacket = false;
        bool isSecondStationReadedPacket = false;
        bool isElips = false;
        int INDEX = 0;
        while (!quit)
        {
            if(sync->syncBuffer1()->pop(packet[CHANNEL_FIRST]))
                isFirstStationReadedPacket = true;

            if(sync->syncBuffer2()->pop(packet[CHANNEL_SECOND]))
                isSecondStationReadedPacket = true;

            if(sync->sumDivMethodBuffer()->pop(v))
                isElips = true;

            if(isFirstStationReadedPacket && isSecondStationReadedPacket && isElips)
            {

                ChannelDataT channelData1(packet[CHANNEL_FIRST].block_number(),
                                          packet[CHANNEL_FIRST].ddc_sample_counter(),
                                          packet[CHANNEL_FIRST].adc_period_counter());

                ChannelDataT channelData2(packet[CHANNEL_SECOND].block_number(),
                                          packet[CHANNEL_SECOND].ddc_sample_counter(),
                                          packet[CHANNEL_SECOND].adc_period_counter());
                // qDebug()<<"BA_3";
                const float* data1 = packet[CHANNEL_FIRST].sample().data();
                const float* data2 = packet[CHANNEL_SECOND].sample().data();
                //В dataPairSingal заносятся I компоненты с канала 1 и 2
                for(quint32 i = 0; i < blockSize; i++)
                {
                    dataPairSingal[i * 2] = data1[i * 2];
                    dataPairSingal[i * 2 + 1] = data2[i * 2];
                }

                channelPlot->updateSignalData(INDEX, channelData1, channelData2);
                channelPlot->updateSignalComponent(INDEX, dataPairSingal.get(), blockSize);

                memcpy(sumSubData.get(), reinterpret_cast<float*>(v.data()),
                       sizeof (float)*blockSize * COUNT_SIGNAL_COMPONENT);

                // qDebug()<<"BA_5";
                elipsPlot->update(INDEX, sumSubData.get(), blockSize);

                isFirstStationReadedPacket = false;
                isSecondStationReadedPacket = false;
                isElips = false;
            }
        }
    });
}

void PlotMonitoring::onDeviceSetListNotReady()
{
    quit = true;
    sync->stop();
}
