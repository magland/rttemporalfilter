#include "rttemporalfilter.h"
#include <malloc.h>
#include <QList>
#include <QDebug>
#include "mdaio.h"

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
    d=new RTTemporalFilterPrivate;
    d->q=this;
    d->m_num_channels=0;
    d->m_data=0;
    d->m_filter_size=0;
    d->m_filter_weights=0;
    d->m_data_index=0;
    d->m_tmp=0;
}

RTTemporalFilter::~RTTemporalFilter()
{
    if (d->m_data) free(d->m_data);
    if (d->m_tmp) free(d->m_tmp);
    if (d->m_filter_weights) free(d->m_filter_weights);
    delete d;
}

bool RTTemporalFilter::performFilter(const char *input_path, const char *output_path, int filter_size, float *filter_weights)
{
    FILE *FF=fopen(input_path,"r");
    if (!FF) {
        printf("Unable to open input file: %s\n",input_path);
        return false;
    }
    FILE *FF_output=fopen(output_path,"w");
    if (!FF_output) {
        fclose(FF);
        printf("Unable to open output file: %s\n",output_path);
        return false;
    }
    MDAIO_HEADER HH;
    if (!mda_read_header(&HH,FF)) {
        fclose(FF); fclose(FF_output);
        printf("Problem reading mda header: %s\n",input_path);
        return false;
    }
    if (HH.num_dims!=2) {
        fclose(FF); fclose(FF_output);
        printf("Expected 2 dimensions, got %d.\n",HH.num_dims);
        return false;
    }
    if (HH.data_type!=MDAIO_TYPE_FLOAT32) {
        fclose(FF); fclose(FF_output);
        printf("Expected type float32, got type %d.\n",HH.data_type);
        return false;
    }
    int num_channels=HH.dims[0];
    int num_timepoints=HH.dims[1];

    MDAIO_HEADER HH_output;
    HH_output.data_type=MDAIO_TYPE_FLOAT32;
    HH_output.num_dims=2;
    HH_output.dims[0]=num_channels;
    HH_output.dims[1]=num_timepoints;
    if (!mda_write_header(&HH_output,FF_output)) {
        fclose(FF); fclose(FF_output);
        printf("Problem writing mda header: %s\n",output_path);
        return false;
    }

    RTTemporalFilter XX;
    XX.initialize(filter_size,filter_weights,num_channels);

    float *buf=(float *)malloc(sizeof(float)*num_channels);
    for (int ch=0; ch<num_channels; ch++) buf[ch]=0;
    for (int cc=0; cc<filter_size/2; cc++) {
        mda_write_float32(buf,&HH_output,num_channels,FF_output);
    }
    for (int t=0; t<num_timepoints; t++) {
        if (mda_read_float32(buf,&HH,num_channels,FF)!=num_channels) {
            fclose(FF); fclose(FF_output);
            printf("Problem reading data at timepoint %d of %d.\n",t,num_timepoints);
            return false;
        }
        XX.addData(buf);
        int t2=t-filter_size/2;
        if ((t2>=0)&&(t2+filter_size-1<num_timepoints)) {
            XX.getFilteredData(buf,t2);
            mda_write_float32(buf,&HH_output,num_channels,FF_output);
        }
    }
    for (int ch=0; ch<num_channels; ch++) buf[ch]=0;
    for (int cc=0; cc<filter_size-filter_size/2; cc++) {
        mda_write_float32(buf,&HH_output,num_channels,FF_output);
    }
    free(buf);

    fclose(FF); fclose(FF_output);

    return true;
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
    if (d->m_data) free(d->m_data);
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
        int ii=(d->m_data_index-d->m_filter_size/2+jj+d->m_filter_size) % d->m_filter_size;
        for (int ch=0; ch<d->m_num_channels; ch++) {
            d->m_tmp[ch]+=d->m_data[ch+ii*d->m_num_channels]*d->m_filter_weights[jj];
        }
    }
    for (int ch=0; ch<d->m_num_channels; ch++) {
        d->m_filtered_data << d->m_tmp[ch];
    }
}

void RTTemporalFilter::getFilteredData(float *filtered_data, int timepoint)
{
    for (int i=0; i<d->m_num_channels; i++) {
        filtered_data[i]=d->m_filtered_data.value(i+timepoint*d->m_num_channels);
    }
}

