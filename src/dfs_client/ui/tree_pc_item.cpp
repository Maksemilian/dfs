#include "tree_pc_item.h"
#include "receiver.pb.h"

#include <QTextEdit>
#include <QGridLayout>
#include <QLabel>

PcWidget::PcWidget(const QString& ipAddress, const QString& port)
    : _teIp(new QTextEdit(ipAddress)),
      _tePort(new QTextEdit(port))
{
    QGridLayout* gl = new QGridLayout;
    gl->addWidget(new QLabel("ip:"), 0, 0);
    gl->addWidget(_teIp, 0, 1);
    gl->addWidget(new QLabel("port:"), 1, 0);
    gl->addWidget(_tePort, 1, 1);
}


void PcWidget::onConnected()
{

}

//****************************

TreePcWidgetItem::TreePcWidgetItem(const QString& ipAddress, const QString& port,
                                   QTreeWidget* parent)
    : QTreeWidgetItem (parent),
      _pcWidget(new PcWidget(ipAddress, port)),
      _desctopClient(std::make_unique<DesctopClient>())
{
    setText(0, ipAddress + " : " + port);

    this->setExpanded(true);

//    connect(_desctopClient, &DesctopClient::desctopInfoReceived,
//            this, &TreePcWidgetItem::onInfoUpdate);

}

void TreePcWidgetItem::connectToComputer()
{
    //_desctopClient->connectToHost(getIpAddress(), getPort().toUShort());
}

void TreePcWidgetItem::onInfoUpdate(const proto::desctop::DesctopInfo& desctopInfo)
{
    /*
    QList<proto::receiver::DeviceSetInfo>list = getTestDeviceSetList();
    int dsCount = 0;
    for (const auto& ds : list)
    {
        QTreeWidgetItem* dsItem = new QTreeWidgetItem(this, {"DS_" + QString::number(dsCount)});
        this->addChild(dsItem);
        dsItem->setExpanded(true);
        for(int deviceIndex = 0; deviceIndex < ds.device_info_size(); ++deviceIndex)
        {
            QTreeWidgetItem* devItem = new QTreeWidgetItem(dsItem, {ds.device_info(deviceIndex).serial_number().data()});
            dsItem->addChild(devItem);
        }
        ++dsCount;
    }
    */
    int deviceSetInfoSize = desctopInfo.device_set_info_size();

    for (int i = 0; i < deviceSetInfoSize; ++i)
    {
        QTreeWidgetItem* dsItem = new QTreeWidgetItem(this, {"DS_" + QString::number(i)});
        this->addChild(dsItem);
        dsItem->setExpanded(true);
        const auto& ds = desctopInfo.device_set_info(i);
        for(int deviceIndex = 0; deviceIndex < ds.device_info_size(); ++deviceIndex)
        {
            QTreeWidgetItem* devItem = new QTreeWidgetItem(dsItem, {ds.device_info(deviceIndex).serial_number().data()});
            dsItem->addChild(devItem);
        }
    }
}

QList<proto::receiver::DeviceSetInfo> TreePcWidgetItem::getTestDeviceSetList()
{
    QList<proto::receiver::DeviceSetInfo>list;
    for (int i = 0; i < 3; i++)
    {
        proto::receiver::DeviceSetInfo ds;

        proto::receiver::DeviceInfo* d1 = ds.add_device_info();
        d1->set_serial_number(QString::number(rand() % 10000 + 100000).toStdString());

        proto::receiver::DeviceInfo* d2 = ds.add_device_info();
        d2->set_serial_number(QString::number(rand() % 10000 + 100000).toStdString());

        list.append(ds);
    }
    return  list;
}

QString TreePcWidgetItem::getIpAddress()
{
    return _pcWidget->_teIp->toPlainText();
}

QString TreePcWidgetItem::getPort()
{
    return _pcWidget->_tePort->toPlainText();
}
