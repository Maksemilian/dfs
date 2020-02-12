#include "ui_main_window.h"
#include "main_window.h"

#include "command_factory.h"
#include "command_hierarch.h"

#include "tool_frequency_line_edit.h"
#include "tool_preselector_widget.h"
#include "tool_switch_button.h"
#include "tool_widgets.h"

#include "plot_monitoring.h"

#include "client_ds_ui.h"
#include "client_ds_ui_list.h"

#include "tree_devices.h"

#include <QDockWidget>
#include <QCheckBox>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStackedWidget>

//**************CONSTANTS*******************
const QString MainWindow::SETTINGS_FILE_NAME = "device_set.ini";

MainWindow:: MainWindow(QWidget* parent):
    QMainWindow(parent), ui(new Ui::MainWindow)
    , _clientManager(std::make_shared<ClientManager>())
{
    ui->setupUi(this);
    centralWidget()->setLayout(new QVBoxLayout);
    setObjectName("MainWindow");

    treeDevices = new DeviceMonitoring(_clientManager, this);

    setLeftDockWidget(treeDevices, "DeviceSetList");

    //************SETTING TOOLBAR***************
    //****TOP

    ui->mainToolBar->layout()->setSpacing(10);
    ui->mainToolBar->setMovable(false);
    setTopToolBar(ui->mainToolBar);
    toolBarMap.insert(Qt::TopToolBarArea, ui->mainToolBar);

    //****BOTTOM

    QToolBar* bottomToolBar = new QToolBar;
    bottomToolBar->setMovable(false);
    bottomToolBar->layout()->setSpacing(10);
    setBottomToolBar(bottomToolBar);
    addToolBar(Qt::BottomToolBarArea, bottomToolBar);

    toolBarMap.insert(Qt::BottomToolBarArea, bottomToolBar);

    loadSettings();

    //************** ELIPSE PLOT***************************
    plotMonitoring = new PlotMonitoring(this);
    plotMonitoring->ds = this;
    connect(_clientManager.get(), &ClientManager::ready,
            plotMonitoring, &PlotMonitoring::onDeviceSetListReady);

    connect(_clientManager.get(), &ClientManager::notReady,
            plotMonitoring, &PlotMonitoring::onDeviceSetListNotReady);

    setCentralWidget(plotMonitoring);

    showReceiverSettingsTool();

    //WARNING UNCOMMENT IN RELEASE VERSION
    //    hideReceiverSettingsTool();
}

MainWindow::~MainWindow()
{
    saveSetting();
    delete ui;
}

void MainWindow::setCentralWidget(QWidget* widget)
{
    centralWidget()->layout()->addWidget(widget);
}

void MainWindow::setLeftDockWidget(QWidget* widget, const QString& title)
{
    QDockWidget* leftDockWidget = new QDockWidget(title, this);
    leftDockWidget->setWidget(widget);
    addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);

}

void MainWindow::setRightDockWidget(QWidget* widget, const QString& title)
{
    QDockWidget* rightDockWidget = new QDockWidget(title, this);
    rightDockWidget->setWidget(widget);
    addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
}

//****************** SETTING TOP TOOL BAR *************************

