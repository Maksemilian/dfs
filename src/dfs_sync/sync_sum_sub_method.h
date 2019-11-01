#ifndef SYNC_SUM_SUB_METHOD_H
#define SYNC_SUM_SUB_METHOD_H

#include "sync_base.h"
#include <QObject>
#include <math.h>

#include "ipp.h"
#include "ippvm.h"

class SumSubMethod
{
    //1250 Гц - частота гетеродина создающего sin/cos коэффициенты
    const double HETERODYNE_FREQUENCY=1250.0;
    const double DOUBLE_PI=2*M_PI;
    const double DELTA_ANGLE_IN_RADIAN;
public:
    SumSubMethod(quint32 sampleRate,quint32 blockSize):
        DELTA_ANGLE_IN_RADIAN((360*(HETERODYNE_FREQUENCY/sampleRate))*(M_PI/180)),
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

    const VectorIpp32fc& calc(const Ipp32fc *dst1,const Ipp32fc *dst2,quint32 blockSize)
    {
        //1 ***** Создаем коэффициенты
        for(quint32 i=0;i<blockSize;i++,currentAngleRad+=DELTA_ANGLE_IN_RADIAN){
            re[i]= static_cast<Ipp32f>(cos(currentAngleRad));
            im[i]= static_cast<Ipp32f>(sin(currentAngleRad));
            if(currentAngleRad>=DOUBLE_PI){
                re[i]=1;
                im[i]=0;
                currentAngleRad=0.0;
            }
        }

        ippsRealToCplx_32f(re.data(),im.data(),dstSumDiv.data(),
                           static_cast<int>(blockSize));

        // qDebug()<<"******************Src Coef:";

        //2 ***** Фильтруем 2 канала
        //WARNING ФИЛЬТРАЦИЯ НЕ ИСПОЛЬЗУЕТСЯ

        //2 ***** Комплексная сумма двух сигналов
        ippsAdd_32fc(dst1,dst2,complexSum.data(),static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER SUM:";

        //3 ***** Комплексная разность двух сигналов
        ippsSub_32fc(dst1,dst2,complexSub.data(),static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER SUB:";

        //4 ***** Умножить массив коэффициентов на сумму
        ippsMul_32fc(dstSumDiv.data(),complexSum.data(),mulSumOnCoef.data(),
                     static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER MUL SUM:";

        //5 ***** Умножить массив коэффициентов на разность
        ippsMul_32fc(dstSumDiv.data(),complexSub.data(),mulSubOnCoef.data(),
                     static_cast<int>(blockSize));
        //qDebug()<<"******************DATA AFTER MUL SUB:";

        //        for(quint32 i=0;i<blockSize;i++){
        //            dstSumDiv[i].re=mulSumOnCoef[i].re;
        //            dstSumDiv[i].im=mulSubOnCoef[i].im;
        //        }
        memcpy(dstSumDiv.data(),mulSubOnCoef.data(),sizeof (Ipp32fc)*blockSize);
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
};
#endif // SYNC_SUM_SUB_METHOD_H
