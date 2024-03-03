#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define DELAY_SECONDS 0.01

// Function to read CPU usage of a specific process ID
float read_process_cpu_usage(int pid) {
    char stat_path[256];
    sprintf(stat_path, "/proc/%d/stat", pid);

    FILE* file = fopen(stat_path, "r");
    if (!file) {
        perror("Error opening stat file");
        exit(EXIT_FAILURE);
    }

    // Variables to store values read from /proc/[pid]/stat
    unsigned long utime, stime, cutime, cstime;
    fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %lu %lu %lu %lu",
           &utime, &stime, &cutime, &cstime);

    fclose(file);

    // Calculate total time spent by the process
    unsigned long total_time = utime + stime + cutime + cstime;

    // Calculate process CPU usage percentage
    float cpu_usage = total_time / (float) sysconf(_SC_CLK_TCK);
    return cpu_usage;
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

    fprintf(output_file, "Time,CPU Usage\n");

    while (1) {
        float cpu_usage = read_process_cpu_usage(pid);
        fprintf(output_file, "%ld,%f\n", time(NULL), cpu_usage);
        fflush(output_file);
        usleep(DELAY_SECONDS * 1000000); // usleep takes microseconds
    }

    fclose(output_file);

    return EXIT_SUCCESS;
}
