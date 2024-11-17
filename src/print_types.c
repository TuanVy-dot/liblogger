#include <stdio.h>
#include "print_types.h"

void __logger_print_ch__(FILE *f, char *pCh) {
    fprintf(f, "%c", *pCh);
}

void __logger_print_i__(FILE *f, int *pI) {
    fprintf(f, "%d", *pI);
}

void __logger_print_f__(FILE *f, double *pF) {
    fprintf(f, "%f", *pF);
}

void __logger_print_str__(FILE *f, char **pS) {
    fprintf(f, "%s", *pS);
}

void __logger_print_ptr__(FILE *f, void **pPtr) {
    fprintf(f, "%p", *pPtr);
}
