#include <stdlib.h>
#include <stdio.h>

void usage() {
    printf("Usage: rttemporalfilter [fname]\n");
}

int main(int argc, char *argv[])
{
    if (argc<2) {
        usage();
        return 0;
    }
    QString path=QString(argv[1]);




    else {
        int num_channels;
        fread(&num_channels,sizeof(unsigned int),1,stdin);
        printf("num channels = %d\n",num_channels);
        float *buf=(float *)malloc(sizeof(float)*num_channels);
        while (!feof(stdin)) {
            fread(buf,sizeof(float),num_channels,stdin);
            for (int i=0; i<num_channels; i++) {
                printf(" %f",buf[i]);
            }
        }
        free(buf);
        printf("\n");
    }
    return 0;
}