void MainWindow::setTopToolBar(QToolBar* topToolBar)
{
    if(!topToolBar)
    {
        return;
    }

    //DDC1 Frequency
    MacroCommand* macroFreq = FactoryCommand::getMacroCommand();
    //    macroFreq->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
    macroFreq->addCommand(FactoryCommand::getStopDdc1Command(_clientManager.get(), this));
    macroFreq->addCommand(FactoryCommand::getFrequencyCommand(_clientManager.get(), this));
    macroFreq->addCommand(FactoryCommand::getStartDdc1Command(_clientManager.get(), this));

    leDDC1Frequency = new FrequencyLineEdit(this);
    leDDC1Frequency->setUserData(USER_DATA_ID, macroFreq);
    connect(leDDC1Frequency, &FrequencyLineEdit::changed,
            this, &MainWindow::widgetChanged);

    //DDC1 Bandwith
    MacroCommand* macroBandwith = FactoryCommand::getMacroCommand();
    //    macroBandwith->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
    macroBandwith->addCommand(FactoryCommand::getStopDdc1Command(_clientManager.get(), this));
    macroBandwith->addCommand(FactoryCommand::getSetDdc1Command(_clientManager.get(), this));
    macroBandwith->addCommand(FactoryCommand::getStartDdc1Command(_clientManager.get(), this));
    //    macroBandwith->addCommand(FactoryCommand::getSyncStartCommand(syncManager,this));

    cbDDC1Bandwith = new BandwithComboBox(this);
    cbDDC1Bandwith->setUserData(USER_DATA_ID, macroBandwith);
    connect(cbDDC1Bandwith, &BandwithComboBox::changed, this, &MainWindow::widgetChanged);

    //Samples per Buffer

    cbSamplesPerBuffer = new SampleRateComboBox(this);
    cbSamplesPerBuffer->setEnabled(false);
    connect(cbSamplesPerBuffer, &SampleRateComboBox::changed, this, &MainWindow::widgetChanged);

    topToolBar->addWidget(leDDC1Frequency);

    topToolBar->addSeparator();
    topToolBar->addWidget(cbDDC1Bandwith);
    topToolBar->addSeparator();
    topToolBar->addWidget(cbSamplesPerBuffer);

    topToolBar->addSeparator();

}

//****************** SETTING BOTTOM TOOL BAR*************************

void MainWindow::setBottomToolBar(QToolBar* bottomToolBar)
{
    if(!bottomToolBar)
    {
        return ;
    }


    //power BUTTON
    pbPower = new SwitchButton("On", "Off", false, this);
    connect(pbPower, &SwitchButton::changed, this, &MainWindow::widgetChanged);
    MacroCommand* macroCommand = FactoryCommand::getMacroCommand();

    macroCommand->addCommand(FactoryCommand::getPowerComandOn(_clientManager.get(), this));
    macroCommand->addCommand(FactoryCommand::getSettingsCommand(_clientManager.get(), this));
    macroCommand->addCommand(FactoryCommand::getStartDdc1Command(_clientManager.get(), this));
    macroCommand->addCommand(FactoryCommand::getStartSendingStreamCommand(_clientManager.get()));
    //    macroCommand->addCommand(FactoryCommand::getSyncStartCommand(syncManager,this));
    //    macroCommand->addCommand(FactoryCommand::getAddTaskCommand(widgetDirector,this));

    pbPower->setUserData(USER_DATA_POWER_ON, macroCommand);

    MacroCommand* mc = FactoryCommand::getMacroCommand();
    //    mc->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
    mc->addCommand(FactoryCommand::getStopDdc1Command(_clientManager.get(), this));
    mc->addCommand(FactoryCommand::getStopSendingStreamCommand(_clientManager.get()));
    mc->addCommand(FactoryCommand::getPowerComandOff(_clientManager.get(), this));

    pbPower->setUserData(USER_DATA_POWER_OFF, mc);

    //attenuator BUTTON
    pbAttenuatorEnable = new SwitchButton("Atten Enable", "Atten Disable", false, this);
    pbAttenuatorEnable->setUserData(USER_DATA_ID,
                                    FactoryCommand::getAttenuatorCommand(_clientManager.get(), this));
    connect(pbAttenuatorEnable, &SwitchButton::changed, this, &MainWindow::widgetChanged);
    //attenuator COMBO BOX
    cbAttenuationLevel = new AttenuatorComboBox(this);
    cbAttenuationLevel->setUserData(USER_DATA_ID,
                                    FactoryCommand::getAttenuatorCommand(_clientManager.get(), this));
    connect(cbAttenuationLevel, &AttenuatorComboBox::changed,
            this, &MainWindow::widgetChanged);
    //preamplifier BUTTON
    pbPreamplifierEnable = new SwitchButton("Pream Enable", "Pream Disable", false, this);
    pbPreamplifierEnable->setUserData(USER_DATA_ID, FactoryCommand::getPreamplifireCommand(_clientManager.get(), this));
    connect(pbPreamplifierEnable, &SwitchButton::changed,
            this, &MainWindow::widgetChanged);

    //preseector BUTTON
    pbPreselectorEnable = new SwitchButton("Pres Enable", "Pres Disable", false, this);
    pbPreselectorEnable->setUserData(USER_DATA_ID, FactoryCommand::getPreselectorCommand(_clientManager.get(), this));
    connect(pbPreselectorEnable, &SwitchButton::changed, this, &MainWindow::widgetChanged);

    //preseector LOW FREQUENCY COMBO BOX
    preselectorWidget = new PreselectorWidget(this);
    preselectorWidget->setUserData(USER_DATA_ID, FactoryCommand::getPreselectorCommand(_clientManager.get(), this));
    connect(preselectorWidget, &PreselectorWidget::changed, this, &MainWindow::widgetChanged);

    //ADC NOICE BLANKER BUTTON
    pbAdcNoiceBlanckerEnabled = new SwitchButton("ADC Enable", "ADC Disable", false, this);
    pbAdcNoiceBlanckerEnabled->setUserData(USER_DATA_ID, FactoryCommand::getAdcEnabledCommand(_clientManager.get(), this));
    connect(pbAdcNoiceBlanckerEnabled, &SwitchButton::changed, this, &MainWindow::widgetChanged);

    //ADC NOICE BLANKER LENE EDIT
    leAdcNoiceBlanckerThreshold = new ToolBarLineEdit(this);
    leAdcNoiceBlanckerThreshold->setFixedWidth(100);
    leAdcNoiceBlanckerThreshold->setUserData(USER_DATA_ID, FactoryCommand::getAdcThresholdCommand(_clientManager.get(), this));
    connect(leAdcNoiceBlanckerThreshold, &ToolBarLineEdit::changed, this, &MainWindow::widgetChanged);

    // End Widgets

    bottomToolBar->addWidget(pbAttenuatorEnable);
    bottomToolBar->addWidget(cbAttenuationLevel);
    bottomToolBar->addSeparator();

    bottomToolBar->addWidget(pbPreamplifierEnable);
    bottomToolBar->addSeparator();

    bottomToolBar->addWidget(pbPreselectorEnable);
    bottomToolBar->addWidget(preselectorWidget);
    bottomToolBar->addSeparator();

    bottomToolBar->addWidget(pbAdcNoiceBlanckerEnabled);
    bottomToolBar->addWidget(leAdcNoiceBlanckerThreshold);

    bottomToolBar->addWidget(pbPower);
    bottomToolBar->addSeparator();
}

