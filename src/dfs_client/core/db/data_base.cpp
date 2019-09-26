#include "data_base.h"

#include <QCoreApplication>

#include <QDateTime>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>

const QString DataBase::DATA_BASE_NAME="test.db";

const QString DataBase::TABLE_SYNC_TASK="sync_task";
const QString DataBase::TABLE_DEVICE_SET="device_set";
const QString DataBase::TABLE_SYNC_TASK_AND_DEVICE_SET="sync_task_and_device_set";
const QString DataBase::DATE_FORMAT="yyyy-MM-dd";


void DataBase::connect()
{
    open();
}

bool DataBase::open()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QCoreApplication::applicationDirPath()+"/"+DATA_BASE_NAME);

    if (!db.open()){
        return true;
    }else {
        return false;
    }
}

void DataBase::close()
{
    db.close();
}

void DataBase::selectByDate(const QVariant &taskDate)
{
    QSqlQuery querySyncTaskTable;

    if(!querySyncTaskTable.exec("SELECT * FROM "+TABLE_SYNC_TASK+
                                " WHERE st_date ='"+taskDate.toString()+"'"))
        qDebug() << "error select into "
                 << TABLE_SYNC_TASK
                 << querySyncTaskTable.lastError().text();

    while(querySyncTaskTable.next())
        qDebug()<<"Name"<<querySyncTaskTable.value(2).toString()
               <<"Date"<<querySyncTaskTable.value(3).toString();

}

bool DataBase::isTaskExist(const QVariant &taskName,const QVariant &taskDate)
{
    QSqlQuery querySyncTaskTable;

    if(!querySyncTaskTable.exec("SELECT st_id FROM "+TABLE_SYNC_TASK+
                                " WHERE st_name = '"+taskName.toString()+"' "
                                " AND st_date LIKE '"+taskDate.toDateTime().date().
                                toString(DATE_FORMAT)+"%'")){
        qDebug() << "error select into "
                 << TABLE_SYNC_TASK
                 << querySyncTaskTable.lastError().text();

    }else if(querySyncTaskTable.first())
        return true;

    return false;
}

void DataBase::insertToTaskTable(const QVariantList &syncTaskData)
{
    QSqlQuery querySyncTaskTable;

    querySyncTaskTable.prepare("insert into "+ TABLE_SYNC_TASK +" (st_state,st_name,st_date)"
                                                                "values(?,?,?) ");

    querySyncTaskTable.addBindValue(syncTaskData[0].toInt());
    querySyncTaskTable.addBindValue(syncTaskData[1].toString());
    querySyncTaskTable.addBindValue(syncTaskData[2].toString());

    if(!querySyncTaskTable.exec())
        qDebug() << "error insert into "
                 << TABLE_SYNC_TASK
                 << querySyncTaskTable.lastError().text();
}

void DataBase::insertToDeviceSetTable(const QVariantList &data)
{
    QSqlQuery query;

    query.prepare("insert into "+ TABLE_DEVICE_SET +" (ds_name) values(?)");

    query.addBindValue(data[0].toString());

    if(!query.exec())
        qDebug() << "error insert into "
                 << TABLE_DEVICE_SET
                 << query.lastError().text();
}

