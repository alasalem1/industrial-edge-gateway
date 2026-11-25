#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#define DEFAULT_OUTPUT "/run/sentinel-edge/telemetry.json"
#define SOURCE_NAME "SIMULATED-C-ACQUISITION"

#define TEMP_LIMIT 80.0
#define PRESSURE_LIMIT 10.0

static long long current_timestamp_ms(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
        return -1;

    return (long long)ts.tv_sec * 1000LL +
           ts.tv_nsec / 1000000LL;
}

static int create_parent_directory(const char *path)
{
    char directory[1024];

    if (strlen(path) >= sizeof(directory))
        return -1;

    strcpy(directory, path);

    char *separator = strrchr(directory, '/');

    if (separator == NULL)
        return 0;

    if (separator == directory)
        return 0;

    *separator = '\0';

    if (mkdir(directory, 0755) != 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    return 0;
}

static int publish_telemetry(
    const char *output_path,
    double temperature,
    double pressure,
    double vibration,
    int motor_rpm)
{
    char temporary_path[1100];

    if (snprintf(temporary_path, sizeof(temporary_path),
                 "%s.tmp", output_path) >=
        (int)sizeof(temporary_path)) {
        fprintf(stderr, "Output path is too long\n");
        return -1;
    }

    long long timestamp = current_timestamp_ms();

    if (timestamp < 0) {
        perror("clock_gettime");
        return -1;
    }

    FILE *file = fopen(temporary_path, "w");

    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    int result = fprintf(
        file,
        "{\n"
        "  \"schema_version\": 1,\n"
        "  \"timestamp\": %lld,\n"
        "  \"source\": \"%s\",\n"
        "  \"temperature_c\": %.1f,\n"
        "  \"pressure_bar\": %.1f,\n"
        "  \"vibration_mm_s\": %.1f,\n"
        "  \"motor_rpm\": %d\n"
        "}\n",
        timestamp,
        SOURCE_NAME,
        temperature,
        pressure,
        vibration,
        motor_rpm
    );

    if (result < 0 || fflush(file) != 0 ||
        fsync(fileno(file)) != 0) {
        perror("telemetry write");
        fclose(file);
        unlink(temporary_path);
        return -1;
    }

    if (fclose(file) != 0) {
        perror("fclose");
        unlink(temporary_path);
        return -1;
    }

    if (rename(temporary_path, output_path) != 0) {
        perror("rename");
        unlink(temporary_path);
        return -1;
    }

    return 0;
}

int main(void)
{
    const char *output_path =
        getenv("SENTINEL_EDGE_TELEMETRY_FILE");

    if (output_path == NULL || output_path[0] == '\0')
        output_path = DEFAULT_OUTPUT;

    if (create_parent_directory(output_path) != 0) {
        fprintf(stderr, "Cannot create telemetry directory\n");
        return EXIT_FAILURE;
    }

    srand((unsigned int)time(NULL));

    printf("SentinelEdge Industrial Data Acquisition\n");
    printf("Source: %s\n", SOURCE_NAME);
    printf("Output: %s\n", output_path);
    printf("Publishing interval: 2 seconds\n");

    while (1) {
        double temperature = 30.0 + (rand() % 451) / 10.0;
        double pressure = 2.0 + (rand() % 61) / 10.0;
        double vibration = 0.2 + (rand() % 49) / 10.0;
        int motor_rpm = 900 + (rand() % 901);

        if (publish_telemetry(
                output_path,
                temperature,
                pressure,
                vibration,
                motor_rpm) != 0) {
            fprintf(stderr, "Failed to publish telemetry\n");
        } else {
            printf(
                "Temperature: %.1f C | "
                "Pressure: %.1f bar | "
                "Vibration: %.1f mm/s | "
                "Motor: %d RPM\n",
                temperature,
                pressure,
                vibration,
                motor_rpm
            );
        }

        if (temperature > TEMP_LIMIT)
            printf("ALARM: High temperature!\n");

        if (pressure > PRESSURE_LIMIT)
            printf("ALARM: High pressure!\n");

        sleep(2);
    }

    return EXIT_SUCCESS;
}
