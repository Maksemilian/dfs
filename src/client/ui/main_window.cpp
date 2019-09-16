#include "ui_main_window.h"
#include "main_window.h"

#include "main_window_settings.h"

#include "core/command/factory_command.h"
#include "core/command/command_hierarch.h"

#include "core/sync_manager.h"

#include "tool/frequency_line_edit.h"
#include "tool/preselector_widget.h"
#include "tool/switch_button.h"
#include "tool/widgets.h"

#include "ui/db/receiver_station_panel.h"
#include "ui/db/widget_director.h"

#include "ui/plot/channel_plot.h"
#include "ui/plot/elips_plot.h"

#include <QDockWidget>
#include <QCheckBox>
#include <QDebug>
#include <QFile>

//**************CONSTANTS*******************
const QString MainWindow::SETTINGS_FILE_NAME="device_set.ini";

MainWindow:: MainWindow(QWidget *parent):
    QMainWindow(parent),ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget()->setLayout(new QVBoxLayout);

    //    syncManager=new SyncManager;
    //TODO FIX THIS CODE
    syncManager=&SyncManager::instance();
    //************** WIDGET DIRECTOR ******************

    widgetDirector=new WidgetDirector;
    setLeftDockWidget(widgetDirector,"Widget Director");

    connect(syncManager,&SyncManager::stationConnected,
            widgetDirector,&WidgetDirector::createStationData);

    connect(syncManager,&SyncManager::stationDisconnected,
            widgetDirector,&WidgetDirector::removeStationData);

    connect(syncManager,&SyncManager::commandError,[this](const QString &error){
        setArrowCursor();
        QMessageBox::warning(this,"Command Error",error);
    });
    //************SETTING TOOLBAR***************
    //****TOP

    ui->mainToolBar->layout()->setSpacing(10);
    ui->mainToolBar->setMovable(false);
    setTopToolBar(ui->mainToolBar);
    toolBarMap.insert(Qt::TopToolBarArea,ui->mainToolBar);

    //****BOTTOM

    QToolBar *bottomToolBar=new QToolBar;
    bottomToolBar->setMovable(false);
    bottomToolBar->layout()->setSpacing(10);
    setBottomToolBar(bottomToolBar);
    addToolBar(Qt::BottomToolBarArea,bottomToolBar);

    toolBarMap.insert(Qt::BottomToolBarArea,bottomToolBar);

    loadSettings();

    //************** ELIPSE PLOT***************************

    elipsPlot=new ElipsPlot;
    setRightDockWidget(elipsPlot,"Elipse");

    //************** CHANNEL PLOT**************************

    channelPlot=new ChannelPlot(2,getSamplesPerBuffer());
    setCentralWidget(channelPlot);

    //************** SUBSCRIBE ****************

    syncManager->addSyncSignalUpdater(channelPlot);
    syncManager->addSumDivUpdater(elipsPlot);

    //************** OTHER ******************
    connect(syncManager,&SyncManager::syncReady,
            this,&MainWindow::showReceiverSettingsTool);

    connect(syncManager,&SyncManager::syncNotReady,
            this,&MainWindow::hideReceiverSettingsTool);

    connect(syncManager,&SyncManager::syncReady,
            widgetDirector,&WidgetDirector::enable);

    connect(syncManager,&SyncManager::syncNotReady,
            widgetDirector,&WidgetDirector::disable);

    //TODO UNCOMMENT IN RELEASE VERSION
    //    hideReceiverSettingsTool();
}

MainWindow::~MainWindow()
{
    saveSetting();
    delete ui;
}

void MainWindow::setCentralWidget(QWidget *widget)
{
    centralWidget()->layout()->addWidget(widget);
}

void MainWindow::setLeftDockWidget(QWidget *widget,const QString &title)
{
    QDockWidget *leftDockWidget=new QDockWidget(title,this);
    leftDockWidget->setWidget(widget);
    addDockWidget(Qt::LeftDockWidgetArea,leftDockWidget);
}

void MainWindow::setRightDockWidget(QWidget *widget,const QString &title)
{
    QDockWidget *rightDockWidget=new QDockWidget(title,this);
    rightDockWidget->setWidget(widget);
    addDockWidget(Qt::RightDockWidgetArea,rightDockWidget);
}

//****************** SETTING TOP TOOL BAR *************************

