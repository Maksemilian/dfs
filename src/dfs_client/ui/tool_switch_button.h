#ifndef SWITCH_BUTTON_H
#define SWITCH_BUTTON_H


#include <QPushButton>

class QStateMachine;
class QState;
class MainWindow;

class SwitchButton : public QPushButton/*,public IToolBarWidget*/
{
    Q_OBJECT
public:
    SwitchButton(/*MainWindow*mainWindow,*/QWidget *parent=nullptr);

//    SwitchButton(QString nameOn,QString nameOff,bool clicked,
//                 /*MainWindow*mainWindow,*/QWidget *parent=nullptr);

    SwitchButton(const QString &nameOn,const QString &nameOff,bool clicked,
                 QWidget *parent=nullptr);

    void setCurrentState(bool clicked);
    bool currentState();
private:
    void setButtonStates(QString nameOn,QString nameOff,bool clicked);
    void changedState();
signals:
    void switched(bool active);
    void stateChanged(bool state);
    void changed();
protected:
    void mousePressEvent(QMouseEvent *e);
private:
    static const char *STATE_BUTTON;

    QStateMachine *machine;
    QState *stateFirst;
    QState *stateSecond;
};


#endif // SWITCH_BUTTON_H
