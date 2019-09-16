#ifndef INTERFACE_TOOLBAR_H
#define INTERFACE_TOOLBAR_H
class MainWindow;

class IToolBarWidget
{
public:
    virtual ~IToolBarWidget()=default;
    IToolBarWidget(MainWindow*mainWindow)
        :_mainWindow(mainWindow)
    {}
    virtual void changed();
private:
    MainWindow *_mainWindow;
};

#endif // INTERFACE_TOOLBAR_H
