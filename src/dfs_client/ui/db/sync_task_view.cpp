#include "ui_sync_task_view.h"

#include "sync_task_view.h"

#include "widget_director.h"

#include <QCheckBox>

#include <QSqlRelationalTableModel>
#include <QSqlRecord>
#include <QSqlQuery>

#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QDateEdit>
#include <QMap>
#include <QVariant>

const QString SyncTaskView::DATE_FORMAT_DD_MM_YYYY="dd.MM.yyyy";
const QString SyncTaskView::DATE_FORMAT_YYYY_DD_MM="yyyy-MM-dd";
const QString SyncTaskView::DATE_TIME_FORMAT="yyyy-MM-dd hh:mm";

CheckBoxDelegate::CheckBoxDelegate( QObject* parent ) :
    QStyledItemDelegate( parent ) {
}

void CheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const
{

    if(index.column() == 1){
        painter->save();
        //QCheckBox::paintSection(painter, rect, logicalIndex);
        bool data = index.data().toBool();
//        qDebug()<<"CheckBoxState"<<data;
        QStyleOptionButton checkboxstyle;
        QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                                    &checkboxstyle);
        checkboxstyle.rect = option.rect;
        checkboxstyle.rect.setLeft(option.rect.x() + option.rect.width()/2 - checkbox_rect.width()/2);
        checkboxstyle.palette.setColor(QPalette::Highlight, index.data(Qt::BackgroundRole).value<QColor>());
        checkboxstyle.state = QStyle::State_Enabled | QStyle::State_Active;
        if(data)
            checkboxstyle.state = QStyle::State_On|QStyle::State_Enabled;
        else
            checkboxstyle.state = QStyle::State_Off|QStyle::State_Enabled;

        QApplication::style()->drawControl(QStyle::CE_CheckBox,
                                           &checkboxstyle, painter);
        painter->restore();
    }else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}

static const int PROGRESS_BAR_HEIGHT_PX = 20;

ProgressBarDelegate::ProgressBarDelegate( QObject* parent ) : QStyledItemDelegate( parent ) {
}

QString ProgressBarDelegate::getSizeHumanReadable(qint64 bytes)
{
    float num = bytes;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";
    QStringListIterator i(list);
    QString unit("bytes");
    while(num >= static_cast<float>(1024.0) && i.hasNext()) {
        unit = i.next();
        num /= static_cast<float>(1024.0);
    }
    return QString::fromLatin1("%1 %2").arg(static_cast<double>(num), 3, 'f', 1).arg(unit);
}

void ProgressBarDelegate::paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
        ) const {
    if(index.column() == 1){
        painter->save();
        //QCheckBox::paintSection(painter, rect, logicalIndex);
        bool data = index.data().toBool();
//        qDebug()<<"CheckBoxState"<<data;
        QStyleOptionButton checkboxstyle;
        QRect checkbox_rect = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,
                                                                    &checkboxstyle);
        checkboxstyle.rect = option.rect;
        checkboxstyle.rect.setLeft(option.rect.x() + option.rect.width()/2 - checkbox_rect.width()/2);
        checkboxstyle.palette.setColor(QPalette::Highlight,
                                       index.data(Qt::BackgroundRole).value<QColor>());
        checkboxstyle.state = QStyle::State_Enabled | QStyle::State_Active;
        if(data)
            checkboxstyle.state = QStyle::State_On|QStyle::State_Enabled;
        else
            checkboxstyle.state = QStyle::State_Off|QStyle::State_Enabled;

        QApplication::style()->drawControl(QStyle::CE_CheckBox,
                                           &checkboxstyle, painter);
        painter->restore();
    }else
    if(index.column()==4){
        painter->save();

//        qDebug()<<"Progress"<<index.data().toLongLong();
        QMap<QString,QVariant>progress=index.data().toMap();

        QString text = QString::fromLatin1(" %1 / %2 %3")
                .arg(progress["size"].toString())
                .arg(progress["bytesTotal"].toString())
                .arg(progress["percentage"].toString() + "%");

        //        int progress = index.data().toInt();
        //    int progress = index.model()->data(index, Qt::DisplayRole).toInt();
//        qDebug()<<"progress"<<text<<index;
        QStyleOptionProgressBar progressBarOption;
        QRect r = option.rect;
        r.setHeight( PROGRESS_BAR_HEIGHT_PX );
        r.moveCenter(option.rect.center());
        progressBarOption.rect = r;
        progressBarOption.textAlignment = Qt::AlignCenter;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = progress["percentage"].toInt() ;
        progressBarOption.text = text;
        //                QString::number( progress["percentage"].toInt()  ) + "%";
        progressBarOption.textVisible = true;

        QStyledItemDelegate::paint( painter, option, QModelIndex() );
        QApplication::style()->drawControl( QStyle::CE_ProgressBar,
                                            &progressBarOption, painter );
        painter->restore();
    }else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}

