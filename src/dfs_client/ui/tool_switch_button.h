#ifndef SWITCH_BUTTON_H
#define SWITCH_BUTTON_H


#include <QPushButton>

class QStateMachine;
class QState;

/*! \addtogroup client
 */
///@{

/*!
 * \brief Виждет кнопки переключения.Храни два состояния
 * - on и off и в зависимости от этих состояний на кнопке
 * отображается разный текст. Внутренняя реализация сделана
 * на основе класса конечного автомата QStateMachine.
 */
class SwitchButton : public QPushButton
{
    Q_OBJECT
  public:
    SwitchButton(QWidget* parent = nullptr);

    SwitchButton(const QString& nameOn, const QString& nameOff, bool clicked,
                 QWidget* parent = nullptr);

    void setCurrentState(bool clicked);
    bool currentState();
  private:
    void setButtonStates(QString nameOn, QString nameOff, bool clicked);
    void changedState();
  signals:
    void switched(bool active);
    void stateChanged(bool state);
    void changed();
  protected:
    void mousePressEvent(QMouseEvent* e);
  private:
    static const char* STATE_BUTTON;

    QStateMachine* machine;
    QState* stateFirst;
    QState* stateSecond;
};

///@}

#endif // SWITCH_BUTTON_H
