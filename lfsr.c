#include <stdio.h>
#include <stdint.h>

#if LFSR6BIT
// 6-bit LFSR with taps at bit 6 and bit 5 (polynomial x^6 + x^5 + 1)
uint8_t lfsr6(uint8_t state) {
    // Extract the bits at positions 6 and 5 (bit 5 and bit 4 zero-indexed)
    // Bits are numbered from 0 (LSB) to 5 (MSB)
    uint8_t bit6 = (state >> 5) & 1;
    uint8_t bit5 = (state >> 4) & 1;

    // XOR the tapped bits to get the new input bit
    uint8_t new_bit = bit6 ^ bit5;

    // Shift left by 1 and insert new_bit at LSB
    state = ((state << 1) & 0x3F) | new_bit; // mask to 6 bits

    return state;
}

int main() {
    uint8_t state = 0x01; // initial non-zero seed (cannot be zero)

    printf("6-bit LFSR sequence:\n");
    for (int i = 0; i < 63; i++) { // maximal length for 6-bit LFSR is 2^6 - 1 = 63
        printf("%02X ", state);
        state = lfsr6(state);
    }
    printf("\n");

    return 0;
}
#endif

#if LFSR23BIT
#include <stdio.h>
#include <stdint.h>

// 23-bit LFSR with taps at bit 23 and bit 18 (polynomial x^23 + x^18 + 1)
uint32_t lfsr23(uint32_t state) {
    // Extract bits at positions 22 and 17 (zero-indexed)
    uint32_t bit23 = (state >> 22) & 1;
    uint32_t bit18 = (state >> 17) & 1;

    // XOR the tapped bits to get the new input bit
    uint32_t new_bit = bit23 ^ bit18;

    // Shift left by 1 and insert new_bit at LSB
    state = ((state << 1) & 0x7FFFFF) | new_bit; // mask to 23 bits (0x7FFFFF = 23 ones)

    return state;
}

int main() {
    uint32_t state = 0x1; // initial non-zero seed (cannot be zero)

    printf("23-bit LFSR sequence (first 30 values):\n");
    for (int i = 0; i < 30; i++) {
        printf("%06X\n", state);
        state = lfsr23(state);
    }

    return 0;
}
#endif

#if LFSR16BIT
#include <stdio.h>
#include <stdint.h>

// 16-bit LFSR with taps at bits 16, 14, 13, and 11 (polynomial x^16 + x^14 + x^13 + x^11 + 1)
uint16_t lfsr16(uint16_t state) {
    // Extract tapped bits (zero-indexed)
    uint16_t bit16 = (state >> 15) & 1;
    uint16_t bit14 = (state >> 13) & 1;
    uint16_t bit13 = (state >> 12) & 1;
    uint16_t bit11 = (state >> 10) & 1;

    // XOR the tapped bits to get the new input bit
    uint16_t new_bit = bit16 ^ bit14 ^ bit13 ^ bit11;

    // Shift left by 1 and insert new_bit at LSB
    state = ((state << 1) & 0xFFFF) | new_bit; // mask to 16 bits

    return state;
}

int main() {
    uint16_t state = 0x1; // initial non-zero seed (cannot be zero)

    printf("16-bit LFSR sequence (first 30 values):\n");
    for (int i = 0; i < 30; i++) {
        printf("%04X\n", state);
        state = lfsr16(state);
    }

    return 0;
}
#endif

// the table of which bit taps for various lengths give maximum length period
// https://datacipy.elektroniche.cz/lfsr_table.pdf
// starts at 2 (2 bits numbered 1-2)
// dummies added to make offsetting into the array easier
// the first digit is how many taps. The rest are the taps or 0 for not a tap
static int BITS [33][5] =
{
    {0,0,0,0,0},        // 0
    {0,0,0,0,0},        // 1
    {2,2,1,0,0},        // 2
    {2,3,2,0,0},        // 3
    {2,4,3,0,0},        // 4
    {4,5,4,3,2},        // 5
    {4,6,5,3,2},        // 6
    {4,7,6,5,4},        // 7
    {4,8,6,5,4},        // 8
    {4,9,8,6,5},        // 9
    {4,10,9,7,6},       // 10
    {4,11,10,9,7},      // 11
    {4,12,11,8,6},      // 12
    {4,13,12,10,9},     // 13
    {4,14,13,11,9},     // 14
    {4,15,14,13,11},    // 15
    {4,16,14,13,11},    // 16
    {4,17,16,15,14},    // 17
    {4,18,17,16,13},    // 18
    {4,19,18,17,14},    // 19
    {4,20,19,16,14},    // 20
    {4,21,20,19,16},    // 21
    {4,22,19,18,17},    // 22
    {4,23,22,20,18},    // 23
    {4,24,23,21,20},    // 24
    {4,25,24,23,22},    // 25
    {4,26,25,24,20},    // 26
    {4,27,26,25,22},    // 27
    {4,28,27,24,22},    // 28
    {4,29,28,27,25},    // 29
    {4,30,29,26,24},    // 30
    {4,31,30,29,28},    // 31
    {4,32,30,26,25}     // 32
};
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
