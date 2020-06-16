#ifndef CHANNEL_H
#define CHANNEL_H

#include <QTcpSocket>
#include <memory>

/*!
 * \defgroup net Net
 * \image html class_net.jpg "Диаграмма классов модуля net"
 * \brief Модуль статической библиотеки содержащий классы для
 * для передачи данных по протоколу TCP между клиентским
 * приложением и серверным приложением.
 */
namespace net
{
///@{
    /*!
                 * \brief Абстрактный базовый класс предоставляющий
                 * основные методы для отправки и приема данных по сети.
                 * Каждый наследник класса Channel должен реализовывать метод
                 * internalMessageReceive для обработки принятых сообщений
                 */
    class Channel: public QObject
    {
        Q_OBJECT
      protected:
        enum  KeyExchangeState {HELLO = 0, KEY_EXCHANGE = 1, SESSION = 2, DONE = 3};
      public:
        enum  ChannelState { NOT_CONNECTED, CONNECTED, ESTABLISHED};
        Channel(QObject* parent = nullptr);
        Channel(qintptr handle, QObject* parent = nullptr);
        void writeToConnection(const QByteArray& commandData);
        inline ChannelState state()
        {
            return  _channelState ;
        }
        QHostAddress peerAddress();
        ~Channel();
        bool waitChannel(int msec);
        bool isOpen();
        bool isWritable();
        bool flush();
        void moveToThread(QThread* thread);

      signals:
        void messageReceived(const QByteArray& buffer);
        void finished();
      protected:
        /*!
             * \brief Чисто виртуальный метод , который должен быть переопределен
             * в производных классах для того чтобы читать входящие данные
             * \param buffer Массив байт принятых из сети
             */
        virtual void internalMessageReceive(const QByteArray& buffer) = 0;
        ///**************WRITE/READ****************
      public:
        qint64 readFromSocket(qint64 bytes);
        qint64 writeToSocket(qint64 bytes);

        qint64 readDataFromBuffer(char* data, qint64 size);
        qint64 writeDataToBuffer(const char* data, qint64 size);

        bool   canTransferMore();
        qint64 socketBytesAvailable();
      private:
        void onReadyRead();
        void onMessageReceive();

      protected:
        std::unique_ptr<QTcpSocket> _socket;
        //    QTcpSocket *_socket;
        KeyExchangeState keyExchangeState = KeyExchangeState::HELLO;
        ChannelState _channelState = ChannelState::NOT_CONNECTED;
      private:
        struct ReadContext
        {
            QByteArray buffer;
            bool paused = false;

            // To this buffer reads data from the network.
            //        QByteArray buffer;

            // If the flag is set to true, then the buffer size is read from the network, if false,
            // then no.
            bool buffer_size_received = false;

            // Size of |buffer|.
            int buffer_size = 0;

            // Number of bytes read into the |buffer|.
            int64_t bytes_transferred = 0;
        };

        ReadContext read_;
        QByteArray outgoingBuffer;
        qint64 answerSize = 0;
    };
///@}
}

#endif // CHANNEL_H
