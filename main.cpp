#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc>=2) {
        unsigned int num_channels=atoi(argv[1]);
        fwrite(&num_channels,sizeof(unsigned int),1,stdout);
        for (int i=0; i<num_channels; i++) {
            float val=i;
            fwrite(&val,sizeof(float),1,stdout);
        }
    }
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

