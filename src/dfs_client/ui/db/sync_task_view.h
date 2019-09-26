#ifndef SYNC_TASK_VIEW_H
#define SYNC_TASK_VIEW_H

#include <QStyledItemDelegate>
#include <QDateTime>

namespace Ui {
class SyncTaskView;
}

class QTableView;

class QSqlTableModel;
class QSqlRelationalTableModel;


class CheckBoxDelegate : public QStyledItemDelegate {
public:
    CheckBoxDelegate( QObject* parent = nullptr );
    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

class ProgressBarDelegate : public QStyledItemDelegate {
public:
    ProgressBarDelegate( QObject* parent = 0 );
    QString getSizeHumanReadable(qint64 bytes);
    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
};

struct TaskDataRow
{
    QString name;
    QDateTime dateTime;
    QStringList deviceSetNameList;
};

class SyncTaskView : public QWidget
{
    Q_OBJECT
public:
    static const QString DATE_TIME_FORMAT;
    static const QString DATE_FORMAT_DD_MM_YYYY;
    static const QString DATE_FORMAT_YYYY_DD_MM;

    explicit SyncTaskView(const QString &tableName,QWidget *parent = nullptr);
    ~SyncTaskView()override;

    int selectedRowCount();
    int getTaskIdInRow(int row);
    QString getTaskNameInRow(int row);
    QDateTime getTaskDateTimeInRow(int row);

    int getSelectedRow();
    QList<int>selectedRows();
    int selectedRow();

    void removeProgressBarDelegateForRow(int row);
    void setProgressBarDelegateForRow(int row);
    Q_SIGNAL void rowSelected(int row,bool selectedStatus);

    Q_SLOT   void onTaskLoadUpdate(qint64 bytesReaded,qint64 bytesSize);
    Q_SLOT void update();
    Q_SLOT QAbstractItemModel* getModel();

//    void hideColumn(int column);
//    void showColumn(int column);
private:
    Q_SLOT void onCheckBoxFilterSelected(int selectState);
    Q_SLOT void onTableViewSelected(const QModelIndex &index);
    Q_SLOT void onTableViewFiltred(const QDate &date);
private:
    Ui::SyncTaskView *ui;
    QSqlTableModel *taskTableModel;
};

#endif // SYNC_TASK_VIEW_H
