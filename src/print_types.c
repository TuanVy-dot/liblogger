#include <stdio.h>
#include "print_types.h"

void __logger_print_c__(FILE *f, char *pCh) {
    fprintf(f, "%c\n", *pCh);
}

void __logger_print_i__(FILE *f, int *pI) {
    fprintf(f, "%d\n", *pI);
}

void __logger_print_f__(FILE *f, double *pF) {
    fprintf(f, "%f\n", *pF);
}

void __logger_print_s__(FILE *f, char **pS) {
    fprintf(f, "%s\n", *pS);
}

void __logger_print_p__(FILE *f, void **pPtr) {
    fprintf(f, "%p\n", *pPtr);
}