//************************ TABLE VIEW ************************
#include <QProgressBar>
SyncTaskView::SyncTaskView(const QString &tableName,QWidget *parent)
    : QWidget(parent),ui(new Ui::SyncTaskView)
{
    ui->setupUi(this);
    setFixedWidth(400);

    // *************** SYNC TASK TABLE
    taskTableModel=new QSqlTableModel(this);
    taskTableModel->setTable(tableName);
    taskTableModel->select();
    taskTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //    taskTableModel->setEditStrategy(QSqlTableModel::OnRowChange);
    //    taskTableModel->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);

    //***************** TASK VIEW
    //WARNING CONSTATNT
    connect(ui->tbvTaskTable,&QTableView::clicked,this,&SyncTaskView::onTableViewSelected);

    ui->tbvTaskTable->setFixedHeight(300);
//    ui->tbvTaskTable->setIndexWidget(4,new QProgressBar);
//    ui->tbvTaskTable->setHorizontalHeader(
//                new CheckBoxHeader(Qt::Horizontal, ui->tbvTaskTable));

    //WARNING БЕЗ ДЕЛЕГАТА НЕ ДОБАВЛЯЮТСЯ ДАННЫЕ
//    ui->tbvTaskTable->setItemDelegate(new CheckBoxDelegate);
            ui->tbvTaskTable->setItemDelegateForColumn( 1, new CheckBoxDelegate );
    //    qobject_cast<ProgressBarDelegate*>(ui->tbvTaskTable->itemDelegateForColumn(4));
    ui->tbvTaskTable->setModel(taskTableModel);

    ui->tbvTaskTable->setColumnHidden(taskTableModel->fieldIndex("st_id"), true);
//    ui->tbvTaskTable->setColumnHidden(taskTableModel->fieldIndex("st_size"), true);

    ui->tbvTaskTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbvTaskTable->setSortingEnabled(true);
    ui->tbvTaskTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    //    ui->tbvTaskTable->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->tbvTaskTable->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tbvTaskTable->setShowGrid(false);
    ui->tbvTaskTable->verticalHeader()->hide();
    ui->tbvTaskTable->setAlternatingRowColors(true);

    ui->tbvTaskTable->resizeColumnsToContents();

    ui->tbvTaskTable->resize(ui->tbvTaskTable->width(),ui->tbvTaskTable->height());

//    ui->tbvTaskTable->setColumnWidth(4,150);

    //**************** SET FILTER *************
    connect(ui->deSyncTask,&QDateEdit::dateChanged,this,&SyncTaskView::onTableViewFiltred);

    ui->deSyncTask->setDate(QDate::currentDate());
    onTableViewFiltred(QDate::currentDate());

    ui->chbDateFilter->setCheckState(Qt::Checked);
    connect(ui->chbDateFilter,&QCheckBox::stateChanged,this,&SyncTaskView::onCheckBoxFilterSelected);
}

SyncTaskView::~SyncTaskView()
{
    //    qDebug("DESTR SYNC TASK WIEV");
    delete ui;
}