void DataBase::insertToSyncTaskAndDeviceSetTable(const QVariantList &syncTaskData,
                                                 const QVariantList &deviceSetData){

    QSqlQuery querySyncTaskTable;

    querySyncTaskTable.prepare("insert into "+ TABLE_SYNC_TASK +" (st_state,st_name,st_date)"
                                                                "values(?,?,?) ");

    querySyncTaskTable.addBindValue(syncTaskData[0].toInt());
    querySyncTaskTable.addBindValue(syncTaskData[1].toString());
    querySyncTaskTable.addBindValue(syncTaskData[2].toString());

    if(!querySyncTaskTable.exec()){
        qDebug() << "error insert into " << TABLE_SYNC_TASK<< querySyncTaskTable.lastError().text();
        return;
    }

    if(!querySyncTaskTable.exec("SELECT st_id FROM "+TABLE_SYNC_TASK+
                                " WHERE st_name = '"+syncTaskData[1].toString()+"'"
                                " AND   st_date = '"+syncTaskData[2].toString()+"'")){
        qDebug() << "error insert into "
                 << TABLE_SYNC_TASK
                 << querySyncTaskTable.lastError().text();
        return ;
    }

    querySyncTaskTable.next();
    int taskId=querySyncTaskTable.value(0).toInt();

    qDebug()<<"TASK ID"<<taskId;
    querySyncTaskTable.clear();

    //*2
    QSqlQuery queryDeviceSetTable;
    QString strQuery="SELECT ds_id FROM "+TABLE_DEVICE_SET+" WHERE ds_name IN(";;
    QString strBindValue="'?'";
    strQuery+="'"+deviceSetData[0].toString()+"'";

    for(int i=1;i<deviceSetData.size();i++)
        strQuery+=",'"+deviceSetData[i].toString()+"'";

    strQuery+=")";

    qDebug()<<strQuery;
    if(!queryDeviceSetTable.exec(strQuery)){
        qDebug() << "error select from "
                 << TABLE_DEVICE_SET
                 << querySyncTaskTable.lastError().text();
        return;
    }

    //*3
    QSqlQuery querySyncTaskAndDeviceSetTable;
    querySyncTaskAndDeviceSetTable.prepare("insert into "+TABLE_SYNC_TASK_AND_DEVICE_SET+
                                           " (st_id,ds_id) values(? , ?)");
    while(queryDeviceSetTable.next()){
        int dsId=queryDeviceSetTable.value(0).toInt();
        qDebug()<<"queryDeviceSetTable dsId"<<dsId;

        querySyncTaskAndDeviceSetTable.addBindValue(taskId);
        querySyncTaskAndDeviceSetTable.addBindValue(dsId);

        if(!querySyncTaskAndDeviceSetTable.exec()){
            qDebug()<<"erro insert into "
                   <<TABLE_SYNC_TASK_AND_DEVICE_SET
                  <<querySyncTaskAndDeviceSetTable.lastError().text();
            break;
        };
    }
}

void DataBase::updateTaskSizeToTaskTableById(const QVariant &id, const QVariant &progress)
{
    QSqlQuery query;
    QString strQuery="UPDATE "+TABLE_SYNC_TASK+" SET st_size ='"+ progress.toString()+"' WHERE st_id ="+id.toString();

    if(!query.exec(strQuery))
        qDebug()<<"BAD ERROR:"<<strQuery;
}

void DataBase::updateTaskStatusToTaskTableById(const QVariant &id, const QVariant &status)
{
    QSqlQuery query;
    QString strQuery="UPDATE "+TABLE_SYNC_TASK+" SET st_state ="+ status.toString()+" WHERE st_id ="+id.toString();

    if(!query.exec(strQuery))
        qDebug()<<"BAD ERROR:"<<strQuery;
}

QStringList DataBase::getDeviceSetNamesBySyncTaskId(const QVariant &syncTaskId)
{
    QStringList deviceSetNameList;
    qDebug()<<"Sel Task Id:"<<syncTaskId.toString();
    QSqlQuery q;
    //qDebug()<<"THIS:"<<taskTableModel->itemData(index);
    QString syncTaskAndDeviceSetTableQuery=
            "SELECT ds_id FROM "+TABLE_SYNC_TASK_AND_DEVICE_SET+" WHERE st_id="
            +            syncTaskId.toString() ;

    if(!q.exec(syncTaskAndDeviceSetTableQuery))
        qDebug()<<"ERROR EXEC_1:"
               <<syncTaskAndDeviceSetTableQuery;


    QString deviceSetTableQuery="SELECT ds_name FROM "+TABLE_DEVICE_SET+" WHERE ds_id IN(";
    while (q.next()) {
        int dsId=q.value(0).toInt();
        qDebug()<<"DS ID"<<dsId;
        deviceSetTableQuery+=QString::number(dsId);
        deviceSetTableQuery+="," ;
    }
    deviceSetTableQuery.remove(deviceSetTableQuery.length()-1,1);
    deviceSetTableQuery+=")" ;

    q.clear();

    if(!q.exec(deviceSetTableQuery)){
        qDebug()<<"ERROR EXEC_2:"
               <<deviceSetTableQuery;
        return deviceSetNameList;
    }

    while (q.next()) {
        QSqlRecord rec=q.record();
        deviceSetNameList<< rec.value("ds_name").toString();
    }
    return deviceSetNameList;
}

