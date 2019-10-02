#include "i_toolbar.h"
#include "ui/main_window.h"

void IToolBarWidget::changed()
{
    if(_mainWindow)
    _mainWindow->widgetChanged(this);
}
