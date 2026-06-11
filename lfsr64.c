#include <stdio.h>
#include <stdint.h>
unsigned lfsr_xorshift(void)
{
    uint16_t start_state = 0xACE1u;  /* Any nonzero start state will work. */
    uint16_t lfsr = start_state;
    unsigned period = 0;

    do
    {   // 7,9,13 triplet from http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
	    printf ("%hu ", lfsr);
	    if (!(period % 16)) printf ("\n");
        lfsr ^= lfsr >> 7;
        lfsr ^= lfsr << 9;
        lfsr ^= lfsr >> 13;
        ++period;
    }
    while (lfsr != start_state);

    return period;
}

int main2 (void)
{
	printf ("\nperiod: %u\n", lfsr_xorshift ());

	return 0;
}

// xorshift64s, variant A_1(12,25,27) with multiplier M_32 from line 3 of table 5
uint64_t xorshift64star(void) {
    // initial seed must be nonzero, don't use a static variable for the state if multithreaded
    static uint64_t x = 0xface;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    //return x * 0x2545F4914F6CDD1DULL;
    return x;
}

int main (void)
{
    for (int i = 0; i < 65536; i++)
    {
        uint64_t n = xorshift64star ();
	printf ("%lu\n", n);
	//if (!(i % 8)) printf ("\n");
    }
}
