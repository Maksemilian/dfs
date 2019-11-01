#include "plot_channel.h"

const int ChannelPlot::GRAPH_COLORS_COUNT=4;

const QColor ChannelPlot::graphColors[ChannelPlot::GRAPH_COLORS_COUNT]={
    QColor(255,0,0),
    QColor(0,0,255),
    QColor(0,255,0),
    QColor(0,255,255)
};

ChannelData::ChannelData(QCustomPlot *plot,QCPLayer *layer)
    :QCPLayoutGrid()
{
    for(int i=0,p=0;i<ET_SIZE;i++){
        QCPTextElement *labelTextElement=new QCPTextElement(plot);
        labelTextElement->setLayer(layer);
        labelTextElevents<<labelTextElement;

        switch (i) {
        case ET_NAME:
            labelTextElement->setText("RN    :");
            break;
        case ET_BLOCK_NUMBER:
            labelTextElement->setText("BN    :");
            break;
        case ET_DDC_SAMPLE_COUNTER:
            labelTextElement->setText("DDC_SC:");
            break;
        case ET_ADC_PERIOD_COUNTER:
            labelTextElement->setText("ADC_PC:");
            break;
        }

        QCPTextElement *textValueElement=new QCPTextElement(plot);
        textValueElement->setLayer(layer);
        textValuetElevents<<textValueElement;

        addElement(0,p++,labelTextElement);
        addElement(0,p++,textValueElement);
        //        qDebug()<<0<<i<<"and"<<0<<i+1;

    }
}

void ChannelData::setName(const QString &name){
    textValuetElevents[ET_NAME]->setText(name);
}

void ChannelData::setData(quint32 blockNumber, double ddcSampleCounter,
                          quint64 adcPeriodCounter){
    textValuetElevents[ET_BLOCK_NUMBER]->setText(QString::number(blockNumber));
    textValuetElevents[ET_DDC_SAMPLE_COUNTER]->setText(QString::number(ddcSampleCounter));
    textValuetElevents[ET_ADC_PERIOD_COUNTER]->setText(QString::number(adcPeriodCounter));
}

ChannelPlot::ChannelPlot(int channelCount, quint32 blockSize, QWidget *parent)
    :QCustomPlot(parent)
{
    legend->setVisible(true);
    legend->setFont(QFont("Helvetica",9));
    for(int i=0;i<channelCount;i++){
        QCPGraph *graph=addGraph();
        graph->setName("I_"+QString::number(i+1));
        graph->setScatterStyle(QCPScatterStyle::ssCross);
        graph->setPen(graphColors[i]);
        ChannelData *channelData=new ChannelData(this,legend->layer());
        legend->addElement(i,1,channelData);
        channelDataList<<channelData;
    }

    for(int i=0;i<static_cast<int>(blockSize);i++){
        key.append(i);
    }

    componentType=CT_I;

    QCPRange xRange,yRange;
    xRange.lower=RANGE_SIGNAL_LOWER_BORDER_X_AXIS;
    xRange.upper=RANGE_SIGNAL_UPPER_BORDER_X_AXIS;

    yRange.lower=RANGE_SIGNAL_LOWER_BORDER_Y_AXIS;
    yRange.upper=RANGE_SIGNAL_UPPER_BORDER_Y_AXIS;

    xAxis->setRange(xRange);
    yAxis->setRange(yRange);

    xAxis2->setVisible(true);
    yAxis2->setVisible(true);

    xAxis2->setRange(xRange);
    yAxis2->setRange(yRange);

    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void ChannelPlot::setGraphName(int graphId, const QString &name){
    Q_ASSERT_X(graphId>=0&&graphId<this->graphCount(),"ChannelPlot::setGraphName();","index bount of range");
    channelDataList[graphId]->setName(name);
}

void ChannelPlot::updateSignalData(int index,
                                   const ChannelDataT &channelData1,
                                   const ChannelDataT &channelData2)
{
    Q_UNUSED(index);
    channelDataList[0]->setData(channelData1.blockNumber,
                                channelData1.ddcCounter,
                                channelData1.adcCounter);


    channelDataList[1]->setData(channelData2.blockNumber,
                                channelData2.ddcCounter,
                                channelData2.adcCounter);
}

void ChannelPlot::updateSignalComponent(int index,
                                        const float *data,
                                        quint32 dataSize)
{
//    Q_ASSERT_X(graphId>=0&&graphId<this->graphCount(),"ChannelPlot::setDDC1Stream","index bount of range");
    Q_UNUSED(index);
    QVector<double>componentsI1(static_cast<int>(dataSize));
    QVector<double>componentsI2(static_cast<int>(dataSize));

//    qDebug()<<"Data Size"<<dataSize;

    for(int i=0;i<static_cast<int>(dataSize);i++){
        componentsI1[i]=static_cast<double>(data[i*2]);
        componentsI2[i]=static_cast<double>(data[i*2+1]);
    }

    mutex.lock();
    graph(0)->setData(key,componentsI1);
    graph(1)->setData(key,componentsI2);
    mutex.unlock();

    QApplication::postEvent(this,new QEvent(QEvent::User));
}


void ChannelPlot::customEvent(QEvent *event){
    Q_UNUSED(event);
    mutex.lock();
    replot();
    mutex.unlock();
    //TODO DELETE EVENT
//    delete event;
}
