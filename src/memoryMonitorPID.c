#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define DELAY_SECONDS 0.01

// Function to read memory usage of a specific process ID
unsigned long long read_process_memory_usage(int pid) {
    char statm_path[256];
    sprintf(statm_path, "/proc/%d/statm", pid);

    FILE* file = fopen(statm_path, "r");
    if (!file) {
        perror("Error opening statm file");
        exit(EXIT_FAILURE);
    }

    unsigned long long mem_size;
    // Read the first value in statm file, which represents the total program size
    fscanf(file, "%llu", &mem_size);

    fclose(file);

    // Convert pages to bytes (assuming page size is 4KB)
    unsigned long long mem_usage_bytes = mem_size * 4096;

    return mem_usage_bytes;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <pid> <output_file.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pid = atoi(argv[1]);
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    fprintf(output_file, "Time,Memory Usage (bytes)\n");

    while (1) {
        unsigned long long mem_usage = read_process_memory_usage(pid);
        fprintf(output_file, "%ld,%llu\n", time(NULL), mem_usage);
        fflush(output_file);
        usleep(DELAY_SECONDS * 1000000); // usleep takes microseconds
    }

    fclose(output_file);

    return EXIT_SUCCESS;
}
