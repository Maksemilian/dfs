#ifndef SUMSUBMETHOD_H
#define SUMSUBMETHOD_H

#include "sync_global.h"

#include <math.h>

#include <QQueue>
#include <QVector>

#include "ipp.h"
#include "ippvm.h"

using namespace std;


class SumSubMethod
{
    //1250 Гц - частота гетеродина создающего sin/cos коэффициенты
    const double HETERODYNE_FREQUENCY = 1250.0;
    const double DOUBLE_PI = 2 * IPP_PI;
    const double DELTA_ANGLE_IN_RADIAN;
  public:
    SumSubMethod(quint32 sampleRate, quint32 blockSize):
        DELTA_ANGLE_IN_RADIAN((360 * (HETERODYNE_FREQUENCY / sampleRate)) * (IPP_PI / 180)),
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


    const ShPtrIpp32fcBuffer& buffer()
    {
        return sumDivBuffer;
    }
    void apply(const proto::receiver::Packet& pct1,
               const proto::receiver::Packet& pct2,
               quint32 blockSize)
    {
        //******* SUM-SUB METHOD **************
        const Ipp32fc* dst1 = reinterpret_cast<const Ipp32fc*>
                              (pct1.sample().data());

        const Ipp32fc* dst2 = reinterpret_cast<const Ipp32fc*>
                              (pct2.sample().data());

        sumDivBuffer->push(calc(dst1, dst2, blockSize));
    }

    VectorIpp32fc applyT(const proto::receiver::Packet& pct1,
                         const proto::receiver::Packet& pct2,
                         quint32 blockSize)
    {
        //******* SUM-SUB METHOD **************
        const Ipp32fc* dst1 = reinterpret_cast<const Ipp32fc*>
                              (pct1.sample().data());

        const Ipp32fc* dst2 = reinterpret_cast<const Ipp32fc*>
                              (pct2.sample().data());

        return calc(dst1, dst2, blockSize);
    }
  private:
    const VectorIpp32fc& calc(const Ipp32fc* dst1, const Ipp32fc* dst2, quint32 blockSize)
    {
        //1 ***** Создаем коэффициенты
        for(quint32 i = 0; i < blockSize; i++, currentAngleRad += DELTA_ANGLE_IN_RADIAN)
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
#endif // SUMSUBMETHOD_H