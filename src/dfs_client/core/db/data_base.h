#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <QSqlDatabase>
#include <QVariantList>

class DataBase
{
    DataBase()=default;
public:
    static const QString DATA_BASE_NAME;

    static const QString TABLE_SYNC_TASK;
    static const QString TABLE_DEVICE_SET;
    static const QString TABLE_SYNC_TASK_AND_DEVICE_SET;

    static const QString DATE_FORMAT;

public:
    static DataBase &instance() {
        static DataBase database;
        return database;
    }

    void connect();
    void insertToTaskTable(const QVariantList &data);
    void insertToDeviceSetTable(const QVariantList &data);
    void insertToSyncTaskAndDeviceSetTable(const QVariantList &syncTaskData,
                                           const QVariantList &deviceSetData);
    void updateTaskSizeToTaskTableById(const QVariant &id,const QVariant &progress);
    void updateTaskStatusToTaskTableById(const QVariant &id,const QVariant &status);

    void removeTaskById(const QVariant &id);
    void removeTaskByName(const QVariant &taskName);
    bool isTaskExist(const QVariant &taskName,const QVariant &taskDate);
    void selectByDate(const QVariant &taskDate);

    QStringList getDeviceSetNamesBySyncTaskId(const QVariant &suncTaskId);
    QVariantList getTaskIdByDeviceNameList(const QVariantList &deviceSetNameList);

private:
    bool open();
    void close();
private:
    QSqlDatabase db;
};

#endif // DATA_BASE_H
