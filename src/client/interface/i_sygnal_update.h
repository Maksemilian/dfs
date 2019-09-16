#ifndef INTERFACE_SYNC_H
#define INTERFACE_SYNC_H

struct ChannelDataT {
    ChannelDataT(unsigned int blockNumber,    double ddcCounter,    unsigned long long adcCounter):
        blockNumber(blockNumber),ddcCounter(ddcCounter),adcCounter(adcCounter)
    {

    }

    unsigned int blockNumber;
    double ddcCounter;
    unsigned long long adcCounter;
};

class ISyncSignalUpdate
{
public:
    virtual ~ISyncSignalUpdate()=default;

    virtual void updateSignalData(int index,
                                  const ChannelDataT &channelData1,
                                  const ChannelDataT &channelData2)=0;

    virtual void updateSignalComponent(int signalIndex,
                                       const float *data,
                                       unsigned int dataSize)=0;
};

class ISumDivSignalUpdate
{
public:
    virtual ~ISumDivSignalUpdate()=default;
    virtual void update(int signalIndex,const float *sumSubMethodData, unsigned int dataSize)=0;
};

#endif // INTERFACE_SYNC_H