void SyncTaskView::update()
{
    taskTableModel->select();
    //    onCheckBoxFilterSelected(Qt::Checked);
    ui->tbvTaskTable->resizeColumnsToContents();
    //     onTableViewFiltred(QDate::currentDate());
}

void SyncTaskView::onCheckBoxFilterSelected(int selectedState)
{
    switch (selectedState) {
    case Qt::Checked:
    {
        qDebug()<<"CHECKED";
        ui->deSyncTask->setEnabled(true);
        QDate date=QDate::fromString(ui->deSyncTask->text(),DATE_FORMAT_DD_MM_YYYY);
        ui->deSyncTask->setDate(date);
        onTableViewFiltred(date);
        break;
    }
    case Qt::Unchecked:
        qDebug()<<"UNCHECKED";
        ui->deSyncTask->setDisabled(true);
        taskTableModel->setFilter("");
        taskTableModel->select();
        break;
    }
}

QAbstractItemModel* SyncTaskView::getModel()
{
    return   ui->tbvTaskTable->model();
}

int SyncTaskView::selectedRowCount()
{
    return ui->tbvTaskTable->selectionModel()->selectedRows().count();
}

void SyncTaskView::onTableViewFiltred(const QDate &date)
{
    qDebug()<<"onTableViewFiltered";
    qDebug()<<"Selected Date:"<<date;
    taskTableModel->setFilter("st_date LIKE '"+date.toString(DATE_FORMAT_YYYY_DD_MM)+"%'");
    taskTableModel->select();
}

void SyncTaskView::onTableViewSelected(const QModelIndex &index)
{
    if(index.column()==1){
        bool data= index.data().toBool();
        qDebug()<<"Clicked"<<data;
        if(data){
          QMessageBox::StandardButton st= QMessageBox::question(this,"Q","Want you remove task ?");
          if(st==QMessageBox::Yes)
              QMessageBox::information(this,"Info","Task cannot remove");
        }
    }
//    if(index.column()==4)
//    ui->tbvTaskTable->setIndexWidget(index,new QProgressBar);
    qDebug()<<"ON TABLE VIEW SELECTED"<<index;


    QSqlRecord selectedRecord= taskTableModel->record(index.row());
    bool selectedStatus=ui->tbvTaskTable->selectionModel()->isSelected(index);
    emit rowSelected(selectedRecord.value("st_id").toInt(),selectedStatus);
}


void SyncTaskView::removeProgressBarDelegateForRow(int row)
{
    QAbstractItemDelegate *deleg=ui->tbvTaskTable->itemDelegateForRow(row);
    ui->tbvTaskTable->setItemDelegateForRow(row,nullptr);
    deleg->deleteLater();
    connect(deleg,&QAbstractItemDelegate::destroyed,[this]{
        taskTableModel->select();
    });
}

void SyncTaskView::setProgressBarDelegateForRow(int row)
{
    ui->tbvTaskTable->setItemDelegateForRow(row, new ProgressBarDelegate );
    ui->tbvTaskTable->setColumnWidth(4,150);
}

int SyncTaskView::getSelectedRow()
{
    int removedRow=ui->tbvTaskTable->currentIndex().row();
    ui->tbvTaskTable->model()->setData(ui->tbvTaskTable->currentIndex(),20,Qt::UserRole);

    QSqlRecord selectedRecord= taskTableModel->record(removedRow);
    int taskId=selectedRecord.value("st_id").toInt();
    return taskId;
}

QList<int>SyncTaskView::selectedRows()
{
    int count = ui->tbvTaskTable->selectionModel()->selectedRows().count();
    QList<int> list;
    for(int i=0;i<count;i++)
        list<<ui->tbvTaskTable->selectionModel()->selectedRows().at(i).row();

    return list;
}

int SyncTaskView::selectedRow()
{
    return  ui->tbvTaskTable->selectionModel()->selectedRows().first().row();
}

int SyncTaskView::getTaskIdInRow(int row)
{
    QSqlRecord selectedRecord=taskTableModel->record(row);
    return selectedRecord.value("st_id").toInt();
}

