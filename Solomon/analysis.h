#include "types.h"

#ifndef ANALYSIS_H
#define ANALYSIS_H
void
syndromes(Polynomial *m, int symn, Polynomial *r);

void
forney_syndromes(Polynomial *syn, Polynomial *errp, int order, Polynomial *r);

void
find_locator(Polynomial *syn, int errpo, int symn, Polynomial *r);

void
find_errors(Polynomial *l, int order, Polynomial *r);
#endif


