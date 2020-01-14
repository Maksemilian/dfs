#include "tree_devices.h"
#include "tree_pc_item.h"

#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QSettings>
#include <QFile>
#include <QApplication>

const QString TreeDevices::SETTINGS_FILE_NAME = "tree_devices.ini";

TreeDevices::TreeDevices(QWidget* parent):
    QWidget (parent),
    _twDevices(new QTreeWidget(this)),
    _pbAddDevice(new QPushButton("Add PC", this))
{
    setLayout(new QVBoxLayout);
    layout()->addWidget(_twDevices);
    layout()->addWidget(_pbAddDevice);

    _twDevices->setColumnCount(1);
    _twDevices->headerItem()->setHidden(true);
    connect(_pbAddDevice, &QPushButton::clicked,
            [this]
    {
        QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("User name:"), QLineEdit::Normal);
        QStringList stringList = text.split(":");
        _twDevices->addTopLevelItem(
            new TreePcWidgetItem(stringList[0], stringList[1], _twDevices));
    });
    _twDevices->setAutoFillBackground(true);
    loadSettings();
}

TreeDevices::~TreeDevices()
{
    saveSettings();
}

void TreeDevices::connectToAllComputer()
{

}

void TreeDevices::saveSettings()
{
    QSettings settings(QApplication::applicationDirPath() + "/" + SETTINGS_FILE_NAME, QSettings::IniFormat);
    QString prefix = "PC_";
    for (int i = 0; i < _twDevices->topLevelItemCount(); ++i)
    {
        if(TreePcWidgetItem* item =
                    dynamic_cast<TreePcWidgetItem*>(_twDevices->topLevelItem(i)))

        {
            settings.beginGroup(prefix + QString::number(i));
            settings.setValue("ip", item->getIpAddress());
            settings.setValue("port", item->getPort());
            settings.endGroup();
        }
    }

}

void TreeDevices::loadSettings()
{
    QSettings s(QApplication::applicationDirPath() + "/" + SETTINGS_FILE_NAME, QSettings::IniFormat);
    QStringList groups = s.childGroups();
    for(const auto& group : groups)
    {
        s.beginGroup(group);

        QString ip = s.value("ip").toString();
        QString port = s.value("port").toString();

        _twDevices->addTopLevelItem(new TreePcWidgetItem(ip, port, _twDevices));

        s.endGroup();
    }
}
