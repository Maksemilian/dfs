#ifndef CHANNEL_PLOT_H
#define CHANNEL_PLOT_H

#include "qcustomplot.h"

#include <QColor>

class ChannelData: public QCPLayoutGrid
{
    enum ElementType
    {
        ET_NAME = 0,
        ET_BLOCK_NUMBER = 1,
        ET_DDC_SAMPLE_COUNTER = 2,
        ET_ADC_PERIOD_COUNTER = 3,
        ET_SIZE = 4
    };
  public:
    ChannelData(QCustomPlot* plot, QCPLayer* layer);
    void setName(const QString& name);
    void setData(quint32 blockNumber, double ddcSampleCounter, quint64 adcPeriodCounter);
  private:
    QList<QCPTextElement*>labelTextElevents;
    QList<QCPTextElement*>textValuetElevents;
};

struct ChannelDataT
{
    ChannelDataT(unsigned int blockNumber,    double ddcCounter,    unsigned long long adcCounter):
        blockNumber(blockNumber), ddcCounter(ddcCounter), adcCounter(adcCounter)
    {

    }

    unsigned int blockNumber;
    double ddcCounter;
    unsigned long long adcCounter;
};

class ChannelPlot : public QCustomPlot
//        public SyncSignalPairChannelEventListener

{
    int RANGE_SIGNAL_LOWER_BORDER_X_AXIS = 0;
    int RANGE_SIGNAL_UPPER_BORDER_X_AXIS = 350;

    int RANGE_SIGNAL_LOWER_BORDER_Y_AXIS = -200000;
    int RANGE_SIGNAL_UPPER_BORDER_Y_AXIS = 200000;

    static const int GRAPH_COLORS_COUNT;

    static const QColor graphColors[];

  public:
    enum ComponentType
    {
        CT_I,
        CT_Q,
        CT_I_Q
    };
    ChannelPlot(int channelCount, quint32 blockSize, QWidget* parent = nullptr);

    void updateSignalData(int index,
                          const ChannelDataT& channelData1,
                          const ChannelDataT& channelData2);
    void updateSignalComponent(int signalIndex,
                               const float* data,
                               quint32 dataSize) ;

    void setGraphName(int id, const QString& name);
  private:
    void customEvent(QEvent* event)override;
  private:
    QList<ChannelData*>channelDataList;
    QVector<double>key;
    ComponentType componentType;
    QMutex mutex;
    QButtonGroup* buttonGroup;
};

#endif // CHANNEL_PLOT_H
