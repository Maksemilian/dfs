#include "widget_director.h"

#include "list_station_panel_widget.h"
#include "receiver_station_panel.h"
#include "sync_task_view.h"

#include "core/db/data_base.h"
#include "core/sync_manager.h"

#include <QPushButton>
#include <QTextBrowser>

#include <QVBoxLayout>
#include <QMessageBox>

#include <QDate>
#include <QDebug>
#include <QApplication>

PanelId::PanelId(qintptr id):id(id){}

TaskProgressBarUpdater::TaskProgressBarUpdater(const QStringList &taskFileNames,
                                               QAbstractItemModel*model,int row)
    :model(model),row(row)
{
    for (QString taskFileName:taskFileNames)
        fileInfos<<QFileInfo(QApplication::applicationDirPath()+"/"+taskFileName);

    progress["bytesReceived"] = QString::number(0);
    progress["bytesTotal"]    = QString::number(0);
    progress["size"]          = QString::number(0);
    progress["percentage"]    = "0";

//    model->setProgressBarDelegateForRow(row);
    //    progress["speed"]         = QString::number((double)outputFile->size()/timer.elapsed(),'f',0).append(" KB/s");
    //    progress["time"]          = QString::number((double)timer.elapsed()/1000,'f',2).append("s");

}

QString TaskProgressBarUpdater::getSizeHumanReadable(qint64 bytes)
{
    float num = bytes; QStringList list;
    list << "KB" << "MB" << "GB" << "TB";
    QStringListIterator i(list);
    QString unit("bytes");
    while(num >= 1024.0 && i.hasNext()) {
        unit = i.next(); num /= 1024.0;
    }
    return QString::fromLatin1("%1 %2").arg(num, 3, 'f', 1).arg(unit);
}

void TaskProgressBarUpdater::start(qint64 taskSize)
{
    this->taskSize=taskSize;
    //    progress["bytesTotal"]  =QString::number(taskSize);
    progress["bytesTotal"]  =getSizeHumanReadable(taskSize);
    connect(&timer,&QTimer::timeout,this,&TaskProgressBarUpdater::onTimer);
    timer.start();
}

void TaskProgressBarUpdater::stop()
{
    disconnect(&timer,&QTimer::timeout,this,&TaskProgressBarUpdater::onTimer);
    timer.stop();
//    model->removeProgressBarDelegateForRow(row);
    //    taskView->update();
    emit finished();
}

void TaskProgressBarUpdater::onTimer()
{
    qint64 currentSize=0;

    for(QFileInfo&fileInfo:fileInfos){
        if(fileInfo.exists()){
            fileInfo.refresh();
            currentSize+=fileInfo.size();
            //    qDebug()<<"FILE INFO"<<fileInfo.exists()<<fileInfo.filePath()<<fileInfo.size();
            if(currentSize==taskSize){
                qDebug()<<"file loaded";
                stop();
                emit taskLoaded(currentSize);
            }
        }else {
            qDebug()<<"Error file"<<fileInfo.fileName();
            stop();
            emit taskLoaded(currentSize);
        }
    }
    progress["bytesReceived"]=QString::number(currentSize);
    progress["size"]=getSizeHumanReadable(currentSize);
    progress["percentage"] =QString::number(static_cast<int>((currentSize*100)/taskSize));
    //    int progress=static_cast<int>((currentSize*100)/taskSize);

    //    qDebug()<<"Update"<<currentSize<<taskSize<<progress;
    QModelIndex actionIndex = model->index(row,4);
    //    taskView->getModel()->setData(actionIndex, progress);
    model->setData(actionIndex, progress);
}

//******************************* Widget Director ****************************
WidgetDirector::WidgetDirector(QWidget*parent)
    :QWidget (parent),
      listStationPanelWidget(new ListStationPanelWidget(this)),
      pbLoadTask(new QPushButton(this)),
      pbRemoveTask(new QPushButton(this))
{
    //    connect(&timer,&QTimer::timeout,this,&WidgetDirector::timerLoadTask);

    DataBase::instance().connect();

    taskView=new SyncTaskView (DataBase::TABLE_SYNC_TASK,this);
    textBrowser=new QTextBrowser(this);
    textBrowser->setFixedHeight(50);

    setLayout(new QVBoxLayout);
    layout()->addWidget(listStationPanelWidget);
    layout()->addWidget(taskView);
    layout()->addWidget(textBrowser);

    layout()->addWidget(pbLoadTask);
    layout()->addWidget(pbRemoveTask);

    pbLoadTask->setText("Load Task");
    pbRemoveTask->setText("Remove Task");

    pbLoadTask->setEnabled(false);
    pbRemoveTask->setEnabled(false);

    connect(&SyncManager::instance(),&SyncManager::taskSize,
            [this](const QString &taskName,qint64 taskSize)
    {
        qDebug()<<"Task Name"<<taskName<<"Task Size"<<taskSize;
        taskView->setProgressBarDelegateForRow(taskView->selectedRow());
        this->taskUpdater->start(taskSize);
    });

    connect(pbLoadTask,&QPushButton::clicked,
            this,&WidgetDirector::onLoadSelectedTask);

    connect(pbRemoveTask,&QPushButton::clicked,
            this,&WidgetDirector::onRemoveSelectedTask);

    connect(taskView,&SyncTaskView::rowSelected,[this](int row,bool selectedStatus)
    {
        if(selectedStatus||taskView->selectedRowCount()>0){
            pbLoadTask->setEnabled(true);
            pbRemoveTask->setEnabled(true);
            QStringList deviceSetNameList= DataBase::instance().getDeviceSetNamesBySyncTaskId(row);
            textBrowser->clear();
            if(!deviceSetNameList.isEmpty())
                for (QString deviceSetName:deviceSetNameList)
                    textBrowser->append(deviceSetName);
        }else {
            pbRemoveTask->setEnabled(false);
            pbLoadTask->setEnabled(false);
            textBrowser->clear();
        }
    });
}

