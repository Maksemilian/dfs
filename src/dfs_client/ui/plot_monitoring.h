#ifndef PLOT_MONITORING_H
#define PLOT_MONITORING_H

#include <QWidget>
//#include "sync_pair_channel.h"
#include <memory>

class IDeviceSetSettings;
class ChannelPlot;
class ElipsPlot ;
class DeviceSetWidget;
class SyncPairChannel;
class PlotMonitoring :public QWidget
{
    Q_OBJECT
public:
    PlotMonitoring(QWidget *parent=nullptr);
    IDeviceSetSettings *ds=nullptr;
public:
    void onDeviceSetListReady(const QList<DeviceSetWidget*>&dsList);
    void onDeviceSetLIstNotReady();
private:
    ChannelPlot*channelPlot;//central widget
    ElipsPlot *elipsPlot;//right dock idget
    std::unique_ptr<SyncPairChannel> sync;
};

#endif // PLOT_MONITORING_H
