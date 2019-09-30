#ifndef WIDGETS_H
#define WIDGETS_H

#include "i_toolbar.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>

using UInt32Bandwth =quint32;
using Uint32SampleRate=quint32;

class ToolBarLineEdit:public QLineEdit,public IToolBarWidget
{
    Q_OBJECT
public:
    ToolBarLineEdit(MainWindow*mainWindow,QWidget*parent=nullptr);
};


class ToolBarComboBox:public QComboBox,public IToolBarWidget
{
    Q_OBJECT
public:
    ToolBarComboBox(MainWindow*mainWindow,QWidget*parent=nullptr);
};

class BandwithComboBox:public ToolBarComboBox
{
    static const int BANDWITH_SIZE;
    static const quint32 bandwithArray[];
public:
    BandwithComboBox(MainWindow*mainWindow,QWidget*parent=nullptr);
    static  QMap<UInt32Bandwth,Uint32SampleRate>bandwithAndSampleRateMap();
    inline quint32 getCurrentBandwith()
    {
        return bandwithArray[currentIndex()];
    }
};

class SampleRateComboBox:public ToolBarComboBox
{
    static const int SAMPLES_PER_BUFFER_SIZE;
    static const quint16 samplesPerBufferArray[];
public:
    SampleRateComboBox(MainWindow*mainWindow,QWidget*parent=nullptr);
};


class AttenuatorComboBox:public ToolBarComboBox
{
    static const int ATTENUATOR_SIZE;
    static const quint8 attenuatorArray[];
public:
    AttenuatorComboBox(MainWindow*mainWindow,QWidget*parent=nullptr);
};


#endif // WIDGETS_H
