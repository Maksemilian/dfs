#ifndef OBSERVER_PACKET_H
#define OBSERVER_PACKET_H

#include <QList>
#include "receiver.pb.h"

class ObserverPacket
{
  public:
    virtual ~ObserverPacket() = default;
    virtual void update(const proto::receiver::Packet& pct1,
                        const proto::receiver::Packet& pct2) = 0;
};

class Subject
{
  public:
    void subscribe(ObserverPacket* observer);
    void unSubscribe(ObserverPacket* oserver);
    void notify(const proto::receiver::Packet& pct1, const proto::receiver::Packet& pct2);
  private:
    QList<ObserverPacket*> _observers;
};
#endif // OBSERVER_PACKET_H
