#ifndef RTTEMPORALFILTER_H
#define RTTEMPORALFILTER_H

class RTTemporalFilterPrivate;
class RTTemporalFilter
{
public:
    friend RTTemporalFilterPrivate;
    RTTemporalFilter();
    virtual ~RTTemporalFilter();
    void initialize(int filter_size,float *filter_weights,int num_channels);
    void addData(float *data);
    void getFilteredData(float *filtered_data,int timepoint);
private:
   RTTemporalFilterPrivate *d;
};

#endif // RTTEMPORALFILTER_H
