all: philox.c
		gcc -I../random123/include -I../random123/examples philox.c -o philox
		gcc MT19937.c -o MT19937
		gcc rand_r.c -o rand_r
		gcc xoroshiro128starstar.c -o xoroshiro128starstar
		gcc lcg.c -o lcg
		gcc lfsr.c -o lfsr
