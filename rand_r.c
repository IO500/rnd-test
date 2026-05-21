#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>

#define RANDALGO_GOLDEN_RATIO_PRIME        0x9e37fffffffc0001UL

/* Create 64 bit random number, see https://www.pcg-random.org/posts/does-it-beat-the-minimal-standard.html */
typedef struct {
  uint64_t high;
  uint64_t low;
} iint128_t;

static iint128_t  randstate = {.high = 0, .low = 1};
static iint128_t  randmulti = {.high = 0x0fc94e3bf4e9ab32, .low = 0x866458cd56f5e605};

void srand64(uint64_t state){
  randstate.high = 0;
  randstate.low = state;
}

uint64_t rand64(void) {
  iint128_t res;
  res = randmulti;
  /* this is for testing only, produces some fair random number */
  res.low = randstate.low * randmulti.low;
  res.high = randstate.high * randmulti.high + (res.low >> 32);
  randstate = res;
  return res.high;
}

int main (int argc, char ** argv)
{
    uint64_t seed = RANDALGO_GOLDEN_RATIO_PRIME;
    // The min and max 4KB blocks based on file sizes driven by a network rate and 300 seconds
    uint64_t min = 1024l * 1024 * 300 / 4096; // 1 MB/sec for 300 seconds 
    uint64_t max = 1024l * 1024 * 200 * 300 / 4096; // 200 GB/sec for 300 seconds
    FILE * f = fopen ("rand_r.txt", "w");

    fprintf (f, "file sizes min: %llu max: %llu\n", min, max);
    for (int j = min; j <= max; j = j * 2)
    {
	fprintf (stderr, "testing %d\n", j);
        srand (seed);
	char * nums = (char *) calloc (j, sizeof (char));
        for (int i = 0; i <= j; i++)
        {
	    uint64_t n = rand64 () % j;
	    nums [n]++;
	    
            //printf ("%llu\n", n);
        }
	for (int i = 0; i < j; i++)
	{
            if (nums [i] == 0)
	        fprintf (f, "%llu missed %u\n", j, i);
	    else
		if (nums [i] != 1)
	            fprintf (f, "%llu repeated %u times: %d\n", j, i, (uint32_t) nums [i]);
	}
	free (nums);
    }
    fclose (f);

    return 0;
}
