#ifndef PLOT_MONITORING_H
#define PLOT_MONITORING_H

#include <QWidget>
//#include <memory>
#include "sync_controller.h"
class IDeviceSettings;
class ChannelPlot;
class ElipsPlot ;
class DeviceWidget;
class SyncController;

using ShPtrPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class PlotMonitoring : public QWidget
{
    Q_OBJECT
    enum Channel
    {
        CHANNEL_FIRST = 0,
        CHANNEL_SECOND = 1,
        CHANNEL_SIZE = 2
    };
  public:
    PlotMonitoring(QWidget* parent = nullptr);
    IDeviceSettings* ds = nullptr;
  public:
//    void onDeviceSetListReady(const QList<DeviceSetWidget*>& dsList);
    void onDeviceSetListReady(const std::vector<ShPtrPacketBuffer>& buffers);
    void onDeviceSetListNotReady();
  private:
    ChannelPlot* channelPlot; //central widget
    ElipsPlot* elipsPlot;//right dock idget
    std::unique_ptr<SyncController> sync;
    std::atomic_bool quit;
};

#endif // PLOT_MONITORING_H
