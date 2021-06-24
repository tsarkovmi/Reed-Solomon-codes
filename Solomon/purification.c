#include <string.h> /* memcpy */
#include "galois.h" /* gmul, gdiv, g2pow, ginv */
#include "galois_poly.h" /* gpoly_add, gpoly_mul, gpoly_eval */
#include "types.h" /* Polynomial */

int
purification(Polynomial *m, Polynomial *syn, Polynomial *errp, Polynomial *l){
    Polynomial coefp = {{0}, 0};
	Polynomial synr = {{0}, 0};
	Polynomial eval = {{0}, 0};
	Polynomial x = {{0}, 0};
	Polynomial xl = {{0}, 0};
	Polynomial e = {{0}, 0};
	Polynomial m2 = {{0}, 0};
	
	int i;
	int j;
	int li;

	unsigned char xi;
	unsigned char xii;
	unsigned char xiil;
	unsigned char mul;
	unsigned char prime = 1;
	unsigned char y;

    coefp.order = errp->order;
    for (i = 0; i < errp->order; i++){
        coefp.data[i] = m->order - 1 - errp->data[i];
    }

    synr.order = syn->order;
    for (i = 0; i < syn->order; i++){
        synr.data[syn->order-i-1] = syn->data[i];
    }

    gpoly_mul(&synr, l, &eval);
    for (i = 0; i < l->order; i++){
        eval.data[i] = eval.data[i+(eval.order-l->order)];
    }
    eval.order = l->order;

    x.order = coefp.order;
 
    for (i = 0; i < coefp.order; i++){
        li = coefp.data[i];
        x.data[i] = g2pow(li);
    }
    
    xl.order = x.order;
    for (i = 0; i < x.order; i++){
        xl.data[i] = LOG_LUT[x.data[i]];
    }

    e.order = m->order;

    for (i = 0; i < x.order; i++){
        xi = x.data[i];
        xii = ginv(xi);
        xiil = LOG_LUT[xii];   
 
        for (j = 0; j < x.order; j++){
            if (j != i){
                mul = 1 ^ EXP_LUT[(xl.data[j] + xiil) % 255];
                prime = gmul(prime, mul);
            }
        }

        if (prime == 0){
            return 1;
        }  

        y = gpoly_eval(&eval, xii);
        y = gmul(xi, y);

        e.data[errp->data[i]] = gdiv(y, prime);       
    }

    gpoly_add(m, &e, &m2);    
    
    memcpy(m->data, m2.data, m2.order * sizeof(char));
    m->order = m2.order;
    return 0;
}