void MainWindow::setTopToolBar(QToolBar *topToolBar)
{
    if(!topToolBar) return;

    //DDC1 Frequency
    MacroCommand *macroFreq=FactoryCommand::getMacroCommand();
    macroFreq->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
    macroFreq->addCommand(FactoryCommand::getStopDdc1Command(syncManager,this));
    macroFreq->addCommand(FactoryCommand::getFrequencyCommand(syncManager,this));
    macroFreq->addCommand(FactoryCommand::getStartDdc1Command(syncManager,this));
    macroFreq->addCommand(FactoryCommand::getSyncStartCommand(syncManager,this));

    macroFreq->addCommand(FactoryCommand::getAddTaskCommand(widgetDirector,this));

    leDDC1Frequency =new FrequencyLineEdit(this,topToolBar);
    leDDC1Frequency->setUserData(USER_DATA_ID,macroFreq);

    //DDC1 Bandwith
    MacroCommand *macroBandwith=FactoryCommand::getMacroCommand();
    macroBandwith->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
    macroBandwith->addCommand(FactoryCommand::getStopDdc1Command(syncManager,this));
    macroBandwith->addCommand(FactoryCommand::getSetDdc1Command(syncManager,this));
    macroBandwith->addCommand(FactoryCommand::getStartDdc1Command(syncManager,this));
    macroBandwith->addCommand(FactoryCommand::getSyncStartCommand(syncManager,this));

    cbDDC1Bandwith=new BandwithComboBox(this,topToolBar);
    cbDDC1Bandwith->setUserData(USER_DATA_ID,macroBandwith);

    //Samples per Buffer

    cbSamplesPerBuffer=new SampleRateComboBox(this,topToolBar);
    cbSamplesPerBuffer->setEnabled(false);

    topToolBar->addWidget(leDDC1Frequency);

    topToolBar->addSeparator();
    topToolBar->addWidget(cbDDC1Bandwith);
    topToolBar->addSeparator();
    topToolBar->addWidget(cbSamplesPerBuffer);

    topToolBar->addSeparator();
}

//****************** SETTING BOTTOM TOOL BAR*************************

void MainWindow::setBottomToolBar(QToolBar *bottomToolBar)
{
    if(!bottomToolBar) return ;
    //power BUTTON
    pbPower=new SwitchButton("On","Off",false,this,bottomToolBar);

    MacroCommand*macroCommand=FactoryCommand::getMacroCommand();
    macroCommand->addCommand(FactoryCommand::getPowerComandOn(syncManager,this));
    macroCommand->addCommand(FactoryCommand::getSettingsCommand(syncManager,this));
    macroCommand->addCommand(FactoryCommand::getStartDdc1Command(syncManager,this));
    macroCommand->addCommand(FactoryCommand::getSyncStartCommand(syncManager,this));
    macroCommand->addCommand(FactoryCommand::getAddTaskCommand(widgetDirector,this));

    pbPower->setUserData(USER_DATA_POWER_ON,macroCommand);

    MacroCommand*mc=FactoryCommand::getMacroCommand();
    mc->addCommand(FactoryCommand::getSyncStopCommand(syncManager,this));
    mc->addCommand(FactoryCommand::getStopDdc1Command(syncManager,this));
    mc->addCommand(FactoryCommand::getPowerComandOff(syncManager,this));

    pbPower->setUserData(USER_DATA_POWER_OFF,mc);

    //attenuator BUTTON
    pbAttenuatorEnable=new SwitchButton("Atten Enable","Atten Disable",false,this,bottomToolBar);
    pbAttenuatorEnable->setUserData(USER_DATA_ID,FactoryCommand::getAttenuatorCommand(syncManager,this));

    //attenuator COMBO BOX
    cbAttenuationLevel=new AttenuatorComboBox(this,bottomToolBar);
    cbAttenuationLevel->setUserData(USER_DATA_ID,FactoryCommand::getAttenuatorCommand(syncManager,this));

    //preamplifier BUTTON
    pbPreamplifierEnable=new SwitchButton("Pream Enable","Pream Disable",false,this,bottomToolBar);
    pbPreamplifierEnable->setUserData(USER_DATA_ID,FactoryCommand::getPreamplifireCommand(syncManager,this));

    //preseector BUTTON
    pbPreselectorEnable=new SwitchButton("Pres Enable","Pres Disable",false,this,bottomToolBar);
    pbPreselectorEnable->setUserData(USER_DATA_ID,FactoryCommand::getPreselectorCommand(syncManager,this));

    //preseector LOW FREQUENCY COMBO BOX
    preselectorWidget=new PreselectorWidget(this,bottomToolBar);
    preselectorWidget->setUserData(USER_DATA_ID,FactoryCommand::getPreselectorCommand(syncManager,this));

    //ADC NOICE BLANKER BUTTON
    pbAdcNoiceBlanckerEnabled=new SwitchButton("ADC Enable","ADC Disable",false,this,bottomToolBar);
    pbAdcNoiceBlanckerEnabled->setUserData(USER_DATA_ID,FactoryCommand::getAdcEnabledCommand(syncManager,this));

    //ADC NOICE BLANKER LENE EDIT
    leAdcNoiceBlanckerThreshold=new ToolBarLineEdit(this,bottomToolBar);
    leAdcNoiceBlanckerThreshold->setFixedWidth(100);
    leAdcNoiceBlanckerThreshold->setUserData(USER_DATA_ID,FactoryCommand::getAdcThresholdCommand(syncManager,this));
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
    for(QToolBar *toolBar:toolBarMap.values())
        toolBar->setDisabled(true);
}

