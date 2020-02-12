#include "plot_monitoring.h"
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
      quit(true)
{
    QGridLayout* gl = new QGridLayout();
    setLayout(gl);
    //    gl->addWidget(channelPlot, 0, 0, 1, 2);
    //    gl->addWidget(elipsPlot, 1, 0);
    gl->addWidget(elipsPlot, 0, 0, 1, 1);
    gl->addWidget(channelPlot, 0, 1, 1, 2);

    //    sync->setSumDivUpdater(elipsPlot);
    //    sync->setSyncSignalUpdater(channelPlot);
}
void PlotMonitoring::onDeviceSetListReady(
    const std::vector<ShPtrRadioChannel>& channels)
//void PlotMonitoring::onDeviceSetListReady(const QList<DeviceSetWidget*>& dsList)
{

//    Q_ASSERT_X(channels.size() == 2, "PlotMonitoring::onDeviceSetListReady",
//               "sync available only for 2 channel");
    quit = false;
    qDebug() << "START SYNC" << ds->getDDC1Frequency()
             << ds->getSampleRateForBandwith()
             << ds->getSamplesPerBuffer();
    ChannelData data = {ds->getDDC1Frequency(),
                        ds->getSampleRateForBandwith(),
                        ds->getSamplesPerBuffer()
                       };
    channels.front()->setChannelData(data);
    channels.back()->setChannelData(data);

    sync.reset(new Sync2D(channels.front(), channels.back(), data));
    elipsPlot->setSyncData(data);
    qDebug() << "****PLOT MONITORIN"
             << channels.back().use_count()
             << channels.back().use_count()
             << channels.back()->outBuffer().use_count()
             << channels.back()->outBuffer().use_count();
    QtConcurrent::run([this, channels]()
    {
        bool isRead1 = false;
        bool isRead2 = false;

        std::shared_ptr<RadioChannel>channel1 = channels.front();
        std::shared_ptr<RadioChannel>channel2 = channels.back();

        proto::receiver::Packet pct1;
        proto::receiver::Packet pct2;
        qDebug() << "PLOT MONITORING RUN";
        while (!quit)
        {
            if(channel1->outBuffer()->pop(pct1))
            {
                isRead1 = true;
            }

            if(channel2->outBuffer()->pop(pct2))
            {
                isRead2 = true;
            }

            if(isRead1 && isRead2)
            {
                channelPlot->apply(pct1, pct2);
                elipsPlot->apply(pct1, pct2);

                isRead1 = false;
                isRead2 = false;
            }
        }
        qDebug() << "PLOT MONITORING STOP";
    });
}

void PlotMonitoring::onDeviceSetListNotReady()
{
    quit = true;
    sync->stop();
}

/*
void PlotMonitoring::onDeviceSetListReady(const std::vector<ShPtrPacketBuffer>& buffers)
//void PlotMonitoring::onDeviceSetListReady(const QList<DeviceSetWidget*>& dsList)
{
    quit = false;
    qDebug() << "START SYNC" << ds->getDDC1Frequency()
             << ds->getSampleRateForBandwith()
             << ds->getSamplesPerBuffer();
    SyncData data = {ds->getDDC1Frequency(),
                     ds->getSampleRateForBandwith(),
                     ds->getSamplesPerBuffer()
                    };

    sync->start(buffers.front(), buffers.back(), data);

    QtConcurrent::run([this, data]()
    {
        qDebug() << "START UPDATE";
//        quint32 blockSize = 8192;
        quint32 COUNT_SIGNAL_COMPONENT = 2;
        std::unique_ptr<float[]>dataPairSingal(new float[data.blockSize *
                COUNT_SIGNAL_COMPONENT]);
        std::unique_ptr<float[]>sumSubData(new float[data.blockSize *
                COUNT_SIGNAL_COMPONENT]);
//        std::vector<Ipp32fc>v;
        bool isRead1 = false;
        bool isRead2 = false;
        bool isElips = false;
        int INDEX = 0;

        std::shared_ptr<Sync2D>sync2d = sync->getSync2D();
        std::shared_ptr<RadioChannel>channel1 = sync2d->channel1();
        std::shared_ptr<RadioChannel>channel2 = sync2d->channel1();
        SumSubMethod sumSubMethod(data.sampleRate, data.blockSize);

        proto::receiver::Packet pct1;
        proto::receiver::Packet pct2;

        while (!quit)
        {
            if(channel1->outBuffer()->pop(pct1))
            {
                isRead1 = true;
            }

            if(channel2->outBuffer()->pop(pct2))
            {
                isRead2 = true;
            }

            if(isRead1 && isRead2 && isElips)
            {
               VectorIpp32fc v= sumSubMethod.applyT(pct1, pct2, data.blockSize);

                ChannelDataT channelData1(pct1.block_number(),
                                          pct1.ddc_sample_counter(),
                                          pct1.adc_period_counter());

                ChannelDataT channelData2(pct2.block_number(),
                                          pct2.ddc_sample_counter(),
                                          pct2.adc_period_counter());
                // qDebug()<<"BA_3";
                const float* data1 = pct1.sample().data();
                const float* data2 = pct2.sample().data();
                //В dataPairSingal заносятся I компоненты с канала 1 и 2
                for(quint32 i = 0; i < data.blockSize; i++)
                {
                    dataPairSingal[i * 2] = data1[i * 2];
                    dataPairSingal[i * 2 + 1] = data2[i * 2];
                }

                channelPlot->updateSignalData(INDEX,
                                              channelData1, channelData2);
                channelPlot->updateSignalComponent(INDEX,
                                                   dataPairSingal.get(),
                                                   data.blockSize);

//                memcpy(sumSubData.get(), reinterpret_cast<float*>(v.data()),
//                       sizeof (float)*data.blockSize *
//                       COUNT_SIGNAL_COMPONENT);

                // qDebug()<<"BA_5";
                elipsPlot->update(INDEX, v);

                isRead1 = false;
                isRead2 = false;
                isElips = false;
            }
        }
    });
}*/
