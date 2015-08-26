#ifndef RTTEMPORALFILTER_H
#define RTTEMPORALFILTER_H

class RTTemporalFilterPrivate;
class RTTemporalFilter
{
public:
    friend RTTemporalFilterPrivate;
    RTTemporalFilter();
    virtual ~RTTemporalFilter();
    void setNumChannels(int num_channels);
    void addData(float *data);

private:
   RTTemporalFilterPrivate *d;
};

#endif // RTTEMPORALFILTER_H
