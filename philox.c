#include <stdio.h>
#include <Random123/threefry.h>
#include <Random123/philox.h>
#include "example_seeds.h"

int main()
{
    // The min and max 4KB blocks based on file sizes driven by a network rate and 300 seconds
    uint64_t min = 1024l * 1024 * 300 / 4096; // 1 MB/sec for 300 seconds 
    uint64_t max = 1024l * 1024 * 200 * 300 / 4096; // 200 GB/sec for 300 seconds
    FILE * f = fopen ("philox.txt", "w");

    fprintf (f, "file sizes min: %llu max: %llu\n", min, max);
    for (int j = min; j <= max; j = j * 2)
    {
        fprintf (stderr, "testing %d\n", j);
        // Define key and counter types
        philox4x64_key_t key = {{0xdeadbeef, 0xcafebabe}};
        philox4x64_ctr_t ctr = {{0, 0, 0, 0}};
    
        char * nums = (char *) calloc (j, sizeof (char));
        // Generate random numbers by incrementing the counter
        for (unsigned long long i = 0; i <= j / 4; i++)
        {
            ctr.v[0] = i; // change counter to get different random numbers
    
            // Generate a 4x64-bit random number block
            philox4x64_ctr_t rand = philox4x64(ctr, key);
    
            // Print the generated random numbers
            //printf("Random block %llu: ", i);
            for (int l = 0; l < 4; l++)
            {
                uint32_t r = rand.v [l] % j;
		//printf ("%u\n", r);
                nums [r]++;
            }
            for (int k = 0; k < j; k++)
            {
                if (nums [k] == 0)
                    fprintf (f, "%llu missed %u\n", j, k);
                else
                    if (nums [k] != 1)
                        fprintf (f, "%llu repeated %u times: %u\n", j, k, (uint32_t) nums [k]);
            }
        }
        free (nums);
    }
    fclose (f);

    return 0;

}

#if 0
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>

int main3 (int argc, char ** argv)
{
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
    for (int i = 0; i <= j; i++)
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
#endif
