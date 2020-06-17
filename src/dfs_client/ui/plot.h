#ifndef PLOT_H
#define PLOT_H

#include <qcustomplot.h>
#include <utility>

#include "radio_channel.h"
#include "observer_packet.h"
#include "plot_channel.h"

#include "ring_buffer.h"
#include "receiver.pb.h"

#include "ipp.h"
#include "ippvm.h"

using namespace std;

using IQGraph = std::pair<QCPGraph*, QCPGraph*>;
class ElipseLine;
class SignalGraph
{
    int RANGE_SIGNAL_LOWER_BORDER_X_AXIS = 0;
    int RANGE_SIGNAL_UPPER_BORDER_X_AXIS = 350;

    int RANGE_SIGNAL_LOWER_BORDER_Y_AXIS = -200000;
    int RANGE_SIGNAL_UPPER_BORDER_Y_AXIS = 200000;
  public:
    SignalGraph(QCPAxisRect* axisRect, int blockSize = 8192);
    void update(const proto::receiver::Packet& pct1,
                const proto::receiver::Packet& pct2);

  private:
    void updateSignalData(int index,
                          const ChannelDataT& channelData1,
                          const ChannelDataT& channelData2);

    void updateSignalComponent(int index,
                               const float* data,
                               quint32 dataSize);
    IQGraph _iqGraph;
    QVector<double>key;
    QList<ChannelDataWidget*>channelDataList;
    QMutex mutex;
};

class ElipseCurve
{
    int LOWER_BORDER_X_AXIS = -750000;
    int UPPER_BORDER_X_AXIS = 750000;
    int LOWER_BORDER_Y_AXIS = -750000;
    int UPPER_BORDER_Y_AXIS = 750000;

    using RingIpp32fcBuffer = RingBuffer<std::vector<Ipp32fc>>;
    using ShPtrIpp32fcBuffer = std::shared_ptr<RingIpp32fcBuffer>;

    using VectorIpp32fc = std::vector<Ipp32fc>;
    using VectorIpp32f = std::vector<Ipp32f>;

    /*!
     * \brief Класс вычисляет данные для посторение элипса
     * сумма-разностным методом
     */
    class SumSubMethod
    {
        //1250 Гц - частота гетеродина создающего sin/cos коэффициенты
        const double HETERODYNE_FREQUENCY = 1250.0;
        const double DOUBLE_PI = 2 * IPP_PI;
        const double DELTA_ANGLE_IN_RADIAN;
      public:
        SumSubMethod(quint32 sampleRate, quint32 blockSize):
            DELTA_ANGLE_IN_RADIAN((360 * (HETERODYNE_FREQUENCY / sampleRate)) *
                                  (IPP_PI / 180)),
            im(VectorIpp32f(blockSize)),
            re(VectorIpp32f(blockSize)),
            complexSum(VectorIpp32fc(blockSize)),
            complexSub(VectorIpp32fc(blockSize)),
            mulSumOnCoef(VectorIpp32fc(blockSize)),
            mulSubOnCoef(VectorIpp32fc(blockSize)),
            dstSumDiv(VectorIpp32fc(blockSize)),
            currentAngleRad(0.0)
        {

        }

