#include "mprotect-diffckpt.h"

int main() {
   
    size_t elem = 1000000;
    
    init();
    
    //double *arr2 = (double*) malloc(elem * sizeof(double));
    double arr2[elem];
   
    memset(arr2, 0x0, elem*sizeof(double));
    
    add_prot(arr2, elem*sizeof(double));

    int iter = 100000;
    
    for(int i=0; i<(elem-iter); i+=iter) {
        arr2[i] = 1.469;    
        if(i==(5*iter))
            *(int*)NULL = 1;
    }
    
    for(int i=0; i<(elem-iter); i+=iter) {
        printf("arr[%d]: %lf\n", i, arr2[i]);   
    }
    
    finalize();
    
    return 0;
}
