#include "tree_devices.h"

#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QSettings>
#include <QFile>

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
        QTreeWidgetItem* pcItem = new QTreeWidgetItem(_twDevices, {stringList[0] + ":" + stringList[1]});
        _twDevices->addTopLevelItem(pcItem);
    });
    loadSettings();
}

TreeDevices::~TreeDevices()
{
    saveSettings();
}

void TreeDevices::saveSettings()
{
    QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
    QString prefix = "PC_";
    for (int i = 0; i < _twDevices->topLevelItemCount(); ++i)
    {
        settings.beginGroup(prefix + QString::number(i));
        QStringList ipAndPort = _twDevices->topLevelItem(i)->text(0).split(":");
        settings.setValue("ip", ipAndPort[0]);
        settings.setValue("port", ipAndPort[1]);
        settings.endGroup();
    }

}

void TreeDevices::loadSettings()
{
    QSettings s(SETTINGS_FILE_NAME, QSettings::IniFormat);
    QStringList groups = s.childGroups();
    for(const auto& group : groups)
    {
        s.beginGroup(group);

        QString ip = s.value("ip").toString();
        QString port = s.value("port").toString();

        QTreeWidgetItem* pcItem = new QTreeWidgetItem(_twDevices, {ip + ":" + port});
        _twDevices->addTopLevelItem(pcItem);

        s.endGroup();
    }
}
