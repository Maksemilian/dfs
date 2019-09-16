#include "i_toolbar.h"
#include "ui/main_window.h"

void IToolBarWidget::changed()
{
    _mainWindow->widgetChanged(this);
}