void MainWindow::hideReceiverSettingsTool()
{
    for(QToolBar* toolBar : toolBarMap.values())
    {
        toolBar->setDisabled(true);
    }
}

void MainWindow::showReceiverSettingsTool()
{
    for(QToolBar* toolBar : toolBarMap.values())
    {
        toolBar->setEnabled(true);
    }
}



void MainWindow::widgetChanged()
{
    QWidget* widget = qobject_cast<QWidget*>(sender());
    if(widget)
    {
        QVariant variant;
        if (widget == pbPower)
        {
            //pb Power
            quint8 state = pbPower->currentState() ?
                           USER_DATA_POWER_ON : USER_DATA_POWER_OFF ;

            if(AbstractCommand* command =
                        dynamic_cast<AbstractCommand*>(widget->userData(state)))
            {
                command->execute();
            }

            if(pbPower->currentState())
            {
                qDebug() << "********COMMAND ON";
            }
            else
            {
                qDebug() << "********COMMAND OFF";
            }

            return;
        }
        else if(widget == pbAttenuatorEnable)
        {
            //pb Atten
            if(pbAttenuatorEnable->currentState() == true)
            {
                qDebug() << "ATTENUATOR PB";

                if(AbstractCommand* command = dynamic_cast<AbstractCommand*>(widget->userData(USER_DATA_ID)))
                {
                    command->execute();
                }

                cbAttenuationLevel->setEnabled(true);
            }
            else
            {
                cbAttenuationLevel->setDisabled(true);
            }

            return;
        }
        else if (widget == pbPreselectorEnable)
        {
            //pb Preselectors
            if(pbPreselectorEnable->currentState())
            {
                preselectorWidget->setEnabled(true);
            }
            else
            {
                preselectorWidget->setDisabled(true);
                return;
            }
        }
        else if (widget == pbAdcNoiceBlanckerEnabled)
        {
            pbAdcNoiceBlanckerEnabled->currentState() ?
            leAdcNoiceBlanckerThreshold->setEnabled(true) :
            leAdcNoiceBlanckerThreshold->setDisabled(true);
        }
        else if (widget == cbAttenuationLevel)  //cb Atten
        {
        }
        else if (widget == preselectorWidget)   // Preselectors Widget
        {
        }
        else if (widget == pbPreamplifierEnable)   //pb Pream
        {
        }
        else if (widget == leAdcNoiceBlanckerThreshold)
        {
        }
        else if (widget == leDDC1Frequency)
        {
        }

        //**********

        if(AbstractCommand* command = dynamic_cast<AbstractCommand*>(widget->userData(USER_DATA_ID)))
        {
            command->execute();
        }

    }
    else
    {
        qDebug() << "BAD CAST TOOL WIDGET";
    }
}

