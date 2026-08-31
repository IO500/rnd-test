#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int bit_length;       // LFSR length n
    int num_taps;         // Number of taps excluding highest bit
    int taps[4];          // Tap positions (zero-based), max 4 taps here
} LFSRConfig;

// Tap positions are zero-based (LSB = bit 0), excluding the highest bit (bit_length - 1)
// the table of which bit taps for various lengths give maximum length period
// https://datacipy.elektroniche.cz/lfsr_table.pdf
// starts at 2 (2 bits numbered 1-2)
// dummies added to make offsetting into the array easier
// the first digit is how many taps. The rest are the taps or 0 for not a tap
//
// An alternative list is here:
// https://ww2.ams.org/journals/mcom/1973-27-124/S0025-5718-1973-0327722-7/S0025-5718-1973-0327722-7.pdf?t=1779476860989
LFSRConfig lfsr_configs[] =
{
    {0, 1, {0}},               // 0 dummy
    {1, 1, {0}},               // 1 dummy
    {2, 2, {1,0}},             // 2 dummy
    {3, 2, {2,1}},             // 3 validated
    {4 ,2, {3,2}},             // 4 validated
    {5, 2, {4,2}},             // 5 validated
    {6, 4, {5,3,2,0}},         // 6 validated
    {7, 2, {6,2}},             // 7 validated
    {8, 4, {7,3,2,1}},         // 8 validated
    {9, 2, {8,4}},             // 9 validated
    {10, 2, {9,2}},            // 10 validated
    {11, 2, {10,1}},           // 11 validated
    {12, 4, {11,5,3,0}},       // 12 validated
    {13, 4, {12,11,9,8}},      // 13 validated
    {14, 4, {13,12,10,8}},     // 14 validated
    {15, 2, {14,0}},           // 15 validated
    {16, 4, {15,11,2,0}},      // 16 validated
    {17, 2, {16,2}},           // 17 validated
    {18, 2, {17,6}},           // 18 validated
    {19, 4, {18,17,16,12}},    // 19 validated
    {20, 2, {19,2}},           // 20 validated
    {21, 2, {20,1}},           // 21 validated
    {22, 2, {21,0}},           // 22 validated
    {23, 2, {22,4}},           // 23 validated
    {24, 4, {23,6,1,0}},       // 24 validated
    {25, 2, {24,2}},           // 25 validated
    {26, 4, {25,24,23,19}},    // 26 validated
    {27, 4, {26,25,24,21}},    // 27 validated
    {28, 2, {27,2}},           // 28 validated
    {29, 2, {28,1}},           // 29 validated
    {30, 4, {29,28,25,23}},    // 30 validated
    {31, 2, {30,2}},           // 31 validated
    {32, 4, {31,30,29,9}},     // 32 validated
    {33, 2, {32, 12}},         // 33 validated
    {34, 4, {33,32,31,6}}      // 34 validated
};

#define NUM_LFSR_CONFIGS (sizeof(lfsr_configs)/sizeof(lfsr_configs[0]))

typedef struct
{
    uint8_t bits;
    uint64_t state;
    uint64_t mask;
} LFSR;

void lfsr_init (LFSR * lfsr, uint8_t bits, uint64_t seed)
{
    lfsr->bits = bits;
    lfsr->mask = ((1ull << bits) - 1u);
    lfsr->state = seed & lfsr->mask;

    // code to print out the config for this size
#if 0
    printf ("bits: %hhu mask: %llu state: %llu\n", lfsr->bits, lfsr->mask, lfsr->state);
    if (lfsr_configs [bits].num_taps >= 2)
    printf ("taps [0]: %d taps [1]: %d", lfsr_configs [lfsr->bits].taps [0], lfsr_configs [lfsr->bits].taps [1]);
    if (lfsr_configs [bits].num_taps == 4)
    printf (" taps [2]: %d taps [3]: %d\n", lfsr_configs [lfsr->bits].taps [2], lfsr_configs [lfsr->bits].taps [3]);
    else
    printf ("\n");
#endif
}

uint64_t lfsr_step (LFSR * lfsr)
{
    uint64_t s = lfsr->state;
    uint8_t feedback = 0;

    switch (lfsr_configs [lfsr->bits].num_taps)
    {
        case 2:
            feedback =
                ((s >> lfsr_configs [lfsr->bits].taps [0]) ^
                 (s >> lfsr_configs [lfsr->bits].taps [1])
                )
                & 1u;
            break;
        case 4:
            feedback =
                ((s >> lfsr_configs [lfsr->bits].taps [0]) ^
                 (s >> lfsr_configs [lfsr->bits].taps [1]) ^
                 (s >> lfsr_configs [lfsr->bits].taps [2]) ^
                 (s >> lfsr_configs [lfsr->bits].taps [3])
                )
                & 1u;
            break;
        default:
            printf ("ERROR: %d\n", lfsr->bits);
    }

    lfsr->state = ((s << 1) & lfsr->mask) | feedback;

    return lfsr->state;
}

