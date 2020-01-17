#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "i_device_settings.h"
#include "client_manager.h"
#include <QMainWindow>
#include <QMap>

namespace Ui
{
    class MainWindow;
}

using UInt32Bandwth = quint32;
using Uint32SampleRate = quint32;

class ReceiverSettings;

class FrequencyLineEdit;
class ToolBarLineEdit;

class BandwithComboBox;
class AttenuatorComboBox;
class SampleRateComboBox;
class ToolBarComboBox;

class SwitchButton;
class PreselectorWidget;

class SyncManager;

class ChannelPlot;
class ElipsPlot ;
class PlotMonitoring;

class DeviceListWidget ;

class QStackedWidget;

class MainWindow : public QMainWindow,
    public IDeviceSettings
{
    Q_OBJECT

    static const QString SETTINGS_FILE_NAME;
    static const quint8 USER_DATA_ID = 1;

    static const quint8 USER_DATA_POWER_ON = 2;
    static const quint8 USER_DATA_POWER_OFF = 3;

  public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow()override;

    bool getPower()override;
    DeviceSettings getSettings()override;
    quint32 getDDC1Frequency()override;
    quint32 getDDC1Type()override;
    quint32 getSampleRateForBandwith()override;
    quint32 getSamplesPerBuffer()override;
    quint32 getBandwith()override;
    quint32 getAttenuator()override;
    bool getPreamplifierEnabled()override;

    QPair<quint32, quint32> getPreselectors()override;

    bool getAdcNoiceBlankerEnabled()override;
    quint16 getAdcNoiceBlankerThreshold()override;

    void setCentralWidget(QWidget* widget);
    void setLeftDockWidget(QWidget* widget, const QString& title = QString());
    void setRightDockWidget(QWidget* widget, const QString& title = QString());
  private:
    //**** SET TOOL BAR
    void setTopToolBar(QToolBar* topToolBar);
    void setBottomToolBar(QToolBar* bottomToolBar);

    //MAIN WINDOW SETTINGS
    void loadSettings();
    void saveSetting();
  private slots:
    void widgetChanged();
    void hideReceiverSettingsTool();
    void showReceiverSettingsTool();
  private:
    Ui::MainWindow* ui;

    //Top Widgets
    FrequencyLineEdit* leDDC1Frequency;
    ToolBarLineEdit* leAdcNoiceBlanckerThreshold;

    BandwithComboBox* cbDDC1Bandwith;
    SampleRateComboBox* cbSamplesPerBuffer;
    AttenuatorComboBox* cbAttenuationLevel;

    //Bottom Widgets

    SwitchButton* pbAttenuatorEnable;
    SwitchButton* pbPreamplifierEnable;
    SwitchButton* pbPreselectorEnable;
    SwitchButton* pbAdcNoiceBlanckerEnabled;
    SwitchButton* pbPower;

    PreselectorWidget* preselectorWidget;

    QMap<Qt::ToolBarArea, QToolBar*>toolBarMap;

    PlotMonitoring* plotMonitoring;

//    ChannelPlot*channelPlot;//central widget
//    ElipsPlot *elipsPlot;//right dock idget
    QStackedWidget* stackWidget;
    DeviceListWidget* deviceListWidget;

    std::shared_ptr<ClientManager>_clientManager;
};


#endif // MAIN_WINDOW_H