//*********** IDeviceSetSettingsGetable *****************************

bool MainWindow::getPower()
{
    return pbPower->currentState();
}

DeviceSettings  MainWindow::getSettings()
{
    DeviceSettings ds;
    ds.powerEnabled = pbPower->currentState();

    if(ds.powerEnabled)
    {
        //qDebug()<<"POWER ON";
        QString strAttenuationLevel =
            cbAttenuationLevel->currentText().left(cbAttenuationLevel->currentText().indexOf(' '));

        ds.attenuator = strAttenuationLevel.toUInt();

        ds.preamplifier = pbPreamplifierEnable->currentState();
        ds.adcEnabled = pbAdcNoiceBlanckerEnabled->currentState();

        QPair<quint32, quint32>preselector =   preselectorWidget->getPreselectors();

        ds.preselectors.first = preselector.first;
        ds.preselectors.second = preselector.second;

        ds.threshold = leAdcNoiceBlanckerThreshold->text().toUShort();
        ds.ddcType = static_cast<quint32>(cbDDC1Bandwith->currentIndex());
        ds.frequency = leDDC1Frequency->getFrequencyValueInHz();
        ds.samplesPerBuffer = cbSamplesPerBuffer->currentText().toUInt();
    }
    return ds;
}

quint32  MainWindow::getDDC1Frequency()
{
    //    qDebug()<<"getDDC1Frequency()"<<leDDC1Frequency->getFrequencyValueInHz();
    return leDDC1Frequency->getFrequencyValueInHz();
}

quint32 MainWindow::getDDC1Type()
{
    return static_cast<quint32>(cbDDC1Bandwith->currentIndex());
}

quint32 MainWindow::getBandwith()
{
    return cbDDC1Bandwith->getCurrentBandwith();
}

quint32 MainWindow::getSampleRateForBandwith()
{
    quint32 ddc1Bandwith = cbDDC1Bandwith->getCurrentBandwith();

    //    qDebug()<<"Taken Samples Rate:"
    //           <<BandwithComboBox::bandwithAndSampleRateMap()[ddc1Bandwith];

    return    BandwithComboBox::bandwithAndSampleRateMap()[ddc1Bandwith];
}

quint32  MainWindow::getSamplesPerBuffer()
{
    return  cbSamplesPerBuffer->currentText().toUInt();
}

quint32  MainWindow::getAttenuator()
{
    QString strAttenuationLevel = cbAttenuationLevel->currentText().left(cbAttenuationLevel->currentText().indexOf(' '));
    quint32 atten = strAttenuationLevel.toUInt();
    return atten;
}

bool  MainWindow::getPreamplifierEnabled()
{
    return pbPreamplifierEnable->currentState();
}

Preselectors  MainWindow::getPreselectors()
{
    Preselectors p;
    p.first = preselectorWidget->getPreselectors().first;
    p.second = preselectorWidget->getPreselectors().second;
    return p;
}

bool  MainWindow::getAdcNoiceBlankerEnabled()
{
    return pbAdcNoiceBlanckerEnabled->currentState();
}

quint16  MainWindow::getAdcNoiceBlankerThreshold()
{
    return leAdcNoiceBlanckerThreshold->text().toUShort();
}

//void MainWindow::setWaitCursor()
//{
//    setCursor(Qt::WaitCursor);
//}

//void MainWindow::setArrowCursor()
//{
//    setCursor(Qt::ArrowCursor);
//}

//****************************LOAD / SAVE SETTINGS***********************

