#ifndef CHANNEL_HOST_H
#define CHANNEL_HOST_H

#include "channel.h"
#include "key_exchange.pb.h"

namespace net
{
    class ChannelHost: public Channel
    {
        Q_OBJECT
      public:
        ChannelHost(QObject* parent = nullptr);
        ChannelHost(qintptr handle, QObject* parent = nullptr);
        inline SessionType sessionType()
        {
            return _sessionType;
        }
      signals:
        void keyExchangedFinished();
      private:
        void internalMessageReceive(const QByteArray& buffer)override;
        void readClientHello(const QByteArray& buffer);
        void readClientKeyExchange(const QByteArray& buffer);
        void readClientSession(const QByteArray& buffer);

      private:
        quint32 _userKey;
        SessionType _sessionType = SessionType::SESSION_UNKNOWN;
    };

}
#endif // CHANNEL_HOST_H
