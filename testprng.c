#include <stdio.h>
#include "prng.c"

int main(int argc, char *argv[]) {
				prng p;
				prng_init(&p, 56);
				for (int i=0; i<20; i++){
								printf("%u\n", prng_next(&p)%100);
				}
				return 0;
}
