#include "mprotect-diffckpt.h"

int main() {
   
    size_t elem = 1000000;
    
    init();
    
    //double *arr2 = (double*) malloc(elem * sizeof(double));
    double arr2[elem];
   
    add_prot(arr2, elem*sizeof(double));

    int iter = 100000;
    for(int i=0; i<(elem-iter); i+=iter) {
        arr2[i] = 1;    
        //if(i==(5*iter))
        //    *(int*)NULL = 1;
    }
    
    finalize();
    
    return 0;
}
