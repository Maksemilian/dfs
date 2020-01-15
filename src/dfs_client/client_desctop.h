#ifndef CLIENT_PC_H
#define CLIENT_PC_H

#include "client.h"
#include "desctop.pb.h"

class DesctopClient: public Client
{
    Q_OBJECT
  public:
    DesctopClient();
    void sendCommand();
  signals:
    void desctopInfoReceived(const proto::desctop::DesctopInfo& desctopInfo);
  private:
    void onMessageReceived(const QByteArray& buffer) override;
};

#endif // CLIENT_PC_H