void WidgetDirector::enable()
{
    setEnabled(true);
}

void WidgetDirector::disable()
{
    setEnabled(false);
}

void WidgetDirector ::createStationData(qintptr id,const QString &addr,const QString &port,
                                        const QStringList &deviceSetNameList)
{
    ReceiverStationPanel *panel=new ReceiverStationPanel;
    panel->setUserData(USER_DATA_PANEL_ID,new PanelId(id));
    panel->setAddress(addr,port);

    for(const QString &receiverName:deviceSetNameList)
        panel->addReceiver(receiverName);
    //        qDebug()<<"Receiver Name"<<receiverName;

    listStationPanelWidget->appendReceiverStationPanel(panel);
}

void WidgetDirector::removeStationData(qintptr id)
{
    //    qDebug()<<"Remove Station by id"<<id;
    QList<ReceiverStationPanel*>panels=listStationPanelWidget->getPanels();
    for(ReceiverStationPanel*panel:panels){
        if(dynamic_cast<PanelId*>(panel->userData(1))->getId()==id){
            listStationPanelWidget->removeReceiverStationPanel(panel);
            break;
        }
    }
}

void WidgetDirector::onAddSelectedTask(const QString &taskName, const QDateTime &dateTime)
{
    //    qDebug()<<"WidgetDirector::addTask"<<taskName<<dateTime;

    if(!DataBase::instance().isTaskExist(taskName,dateTime.toString(SyncTaskView::DATE_TIME_FORMAT))){
        QStringList deviceSetNameList=listStationPanelWidget->getDeviceSetNameList();
        QVariantList deviceSetVarinantList;

        for(const QString &deviceSetName:deviceSetNameList)
            deviceSetVarinantList<<deviceSetName;

        QVariantList syncTaskData;
        syncTaskData.append(0);
        syncTaskData.append(taskName);
        syncTaskData.append(dateTime.toString(SyncTaskView::DATE_TIME_FORMAT));

        DataBase::instance().insertToSyncTaskAndDeviceSetTable(syncTaskData,deviceSetVarinantList);
        taskView->update();

        qDebug()<<"SyncTaskView::addTask"<<taskName<<dateTime<<deviceSetNameList;
        //        QMessageBox::warning(this,"Adding task",
        //                             "Task "+taskName+" " +dateTime.toString(SyncTaskView::DATE_TIME_FORMAT) +"is already exists");

    }
    emit taskAdded();
}

void WidgetDirector::onLoadSelectedTask()
{
//    qDebug()<<"OnTaskButtonLoadClicked";
    int row=taskView->selectedRow();
    int taskId=taskView->getTaskIdInRow(row);
    QString name=taskView->getTaskNameInRow(row);
    QDateTime dateTime=taskView->getTaskDateTimeInRow(row);

    QStringList deviceSetNameList=
            DataBase::instance().getDeviceSetNamesBySyncTaskId(taskView->getTaskIdInRow(row));

    QStringList taskFileNames;
    for(QString deviceSetName:deviceSetNameList)
        taskFileNames<<deviceSetName+"/"+dateTime.date().toString("yyyy-MM-dd")+
                       "/"+name;

    taskUpdater=new TaskProgressBarUpdater(taskFileNames,taskView->getModel(),row);

    connect(taskUpdater,&TaskProgressBarUpdater::finished,
            taskUpdater,&TaskProgressBarUpdater::deleteLater);

    connect(taskUpdater,&TaskProgressBarUpdater::taskLoaded,
            [this,taskId,row](qint64 bytes){
        qDebug()<<"Task Loaded"<<bytes;
        taskView->removeProgressBarDelegateForRow(row);
        DataBase::instance().updateTaskSizeToTaskTableById(taskId,bytes);
        DataBase::instance().updateTaskStatusToTaskTableById(taskId,true);
        taskView->update();
    });

    SyncManager::instance().loadBroacastFile(name,dateTime.date());
}

void WidgetDirector::onRemoveSelectedTask()
{
    QMessageBox::StandardButton st= QMessageBox::question(this,"Q","Want you remove this task"
                                                                   " from db ?");
    if(st==QMessageBox::Yes){
        int taskId=taskView->getSelectedRow();
        DataBase::instance().removeTaskById(taskId);
        taskView->update();
        qDebug()<<"TASK ID FOR REMOVE"<<taskId;
    }
}

