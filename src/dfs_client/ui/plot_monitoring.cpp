#include "plot_monitoring.h"
#include "sync_pair_channel.h"
#include "client_ds_ui.h"
#include "i_device_set_settings.h"
#include "plot_elipse.h"
#include "plot_channel.h"
#include <QDebug>

PlotMonitoring::PlotMonitoring(QWidget *paret)
    : QWidget (paret),
      channelPlot(new ChannelPlot(2,8192)),
      elipsPlot(new ElipsPlot(this)),
      sync(std::make_unique<SyncPairChannel>())
{
    QGridLayout *gl=new QGridLayout();
    setLayout(gl);
    gl->addWidget(channelPlot,0,0);
    gl->addWidget(elipsPlot,0,1);
    //    setRightDockWidget(elipsPlot,"Elipse");

    sync->addSumDivUpdater(elipsPlot);
    sync->addSyncSignalUpdater(channelPlot);
    //************** CHANNEL PLOT**************************

    //    channelPlot=new ChannelPlot(2,getSamplesPerBuffer());
    //    setCentralWidget(channelPlot);
}

void PlotMonitoring::onDeviceSetListReady(const QList<DeviceSetWidget *> &dsList)
{
    qDebug()<<"START SYNC"<<ds->getDDC1Frequency()
           <<ds->getSampleRateForBandwith()
          <<ds->getSamplesPerBuffer();

    ShPtrBufferPair bufferPair;
    bufferPair.first=dsList.first()->ddc1Buffer();
    bufferPair.second=dsList.last()->ddc1Buffer();
    sync->start(bufferPair,
               ds->getDDC1Frequency(),
               ds->getSampleRateForBandwith(),
               ds->getSamplesPerBuffer());
}

void PlotMonitoring::onDeviceSetLIstNotReady()
{
    sync->stop();
}
