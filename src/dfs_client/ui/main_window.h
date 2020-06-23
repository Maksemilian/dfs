#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "i_device_settings.h"
#include "client_manager.h"
#include "stream_server.h"
#include "sync_2d.h"
#include <QMainWindow>
#include <QMap>

/*!  \defgroup client Client
 * \image html class_client.jpg "Диаграмма классов модуля client"
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
class SamplePerBufferComboBox;

class SwitchButton;
class PreselectorWidget;

class SyncManager;

class Plot;

class DeviceListWidget ;

class QStackedWidget;

/*!
 * \brief Класс главного окна приложения,
 * представляющего графический интерфейс для
 * подключения и управления удаленными приемниками.
 */

class MainWindow : public QMainWindow,
    public IDeviceSettings
{
    Q_OBJECT

    static const QString SETTINGS_FILE_NAME;
    static const quint8 USER_DATA_ID = 1;

    static const quint8 USER_DATA_POWER_ON = 2;
    static const quint8 USER_DATA_POWER_OFF = 3;
    static const quint16 LISTENING_STREAMING_PORT = 9000;
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
        /* const std::vector<ShPtrRadioChannel>& channels*/);
    void onDeviceSetListNotReady();
  private:
    Ui::MainWindow* ui;

    //Top Widgets
    FrequencyLineEdit* leDDC1Frequency;
    ToolBarLineEdit* leAdcNoiceBlanckerThreshold;

    BandwithComboBox* cbDDC1Bandwith;
    SamplePerBufferComboBox* cbSamplesPerBuffer;
    AttenuatorComboBox* cbAttenuationLevel;

    //Bottom Widgets

    SwitchButton* pbAttenuatorEnable;
    SwitchButton* pbPreamplifierEnable;
    SwitchButton* pbPreselectorEnable;
    SwitchButton* pbAdcNoiceBlanckerEnabled;
    SwitchButton* pbPower;

    PreselectorWidget* preselectorWidget;

    QMap<Qt::ToolBarArea, QToolBar*>toolBarMap;

    DeviceListWidget* deviceListWidget;
    std::shared_ptr<ClientManager>_clientManager;
    Plot* plot;
    std::unique_ptr<Sync2D> sync;
    StreamServer streamServer;
};
///@}

#endif // MAIN_WINDOW_H
