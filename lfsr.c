#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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
    bool seen[64] = { false };

    printf("6-bit 4-tap maximum-period LFSR\n");
    printf("Polynomial: x^6 + x^5 + x^3 + x^2 + 1\n\n");

    int count = 0;

    do {
        if (seen[lfsr.state]) {
            printf("ERROR: repeated state before full period: %u\n", lfsr.state);
            return 1;
        }

        seen[lfsr.state] = true;

        //printf("%2d: %2u\n", count, lfsr.state);
        printf("%2u\n", lfsr.state);

        lfsr6_step(&lfsr);
        count++;

    } while (lfsr.state != initial);

    printf("\nUnique nonzero states: %d\n", count);
    printf("Returned to initial state after %d steps.\n", count);
    printf("Initial state again would be: %u\n", lfsr.state);

    return 0;
}
