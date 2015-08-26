#include <stdlib.h>
#include <stdio.h>
#include "mdaio.h"
#include "rttemporalfilter.h"

void usage() {
    printf("Usage: rttemporalfilter [input_fname] [output_fname]\n");
}

int main(int argc, char *argv[])
{
    if (argc<3) {
        usage();
        return 0;
    }
    char *path=argv[1];
    char *path_output=argv[2];

    FILE *FF=fopen(path,"r");
    if (!FF) {
        printf("Unable to open input file: %s\n",path);
        return -1;
    }
    MDAIO_HEADER HH;
    if (!mda_read_header(&HH,FF)) {
        fclose(FF);
        printf("Problem reading mda header: %s\n",path);
        return -1;
    }
    if (HH.num_dims!=2) {
        fclose(FF);
        printf("Expected 2 dimensions, got %d.\n",HH.num_dims);
        return -1;
    }
    if (HH.data_type!=MDAIO_TYPE_FLOAT32) {
        fclose(FF);
        printf("Expected type float32, got type %d.\n",HH.data_type);
        return -1;
    }
    int num_channels=HH.dims[0];
    int num_timepoints=HH.dims[1];


    FILE *FF_output=fopen(path_output,"w");
    if (!FF_output) {
        printf("Unable to open output file: %s\n",path_output);
        return -1;
    }
    MDAIO_HEADER HH_output;
    HH_output.data_type=MDAIO_TYPE_FLOAT32;
    HH_output.num_dims=2;
    HH_output.dims[0]=num_channels;
    HH_output.dims[1]=num_timepoints;
    if (!mda_write_header(&HH_output,FF)) {
        fclose(FF); fclose(FF_output);
        printf("Problem writing mda header: %s\n",path_output);
        return -1;
    }

    int filter_size=5;
    float *filter_weights=(float *)malloc(sizeof(float)*filter_size);
    for (int i=0; i<filter_size; i++) filter_weights[i]=1;
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
            return -1;
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

    return 0;
}