        VectorIpp32fc apply(const proto::receiver::Packet& pct1,
                            const proto::receiver::Packet& pct2)
        {
            quint32 blockSize = pct1.block_size();
//            Q_ASSERT_X(pct1.block_size() == pct2.block_size(),
//                       "VectorIpp32fc apply", "blockSize badly");
            //******* SUM-SUB METHOD **************
            const Ipp32fc* dst1 = reinterpret_cast<const Ipp32fc*>
                                  (pct1.sample().data());

            const Ipp32fc* dst2 = reinterpret_cast<const Ipp32fc*>
                                  (pct2.sample().data());

//            return calc(dst1, dst2, blockSize);
            //1 ***** Создаем коэффициенты
            for(quint32 i = 0; i < blockSize; i++,
                    currentAngleRad += DELTA_ANGLE_IN_RADIAN)
            {
                re[i] = static_cast<Ipp32f>(cos(currentAngleRad));
                im[i] = static_cast<Ipp32f>(sin(currentAngleRad));
                if(currentAngleRad >= DOUBLE_PI)
                {
                    re[i] = 1;
                    im[i] = 0;
                    currentAngleRad = 0.0;
                }
            }

            ippsRealToCplx_32f(re.data(), im.data(), dstSumDiv.data(),
                               static_cast<int>(blockSize));

            // qDebug()<<"******************Src Coef:";

            //2 ***** Фильтруем 2 канала
            //WARNING ФИЛЬТРАЦИЯ НЕ ИСПОЛЬЗУЕТСЯ

            //2 ***** Комплексная сумма двух сигналов
            ippsAdd_32fc(dst1, dst2, complexSum.data(), static_cast<int>(blockSize));
            //qDebug()<<"******************DATA AFTER SUM:";

            //3 ***** Комплексная разность двух сигналов
            ippsSub_32fc(dst1, dst2, complexSub.data(), static_cast<int>(blockSize));
            //qDebug()<<"******************DATA AFTER SUB:";

            //4 ***** Умножить массив коэффициентов на сумму
            ippsMul_32fc(dstSumDiv.data(), complexSum.data(), mulSumOnCoef.data(),
                         static_cast<int>(blockSize));
            //qDebug()<<"******************DATA AFTER MUL SUM:";

            //5 ***** Умножить массив коэффициентов на разность
            ippsMul_32fc(dstSumDiv.data(), complexSub.data(), mulSubOnCoef.data(),
                         static_cast<int>(blockSize));
            //qDebug()<<"******************DATA AFTER MUL SUB:";

            //        for(quint32 i=0;i<blockSize;i++){
            //            dstSumDiv[i].re=mulSumOnCoef[i].re;
            //            dstSumDiv[i].im=mulSubOnCoef[i].im;
            //        }
            memcpy(dstSumDiv.data(), mulSubOnCoef.data(), sizeof (Ipp32fc)*blockSize);
            return  dstSumDiv;
        }

      private:
        VectorIpp32f im;
        VectorIpp32f re;
        VectorIpp32fc complexSum;
        VectorIpp32fc complexSub;
        VectorIpp32fc mulSumOnCoef;
        VectorIpp32fc mulSubOnCoef;

        VectorIpp32fc dstSumDiv;
        double currentAngleRad;
        ShPtrIpp32fcBuffer sumDivBuffer;
    };

  public:
    ElipseCurve(QCPAxisRect* axisRect);
    void update(const proto::receiver::Packet& pct1,
                const proto::receiver::Packet& pct2);
    void resetData(const ChannelData& data);
  private:
    void update(int index, const VectorIpp32fc& v);
  private:
    QCPCurve* _curve;
    QList<ElipseLine*>lines;
    std::unique_ptr<SumSubMethod> sumSubMethod;
};

class Plot: public QCustomPlot, public ObserverPacket
{
    enum GraphType
    {
        GT_GRAPH = 0,
        GT_CURVE = 1
    };
    static const QColor greenColor;
    static const QColor blueColor;
    static const QColor graphColors[];
    static const int GRAPH_COLORS_COUNT = 4;

  public:
    Plot();
    void start(const ChannelData& data);
    void stop();
    void update(const proto::receiver::Packet& pct1,
                const proto::receiver::Packet& pct2)override;
  private:
    void setDataOnPlots(const proto::receiver::Packet& pct1,
                        const proto::receiver::Packet& pct2);
  private:
    SignalGraph* signalGraph;
    ElipseCurve* elipsePlot;

    QMutex mutex;
    std::unique_ptr<RingBuffer<proto::receiver::Packet>> _channel1;
    std::unique_ptr<RingBuffer<proto::receiver::Packet>> _channel2;

    std::atomic_bool quit;
    QFutureWatcher<void> fw;
};

#endif // PLOT_H
