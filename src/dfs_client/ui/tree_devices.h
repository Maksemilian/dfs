#ifndef TREE_DEVICES_H
#define TREE_DEVICES_H

#include <QWidget>

class QTreeWidget;
class QPushButton;
class QTreeWidgetItem;

class TreeDevices : public QWidget
{
    static const QString SETTINGS_FILE_NAME;
    static const int MAX_COUNT_SELECTED_DEVICE_SET = 3;

  public:
    TreeDevices(QWidget* parent = nullptr);
    ~TreeDevices();
    void createDeivceWidgets();
  private:
    void check();
    void saveSettings();
    void loadSettings();
  private:
    QTreeWidget* _twDevices;
    QPushButton* _pbAddDesctopItem;
    QPushButton* _pbCreateSet;
};

#endif // TREE_DEVICES_H
