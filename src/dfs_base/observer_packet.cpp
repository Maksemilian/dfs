#include "observer_packet.h"

void Subject::subscribe(ObserverPacket* observer)
{
    _observers << observer;
}

void Subject::unSubscribe(ObserverPacket* observer)
{
    _observers.removeOne(observer);
}

void Subject::notify(const proto::receiver::Packet& pct1,
                     const proto::receiver::Packet& pct2)
{
    for (ObserverPacket* ob : _observers)
    {
        ob->update(pct1, pct2);
    }
}
