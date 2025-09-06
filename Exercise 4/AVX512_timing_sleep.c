#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <immintrin.h>
#include <unistd.h>

#define ITERATIONS 1000000

// Function to write the timing data to a file.
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
		volatile __m512 result, result2;
		uint64_t *times_before_sleep = (uint64_t *)calloc(ITERATIONS, sizeof(uint64_t));
    	uint64_t *times_after_sleep = (uint64_t *)calloc(ITERATIONS, sizeof(uint64_t));

    	if (times_before_sleep == NULL || times_after_sleep == NULL) {
        	free(times_before_sleep);
        	free(times_after_sleep);
        	return 1;
    	}

	__m512 evens = _mm512_set_ps(2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0, 22.0, 24.0, 26.0, 28.0, 30.0, 32.0);
    __m512 odds = _mm512_set_ps(1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0, 15.0, 17.0, 19.0, 21.0, 23.0, 25.0, 27.0, 29.0, 31.0);

	for(int i=0; i<ITERATIONS; i++)
	{
        start = __rdtscp(&aux);
        result = _mm512_mul_ps(evens, odds);
        end = __rdtscp(&aux);

		times_before_sleep[i] = end - start;

		usleep(100000);

		start2 = __rdtscp(&aux);
		result2 = _mm512_mul_ps(evens, odds);
		end2 = __rdtscp(&aux);

		times_after_sleep[i] = end2 - start2;
	}

    	write_to_file("avx_before_sleep.txt", times_before_sleep, ITERATIONS);
   		write_to_file("avx_after_sleep.txt", times_after_sleep, ITERATIONS);

    	free(times_before_sleep);
    	free(times_after_sleep);

        return 0;
}
