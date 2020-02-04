#ifndef ELIPS_PLOT_H
#define ELIPS_PLOT_H

#include "sync_test.h"
#include "qcustomplot.h"
#include <QMutex>

class ElipseLine : public QCPItemLine
{
    static const QColor blueColor;
    static const QString DEGREE_UNICODE_SYMBOL;

  public:
    ElipseLine(const QPointF& posBegin, const QPointF& posEnd,
               const QString& leftText, const QString& rightText, QCustomPlot* parent);
};

class ElipsPlot: public QCustomPlot
{
    int LOWER_BORDER_X_AXIS = -750000;
    int UPPER_BORDER_X_AXIS = 750000;
    int LOWER_BORDER_Y_AXIS = -750000;
    int UPPER_BORDER_Y_AXIS = 750000;

    static const QPair<QPoint, QPoint> arrayPair[4];
    static const QColor greenColor;
    static const QColor blueColor;
  public:
    ElipsPlot( QWidget* parent = nullptr);
    void setSyncData(const SyncData& data);
    void apply(const proto::receiver::Packet& pctChannel1,
               const proto::receiver::Packet& pctChannel2);
    void update(int index, const float* sumDivData, quint32 dataSize);
    void update(int index, const  VectorIpp32fc& v);
  private:
    void customEvent(QEvent* event)override;
  private:
    QCPCurve* curve;
    QList<ElipseLine*>lines;
    QMutex mutex;
    SyncData data;
    std::unique_ptr<SumSubMethod> sumSubMethod;
};

#endif // ELIPS_PLOT_H
