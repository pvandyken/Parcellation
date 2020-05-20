#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "BundleTools_sp.h"


struct bundle sliceFiber( struct bundle fibras, int sliceNum);

int main(int argc, char *argv[]){
    if (argc ==4){
	    struct bundle f1,f2;
	    f1=read_bundle(argv[1]);
	    
	    int puntos = atoi(argv[3]);
	    f2=sliceFiber(f1,puntos);

	    write_bundle(argv[2], f2.nfibers,f2.npoints,f2.points);
	    int i;
	    for (i=0; i<f1.nfibers; i++){
			free(f1.points[i]);
			free(f2.points[i]);
	    }
	    free(f1.points);
	    free(f1.npoints);
	    free(f2.points);
	    free(f2.npoints);
	}

    else printf("./resampling fileInput.bundles (argv[1]), fileOutput.bundles (argv[2]), numberOfPoints (argv[3])\n");
    return 0;
}

struct bundle sliceFiber( struct bundle fibras, int sliceNum){
    struct bundle sB;
    sB.nfibers = fibras.nfibers;
    sB.npoints = (int32_t*) malloc(sB.nfibers*sizeof(int32_t));
    sB.points = (float**) malloc (sB.nfibers*sizeof(float*));

    int k;
    for(k=0;k<fibras.nfibers;k++){
        int fSize = 1;
        *(sB.npoints+k) = sliceNum;
        sB.points[k] = (float*) malloc((*(sB.npoints+k))*3*sizeof(float));
        *(sB.points[k]+0) = *(fibras.points[k]+0);
        *(sB.points[k]+1) = *(fibras.points[k]+1);
        *(sB.points[k]+2) = *(fibras.points[k]+2);

        float fiberlength = 0;
        float acc_length[*(fibras.npoints+k)];
        acc_length[0] = 0;

        int j;
        for (j = 0; j < *(fibras.npoints+k)-3; j++){
            fiberlength += sqrt( pow(*(fibras.points[k]+(j*3)) - *(fibras.points[k]+(j*3)+3),2) +
                                 pow(*(fibras.points[k]+(j*3)+1) - *(fibras.points[k]+(j*3)+1+3),2) +
                                 pow(*(fibras.points[k]+(j*3)+2) - *(fibras.points[k]+(j*3)+2+3),2));

            acc_length[j + 1] = fiberlength;
        }
        float step = fiberlength / (float)(sliceNum-1);
        float currentLength = step;

        int currentInd = 0;

        float lengthtmp = fiberlength - step*0.5;

        while ( currentLength < lengthtmp){
            if (acc_length[currentInd] < currentLength){
                while (acc_length[currentInd] < currentLength){
                    currentInd++;
                }
                currentInd--;
            }

            float fact = (currentLength - acc_length[currentInd])/(acc_length[currentInd + 1] - acc_length[currentInd]);
            if ( fact > 0.000001 ){
                *(sB.points[k] + fSize*3 + 0) = (*(fibras.points[k] + (int)(currentInd + 1)*3) - *(fibras.points[k] + (int)currentInd*3))*fact + *(fibras.points[k] + (int)currentInd*3);
                *(sB.points[k] + fSize*3 + 1) = (*(fibras.points[k] + (int)(currentInd + 1)*3 + 1) - *(fibras.points[k] + (int)currentInd*3 + 1))*fact + *(fibras.points[k] + (int)currentInd*3 + 1);
                *(sB.points[k] + fSize*3 + 2) = (*(fibras.points[k] + (int)(currentInd + 1)*3 + 2) - *(fibras.points[k] + (int)currentInd*3 + 2))*fact + *(fibras.points[k] + (int)currentInd*3 + 2);
                fSize++;
            }
            else{
                 *(sB.points[k] + fSize*3 + 0) = *(fibras.points[k] + (int)currentInd*3);
                 *(sB.points[k] + fSize*3 + 1) = *(fibras.points[k] + (int)currentInd*3 + 1);
                 *(sB.points[k] + fSize*3 + 2) = *(fibras.points[k] + (int)currentInd*3 + 2);
                fSize++;
            }

            currentLength += step;
        }

        *(sB.points[k] + fSize*3) = *(fibras.points[k] + (*(fibras.npoints + k) - 1)*3);
        *(sB.points[k] + fSize*3 + 1) = *(fibras.points[k] + (*(fibras.npoints + k) - 1)*3 + 1);
        *(sB.points[k] + fSize*3 + 2) = *(fibras.points[k] + (*(fibras.npoints + k) - 1)*3 + 2);
        fSize++;

    }
    return sB;

}
