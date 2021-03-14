#define PREVS 7

typedef struct {
			unsigned int seed;
			unsigned int max;
			unsigned int prev[PREVS];
			unsigned char n;

} prng;
unsigned int prng_next(prng *p);

void prng_init(prng *p, unsigned int seed) {
				// p->max = 4294967296;
				p->n = 0;
				p->seed = seed;
				p->prev[0] = 4294967296 >> 3;
				p->prev[1] = 11;
				p->prev[2] = 3001;
				p->prev[3] = 0;
				p->prev[4] = 975111;
				p->prev[5] = 78;
				p->prev[6] = seed;

				for (int i = 0; i < 150; i++) {
								prng_next(p);
				}
}

unsigned int prng_next(prng *p) {
				// todo optimisable index glissant
				char nn = (p->n + PREVS - 1) % PREVS;
				unsigned int x =
								p->prev[p->n] + p->prev[nn];
				p->n = (p->n + 1) % PREVS;
				p->prev[nn] = x;
				return x;
}
