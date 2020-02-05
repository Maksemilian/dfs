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

  public:
    PlotMonitoring(QWidget* parent = nullptr);
    IDeviceSettings* ds = nullptr;
  public:
    void onDeviceSetListReady(
        const std::vector<ShPtrRadioChannel>& channels);
    void onDeviceSetListNotReady();
  private:
    ChannelPlot* channelPlot; //central widget
    ElipsPlot* elipsPlot;//right dock idget
    std::unique_ptr<SyncController> sync;
    std::atomic_bool quit;
};

#endif // PLOT_MONITORING_H
