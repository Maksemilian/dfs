#include "wrd_callback.h"
#include "wrd_callback_coh_g35.h"
#include "wrd_callback_g35.h"

std::unique_ptr<CohG35Callback>CallbackFactory:: cohG35CallbackInstance(const ShPtrRingPacketBuffer&ddc1Buffer)
{
    return std::make_unique<CohG35Callback>(ddc1Buffer,TimeReader::instance());
}

std::unique_ptr<G35Callback>CallbackFactory:: singleG35CallbackInstance(const ShPtrRingPacketBuffer&ddc1Buffer)
{
    return std::make_unique<G35Callback>(ddc1Buffer,TimeReader::instance());
}