void MainWindow::loadSettings()
{
    QString  settingsFileName = QApplication::applicationDirPath() + "/" + SETTINGS_FILE_NAME;
//    if(QFile::exists(settingsFileName))
//    {
    QSettings s(settingsFileName, QSettings::IniFormat);
    s.beginGroup("attenuator");

    pbAttenuatorEnable->setCurrentState(s.value("enable").toBool());
    cbAttenuationLevel->setCurrentText(QString::number(s.value("attenuation_level_db").toUInt()) + " Db");

    s.endGroup();

    s.beginGroup("preselectors");

    QPair<quint32, quint32>preselectors;
    preselectors.first = s.value("low_frequency").toUInt();
    preselectors.second = s.value("high_frequency").toUInt();

    cbAttenuationLevel->setEnabled(s.value("enable").toBool());
    pbPreselectorEnable->setCurrentState(s.value("enable").toBool());
    preselectorWidget->setWidgetData(preselectors);

    s.endGroup();

    s.beginGroup("preamplifier");
    pbPreamplifierEnable->setCurrentState(s.value("enabled").toBool());
    s.endGroup();

    s.beginGroup("adc_noice_blanker");
    QPair<quint32, quint32>adcNoiceBlanker;
    adcNoiceBlanker.first = s.value("enabled").toBool();
    adcNoiceBlanker.second = s.value("threshold").toUInt();

    leAdcNoiceBlanckerThreshold->setText(QString::number(adcNoiceBlanker.second));
    leAdcNoiceBlanckerThreshold->setEnabled(adcNoiceBlanker.first);

    s.endGroup();

    s.beginGroup("ddc1");
    leDDC1Frequency->setFrequencyValueInHz(s.value("frequency").toUInt());
    cbDDC1Bandwith->setCurrentIndex(static_cast<int>(s.value("type_index").toUInt()));
    cbSamplesPerBuffer->setCurrentText(QString::number(s.value("samples_per_buffer").toUInt()));
    s.endGroup();
//    }
//    else
//    {
//        qDebug() << "FILE " << settingsFileName << "isn't exist";
//    }
}