int test_period_main (void)
{
    // The min and max 4KB blocks based on file sizes driven by a network rate and 300 seconds
    uint64_t min = 1024l * 1024 * 300 / 4096; // 1 MB/sec for 300 seconds
    uint64_t max = 1024l * 1024 * 1024 * 200 * 300 / 4096; // 200 GB/sec for 300 seconds
    uint8_t min_bits = 0;
    uint8_t max_bits = 0;
    uint64_t seed = 1;

    uint64_t t = min;
    while (t >>= 1) min_bits++;
    t = max;
    while (t >>= 1) max_bits++;

    printf ("min(bits): %llu(%u) max(bits): %llu(%u)\n", min, min_bits, max, max_bits);
    LFSR lfsr;

    size_t buf_len = 256;
    char buf [buf_len];
    for (uint8_t i = 3; i < NUM_LFSR_CONFIGS; i++)
    {
        lfsr_init (&lfsr, i, seed);
        snprintf (buf, buf_len, "%u.txt", lfsr.bits);
        FILE * f = fopen (buf, "w");
        do
        {
            fprintf(f, "%llu\n", lfsr.state);
    
            lfsr_step(&lfsr);
        } while (lfsr.state != seed);
        fclose (f);
    }
    return 0;
}

int test_ranges_list_main (void)
{
    uint64_t blocks = 256 + 64 + 16  + 8; // 4 bits out of 9 set

    typedef struct
    {
        uint8_t lfsr_index;
        uint64_t file_base_offset;
    } LFSR_RANGE;

    LFSR_RANGE rnds [NUM_LFSR_CONFIGS];
    uint8_t rnds_count = 0;

    memset (rnds, 0, sizeof (LFSR_RANGE) * NUM_LFSR_CONFIGS);

    uint64_t b = blocks;
    uint8_t bit_offset = 1;

    while (b != 0)
    {
        if (b & 1)
        {
            rnds [rnds_count].lfsr_index = bit_offset - 1;
            if (rnds_count != 0)
            {
                rnds [rnds_count].file_base_offset = rnds [rnds_count - 1].file_base_offset + (1 << rnds [rnds_count - 1].lfsr_index);
            }
            printf ("test file_base_offset: %llu\n", rnds [rnds_count].file_base_offset);
            rnds_count++;
        }
        b >>= 1;
        bit_offset++;
    }

    printf ("%hhu bits set for %llu blocks\n", rnds_count, blocks);
    for (int i = 0; i < rnds_count; i++)
    {
        printf ("lfsr_index: %hhu file_base_offset: %llu\n", rnds [i].lfsr_index, rnds [i].file_base_offset);
    }

    return 0;
}

int main (void)
{
    // for 344 blocks, we'll have these 4 bits set
    uint64_t blocks = 12;//256 + 64 + 16  + 8; // 4 bits out of 9 set

    // That means we'll have the following ranges for each of the bits:
    // bit range file_offset
    //  3    8     0
    //  4   16     8
    //  6   64    24
    //  8  256    88
    //  the formula for the new offset = [n-1].file_base_offset + 2^[(n-1)'s bits]

    typedef struct
    {
        uint8_t lfsr_index;
        LFSR lfsr;
        uint64_t file_base_offset;
    } LFSR_RANGE;

    LFSR_RANGE rnds [NUM_LFSR_CONFIGS];
    uint8_t rnds_count = 0;

    memset (rnds, 0, sizeof (LFSR_RANGE) * NUM_LFSR_CONFIGS);

    uint64_t b = blocks;
    uint8_t bit_offset = 1;
    uint64_t lfsr_seed = 1;

    // build the list of LFSR generators based on set bits
    while (b != 0)
    {
        if (b & 1)
        {
            rnds [rnds_count].lfsr_index = bit_offset - 1;
            lfsr_init (&rnds [rnds_count].lfsr, bit_offset-1, lfsr_seed);
            if (rnds_count != 0)
            {
                rnds [rnds_count].file_base_offset = rnds [rnds_count - 1].file_base_offset + (1 << rnds [rnds_count - 1].lfsr_index);
            }
            rnds_count++;
        }
        b >>= 1;
        bit_offset++;
    }

    // randomize across all of them to generate the right offsets list to cover everything
    int seed = time (NULL);
    uint64_t i = 0;
    for (i = 0; rnds_count > 0; i++)
    {
        int x = rand_r (&seed) % rnds_count;
        uint64_t block_to_read = rnds [x].lfsr.state;
        uint64_t base_offset = rnds [x].file_base_offset;
        lfsr_step (&rnds [x].lfsr);
        //printf ("%d: block_to_read: %llu\n", i, block_to_read);
        printf ("%llu block for lfsr (rnds_count: %d) %d\n", block_to_read, rnds_count, x);
        if (rnds [x].lfsr.state == lfsr_seed)
        {
            block_to_read = 0;
            //printf ("remove this one\n");
            for (int j = x; j < rnds_count; j++)
            {
                rnds [j] = rnds [j + 1];
            }
            rnds_count--;
            printf ("%llu block for lfsr (rnds_count: %d) %d\n", block_to_read, rnds_count, x);
            //printf ("rnds_count: %hhu\n", rnds_count);
        }
    }
    //printf ("i: %llu blocks: %llu\n", i, blocks);

    return 0;
}
