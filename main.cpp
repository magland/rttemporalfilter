#include <stdlib.h>
#include <stdio.h>
#include "mda.h"
#include "rttemporalfilter.h"
#include <QDebug>
#include <QStringList>
#include "get_command_line_params.h"

void usage() {
    printf("Usage: rttemporalfilter --input=input.mda --output=output.mda --filter=filter_weights.mda\n");
}

int main(int argc, char *argv[])
{
    QStringList required_params; required_params << "input" << "output" << "filter";
    QStringList optional_params;
    CLParams PP=get_command_line_params(argc,argv,required_params,optional_params);
    if (!PP.success) {
        usage();
        return -1;
    }
    QString path=PP.named_parameters["input"];
    QString path_output=PP.named_parameters["output"];
    QString path_filter=PP.named_parameters["filter"];

    Mda filter; filter.read(path_filter.toLatin1().data());

    int filter_size=filter.totalSize();
    float *filter_weights=(float *)malloc(sizeof(float)*filter_size);
    for (int i=0; i<filter_size; i++) filter_weights[i]=filter.value1(i);

    RTTemporalFilter::performFilter(path.toLatin1().data(),path_output.toLatin1().data(),filter_size,filter_weights);

    free(filter_weights);

    return 0;
}