QString SyncTaskView::getTaskNameInRow(int row)
{
    QSqlRecord selectedRecord=taskTableModel->record(row);
    return selectedRecord.value("st_name").toString();
}

QDateTime SyncTaskView::getTaskDateTimeInRow(int row)
{
    QSqlRecord selectedRecord=taskTableModel->record(row);
    return selectedRecord.value("st_date").toDateTime();
}

void SyncTaskView::onTaskLoadUpdate(qint64 bytesReaded,qint64 bytesSize)
{
    qDebug()<<"SyncTaskView::onTaskLoadUpdate"<<bytesReaded<<bytesSize;


}
//  ************* SYNC TASK AND DEVICE SET TABLE
//    taskAndDeviceSetModel=new QSqlRelationalTableModel(this);
//    taskAndDeviceSetModel->setTable(DataBase::TABLE_SYNC_TASK_AND_DEVICE_SET);
//    taskAndDeviceSetModel->select();
//    taskAndDeviceSetModel->setEditStrategy(QSqlRelationalTableModel::OnFieldChange);

//    taskAndDeviceSetModel->setJoinMode(QSqlRelationalTableModel::LeftJoin);
//    taskAndDeviceSetModel->setRelation(1, QSqlRelation("sync_task", "st_id", "st_name"));
//    taskAndDeviceSetModel->setRelation(2, QSqlRelation("device_set", "ds_id", "ds_name"));
//    taskAndDeviceSetModel->select();

//*********************************DEL*******************************

//void SyncTaskView::showColumn(int column)
//{
//    if(column==4)
//        ui->tbvTaskTable->setColumnHidden(taskTableModel->fieldIndex("st_size"), false);
//}

//void SyncTaskView::hideColumn(int column)
//{
//    if(column==4)
//        ui->tbvTaskTable->setColumnHidden(taskTableModel->fieldIndex("st_size"), true);
//}


//QList<TaskDataRow> SyncTaskView::getSelectedTaskDataRow()
//{
//    int count = ui->tbvTaskTable->selectionModel()->selectedRows().count();
//    QList<TaskDataRow> taskDataRowList;

//    for(int i=0;i<count;i++){
//        int row=ui->tbvTaskTable->selectionModel()->selectedRows().at(i).row();
//        QSqlRecord selectedRecord=taskTableModel->record(row);
//        TaskDataRow dataRow;
//        dataRow.name=selectedRecord.value("st_name").toString();
//        dataRow.dateTime=selectedRecord.value("st_date").toDateTime();
//        dataRow.deviceSetNameList=db.getDeviceSetNamesBySyncTaskId(selectedRecord.value("st_id").toInt());
//        taskDataRowList<<dataRow;
//    }
//    return taskDataRowList;
//}

//void SyncTaskView::addTask(const QString &taskName,const QDateTime &dateTime,
//                           const QStringList &deviceSetNameList)
//{

//    qDebug()<<"SyncTaskView::addTask";
//    if(db.isTaskContain(taskName,dateTime.toString(DATE_TIME_FORMAT))){
//        QMessageBox::warning(this,"Adding task",
//                             "Task "+taskName+" " +dateTime.toString(DATE_TIME_FORMAT) +"is already exists");
//        return;
//    }
//    //    deviceSetNameList<<"DS#16K25036"<<"DS#16K25038_16K25037";
//    QVariantList deviceSetVarinantList;

//    for(const QString &deviceSetName:deviceSetNameList)
//        deviceSetVarinantList<<deviceSetName;

//    QVariantList syncTaskData;
//    syncTaskData.append(0);
//    syncTaskData.append(taskName);
//    syncTaskData.append(dateTime.toString(DATE_TIME_FORMAT));

//    db.insertToSyncTaskAndDeviceSetTable(syncTaskData,deviceSetVarinantList);

//    taskTableModel->select();
//    ui->tbvTaskTable->resizeColumnsToContents();
//}
