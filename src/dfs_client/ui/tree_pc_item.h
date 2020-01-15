#ifndef TREE_PC_ITEM_H
#define TREE_PC_ITEM_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "receiver.pb.h"
#include "client_desctop.h"

class QTextEdit;
class TreePcWidgetItem;

class PcWidget: public QWidget
{
    friend TreePcWidgetItem;
  public:
    PcWidget(const QString& ipAddress, const QString& port);
  private slots:
    void onConnected();
  private:
    QTextEdit* _teIp;
    QTextEdit* _tePort;
};

class TreePcWidgetItem : public QTreeWidgetItem
{
  public:
    TreePcWidgetItem(const QString& ipAddress, const QString& port,
                     QTreeWidget* parent = nullptr);
    QString getIpAddress();
    QString getPort();
    void connectToComputer();
  private:
    void onInfoUpdate(const proto::desctop::DesctopInfo& desctopInfo);
    QList<proto::receiver::DeviceSetInfo>getTestDeviceSetList();
  private:
    PcWidget* _pcWidget;
    std::unique_ptr<DesctopClient> _desctopClient;

};

#endif // TREE_PC_ITEM_H
