#include <stdio.h>
#include <stdint.h>

int main (void)
{
    // The min and max 4KB blocks based on file sizes driven by a network rate and 300 seconds
    uint64_t min = 1024l * 1024 * 300 / 4096; // 1 MB/sec for 300 seconds
    uint64_t max = 1024l * 1024 * 200 * 300 / 4096; // 200 GB/sec for 300 seconds
    uint32_t min_bits = 0;
    uint32_t max_bits = 0;
    uint32_t bitmask = 1;

    uint64_t t = min;
    while (t >>= 1) min_bits++;
    t = max;
    while (t >>= 1) max_bits++;

    printf ("min(bits): %llu(%u) max(bits): %llu(%u)\n", min, min_bits, max, max_bits);

    uint32_t a = 3401712413U;//214013U;
    uint32_t c = 2531011U;

    for (int j = min, bits=min_bits; j <= max; j *= 2, bits++)
    {
        uint64_t seed = 0xdeadc0dedeadc0de;
        bitmask = (1 << bits) - 1;
        printf ("mask(bits): %x(%u)\n", bitmask, bits);
        printf ("testing: %d\n", j);
        size_t buf_len = 256;
        char buf [buf_len];
        snprintf (buf, buf_len, "%u.txt", j);
        FILE * f = fopen (buf, "w");
    
        for (int i = 0; i < j; i++)
        {
            seed = (seed * a + c);
            uint32_t val = seed & bitmask;
    
            fprintf (f, "%d\n", val);
        }
        fclose (f);
    }
}
