#include "rttemporalfilter.h"

class RTTemporalFilterPrivate {
    RTTemporalFilter *q;
    int m_num_channels;
    float *m_data;
    int m_filter_size;
    int m_data_index;
};

RTTemporalFilter::RTTemporalFilter()
{
    d->q=this;
    d->m_num_channels=0;
    d->m_data=0;
    d->m_filter_size=3;
    d->m_data_index=0;
}

RTTemporalFilter::~RTTemporalFilter()
{
    if (d->m_data) free(d->m_data);
}

void RTTemporalFilter::setNumChannels(int num_channels)
{
    d->m_num_channels=num_channels;
    d->m_data=(float *)malloc(sizeof(float)*d->m_num_channels*d->m_filter_size);
    for (int i=0; i<d->m_num_channels*d->m_filter_size; i++) {
        d->m_data[i]=0;
    }
}

void RTTemporalFilter::addData(float *data)
{
    for (int i=0; i<d->m_num_channels; i++) {
        d->m_data[i+d->m_data_index*d->m_num_channels]=data[i];
    }
    d->m_data_index++;
    if (d->m_data_index>=d->m_num_channels) d->m_data_index=0;
}