//********************* DEL *****************************
/*/
bool WidgetDirector::checkAvailableTask(const QStringList &deviceSetNames)
{
    bool isStationFind=false;
    for(QString deviceSetName:deviceSetNames){
        isStationFind=false;
        for(int i=0;i<listStationPanelWidget->count();i++){
            ReceiverStationPanel*panel=qobject_cast<ReceiverStationPanel*>(
                        listStationPanelWidget->itemWidget(listStationPanelWidget->item(i)));
            if(panel&&panel->getCurrentDeviceSetName()==deviceSetName)
                isStationFind=true;
            //            else qDebug()<<"NOT FIND NAME"<<panel->getCurrentDeviceSetName();
        }

        if(!isStationFind)break;
    }
    qDebug()<<"SyncTaskView::checkAvailableTask"<<isStationFind;
    return isStationFind;
}


void WidgetDirector::loadTaskSelected()
{
    qDebug()<<"OnTaskButtonLoadClicked";
    //    FileLoadingDialog fileLoadingDialog;
    //    fileLoadingDialog.setModal(true);

    QList<int>rowList=taskView->selectedRows();
    QList<TaskDataRow>taskDataRowList;

    for(int row:rowList){
        TaskDataRow dataRow;
        dataRow.name=taskView->getTaskNameInRow(row);
        dataRow.dateTime=taskView->getTaskDateTimeInRow(row);
        dataRow.deviceSetNameList=dataRow.deviceSetNameList=
                DataBase::instance().getDeviceSetNamesBySyncTaskId(taskView->getTaskIdInRow(row));
        taskDataRowList<<dataRow;
    }
    qDebug()<<"***********SIZE"<<taskDataRowList.size();
    for(TaskDataRow dataRow:taskDataRowList){

        QList<TaskRequest>requestList= createTaskRequest(dataRow.name,
                                                         dataRow.dateTime.date(),
                                                         dataRow.deviceSetNameList);

        if(requestList.size()==dataRow.deviceSetNameList.size()){
            //WARNING NEW USING METHOD ADD TASK
            //            fileLoadingDialog.addTask(dataRow.name,dataRow.dateTime.date());
            //            fileLoadingDialog.addTask(requestList);
            TaskLoadingWidget fileLoadingDialog(this);
            fileLoadingDialog.setModal(true);
            fileLoadingDialog.setLabelText("Task: "+dataRow.name +" Date: "+dataRow.dateTime.date().toString("yyyy-MM-dd"));

            connect(&SyncManager::instance(),&SyncManager::taskLoadingProgress,
                    &fileLoadingDialog,&TaskLoadingWidget::setProgress);

            connect(&SyncManager::instance(),&SyncManager::taskLoadingProgress,
                    taskView,&SyncTaskView::onTaskLoadUpdate);

            SyncManager::instance().loadTask(dataRow.name,dataRow.dateTime.date());

            fileLoadingDialog.exec();
        }
        else  QMessageBox::warning(this,"Loading task warning",
                                   "Selected station do not use requested device set");
    }
}

    //    TaskLoadingWidget fileLoadingDialog(this);
    //    fileLoadingDialog.setModal(true);
    //    fileLoadingDialog.setLabelText("Task: "+name +" Date: "+dateTime.date().toString("yyyy-MM-dd"));

    //    connect(&SyncManager::instance(),&SyncManager::taskLoadingProgress,
    //            &fileLoadingDialog,&TaskLoadingWidget::setProgress);

    //    taskUpdaters=new TaskProgressBarUpdater(taskFileNames,taskView,row);
    //    connect(taskUpdaters,&TaskProgressBarUpdater::finished,
    //            taskUpdaters,&TaskProgressBarUpdater::deleteLater);

    //    SyncManager::instance().loadTask(name,dateTime.date());

    //    fileLoadingDialog.exec();
/*/

//QList<TaskRequest> WidgetDirector::createTaskRequest(const QString &taskName,
//                                                     const QDate &taskDate,
//                                                     const QStringList &deviceSetNames){

//    Q_UNUSED(deviceSetNames);
//    //    qDebug()<<"NAME TASK:"<<taskName;
//    //    qDebug()<<"DATA TASK:"<<taskDate;
//    QList<TaskRequest>requestList;
//    for(int i=0;i<listStationPanelWidget->count();i++){

//        ReceiverStationPanel*panel=qobject_cast<ReceiverStationPanel*>(
//                    listStationPanelWidget->itemWidget(listStationPanelWidget->item(i)));
//        if(panel){
//            TaskRequest requestData;

//            requestData.stationAddress=panel->getIpAddress();
//            requestData.stationPort=SERVER_PORT;
//            requestData.fileName=panel->getCurrentDeviceSetName()+
//                    "/"+taskDate.toString(SyncTaskView::DATE_FORMAT_YYYY_DD_MM)+
//                    "/"+taskName;

//            requestList<<requestData;
//        }
//    }
//    return requestList;
//}
