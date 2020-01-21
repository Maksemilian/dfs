#include "tree_devices.h"
#include "tree_pc_item.h"
#include "client_ds_ui_list.h"
#include "client_ds_ui.h"

#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QSettings>
#include <QFile>
#include <QApplication>
#include <QRadioButton>
#include <QButtonGroup>
#include <QMessageBox>

const QString TreeDevices::SETTINGS_FILE_NAME = "tree_devices.ini";


TreeDevices::TreeDevices(QWidget* parent):
    QWidget (parent),
    _twDevices(new QTreeWidget(this)),
    _pbAddDesctopItem(new QPushButton("Add PC", this)),
    _pbCreateSet(new QPushButton("Create Set"))
{
    setLayout(new QVBoxLayout);
    layout()->addWidget(_twDevices);
    layout()->addWidget(_pbAddDesctopItem);
    layout()->addWidget(_pbCreateSet);

    _twDevices->setColumnCount(2);
    _twDevices->headerItem()->setHidden(true);
    _pbCreateSet->setEnabled(false);
    connect(_pbCreateSet, &QPushButton::clicked,
            this, &TreeDevices::createDeivceWidgets);

    connect(_pbAddDesctopItem, &QPushButton::clicked,
            [this]
    {
        QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                             tr("User name:"), QLineEdit::Normal);
        QStringList stringList = text.split(":");
        TreePcWidgetItem* item =  new TreePcWidgetItem(stringList[0], stringList[1], _twDevices);

        _twDevices->addTopLevelItem(item);
    });
    _twDevices->setAutoFillBackground(true);
    _twDevices->setSelectionMode(QAbstractItemView::MultiSelection);
    _twDevices->setSelectionBehavior(QAbstractItemView::SelectItems);

    std::shared_ptr<QButtonGroup> buttonGroup =
        std::make_shared<QButtonGroup>();
    connect(buttonGroup.get(), QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
            [buttonGroup, this](QAbstractButton * checkedButton)
    {
//        qDebug() << "SelItemSize:" << _twDevices->selectedItems().size();
        for(auto button : buttonGroup->buttons())
        {
            qDebug() << button << checkedButton;
            if(button != checkedButton)
            {
//                qDebug() << "Button" << button->isChecked();
                button->setChecked(false);
            }
        }
        check();
    });
    connect(_twDevices, &QTreeWidget::itemClicked,
            [this, buttonGroup](QTreeWidgetItem * item, int column)
    {
        if(column == 0)
        {
            if((item->data(column, Qt::UserRole) ==
                    TreePcWidgetItem::DT_DESCTOP ||
                    item->data(column, Qt::UserRole) ==
                    TreePcWidgetItem::DT_RECEIVER)
                    && item->isSelected())
            {
                item->setSelected(false);
                return ;
            }

            if(item->isSelected())
            {
                QRadioButton* radioBuffon = new QRadioButton(this);
                connect(radioBuffon, &QRadioButton::toggled,
                        [item](bool state)
                {
                    qDebug() << "toggle" << state;
                    item->setData(0, Qt::UserRole + 1, state);
                });
                buttonGroup->addButton(radioBuffon);
                _twDevices->setItemWidget(item, 1, radioBuffon);
            }
            else
            {
                _twDevices->removeItemWidget(item, 1);
            }
            check();
        }
    }
           );
    loadSettings();
}

TreeDevices::~TreeDevices()
{
    saveSettings();
}

void TreeDevices::check()
{
    auto selItems = _twDevices->selectedItems();
    _pbCreateSet->setEnabled(false);
    if(selItems.size() == MAX_COUNT_SELECTED_DEVICE_SET)
    {
        for (auto item : selItems)
        {
            if(item->data(0, Qt::UserRole + 1).toBool())
            {
                _pbCreateSet->setEnabled(true);
                return;
            }
            else
            {
                qDebug() << "User Role:" << item->data(0, Qt::UserRole + 1).toBool();
            }
        }
    }
    else if(selItems.size() > MAX_COUNT_SELECTED_DEVICE_SET)
    {
//        _pbCreateSet->setEnabled(false);
//        QMessageBox::information(this, "Warning", "selected > 3 Devices");
//        selItems.last()->setSelected(false);
    }
}

void TreeDevices::createDeivceWidgets()
{
    QList<QTreeWidgetItem*> items = _twDevices->selectedItems();

    for(auto item : items)
    {
        QTreeWidgetItem* parent = item->parent();
        QString text = item->parent()->text(0);
        QStringList stringList = text.split(":");
        QString address = stringList[0];
        QString port = stringList[1];
        int index = parent->indexOfChild(item);
        bool flag = item->data(0, Qt::UserRole + 1).toBool();
        qDebug() << address << "DS_" << index << flag;

//        DeviceWidget *ds=new DeviceWidget()
    }
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
