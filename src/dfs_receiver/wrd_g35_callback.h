#ifndef WRD_G35_CALLBACK_H
#define WRD_G35_CALLBACK_H

#include "G35DDCAPI.h"
#include "ring_buffer.h"
#include "receiver.pb.h"
#include "trmbl_tsip_reader.h"

using ShPtrRingPacketBuffer = std::shared_ptr<RingBuffer<proto::receiver::Packet>>;

class G35Callback:public IG35DDCDeviceCallback
{
    G35Callback(const ShPtrRingPacketBuffer &buffer,const TimeReader&timeReader);
    VOID __stdcall G35DDC_IFCallback(IG35DDCDevice *Device,CONST SHORT *Buffer,UINT32 NumberOfSamples,WORD MaxADCAmplitude,UINT32 ADCSamplingRate)
    {
        Q_UNUSED(Device);
        Q_UNUSED(Buffer);
        Q_UNUSED(NumberOfSamples);
        Q_UNUSED(MaxADCAmplitude);
        Q_UNUSED(ADCSamplingRate);
    }
    VOID __stdcall G35DDC_DDC1StreamCallback(IG35DDCDevice *Device,CONST VOID *Buffer,UINT32 NumberOfSamples,UINT32 BitsPerSample)
    {
        qDebug()<<"DDC_Stream_Callback";
        
    }
    BOOL __stdcall G35DDC_DDC1PlaybackStreamCallback(IG35DDCDevice *Device,VOID *Buffer,UINT32 NumberOfSamples,UINT32 BitsPerSample)
    {
        Q_UNUSED(Device);
        Q_UNUSED(Buffer);
        Q_UNUSED(NumberOfSamples);
        Q_UNUSED(BitsPerSample);
        return true;
    }
    VOID __stdcall G35DDC_DDC2StreamCallback(IG35DDCDevice *Device,UINT32 Channel,CONST FLOAT *Buffer,UINT32 NumberOfSamples)
    {
        Q_UNUSED(Device);
        Q_UNUSED(Buffer);
        Q_UNUSED(NumberOfSamples);
        Q_UNUSED(Channel);
    }

    VOID __stdcall G35DDC_DDC2PreprocessedStreamCallback(IG35DDCDevice *Device,UINT32 Channel,CONST FLOAT *Buffer,
                                                         UINT32 NumberOfSamples,FLOAT SlevelPeak,FLOAT SlevelRMS)
    {
        Q_UNUSED(Device);
        Q_UNUSED(Buffer);
        Q_UNUSED(NumberOfSamples);
        Q_UNUSED(Channel);
        Q_UNUSED(SlevelPeak);
        Q_UNUSED(SlevelRMS);

    }
    VOID __stdcall G35DDC_AudioStreamCallback(IG35DDCDevice *Device,UINT32 Channel,UINT32 Type,CONST FLOAT *Buffer,UINT32 NumberOfSamples)
    {
        Q_UNUSED(Device);
        Q_UNUSED(Buffer);
        Q_UNUSED(NumberOfSamples);
        Q_UNUSED(Channel);

    }
    BOOL __stdcall G35DDC_AudioPlaybackStreamCallback(IG35DDCDevice *Device,UINT32 Channel,FLOAT *Buffer,UINT32 NumberOfSamples)
    {
        Q_UNUSED(Device);
        Q_UNUSED(Buffer);
        Q_UNUSED(NumberOfSamples);
        Q_UNUSED(Channel);
        return true;
    }
private:
    ShPtrRingPacketBuffer buffer;
    const TimeReader& timeReader;
};

#endif // WRD_G35_CALLBACK_H
