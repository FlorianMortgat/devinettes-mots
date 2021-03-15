#include <stdio.h>
#include "prng.c"

#define BIT 1

int main(int argc, char *argv[]) {
				prng p;
				unsigned char t = 0;
				unsigned int x;
				prng_init(&p, 52);
				for (int i=0; i<16000; i++){
								x = prng_next(&p);
								//x = rot(x, t);
								printf(bit(x,BIT)?"#":" ");
								//t=(t + 2) % 32;
								//printf("%u", bit(prng_next(&p),5));
				}
				return 0;
}
