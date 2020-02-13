#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "i_device_settings.h"
#include "client_manager.h"
#include "sync_2d.h"
#include <QMainWindow>
#include <QMap>

/*!  \defgroup client Client
 *   \brief Модуль Графического приложения
 *    позволяющий принимать и отображать в виде графиков
 *    данные потоков ddc1 с удаленных приемных устройств,
 *    а также синхронизовать ппринимаемые данные в реальном
 *    времени
 */

///@{
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
class DeviceMonitoring;

/*!
 * \brief Класс главного окна приложения
 */

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
  private:
    bool getPower()override;
    DeviceSettings getSettings()override;
    quint32 getDDC1Frequency()override;
    quint32 getDDC1Type()override;
    quint32 getSampleRateForBandwith()override;
    quint32 getSamplesPerBuffer()override;
    quint32 getBandwith()override;
    quint32 getAttenuator()override;
    bool getPreamplifierEnabled()override;

    Preselectors getPreselectors()override;

    bool getAdcNoiceBlankerEnabled()override;
    quint16 getAdcNoiceBlankerThreshold()override;

    void setCentralWidget(QWidget* widget);
    void setLeftDockWidget(QWidget* widget, const QString& title = QString());
    void setRightDockWidget(QWidget* widget, const QString& title = QString());

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
    void onDeviceSetListReady(
        const std::vector<ShPtrRadioChannel>& channels);
    void onDeviceSetListNotReady();
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

//    PlotMonitoring* plotMonitoring;

    DeviceMonitoring* treeDevices;
    std::shared_ptr<ClientManager>_clientManager;

    ChannelPlot* channelPlot; //central widget
    ElipsPlot* elipsPlot;//right dock idget
    std::unique_ptr<Sync2D> sync;
    //TODO проверку остановки потока сделать
    //по состоянию кнопки power
    std::atomic_bool quit;
};
///@}

#endif // MAIN_WINDOW_H
