#ifndef WRD_G35_CALLBACK_H
#define WRD_G35_CALLBACK_H

#include "ring_buffer.h"
#include "receiver.pb.h"
#include "trmbl_tsip_reader.h"

#include "G35DDCAPI.h"

using ShPtrRingPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class G35Callback:public IG35DDCDeviceCallback
{
public:
    G35Callback(const ShPtrRingPacketBuffer &buffer,const TimeReader&timeReader);
    VOID __stdcall G35DDC_IFCallback(IG35DDCDevice *Device,CONST SHORT *Buffer,UINT32 NumberOfSamples,WORD MaxADCAmplitude,UINT32 ADCSamplingRate)
    ;
    VOID __stdcall G35DDC_DDC1StreamCallback(IG35DDCDevice *Device,CONST VOID *Buffer,UINT32 NumberOfSamples,UINT32 BitsPerSample)
    ;
    BOOL __stdcall G35DDC_DDC1PlaybackStreamCallback(IG35DDCDevice *Device,VOID *Buffer,UINT32 NumberOfSamples,UINT32 BitsPerSample)
    ;
    VOID __stdcall G35DDC_DDC2StreamCallback(IG35DDCDevice *Device,UINT32 Channel,CONST FLOAT *Buffer,UINT32 NumberOfSamples)
    ;

    VOID __stdcall G35DDC_DDC2PreprocessedStreamCallback(IG35DDCDevice *Device,UINT32 Channel,CONST FLOAT *Buffer,
                                                         UINT32 NumberOfSamples,FLOAT SlevelPeak,FLOAT SlevelRMS)
    ;
    VOID __stdcall G35DDC_AudioStreamCallback(IG35DDCDevice *Device,UINT32 Channel,UINT32 Type,CONST FLOAT *Buffer,UINT32 NumberOfSamples)
    ;
    BOOL __stdcall G35DDC_AudioPlaybackStreamCallback(IG35DDCDevice *Device,UINT32 Channel,FLOAT *Buffer,UINT32 NumberOfSamples)
    ;
private:
    ShPtrRingPacketBuffer buffer;
    const TimeReader& timeReader;
};

#endif // WRD_G35_CALLBACK_H
