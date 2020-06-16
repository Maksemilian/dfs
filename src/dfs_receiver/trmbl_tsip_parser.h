#ifndef TSIPPARSER_H
#define TSIPPARSER_H

#include <QByteArray>
#include <QObject>

#define MSG_IN_COMPLETE  0
#define TSIP_DLE         1
#define TSIP_IN_PARTIAL  2

#define DLE              0x10 // TSIP packet start/end header
#define ETX              0x03 // TSIP data packet tail

#define MAX_TSIP_PKT_LEN 300  // max length of a TSIP packet

class QSerialPort;

template<typename T, int SIZE>
union AliasCast
{
    T value;
    quint8 byteArray[SIZE];
};

/*! \defgroup receiver Receiver
 * \image html class_receiver.jpg "Диаграмма классов модуля receiver"
 * \brief Модуль статической библиотеки работы
 *  с приемниками Winradio WRG35DDC и Trimble Thunderbolt
 */

///@{
/*!
 * \brief Класс для приниема пакетов спутниковых данных через com порт
 * по протоколу TSIP
 */
class TsipParser: public QObject
{
    Q_OBJECT
    struct PacketBase
    {
        int number;
        PacketBase(): number(0) {}
    };
  public:
    enum PacketName {PN_AB, PN_AC};

    struct PacketAB: public PacketBase
    {
        quint32 ulTimeOfWeek;
        quint16 usWeekNumber;
        PacketAB(): ulTimeOfWeek(0), usWeekNumber(0) {}
    };

    struct PacketAC: public PacketBase
    {
        float fltPPSQuality;
        PacketAC(): fltPPSQuality(0) {}
    };

    explicit TsipParser(QObject* parent = nullptr);

    void parsePkt   (quint8 ucPkt[], int nPktLen);
    void receivePacket(QSerialPort* port);

    PacketName getCurrentPacket()
    {
        return currentPacket;
    }
    PacketAB getPacketAB()
    {
        return packetAB;
    }
    PacketAC getPacketAC()
    {
        return packetAC;
    }
  private:
    void parse0x8F (quint8 ucData[], int nLen);
    void parse0x8FAB (quint8 ucData[], int nLen);
    void parse0x8FAC (quint8 ucData[], int nLen);

    quint16 getUShort (quint8* pucBuf);
    quint32 getULong (quint8* pucBuf);
    qint32  getLong (quint8* pucBuf);
    qint16  getShort (quint8* pucBuf);
    double  getDouble (quint8* pucBuf);
    float   getSingle (quint8* pucBuf);
  private:
    quint8 ucPkt[MAX_TSIP_PKT_LEN];

    int nPktLen;
    int pPktLen;
    int nParseState;

    PacketName currentPacket;
    //Packet 8FAB
    PacketAB packetAB;
    //Packet 8FAC
    PacketAC packetAC;
};

///@}
#endif // TSIPPARSER_H
