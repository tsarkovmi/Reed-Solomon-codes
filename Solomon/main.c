#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>

#include "main.h"

typedef enum { STREAM_TYPE_encode, STREAM_TYPE_decode } STREAM_TYPE;

int 
no_errata_purify(Polynomial* m, int symn){
    Polynomial errp = {{0}, 0};
    return purify(m, &errp, symn);
}

int
stream(char* in_fname, char* out_fname, STREAM_TYPE type, int length, int symn){
    Polynomial msg = {{0}, 0};
	FILE *fpin, *fpout;
	int in_size;
	int bytes;
	int result;

	int (*function)(Polynomial*, int) = NULL;

    fpin = fopen(in_fname, "rb");
    fpout = fopen(out_fname, "wb");

    in_size = 0;
 
    switch (type) {
        case STREAM_TYPE_encode:
            in_size = length;
            function = protect;
        break;

        case STREAM_TYPE_decode:
            in_size = length + symn;
            function = no_errata_purify;
        break;
    }

    while (1){
        bytes = fread(msg.data, 1, in_size, fpin);
        if (bytes == 0){
            break;
        }
        msg.order = bytes;
        result = function(&msg, symn);

        if (result != 0){
            return result;
        }

        fwrite(msg.data, 1, msg.order, fpout);
    }
    fclose(fpin);
    fclose(fpout);
    return 0;
}

int
argint(char* arg, int* out){
    char *ptr = arg;
    long i = strtol(arg, &ptr, 10);
    if (i > 255){
        return 1;
    }
    if (strlen(ptr) != 0){
        return 1;
    }
    *out = i;
    return 0;
}

int 
main(int argc, char** argv){
    int d = 0;
    int s = 0;
	int result;
	STREAM_TYPE type;

	init_rs();

    if (argc < 6){
		printf("usage: %s [operation] [data len] [code len] [input file] [output file]", argv[0]);
        return 0;
    }

    if (strcmp(argv[1], "encode") == 0) {
        type = STREAM_TYPE_encode;
    } else if (strcmp(argv[1], "decode") == 0){
        type = STREAM_TYPE_decode;
    } else {
		printf("incorrect operation\n");
        return 0;
    }
	
	argint(argv[2], &d);
	argint(argv[3], &s);

    if (d == 0){
        d = (s != 0) ? (255 - s) : (245);
    }
            
    if (s == 0){
        s = (d != 0) ? (255 - d) : (10);
    }

    if ((d + s) > 255){
		printf("Too big block size\n");
        return 0;
    }    

    result = stream(argv[4], argv[5], type, d, s); 
    switch (result){
        case 3:
            fprintf(stderr, "I can't fix it, too many errors\n");
        break;

        case 4:
            fprintf(stderr, "I couldn't find all errors\n");
        break;

        case 5:
            fprintf(stderr, "The message can't be fixed\n");
        break;
    }

    return result;
}

