#ifndef ELIPS_PLOT_H
#define ELIPS_PLOT_H

#include "i_sygnal_update.h"

#include "qcustomplot.h"
#include <QMutex>

class ElipseLine :public QCPItemLine
{
    static const QColor blueColor;
    static const QString DEGREE_UNICODE_SYMBOL;

public:
    ElipseLine(const QPointF &posBegin,const QPointF &posEnd,
               const QString &leftText,const QString &rightText,QCustomPlot*parent);
};

class ElipsPlot:public QCustomPlot, public ISumDivSignalUpdate
{
    int LOWER_BORDER_X_AXIS =-750000;
    int UPPER_BORDER_X_AXIS =750000;
    int LOWER_BORDER_Y_AXIS =-750000;
    int UPPER_BORDER_Y_AXIS = 750000;

    static const QPair<QPoint,QPoint> arrayPair[4];
    static const QColor greenColor;
    static const QColor blueColor;
public:
    ElipsPlot(QWidget *parent=nullptr);
    void update(int index,const float*sumDivData,quint32 dataSize)override;
private:
    void customEvent(QEvent *event)override;
private:
    QCPCurve *curve;
    QList<ElipseLine*>lines;
    QMutex mutex;
};

#endif // ELIPS_PLOT_H