void MainWindow::saveSetting()
{
    QString  settingsFileName = QApplication::applicationDirPath() + "/" + SETTINGS_FILE_NAME;
    if(QFile::exists(settingsFileName))
    {
        QSettings s(settingsFileName, QSettings::IniFormat);
        s.beginGroup("attenuator");
        s.setValue("enable", pbAttenuatorEnable->currentState());
        s.setValue("attenuation_level_db", cbAttenuationLevel->getAttenuationLevel());
        s.endGroup();

        s.beginGroup("preselectors");
        s.setValue("enable", pbPreselectorEnable->currentState());
        s.setValue("low_frequency", preselectorWidget->getPreselectors().first);
        s.setValue("high_frequency", preselectorWidget->getPreselectors().second);
        s.endGroup();

        s.beginGroup("preamplifier");
        s.setValue("enabled", pbPreamplifierEnable->currentState());
        s.endGroup();

        s.beginGroup("adc_noice_blanker");
        s.setValue("enabled", pbAdcNoiceBlanckerEnabled->currentState());
        s.setValue("threshold", leAdcNoiceBlanckerThreshold->getValue());
        s.endGroup();

        s.beginGroup("ddc1");
        s.setValue("frequency", leDDC1Frequency->getFrequencyValueInHz());
        s.setValue("type_index", cbDDC1Bandwith->currentTypeIndex());
        s.setValue("samples_per_buffer", cbSamplesPerBuffer->samplesPerBuffer());
        s.endGroup();
        s.sync();
    }
    else
    {
        qDebug() << "FILE " << settingsFileName << "isn't exist";
    }
}
/*
void MainWindow::widgetChanged(IToolBarWidget *toolBarWidget)
{
    QWidget*widget= dynamic_cast<QWidget*>(toolBarWidget);
    if(widget){
        QVariant variant;
        if (widget==pbPower){//pb Power
            if(pbPower->currentState()){qDebug()<<"********COMMAND ON";
                AbstractCommand*command=dynamic_cast<AbstractCommand*>(widget->userData(USER_DATA_POWER_ON));
                if(command)
                    command->execute();
            }else {qDebug()<<"********COMMAND OFF";
                AbstractCommand*command=dynamic_cast<AbstractCommand*>(widget->userData(USER_DATA_POWER_OFF));
                if(command)
                    command->execute();
            }
            return;
        }else if(widget==pbAttenuatorEnable){//pb Atten
            if(pbAttenuatorEnable->currentState()==true){qDebug()<<"ATTENUATOR PB";
                AbstractCommand*command=dynamic_cast<AbstractCommand*>(widget->userData(USER_DATA_ID));
                if(command)
                    command->execute();

                cbAttenuationLevel->setEnabled(true);
            }else cbAttenuationLevel->setDisabled(true);

            return;
        }else if (widget==pbPreselectorEnable) {//pb Preselectors
            if(pbPreselectorEnable->currentState()){
                preselectorWidget->setEnabled(true);
            }else {
                preselectorWidget->setDisabled(true);
                return;
            }
        }else if (widget==pbAdcNoiceBlanckerEnabled) {
            pbAdcNoiceBlanckerEnabled->currentState()==false?
                        leAdcNoiceBlanckerThreshold->setDisabled(true):
                        leAdcNoiceBlanckerThreshold->setEnabled(true);
        }else if(widget==cbAttenuationLevel){//cb Atten
        }else if (widget==preselectorWidget) {// Preselectors Widget
        }else if (widget==pbPreamplifierEnable) {//pb Pream
        }else if (widget==leAdcNoiceBlanckerThreshold) {
        }else if (widget==leDDC1Frequency){
        }

        AbstractCommand*command=dynamic_cast<AbstractCommand*>(widget->userData(USER_DATA_ID));
        if(command)
            command->execute();

    }else qDebug()<<"BAD CAST TOOL WIDGET";
}
void MainWindow::setSettings(const MainWindowSettings::Data &receiverSettings)
{
    pbAttenuatorEnable->setCurrentState(receiverSettings.attenuatorEnable);

    cbAttenuationLevel->setCurrentText(QString::number(receiverSettings.attenuator)+" Db");
    cbAttenuationLevel->setEnabled(receiverSettings.attenuatorEnable);

    pbPreselectorEnable->setCurrentState(receiverSettings.preselectorsEnable);

    preselectorWidget->setWidgetData(receiverSettings.preselectors);

    pbPreamplifierEnable->setCurrentState(receiverSettings.preamplifierEnable);

    leAdcNoiceBlanckerThreshold->setText(QString::number(receiverSettings.adcNoiceBlanker.second));
    leAdcNoiceBlanckerThreshold->setEnabled(receiverSettings.adcNoiceBlanker.first);

    leDDC1Frequency->setFrequencyValueInHz(receiverSettings.ddc1Frequency);
    cbDDC1Bandwith->setCurrentIndex(static_cast<int>(receiverSettings.ddc1TypeIndex));
    cbSamplesPerBuffer->setCurrentText(QString::number(receiverSettings.samplesPerBuffer));
}
*/
//************************** TEST ********************
//    macroFreq->addCommand(FactoryCommand::getSettingsCommand(syncManager,this));
//    macroFreq->addCommand(FactoryCommand::getRestartDdc1Command(syncManager,this));

//    MacroCommand*macroCommand=FactoryCommand::getMacroCommand();
//    macroCommand->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
//    macroCommand->addCommand(FactoryCommand::getStopDdc1Command(syncManager,this));
//    macroCommand->addCommand(FactoryCommand::getSettingsCommand(syncManager,this));
//    macroCommand->addCommand(FactoryCommand::getStartDdc1Command(syncManager,this));
//    macroCommand->addCommand(FactoryCommand::getSyncStartCommand(syncManager,this));
//    macroCommand->addCommand(FactoryCommand::getAddTaskCommand(widgetDirector,this));

//    MacroCommand *macroSamplePerBuffer=FactoryCommand::getMacroCommand();
//    macroSamplePerBuffer->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
//    macroSamplePerBuffer->addCommand(FactoryCommand::getSetDdc1Command(syncManager,this));
//    macroSamplePerBuffer->addCommand(FactoryCommand::getSyncStartCommand(syncManager,this));
//    cbSamplesPerBuffer->setUserData(USER_DATA_ID,FactoryCommand::getRestartDdc1Command(syncManager,this));
//    cbSamplesPerBuffer->setUserData(USER_DATA_ID,macroSamplePerBuffer);
