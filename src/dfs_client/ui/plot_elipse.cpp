#include "plot_elipse.h"
#include <QPointF>


const QColor ElipsPlot::greenColor = QColor(0, 255, 0);
const QColor ElipsPlot::blueColor = QColor(0, 0, 255);

const QColor ElipseLine::blueColor = QColor(0, 0, 255);
const QString ElipseLine::DEGREE_UNICODE_SYMBOL = "\u00B0";
/*! \addtogroup client
 */
///@{
//СЛЕВА НА ПРАВО И СВЕРХУ ВНИЗ
ElipseLine::ElipseLine(const QPointF& posBegin, const QPointF& posEnd,
                       const QString& leftText, const QString& rightText, QCustomPlot* parent)
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

ElipsPlot::ElipsPlot( QWidget* parent)
    : QCustomPlot(parent),
      curve(new QCPCurve(this->xAxis, this->yAxis))
      // data(data),
//      sumSubMethod(data.sampleRate, data.blockSize)
{
//    setFixedWidth (200);
//    setFixedHeight(200);
    curve->setPen(greenColor);
//    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

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

    lines << new ElipseLine(posBegin1, posEnd1, "180", "0", this);
    lines << new ElipseLine(posBegin2, posEnd2, "90", "270", this);
    lines << new ElipseLine(posBegin3, posEnd3, "235", "45", this);
    lines << new ElipseLine(posBegin4, posEnd4, "135", "315", this);
}

void ElipsPlot::apply(const proto::receiver::Packet& pctChannel1,
                      const proto::receiver::Packet& pctChannel2)
{
    Q_ASSERT_X(sumSubMethod.get(), "ElipsPlot::apply", "null");

    update(0, sumSubMethod->applyT(pctChannel1, pctChannel2, data.blockSize));
}

void ElipsPlot::setSyncData(const ChannelData& data)
{
    this->data = data;
    sumSubMethod.reset(new SumSubMethod(data.sampleRate, data.blockSize));
}

void ElipsPlot::update(int index, const VectorIpp32fc& v)
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

    mutex.lock();
    //    curve->setData(componentsI,componentsQ);
    curve->setData(componentsQ, componentsI);

    mutex.unlock();

    QApplication::postEvent(this, new QEvent(QEvent::User));
}


void ElipsPlot::update(int index, const float* sumDivData, quint32 dataSize)
{
    Q_UNUSED(index);
    QVector<double>componentsI(static_cast<int>(dataSize));//re cos
    QVector<double>componentsQ(static_cast<int>(dataSize));//im sin


    for(int i = 0; i < static_cast<int>(dataSize); i++)
    {
        componentsI[i] = static_cast<double>(sumDivData[i * 2]); //cos
        componentsQ[i] = static_cast<double>(sumDivData[i * 2 + 1]); //sin
    }

    //    qDebug()<<"ElipsPlot::setDDC1StreamE"<<"Cos:"<<componentsI[size/2]<<"Sin:"<<componentsQ[size/2];

    mutex.lock();
    //    curve->setData(componentsI,componentsQ);
    curve->setData(componentsQ, componentsI);

    mutex.unlock();

    QApplication::postEvent(this, new QEvent(QEvent::User));
}

void ElipsPlot::customEvent(QEvent* event)
{
    Q_UNUSED(event);
    mutex.lock();
    replot();
    mutex.unlock();
}
///@}
