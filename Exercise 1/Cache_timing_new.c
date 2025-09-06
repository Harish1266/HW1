#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <string.h>

#define ITERATIONS 1000000
#define NUM_SAMPLES 1100000

volatile int target_byte __attribute__((aligned(64)));

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

int main() {
    uint64_t start, end;
    uint32_t aux;
    volatile int temp_hit, temp_miss;

    uint64_t *hit_times_raw = (uint64_t *)calloc(NUM_SAMPLES, sizeof(uint64_t));
    uint64_t *miss_times_raw = (uint64_t *)calloc(NUM_SAMPLES, sizeof(uint64_t));
    
    uint64_t *temp_hit_times = (uint64_t *)calloc(NUM_SAMPLES, sizeof(uint64_t));
    uint64_t *temp_miss_times = (uint64_t *)calloc(NUM_SAMPLES, sizeof(uint64_t));

    if (hit_times_raw == NULL || miss_times_raw == NULL || temp_hit_times == NULL || temp_miss_times == NULL) {
        perror("Memory allocation failed");
        free(hit_times_raw);
        free(miss_times_raw);
        free(temp_hit_times);
        free(temp_miss_times);
        return 1;
    }

    target_byte = 1;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        temp_hit = target_byte;
        _mm_mfence();
        start = __rdtscp(&aux);
        temp_hit = target_byte;
        end = __rdtscp(&aux);
        hit_times_raw[i] = end - start;

        _mm_clflush((void *)&target_byte);
        _mm_mfence();
        start = __rdtscp(&aux);
        temp_miss = target_byte;
        end = __rdtscp(&aux);
        miss_times_raw[i] = end - start;
    }

    write_to_file("hit_times_raw_1.txt", hit_times_raw, NUM_SAMPLES);
    write_to_file("miss_times_raw_1.txt", miss_times_raw, NUM_SAMPLES);

    uint64_t hit_threshold = 150;
    uint64_t miss_threshold = 850;

    write_filtered_to_file("hit_times_new_1.txt", hit_times_raw, ITERATIONS, hit_threshold);
    write_filtered_to_file("miss_times_new_1.txt", miss_times_raw, ITERATIONS, miss_threshold);
    
    free(hit_times_raw);
    free(miss_times_raw);
    free(temp_hit_times);
    free(temp_miss_times);

    return 0;
}
