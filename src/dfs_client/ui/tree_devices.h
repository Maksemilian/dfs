#ifndef TREE_DEVICES_H
#define TREE_DEVICES_H

#include <QWidget>

class QTreeWidget;
class QPushButton;

class TreeDevices : public QWidget
{
    static const QString SETTINGS_FILE_NAME;
  public:
    TreeDevices(QWidget* parent = nullptr);
    ~TreeDevices();
    void connectToAllComputer();
  private:
    void saveSettings();
    void loadSettings();
  private:
    QTreeWidget* _twDevices;
    QPushButton* _pbAddDevice;
    QPushButton* _pbConnectAll;
};

#endif // TREE_DEVICES_H
