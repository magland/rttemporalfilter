#include "rttemporalfilter.h"
#include <malloc.h>
#include <QList>

class RTTemporalFilterPrivate {
public:
    RTTemporalFilter *q;
    int m_num_channels;
    float *m_data;
    int m_filter_size;
    float *m_filter_weights;
    int m_data_index;
    QList<float> m_filtered_data;
    float *m_tmp;
};

RTTemporalFilter::RTTemporalFilter()
{
    d->q=this;
    d->m_num_channels=0;
    d->m_data=0;
    d->m_filter_size=0;
    d->m_filter_weights=0;
    d->m_data_index=0;
    d->m_index=0;
    d->m_tmp=0;
}

RTTemporalFilter::~RTTemporalFilter()
{
    if (d->m_data) free(d->m_data);
    if (d->m_tmp) free(d->m_tmp);
    if (d->m_filter_weights) free(d->m_filter_weights);
}

void RTTemporalFilter::initialize(int filter_size, float *filter_weights, int num_channels)
{
    d->m_filter_size=filter_size;
    if (d->m_filter_weights) free(d->m_filter_weights);
    d->m_filter_weights=(float *)malloc(sizeof(float)*d->m_filter_size);
    for (int ii=0; ii<d->m_filter_size; ii++) {
        d->m_filter_weights[ii]=filter_weights[ii];
    }
    d->m_num_channels=num_channels;
    if (d->m_num_channels) free(d->m_num_channels);
    d->m_data=(float *)malloc(sizeof(float)*d->m_num_channels*d->m_filter_size);
    for (int i=0; i<d->m_num_channels*d->m_filter_size; i++) {
        d->m_data[i]=0;
    }
    if (d->m_tmp) free(d->m_tmp);
    d->m_tmp=(float *)malloc(sizeof(float)*d->m_num_channels);
}

void RTTemporalFilter::addData(float *data)
{
    for (int i=0; i<d->m_num_channels; i++) {
        d->m_data[i+d->m_data_index*d->m_num_channels]=data[i];
    }
    d->m_data_index++;
    if (d->m_data_index>=d->m_num_channels) d->m_data_index=0;

    for (int ch=0; ch<d->m_num_channels; ch++) d->m_tmp[ch]=0;
    for (int jj=0; jj<d->m_filter_size; jj++) {
        int ii=(d->m_data_index+d->m_filter_size/2-jj+d->m_filter_size) % d->m_filter_size;
        for (int ch=0; ch<d->m_num_channels; ch++) {
            d->m_tmp[ch]+=d->m_data[ch+ii*d->m_num_channels]*d->m_filter_weights[jj];
        }
    }
    for (int ch=0; ch<d->m_num_channels; ch++) {
        d->m_filtered_data.append(d->m_tmp[ch]);
    }
}

void RTTemporalFilter::getFilteredData(float *filtered_data, int timepoint)
{
    for (int i=0; i<d->m_num_channels; i++) {
        filtered_data[i]=d->m_filtered_data.value(ch+timepoint*d->m_num_channels);
    }
}

