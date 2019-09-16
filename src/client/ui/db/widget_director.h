#ifndef WIDGET_DIRECTOR_H
#define WIDGET_DIRECTOR_H

#include <QWidget>
#include <QTimer>
#include <QFileInfo>
#include <QMap>
#include <QVariant>
#include <QAbstractItemModel>

class ListStationPanelWidget;
class SyncTaskView;
class WidgetDirector;
class ReceiverStationPanel;
class TaskRequest;

class QTextBrowser;
class QListWidget;
class QPushButton;

class PanelId:public QObjectUserData
{
public:
    PanelId(qintptr id);

    ~PanelId()=default;
    inline qintptr getId(){return id;}
private:
    qintptr id;
};

class TaskProgressBarUpdater:public QObject
{
    Q_OBJECT
public:
    TaskProgressBarUpdater(const QStringList &taskFileNames,
                           QAbstractItemModel*model,int row);
    void start(qint64 taskSize);
    void stop();
    Q_SIGNAL void finished();
    Q_SIGNAL void taskLoaded(qint64 bytes);
private:
    QString getSizeHumanReadable(qint64 bytes);
    void onTimer();

    QFileInfoList fileInfos;
//    SyncTaskView *taskView;
    QAbstractItemModel*model;
    int row;

    QTimer timer;
    qint64 taskSize;
    QMap<QString,QVariant>progress;
};

class WidgetDirector:public QWidget
{
    Q_OBJECT
//    static const int SERVER_PORT=7000;
    static const int SERVER_PORT=8000;
    static const int USER_DATA_PANEL_ID=1;
public:
    WidgetDirector(QWidget*parent=nullptr);
    ~WidgetDirector()=default;

    void onAddSelectedTask(const QString &taskName,const QDateTime &dateTime);

    Q_SLOT void enable();
    Q_SLOT void disable();

    Q_SLOT void createStationData(qintptr id,const QString &addr,const QString &port,
                                  const QStringList &deviceSetNameList);
    Q_SLOT void removeStationData(qintptr id);

    Q_SIGNAL void taskAdded();

private:
    Q_SLOT void onLoadSelectedTask();
    Q_SLOT void onRemoveSelectedTask();


    bool checkAvailableTask(const QStringList &deviceSetNames);
//    QStringList getDeviceSetNameList();

//    Q_SLOT void loadTaskSelected();
//    QList<TaskRequest> createTaskRequest(const QString &taskName,
//                                         const QDate &taskDate,
//                                         const QStringList &deviceSetNames);
private:
    ListStationPanelWidget*listStationPanelWidget;
    SyncTaskView *taskView;
    QTextBrowser *textBrowser;

    QPushButton*pbLoadTask;
    QPushButton*pbRemoveTask;

    TaskProgressBarUpdater *taskUpdater;
};


#endif // WIDGET_DIRECTOR_H
