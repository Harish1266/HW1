#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <immintrin.h>
#include <unistd.h>

#define ITERATIONS 1000000

void write_to_file(const char *filename, uint64_t *data, int count) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return;
    }
    for (int i = 0; i < count; ++i) {
        fprintf(file, "%llu\n", data[i]);
    }
    fclose(file);
}

int main()
{
    uint64_t start, end, start2, end2;
    uint32_t aux;
	volatile __m128 result, result2;
	uint64_t *times_before_sleep = (uint64_t *)calloc(ITERATIONS, sizeof(uint64_t));
    uint64_t *times_after_sleep = (uint64_t *)calloc(ITERATIONS, sizeof(uint64_t));

    if (times_before_sleep == NULL || times_after_sleep == NULL) {
        free(times_before_sleep);
        free(times_after_sleep);
        return 1;
	}

	 __m128 evens = _mm_set_ps(2.0, 4.0, 6.0, 8.0);
	 __m128 odds = _mm_set_ps(1.0, 3.0, 5.0, 7.0);

	for( int i=0; i< ITERATIONS; i++) 
	{
		 start = __rdtscp(&aux);
		 result = _mm_mul_ps(evens, odds);
		 end = __rdtscp(&aux);

		 times_before_sleep[i] = end - start;

		 usleep(100000);

		 start2 = __rdtscp(&aux);
		 result2 = _mm_mul_ps(evens, odds);
		 end2 = __rdtscp(&aux);

		 times_after_sleep[i] = end2 - start2;
	}

    write_to_file("avx_before_sleep.txt", times_before_sleep, ITERATIONS);
   	write_to_file("avx_after_sleep.txt", times_after_sleep, ITERATIONS);

    free(times_before_sleep);
    free(times_after_sleep);

    return 0;
}

