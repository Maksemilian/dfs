#include <QDebug>
#include <QSerialPort>

#include "tsip_parser.h"

TsipParser::TsipParser(QObject *parent ):QObject(parent), nPktLen(0),nParseState(MSG_IN_COMPLETE){}

quint32 TsipParser::getULong (quint8 *pucBuf)
{
    AliasCast<quint32,sizeof(quint32)> castUInt;
    castUInt.byteArray[0] = pucBuf[3];
    castUInt.byteArray[1] = pucBuf[2];
    castUInt.byteArray[2] = pucBuf[1];
    castUInt.byteArray[3] = pucBuf[0];
    return castUInt.value;
}

qint32 TsipParser::getLong (quint8 *pucBuf)
{
    AliasCast<qint32,sizeof(qint32)> castInt;
    castInt.byteArray[0] = pucBuf[3];
    castInt.byteArray[1] = pucBuf[2];
    castInt.byteArray[2] = pucBuf[1];
    castInt.byteArray[3] = pucBuf[0];
    return castInt.value;
}

quint16 TsipParser::getUShort (quint8 *pucBuf)
{
    AliasCast<quint16,sizeof(quint16)> castUShort;
    castUShort.byteArray[0] = pucBuf[1];
    castUShort.byteArray[1] = pucBuf[0];
    return castUShort.value;
}

qint16 TsipParser::getShort (quint8 *pucBuf)
{
    AliasCast<qint16,sizeof(qint16)> castShort;
    castShort.byteArray[0] = pucBuf[1];
    castShort.byteArray[1] = pucBuf[0];
    return castShort.value;
}

double TsipParser::getDouble (quint8* pucBuf)
{
    AliasCast<double,sizeof(double)> castDouble;
    castDouble.byteArray[0] = pucBuf[7];
    castDouble.byteArray[1] = pucBuf[6];
    castDouble.byteArray[2] = pucBuf[5];
    castDouble.byteArray[3] = pucBuf[4];
    castDouble.byteArray[4] = pucBuf[3];
    castDouble.byteArray[5] = pucBuf[2];
    castDouble.byteArray[6] = pucBuf[1];
    castDouble.byteArray[7] = pucBuf[0];
    return castDouble.value;
}

float TsipParser::getSingle (quint8 *pucBuf)
{
    AliasCast<float,sizeof(float)> castFloat;
    castFloat.byteArray[0] = pucBuf[3];
    castFloat.byteArray[1] = pucBuf[2];
    castFloat.byteArray[2] = pucBuf[1];
    castFloat.byteArray[3] = pucBuf[0];
    return castFloat.value;
}

///-------------------RECEIVE PACKET-------------------------

void TsipParser::receivePacket(QSerialPort *port){
    QByteArray data;
    while(true)
    {
        if(port->bytesAvailable()||port->waitForReadyRead())
        {
            data=port->read(1);
            ///parser.receivePacket(data);
            bool ok=0;
            quint8 ucByte=data.toHex().toInt(&ok,16);
            switch (nParseState)
            {
            case MSG_IN_COMPLETE:
                if (ucByte == DLE)
                {
                    nParseState      = TSIP_DLE;
                    nPktLen          = 0;
                    ucPkt[nPktLen++] = ucByte;
                }
                break;
            case TSIP_DLE:
                if (ucByte == ETX)
                {
                    if (nPktLen > 1)
                    {
                        ucPkt[nPktLen++] = DLE;
                        ucPkt[nPktLen++] = ETX;
                        pPktLen = nPktLen;
                        nParseState=MSG_IN_COMPLETE;
                        parsePkt(ucPkt,pPktLen);
                        return;
                    }
                    else
                    {
                        ///nParseState=MSG_IN_COMPLETE;
                    }
                }
                else
                {
                    nParseState = TSIP_IN_PARTIAL;
                    ucPkt[nPktLen++] = ucByte;
                }
                break;
            case TSIP_IN_PARTIAL:
                if (ucByte == DLE)
                {
                    nParseState = TSIP_DLE;
                }
                else
                {
                    ucPkt[nPktLen++] = ucByte;
                }
                break;
            default:
                nParseState = MSG_IN_COMPLETE;
                break;
            }
            if (nPktLen >= MAX_TSIP_PKT_LEN)
            {
                nParseState = MSG_IN_COMPLETE;
                nPktLen = 0;
            }
        }
    }
}

void TsipParser::parsePkt (quint8 ucPkt[], int nPktLen)
{
    switch (ucPkt[1])
    {
    case 0x8F: parse0x8F (&ucPkt[2], nPktLen-4); break;
    default:                                     break;
    }
}

void TsipParser::parse0x8F (quint8 ucData[], int nLen)
{
    switch (ucData[0])
    {
    case 0x20:  break;
    case 0xAB:

        parse0x8FAB (ucData, nLen);
        currentPacket=PN_AB;
        break;
    case 0xAC:
        parse0x8FAC (ucData, nLen);
        currentPacket=PN_AC;
        break;
    default:  break;
    }
}

//----------------------8FAB

void TsipParser::parse0x8FAB (quint8 ucData[], int nLen)
{
    if (nLen != 17)
    {
        return;
    }
    packetAB.number++;
    packetAB.ulTimeOfWeek=getULong (&ucData[1]);
    packetAB.usWeekNumber=getUShort (&ucData[5]);
    qDebug()<<"************Parse0x8FAB:"<<packetAB. ulTimeOfWeek<<"UWN:"<<packetAB.usWeekNumber;
}

//-------------------------8FAC--------------------

void TsipParser::parse0x8FAC (quint8 ucData[], int nLen)
{
    if (nLen != 68)
    {
        return;
    }
    packetAC.number++;
    packetAC.fltPPSQuality=getSingle(&ucData[16]);
    ///qDebug()<<"++++++++++Parse0x8FAC"<<fltPPSQuality;
}

///********************DEL**********************
