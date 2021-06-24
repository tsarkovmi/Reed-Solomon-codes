#include "types.h"

#ifndef GALOIS_POLY_H
#define GALOIS_POLY_H

void
gpoly_add(Polynomial *p, Polynomial *q, Polynomial *r);

void
gpoly_mul(Polynomial *p, Polynomial *q, Polynomial *r);

void
gpoly_scale(Polynomial *p, int s, Polynomial *r);

void
gpoly_rsgen(int n, Polynomial *r);

unsigned char
gpoly_eval(Polynomial *p, int x);

#endif
