#include <string.h> /* memcpy */
#include "galois.h" /* gmul, g2pow, ginv */
#include "galois_poly.h" /* gpoly_add, gpoly_eval, gpoly_scale */
#include "types.h" /* Polynomial */

void
syndromes(Polynomial *m, int symn, Polynomial *r){
    int i;
	
	r->order = symn + 1; 
    r->data[0] = 0;
    for (i = 0; i < symn; i++){
        r->data[i+1] = gpoly_eval(m, g2pow(i));
    }
}

void
forney_syndromes(Polynomial *syn, Polynomial *errp, int order, Polynomial *r){
	int i, j, x;

	Polynomial errpr = {{0}, 0};
    errpr.order = errp->order;

    for (i = 0; i < errp->order; i++){    
        errpr.data[i] = order - 1 - errp->data[i];
    }

    r->order = syn->order - 1;
    memcpy(r->data, syn->data + 1, r->order * sizeof(char));    

    for (i = 0; i < errp->order; i++){
        x = g2pow(errpr.data[i]);
        for (j = 0; j < (r->order - 1); j++){
            r->data[j] = gmul(r->data[j], x) ^ r->data[j+1];
        }
    }
}

void
find_locator(Polynomial *syn, int errpo, int symn, Polynomial *r){
    int shift = 0;
	int i, j, k;
	unsigned char delta;
	Polynomial ol = {{0}, 0};
	Polynomial nl = {{0}, 0};
	Polynomial nl2 = {{0}, 0};
	Polynomial nl3 = {{0}, 0};

    r->data[0] = 1;
    ol.data[0] = 1;
    r->order = 1;
    ol.order = 1;

    if (syn->order > symn){
        shift = syn->order - symn;
    }

    for (i = 0; i < (symn - errpo); i++){
        k = i + shift;
        delta = syn->data[k];
        for (j = 1; j < r->order; j++){
            delta = delta ^ gmul(r->data[r->order-(j+1)], syn->data[k-j]);
        }

        ol.data[ol.order] = 0;
        ol.order++; 
        
        nl.order = 0;
        
        if (delta != 0){
            if (ol.order > r->order){
                gpoly_scale(&ol, delta, &nl);
                gpoly_scale(r, ginv(delta), &ol);
                memcpy(r->data, nl.data, nl.order * sizeof(char));
                r->order = nl.order;
            }
            gpoly_scale(&ol, delta, &nl2); 
            gpoly_add(r, &nl2, &nl3);
            memcpy(r->data, nl3.data, nl3.order * sizeof(char));
            r->order = nl3.order;
        }
    }
}

void
find_errors(Polynomial *l, int order, Polynomial *r){
    Polynomial li = {{0}, 0};
	int i;
    
	li.order = l->order;

    for (i = 0; i < l->order; i++){
        li.data[l->order-i-1] = l->data[i];
    }

    r->order = 0;
    for (i = 0; i < order; i++){
        if (gpoly_eval(&li, g2pow(i)) == 0){
            r->data[r->order] = order - 1 - i;
            r->order++;
        }
    }
}

