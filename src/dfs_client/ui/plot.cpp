#include "plot.h"

#include <QtConcurrent/QtConcurrent>
#include <QMutex>

/*! \addtogroup client
 */
///@{

const QColor Plot::greenColor = QColor(0, 255, 0);
const QColor Plot::blueColor = QColor(0, 0, 255);

const QColor Plot::graphColors[Plot::GRAPH_COLORS_COUNT] =
{
    QColor(255, 0, 0),
    QColor(0, 0, 255),
    QColor(0, 255, 0),
    QColor(0, 255, 255)
};

ChannelDataWidget::ChannelDataWidget(QCustomPlot* plot, QCPLayer* layer)
    : QCPLayoutGrid()
{
    for(int i = 0, p = 0; i < ET_SIZE; i++)
    {
        QCPTextElement* labelTextElement = new QCPTextElement(plot);
        labelTextElement->setLayer(layer);
        labelTextElevents << labelTextElement;

        switch (i)
        {
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

        QCPTextElement* textValueElement = new QCPTextElement(plot);
        textValueElement->setLayer(layer);
        textValuetElevents << textValueElement;

        addElement(0, p++, labelTextElement);
        addElement(0, p++, textValueElement);
        //        qDebug()<<0<<i<<"and"<<0<<i+1;

    }
}

void ChannelDataWidget::setName(const QString& name)
{
    textValuetElevents[ET_NAME]->setText(name);
}

void ChannelDataWidget::setData(quint32 blockNumber, double ddcSampleCounter,
                                quint64 adcPeriodCounter)
{
    textValuetElevents[ET_BLOCK_NUMBER]->setText(QString::number(blockNumber));
    textValuetElevents[ET_DDC_SAMPLE_COUNTER]->setText(QString::number(ddcSampleCounter));
    textValuetElevents[ET_ADC_PERIOD_COUNTER]->setText(QString::number(adcPeriodCounter));
}

SignalGraph::SignalGraph(QCPAxisRect* elipsAxis, int blockSize)
{
    QCPAxis*  xAxis = elipsAxis->axis(QCPAxis::atBottom);
    QCPAxis* yAxis = elipsAxis->axis(QCPAxis::atLeft);

    QCPAxis* xAxis2 = elipsAxis->axis(QCPAxis::atTop);
    QCPAxis* yAxis2 = elipsAxis->axis(QCPAxis::atRight);


    QCPGraph* graph1 = elipsAxis->parentPlot()->addGraph(xAxis, yAxis);
    graph1->setScatterStyle(QCPScatterStyle::ssCross);
    graph1->setName("I_" + QString::number(0));
//    graph1->setPen(graphColors[0]);

    _iqGraph.first = graph1;

    QCPGraph* graph2 = elipsAxis->parentPlot()->addGraph(xAxis, yAxis);
    graph2->setScatterStyle(QCPScatterStyle::ssCross);
    graph2->setName("Q_" + QString::number(0));
//    graph2->setPen(graphColors[1]);

    _iqGraph.second = graph2;

    QCPRange xRange, yRange;
    xRange.lower = RANGE_SIGNAL_LOWER_BORDER_X_AXIS;
    xRange.upper = RANGE_SIGNAL_UPPER_BORDER_X_AXIS;

    yRange.lower = RANGE_SIGNAL_LOWER_BORDER_Y_AXIS;
    yRange.upper = RANGE_SIGNAL_UPPER_BORDER_Y_AXIS;

    xAxis->setRange(xRange);
    yAxis->setRange(yRange);

    xAxis2->setVisible(true);
    yAxis2->setVisible(true);

    xAxis2->setRange(xRange);
    yAxis2->setRange(yRange);

    for(int i = 0; i < blockSize; i++)
    {
        key.append(i);
    }
}

void SignalGraph::update(const proto::receiver::Packet& pct1,
                         const proto::receiver::Packet& pct2)
{
    int blockSize = key.size();


//    ChannelData data = {0, 0, 8192};
//    int INDEX = 0;
//    quint32 COUNT_SIGNAL_COMPONENT = 2;
//    std::unique_ptr<float[]>dataPairSingal(new float[data.blockSize*
//                                           COUNT_SIGNAL_COMPONENT]);

//    ChannelDataT channelData1(pct1.block_number(),
//                              pct1.ddc_sample_counter(),
//                              pct1.adc_period_counter());

//    ChannelDataT channelData2(pct2.block_number(),
//                              pct2.ddc_sample_counter(),
//                              pct2.adc_period_counter());
    // qDebug()<<"BA_3";
    const float* data1 = pct1.sample().data();
    const float* data2 = pct2.sample().data();
    //В dataPairSingal заносятся I компоненты с канала 1 и 2
    QVector<double>componentsI1(static_cast<int>(key.size()));
    QVector<double>componentsI2(static_cast<int>(key.size()));

    for(int i = 0; i < blockSize; i++)
    {
        componentsI1[i] = static_cast<double>(data1[i * 2]);
        componentsI2[i] = static_cast<double>(data2[i * 2]);
    }

//    mutex.lock();
    _iqGraph.first->setData(key, componentsI1);
    _iqGraph.second->setData(key, componentsI2);
//    mutex.unlock();

//    QApplication::postEvent(this, new QEvent(QEvent::User));
}

//************** ELIPSE ***********

class ElipseLine : public QCPItemLine
{
    static const QColor blueColor;
    static const QString DEGREE_UNICODE_SYMBOL;

  public:
    ElipseLine(const QPointF& posBegin, const QPointF& posEnd,
               const QString& leftText, const QString& rightText,
               QCustomPlot* parent)
        : QCPItemLine(parent)
    {
        setPen(blueColor);

        start->setCoords(posBegin.x(), posBegin.y());
        end->setCoords(posEnd.x(), posEnd.y());

        const int shift = 70000;

        QPointF textPosBegin;
        QPointF textPosEnd;
        if(posBegin.x() == 0.0 && posBegin.y() > 0) //1 Y
        {
            textPosBegin.setX(posBegin.x() - shift);
            textPosBegin.setY(posBegin.y() - shift);

            textPosEnd.setX(posEnd.x() + shift);
            textPosEnd.setY(posEnd.y() + shift);
        }
        else if(posBegin.x() < 0 && posBegin.y() > 0) //2
        {
            textPosBegin.setX(posBegin.x() + shift);
            textPosBegin.setY(posBegin.y() - 2 * shift);

            textPosEnd.setX(posEnd.x() - shift);
            textPosEnd.setY(posEnd.y() + 2 * shift);
        }
        else if(posBegin.x() < 0 && posBegin.y() == 0.0) //3 X
        {
            textPosBegin.setX(posBegin.x() + shift);
            textPosBegin.setY(posBegin.y() - shift);

            textPosEnd.setX(posEnd.x() - shift);
            textPosEnd.setY(posEnd.y() + shift);
        }
        else if(posBegin.x() < 0 && posBegin.y() < 0) //4
        {
            textPosBegin.setX(posBegin.x() + 2 * shift);
            textPosBegin.setY(posBegin.y() + shift);

            textPosEnd.setX(posEnd.x() - 2 * shift);
            textPosEnd.setY(posEnd.y() - shift);
        }

        QCPItemText* beginItemText = new QCPItemText(parent);
        beginItemText->setText(leftText + DEGREE_UNICODE_SYMBOL);
        beginItemText->position->setCoords( textPosBegin.x(), textPosBegin.y());
        beginItemText->setFont(QFont("Helvetica", 9));

        QCPItemText* endItemText = new QCPItemText(parent);
        endItemText->setText(rightText + DEGREE_UNICODE_SYMBOL);
        endItemText->position->setCoords(textPosEnd.x(), textPosEnd.y());
        endItemText->setFont(QFont("Helvetica", 10));

    }
};

const QColor ElipseLine::blueColor = QColor(0, 0, 255);
const QString ElipseLine::DEGREE_UNICODE_SYMBOL = "\u00B0";

ElipseCurve::ElipseCurve(QCPAxisRect* elipsAxis)
{
    QCPAxis*  xAxis = elipsAxis->axis(QCPAxis::atBottom);
    QCPAxis* yAxis = elipsAxis->axis(QCPAxis::atLeft);

    QCPAxis* xAxis2 = elipsAxis->axis(QCPAxis::atTop);
    QCPAxis* yAxis2 = elipsAxis->axis(QCPAxis::atRight);
//    sumSubMethod.reset(new SumSubMethod(sampleRate, blockSize));
    _curve = new QCPCurve(xAxis, yAxis);
    _curve->setName("Elipse");
//    _curve->setPen(greenColor);

    // X INIT
    QCPRange xRange(LOWER_BORDER_X_AXIS, UPPER_BORDER_X_AXIS);
    xAxis->setRange(xRange);
    xAxis->setTicks(false);

    xAxis2->setRange(xRange);
    xAxis2->setVisible(true);
    xAxis2->setTicks(false);

    // Y INIT
    QCPRange yRange(LOWER_BORDER_Y_AXIS, UPPER_BORDER_Y_AXIS);
    yAxis->setRange(yRange);
    yAxis->setTicks(false);

    yAxis2->setRange(yRange);
    yAxis2->setVisible(true);
    yAxis2->setTicks(false);

    QPointF posBegin1(LOWER_BORDER_X_AXIS, 0);
    QPointF posEnd1(UPPER_BORDER_X_AXIS, 0);

    QPointF posBegin2(0, -LOWER_BORDER_Y_AXIS);
    QPointF posEnd2(0, LOWER_BORDER_Y_AXIS);

    QPointF posBegin3(LOWER_BORDER_X_AXIS, LOWER_BORDER_Y_AXIS);
    QPointF posEnd3(UPPER_BORDER_X_AXIS, UPPER_BORDER_Y_AXIS);

    QPointF posBegin4(LOWER_BORDER_X_AXIS, UPPER_BORDER_Y_AXIS);
    QPointF posEnd4(UPPER_BORDER_X_AXIS, LOWER_BORDER_X_AXIS);

    lines << new ElipseLine(posBegin1, posEnd1, "180", "0", elipsAxis->parentPlot());
    lines << new ElipseLine(posBegin2, posEnd2, "90", "270", elipsAxis->parentPlot());
    lines << new ElipseLine(posBegin3, posEnd3, "235", "45", elipsAxis->parentPlot());
    lines << new ElipseLine(posBegin4, posEnd4, "135", "315", elipsAxis->parentPlot());
}

void ElipseCurve::resetData(const ChannelData& data)
{
    sumSubMethod.reset(new SumSubMethod(data.sampleRate, data.blockSize));
}

void ElipseCurve::update(int index, const VectorIpp32fc& v)
{
    Q_UNUSED(index);
    QVector<double>componentsI(static_cast<int>(v.size()));//re cos
    QVector<double>componentsQ(static_cast<int>(v.size()));//im sin

    int i = 0;
    for(const auto& it : v )
    {
        componentsI[i] = static_cast<double>(it.im); //cos
        componentsQ[i] = static_cast<double>(it.re); //sin
        ++i;
    }

    //    qDebug()<<"ElipsPlot::setDDC1StreamE"<<"Cos:"<<componentsI[size/2]<<"Sin:"<<componentsQ[size/2];

    //    curve->setData(componentsI,componentsQ);
    _curve->setData(componentsQ, componentsI);
}

void ElipseCurve::update(const proto::receiver::Packet& pct1,
                         const proto::receiver::Packet& pct2)
{
    Q_ASSERT_X(sumSubMethod.get(), "ElipsPlot::apply", "null");

    update(0, sumSubMethod->apply(pct1, pct2));
}

//***************** PLOT ************

Plot::Plot():
    _channel1(std::make_unique<RingBuffer<proto::receiver::Packet>>(4)),
    _channel2(std::make_unique<RingBuffer<proto::receiver::Packet>>(4))
{
    plotLayout()->addElement(0, 1, new QCPAxisRect(this));

    legend->setVisible(true);
    legend->setFont(QFont("Helvetica", 9));

    plotLayout()->setColumnStretchFactor(0, 2);
    plotLayout()->setColumnStretchFactor(1, 3);

    QCPAxisRect* elipsAxis = qobject_cast<QCPAxisRect*>(plotLayout()->element(0, 0));
    elipsePlot = new ElipseCurve(elipsAxis);

    QCPAxisRect* graphAxis = qobject_cast<QCPAxisRect*>(plotLayout()->element(0, 1));
    signalGraph = new SignalGraph(graphAxis);

}

void Plot::update(const proto::receiver::Packet& pct1, const proto::receiver::Packet& pct2)
{
    _channel1->push(pct1);
    _channel2->push(pct2);
}

void Plot::start(const ChannelData& data)
{
    quit = false;
    elipsePlot->resetData(data);
    fw.setFuture(QtConcurrent::run([this, data]()
    {
        qDebug() << "PLOT MONITORING RUN";

        bool isRead1 = false;
        bool isRead2 = false;

        proto::receiver::Packet pct1;
        proto::receiver::Packet pct2;

        while (!quit)
        {
            if(_channel1->pop(pct1))
            {
                isRead1 = true;
            }

            if(_channel2->pop(pct2))
            {
                isRead2 = true;
            }

            if(isRead1 && isRead2)
            {
                Q_ASSERT(data.blockSize == pct1.block_size()
                         && data.blockSize == pct2.block_size());

                setDataOnPlots(pct1, pct2);
                isRead1 = false;
                isRead2 = false;
            }
        }
        qDebug() << "PLOT MONITORING STOP";
    }));
}

void Plot::stop()
{
    quit = true;
    fw.waitForFinished();
}

void Plot::setDataOnPlots(const proto::receiver::Packet& pct1,
                          const proto::receiver::Packet& pct2)
{
    mutex.lock();
    signalGraph->update(pct1, pct2);
    elipsePlot->update(pct1, pct2);
    mutex.unlock();

    QApplication::postEvent(this, new QEvent(QEvent::User));
}
///@}