void MainWindow::showReceiverSettingsTool()
{
    for(QToolBar *toolBar:toolBarMap.values())
        toolBar->setEnabled(true);
}

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

        //**********
        AbstractCommand*command=dynamic_cast<AbstractCommand*>(widget->userData(USER_DATA_ID));
        if(command)
            command->execute();

    }else qDebug()<<"BAD CAST TOOL WIDGET";
}

//*********** IDeviceSetSettingsGetable *****************************

bool MainWindow::getPower()
{
    return pbPower->currentState();
}

DeviceSetSettings  MainWindow::getSettings()
{
    DeviceSetSettings ds;
    ds.powerEnabled=pbPower->currentState();

    if(ds.powerEnabled){
        //qDebug()<<"POWER ON";
        QString strAttenuationLevel=
                cbAttenuationLevel->currentText().left(cbAttenuationLevel->currentText().indexOf(' '));

        ds.attenuator=strAttenuationLevel.toUInt();

        ds.preamplifier=pbPreamplifierEnable->currentState();
        ds.adcEnabled=pbAdcNoiceBlanckerEnabled->currentState();

        QPair<quint32,quint32>preselector=   preselectorWidget->getPreselectors();

        ds.preselectors.first=preselector.first;
        ds.preselectors.second=preselector.second;

        ds.threshold=leAdcNoiceBlanckerThreshold->text().toUShort();
        ds.ddcType=static_cast<quint32>(cbDDC1Bandwith->currentIndex());
        ds.frequency=leDDC1Frequency->getFrequencyValueInHz();
        ds.samplesPerBuffer=cbSamplesPerBuffer->currentText().toUInt();
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
    quint32 ddc1Bandwith=cbDDC1Bandwith->getCurrentBandwith();

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
    QString strAttenuationLevel= cbAttenuationLevel->currentText().left(cbAttenuationLevel->currentText().indexOf(' '));
    quint32 atten=strAttenuationLevel.toUInt();
    return atten;
}

bool  MainWindow::getPreamplifierEnabled()
{
    return pbPreamplifierEnable->currentState();
}

QPair<quint32,quint32>  MainWindow::getPreselectors()
{
    return preselectorWidget->getPreselectors();
}

bool  MainWindow::getAdcNoiceBlankerEnabled()
{
    return pbAdcNoiceBlanckerEnabled->currentState();
}

quint16  MainWindow::getAdcNoiceBlankerThreshold()
{
    return leAdcNoiceBlanckerThreshold->text().toUShort();
}

void MainWindow::setWaitCursor()
{
    setCursor(Qt::WaitCursor);
}

void MainWindow::setArrowCursor()
{
    setCursor(Qt::ArrowCursor);
}

//****************************LOAD / SAVE SETTINGS***********************

void MainWindow::loadSettings()
{
    QFile file(QApplication::applicationDirPath()+"/"+SETTINGS_FILE_NAME);
    if(file.exists()){
        MainWindowSettings settingsHandler(file.fileName());
        MainWindowSettings::Data receiverSettings=settingsHandler.load();
        setSettings(receiverSettings);
    }else   qDebug()<<"FILE "<<file.fileName()<<"isn't exist";
}

void MainWindow::saveSetting()
{
    MainWindowSettings::Data receiverSettings;
    QString attetuatorText=cbAttenuationLevel->currentText();
    receiverSettings.attenuatorEnable=pbAttenuatorEnable->currentState();
    receiverSettings.attenuator=attetuatorText.left(attetuatorText.indexOf(' ')).toUInt();

    receiverSettings.preselectorsEnable=pbPreselectorEnable->currentState();
    QPair<quint32,quint32>pair=preselectorWidget->getPreselectors();

    receiverSettings.preselectors.first=pair.first;
    receiverSettings.preselectors.second=pair.second;
    //    qDebug()<<lowFrequency<<highFrequency;
    receiverSettings.preamplifierEnable=pbPreamplifierEnable->currentState();

    quint16 threashold=leAdcNoiceBlanckerThreshold->text().toUShort();
    receiverSettings.adcNoiceBlanker.first=pbAdcNoiceBlanckerEnabled->currentState();
    receiverSettings.adcNoiceBlanker.second=threashold;

    quint32 frequency=leDDC1Frequency->getFrequencyValueInHz();
    //qDebug()<<"Freq"<<frequency;
    receiverSettings.ddc1Frequency=frequency;
    receiverSettings.ddc1TypeIndex=static_cast<quint32>(cbDDC1Bandwith->currentIndex());
    receiverSettings.samplesPerBuffer=cbSamplesPerBuffer->currentText().toUInt();

    QFile file(QApplication::applicationDirPath()+"/"+SETTINGS_FILE_NAME);

    if(file.exists()){
        MainWindowSettings settingsHandler(file.fileName());
        settingsHandler.save(receiverSettings);
    }else  qDebug()<<"FILE "<<file.fileName()<<"isn't exist";

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
