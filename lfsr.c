#include <stdio.h>
#include <stdint.h>

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
    {13, 4, {12,11,9,8}},     // 13
    {14, 4, {13,12,10,8}},     // 14
    {15, 2, {14,0}},    // 15
    {16, 4, {15,11,2,0}},    // 16
    {17, 2, {16,2}},    // 17
    {18, 2, {17,6}},    // 18
    {19, 4, {18,17,16,12}},    // 19
    {20, 2, {19,2}},    // 20
    {21, 2, {20,1}},    // 21
    {22, 2, {21,0}},    // 22
    {23, 2, {22,4}},    // 23
    {24, 4, {23,6,1,0}},    // 24
    {25, 2, {24,2}},    // 25
    {26, 4, {25,24,23,19}},    // 26
    {27, 4, {26,25,24,21}},    // 27
    {28, 2, {27,2}},    // 28
    {29, 2, {28,1}},    // 29
    {30, 4, {29,28,25,23}},    // 30
    {31, 2, {30,2}},    // 31
    {32, 4, {31,30,29,9}},     // 32
    {33, 2, {32, 12}},               // 33
    {34, 4, {33,32,31,6}}                // 34
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

    printf ("bits: %hhu mask: %llu state: %llu\n", lfsr->bits, lfsr->mask, lfsr->state);
    if (lfsr_configs [bits].num_taps >= 2)
    printf ("taps [0]: %d taps [1]: %d", lfsr_configs [lfsr->bits].taps [0], lfsr_configs [lfsr->bits].taps [1]);
    if (lfsr_configs [bits].num_taps == 4)
    printf (" taps [2]: %d taps [3]: %d\n", lfsr_configs [lfsr->bits].taps [2], lfsr_configs [lfsr->bits].taps [3]);
    else
    printf ("\n");
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

int main (void)
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

