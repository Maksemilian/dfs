#include "tool_switch_button.h"

#include <QStateMachine>
#include <QDebug>
#include <QMouseEvent>

const char* SwitchButton::STATE_BUTTON = "state_button";


SwitchButton::SwitchButton(/*MainWindow*mainWindow,*/QWidget* parent):
    QPushButton(parent)/*,IToolBarWidget(mainWindow)*/ {}

//SwitchButton::SwitchButton(QString nameOn, QString nameOff,bool clicked,
//                           /*MainWindow*mainWindow,*/QWidget *parent)
//    :QPushButton(parent)/*,IToolBarWidget(mainWindow)*/
//{
//    setButtonStates(nameOn,nameOff,clicked);
//}

SwitchButton::SwitchButton(const QString& nameOn, const QString& nameOff,
                           bool clicked, QWidget* parent):
    QPushButton (parent)/*,IToolBarWidget (nullptr)*/
{
    setButtonStates(nameOn, nameOff, clicked);
}

void SwitchButton::setButtonStates(QString nameOn, QString nameOff, bool clicked)
{
    machine = new QStateMachine;
    stateFirst = new QState();///on
    stateFirst->setObjectName(nameOn);
    stateFirst->assignProperty(this, "text", nameOn);
    stateFirst->assignProperty(this, STATE_BUTTON, false);

    stateSecond = new QState();///off
    stateSecond->setObjectName(nameOff);
    stateSecond->assignProperty(this, "text", nameOff );
    stateSecond->assignProperty(this, STATE_BUTTON, true);

    stateFirst->addTransition(this, &SwitchButton::clicked, stateSecond);
    stateSecond->addTransition(this, &SwitchButton::clicked, stateFirst);

    machine->addState(stateFirst);
    machine->addState(stateSecond);

    if(!clicked)
    {
        machine->setInitialState(stateFirst);
    }
    else
    {
        machine->setInitialState(stateSecond);
    }

    machine->start();

    connect(this, &SwitchButton::clicked, this, &SwitchButton::changedState);
}

void SwitchButton::changedState()
{
    if(property(STATE_BUTTON).toBool())
    {
        setProperty(STATE_BUTTON, false);
    }
    else
    {
        setProperty(STATE_BUTTON, true);
    }
    emit stateChanged(property(STATE_BUTTON).toBool());
    changed();
    qDebug() << "Changed SWITCH BUTTON";
}

bool SwitchButton::currentState()
{
    return property(STATE_BUTTON).toBool();
}

void SwitchButton::setCurrentState(bool clicked)
{
    qDebug() << "Set Current:" << clicked;
    if(clicked)
    {
        machine->setInitialState(stateSecond);
    }
    else
    {
        machine->setInitialState(stateFirst);
    }
    setProperty(STATE_BUTTON, clicked);
}

void SwitchButton::mousePressEvent(QMouseEvent* e)
{
    Q_UNUSED(e);
    if(cursor().shape() != Qt::WaitCursor)
        click();
}
