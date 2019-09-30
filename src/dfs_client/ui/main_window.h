#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "i_device_set_settings.h"
#include "main_window_settings.h"

#include <QMainWindow>
#include <QMap>

namespace Ui {
class MainWindow;
}

using UInt32Bandwth =quint32;
using Uint32SampleRate=quint32;

class IToolBarWidget;

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

class WidgetDirector ;
class DeviceSetListWidget ;
class MainWindow : public QMainWindow,
        public IDeviceSetSettings
{
    Q_OBJECT

    static const QString SETTINGS_FILE_NAME;
    static const quint8 USER_DATA_ID=1;

    static const quint8 USER_DATA_POWER_ON=2;
    static const quint8 USER_DATA_POWER_OFF=3;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow()override;

    bool getPower()override;
    DeviceSetSettings getSettings()override;
    quint32 getDDC1Frequency()override;
    quint32 getDDC1Type()override;
    quint32 getSampleRateForBandwith()override;
    quint32 getSamplesPerBuffer()override;
    quint32 getBandwith()override;
    quint32 getAttenuator()override;
    bool getPreamplifierEnabled()override;

    QPair<quint32,quint32> getPreselectors()override;

    bool getAdcNoiceBlankerEnabled()override;
    quint16 getAdcNoiceBlankerThreshold()override;

    void setWaitCursor()override;
    void setArrowCursor()override;

    Q_SLOT void hideReceiverSettingsTool();
    Q_SLOT void showReceiverSettingsTool();

    void widgetChanged(IToolBarWidget*toolBarWidget);

    void setCentralWidget(QWidget *widget);
    void setLeftDockWidget(QWidget*widget,const QString &title=QString());
    void setRightDockWidget(QWidget*widget,const QString &title=QString());
private:
    //**** SET TOOL BAR
    void setTopToolBar(QToolBar *topToolBar);
    void setBottomToolBar(QToolBar *bottomToolBar);

    //MAIN WINDOW SETTINGS
    void loadSettings();
    void setSettings(const MainWindowSettings::Data &receiverSettings);
    void saveSetting();

private:
    Ui::MainWindow *ui;

    //Top Widgets
    FrequencyLineEdit *leDDC1Frequency;
    ToolBarLineEdit *leAdcNoiceBlanckerThreshold;

    BandwithComboBox *cbDDC1Bandwith;
    SampleRateComboBox *cbSamplesPerBuffer;
    AttenuatorComboBox *cbAttenuationLevel;

    //Bottom Widgets

    SwitchButton *pbAttenuatorEnable;
    SwitchButton *pbPreamplifierEnable;
    SwitchButton* pbPreselectorEnable;
    SwitchButton *pbAdcNoiceBlanckerEnabled;
    SwitchButton *pbPower;

    PreselectorWidget *preselectorWidget;

    QMap<Qt::ToolBarArea,QToolBar*>toolBarMap;

    SyncManager*syncManager;

    ChannelPlot*channelPlot;//central widget
    ElipsPlot *elipsPlot;//right dock idget

    WidgetDirector *widgetDirector;
    DeviceSetListWidget *deviceSetListWidget;
};


#endif // MAIN_WINDOW_H
