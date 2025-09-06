#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <immintrin.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 1000000
#define NUM_SAMPLES 1100000

void write_filtered_to_file(const char *filename, uint64_t *data, int count, uint64_t threshold) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    int filtered_count = 0;
    for (int i = 0; i < NUM_SAMPLES && filtered_count < count; ++i) {
        if (data[i] <= threshold) {
            fprintf(file, "%llu\n", data[i]);
            filtered_count++;
        }
    }
    fclose(file);
}

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
        uint64_t start, end;
        uint32_t aux;
        uint64_t *avx_times = (uint64_t *)calloc(NUM_SAMPLES, sizeof(uint64_t));
        uint64_t *avx_raw = (uint64_t *)calloc(NUM_SAMPLES, sizeof(uint64_t));

        if (avx_times == NULL || avx_raw == NULL) {
                perror("Memory allocation failed");
                free(avx_times);
                free(avx_raw);
                return 1;
        }

        __m512 evens = _mm512_set_ps(2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0, 18.0, 20.0, 22.0, 24.0, 26.0, 28.0, 30.0, 32.0);
        __m512 odds = _mm512_set_ps(1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0, 15.0, 17.0, 19.0, 21.0, 23.0, 25.0, 27.0, 29.0, 31.0);

        volatile __m512 result;

        for(int i=0; i<NUM_SAMPLES; i++)
        {
                start = __rdtscp(&aux);

                result = _mm512_mul_ps(evens, odds);

                end = __rdtscp(&aux);

                avx_raw[i] = end - start;
        }

	write_to_file("avx512_raw_1.txt", avx_raw, NUM_SAMPLES);

        uint64_t threshold = 150;

        write_filtered_to_file("avx_times_new_1.txt", avx_raw, ITERATIONS, threshold);

        free(avx_times);
        free(avx_raw);

        return 0;
}

