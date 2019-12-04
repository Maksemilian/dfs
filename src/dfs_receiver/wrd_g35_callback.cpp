#include "wrd_g35_callback.h"

G35Callback::G35Callback(const ShPtrRingPacketBuffer &buffer, const TimeReader &timeReader)
    :buffer(buffer),timeReader(timeReader)
{

}
