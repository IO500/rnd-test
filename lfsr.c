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
    {0, 1, {0}},               // 0
    {1, 1, {0}},               // 1
    {2, 2, {2,1}},             // 2
    {3, 2, {3,2}},             // 3
    {4 ,2, {4,3}},             // 4
    {5, 4, {5,4,3,2}},         // 5
    {6, 4, {6,5,3,2}},         // 6
    {7, 4, {7,6,5,4}},         // 7
    {8, 4, {8,6,5,4}},         // 8
    {9, 4, {9,8,6,5}},         // 9
    {10, 4, {10,9,7,6}},       // 10
    {11, 4, {11,10,9,7}},      // 11
    {12, 4, {12,11,8,6}},      // 12
    {13, 4, {13,12,10,9}},     // 13
    {14, 4, {14,13,11,9}},     // 14
    {15, 4, {15,14,13,11}},    // 15
    {16, 4, {16,14,13,11}},    // 16
    {17, 4, {17,16,15,14}},    // 17
    {18, 4, {18,17,16,13}},    // 18
    {19, 4, {19,18,17,14}},    // 19
    {20, 4, {20,19,16,14}},    // 20
    {21, 4, {21,20,19,16}},    // 21
    {22, 4, {22,19,18,17}},    // 22
    {23, 4, {23,22,20,18}},    // 23
    {24, 4, {24,23,21,20}},    // 24
    {25, 4, {25,24,23,22}},    // 25
    {26, 4, {26,25,24,20}},    // 26
    {27, 4, {27,26,25,22}},    // 27
    {28, 4, {28,27,24,22}},    // 28
    {29, 4, {29,28,27,25}},    // 29
    {30, 4, {30,29,26,24}},    // 30
    {31, 4, {31,30,29,28}},    // 31
    {32, 4, {32,30,26,25}}     // 32
};

#define NUM_LFSR_CONFIGS (sizeof(lfsr_configs)/sizeof(lfsr_configs[0]))

#define LFSR_BITS 6
#define LFSR_MASK ((1u << LFSR_BITS) - 1u)  // 0x3F = 63

typedef struct {
    uint8_t state;  // lower 6 bits used, range 0..63
} LFSR6;

// Initialize with nonzero 6-bit seed
void lfsr6_init(LFSR6 *lfsr, uint8_t seed) {
    seed &= LFSR_MASK;

    if (seed == 0) {
        seed = 1;  // all-zero state is invalid for maximal LFSR
    }

    lfsr->state = seed;
}

// 6-bit, 4-tap, left-shifting maximal LFSR
// Polynomial: x^6 + x^5 + x^3 + x^2 + 1
// Feedback taps: bits 5, 3, 2, 0
uint8_t lfsr6_step(LFSR6 *lfsr) {
    uint8_t s = lfsr->state;

    uint8_t feedback =
        ((s >> 5) ^   // x^5 tap
         (s >> 3) ^   // x^3 tap
         (s >> 2) ^   // x^2 tap
         (s >> 0))    // x^0 tap
        & 1u;

    lfsr->state = ((s << 1) & LFSR_MASK) | feedback;

    return lfsr->state;
}

int main(void) {
    LFSR6 lfsr;
    lfsr6_init(&lfsr, 1);

    uint8_t initial = lfsr.state;

    int count = 0;

    do {
        printf("%2u\n", lfsr.state);

        lfsr6_step(&lfsr);

    } while (lfsr.state != initial);

    return 0;
}

// Generate full period output for LFSR of given config
void generate_lfsr_sequence(const LFSRConfig *config)
{
    uint64_t state = 0xdeadc0dedeadc0de; // non-zero seed
    uint64_t mask = ((uint64_t)1 << config->bit_length) - 1;
    uint64_t period = ((uint64_t)1 << config->bit_length) - 1;

    printf("%d-bit LFSR sequence (period %llu):\n", config->bit_length, period);

    size_t buf_len = 256;
    char buf [buf_len];
    snprintf (buf, buf_len, "%u.txt", config->bit_length);
    FILE * f = fopen (buf, "w");
    //FILE * f = stdout;
    for (uint64_t i = 0; i < period; i++)
    {
        printf ("bit length: %d\n", config->bit_length);
        //uint8_t tap0 = config->bit_length - config->taps [0];
        //uint8_t tap1 = config->bit_length - config->taps [1];
        //uint8_t tap2 = config->bit_length - config->taps [2];
        //uint8_t tap3 = config->bit_length - config->taps [3];
        uint8_t tap0 = config->taps [0];
        uint8_t tap1 = config->taps [1];
        uint8_t tap2 = config->taps [2];
        uint8_t tap3 = config->taps [3];
        printf ("taps: %hhu, %hhu, %hhu, %hhu\n", tap0, tap1, tap2, tap3);
        uint64_t out_bit = ((state >> tap0) ^ (state >> tap1) ^ (state >> tap2) ^ (state >> tap3)) & 1u;
        state = (state >> 1) | (out_bit << config->bit_length);
        state &= mask;
        fprintf (f, "%llu\n", state);
    }
    fclose (f);
}

int main2()
{
    // The min and max 4KB blocks based on file sizes driven by a network rate and 300 seconds
    uint64_t min = 1024l * 1024 * 300 / 4096; // 1 MB/sec for 300 seconds
    uint64_t max = 1024l * 1024 * 1024 * 200 * 300 / 4096; // 200 GB/sec for 300 seconds
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

    //for (int i = 0; i < NUM_LFSR_CONFIGS; i++)
    //for (int i = 0; i < 10; i++)
    int i = 3;
    {
        generate_lfsr_sequence(&lfsr_configs[i]);
    }
    return 0;
}

