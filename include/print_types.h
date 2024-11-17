#ifndef PRINT_TYPES_H

#define PRINT_TYPES_H

#include <stdio.h>

/* Those functions are just wrappers for printf with specifiers */
void __logger_print_c__(FILE *f, char *pCh); /* print as ASCII char %c specifier */
void __logger_print_i__(FILE *f, int *pI); /* print using %d specifier */
void __logger_print_f__(FILE *f, double *pF); /* print using %f specifier (same as %lf) */
void __logger_print_s__(FILE *f, char **pS); /* print using %s specifier */
void __logger_print_p__(FILE *f, void **pPtr); /* print using %p specifiers */

#endif
