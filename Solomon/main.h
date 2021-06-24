#include "types.h"

#ifndef MAIN_H
#define MAIN_H
void
init_rs(void);

int
protect(Polynomial *m, int symn);

int
purify(Polynomial *m, Polynomial *errp, int symn); 
#endif
