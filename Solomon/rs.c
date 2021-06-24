#include <string.h> /* memcpy */
#include "galois.h" /* LOG_LUT, EXP_LUT */
#include "galois_poly.h" /* Polynomial, gpoly_rsgen */
#include "analysis.h" /* syndromes, forney_syndromes, 
                            find_locator, find_errors */
#include "purification.h" /* purification */

#include <stdio.h>

void
init_rs(void){
    init_galois_lut();
}

Polynomial g = {{0}, 0};

int
protect(Polynomial *m, int symn){
    int i, j;
	unsigned char coef, coefl;
	Polynomial r = {{0}, 0};
	
	if ((m->order + symn) > 255){
        /* Message too long */
        return 1;
    }
    
    if (g.order != symn + 1){
        gpoly_rsgen(symn, &g);
        for (i = 0; i < g.order; i++){
            g.data[i] = LOG_LUT[g.data[i]];
        } 
    }

    memcpy(r.data, m->data, m->order * sizeof(char));
    r.order = m->order;

    for (i = m->order; i < 255; i++){
        r.data[i] = 0;
    }

    for (i = 0; i < (m->order); i++){
        coef = r.data[i];
        if (coef != 0){
            coefl = LOG_LUT[coef];
            for (j = 1; j < g.order; j++){
                r.data[i+j] = r.data[i+j] ^ EXP_LUT[(g.data[j] + coefl) % 255];
            }
        }
    }

    memcpy(r.data, m->data, m->order * sizeof(char));
    memcpy(m->data, r.data, (m->order + symn) * sizeof(char));

    m->order += symn;
    return 0;
}

int
purify(Polynomial *m, Polynomial *errp, int symn){
    int i;
	Polynomial syn = {{0}, 0};
	Polynomial fsyn = {{0}, 0};
	Polynomial l = {{0}, 0};
	Polynomial errp2 = {{0}, 0};
	Polynomial errp3 = {{0}, 0};

	int nonz = 0;

    if (m->order > 255){
        /* Message too long */
        return 1;
    }

    if (errp->order > symn){
        /* Too many erasures */
        return 2;
    }

    for (i = 0; i < errp->order; i++){
        m->data[errp->data[i]] = 0;
    }
    
    syndromes(m, symn, &syn);
    
    for (i = 0; i < symn; i++){
        if (syn.data[i] != 0){
            nonz++;
        }
    }

    if (nonz == 0){
        /* Already purified */
        m->order -= symn;
        return 0;
    }
    forney_syndromes(&syn, errp, m->order, &fsyn);
   
    find_locator(&fsyn, errp->order, symn, &l);
    
    if (((((l.order-1)-errp->order) * 2) + errp->order) > symn){
        /* Too many errors */
        return 3;
    }

    find_errors(&l, m->order, &errp2);
    
    if (errp2.order == 0 || errp2.order != l.order - 1){
        /* Could not find the errors */
        return 4;
    }

    errp3.order = errp->order + errp2.order;

    memcpy(errp3.data, errp->data, errp->order * sizeof(char));
    memcpy(errp3.data, errp2.data + errp->order, 
           errp2.order * sizeof(char));   

    purification(m, &syn, &errp3, &l);
    
    syndromes(m, symn, &syn);
    for (i = 0; i < symn; i++){
        if (syn.data[i] != 0){
            /* Could not fix the message */
            return 5;
        }
    }
   
    m->order -= symn;
    return 0; 
}