void DataBase::removeTaskById(const QVariant &id)
{
    QString strDeleteQuery=
            "DELETE FROM "+TABLE_SYNC_TASK+" WHERE st_id="+id.toString();
    QSqlQuery q;
    if(!q.exec(strDeleteQuery)){
        qDebug() << "error delete by id from "
                 << TABLE_SYNC_TASK
                 << q.lastError().text();
        return;
    }

    strDeleteQuery=
            "DELETE FROM "+TABLE_SYNC_TASK_AND_DEVICE_SET+" WHERE st_id="+id.toString();

    if(!q.exec(strDeleteQuery))
        qDebug() << "error delete by id from "
                 << TABLE_SYNC_TASK_AND_DEVICE_SET
                 << q.lastError().text();
}

void DataBase::removeTaskByName(const QVariant &taskName)
{
    QString strDeleteQuery=
            "DELETE FROM "+TABLE_SYNC_TASK+" WHERE st_name='"+taskName.toString()+"'";
    QSqlQuery q;
    if(!q.exec(strDeleteQuery)){
        qDebug() << "error delete by name from " << TABLE_SYNC_TASK<< q.lastError().text();
    }
}

QVariantList DataBase::getTaskIdByDeviceNameList(const QVariantList &deviceSetNameList)
{
    QVariantList list;
    QString deviceSetTableQuery="SELECT ds_id FROM "+TABLE_DEVICE_SET+" WHERE ds_name IN(";
    for(const QVariant &deviceName:deviceSetNameList) {
        qDebug()<<"DS ID"<<deviceName;
        deviceSetTableQuery+="'"+deviceName.toString()+"'";
        deviceSetTableQuery+="," ;
    }
    deviceSetTableQuery.remove(deviceSetTableQuery.length()-1,1);
    deviceSetTableQuery+=")" ;
    QSqlQuery q;

    if(!q.exec(deviceSetTableQuery)){
        qDebug()<<"ERROR SELECT"<<TABLE_DEVICE_SET<<deviceSetTableQuery;
        return list;
    }
    qDebug()<<deviceSetTableQuery;

    QString queryDistinct="SELECT DISTINCT st_id FROM "+TABLE_SYNC_TASK_AND_DEVICE_SET+" WHERE ds_id IN(";
    while (q.next()) {
        int dsId= q.value(0).toInt();
        qDebug()<<"DS ID"<<dsId;
        queryDistinct+=QString::number(dsId);
        queryDistinct+="," ;
    }
//    qDebug()<<"BEFORE"<<queryDistinct;
    queryDistinct.remove(queryDistinct.length()-1,1);
    queryDistinct+=")" ;
    q.clear();
    if(!q.exec(queryDistinct)){
        qDebug()<<"ERROR SELECT"<<TABLE_SYNC_TASK_AND_DEVICE_SET<<queryDistinct;
        return list;
    }
//    qDebug()<<"AFTER"<<queryDistinct;
    while(q.next()){
        int stId=q.value(0).toInt();
        qDebug()<<"St_id"<<stId;
        list<<stId;
    }
    return list;
}
