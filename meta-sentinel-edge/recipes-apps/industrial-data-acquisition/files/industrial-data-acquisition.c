#define _POSIX_C_SOURCE 200809L

#include "sensor_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>

#define DEFAULT_OUTPUT "/run/sentinel-edge/telemetry.json"

#define TEMP_LIMIT 80.0
#define PRESSURE_LIMIT 10.0

typedef int (*BackendInitFn)(void);
typedef int (*BackendReadFn)(SensorReading *);
typedef const char *(*BackendNameFn)(void);

static BackendInitFn backend_init_fn;
static BackendReadFn backend_read_fn;
static BackendNameFn backend_name_fn;

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

    if (separator == NULL || separator == directory)
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
    const SensorReading *reading)
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
        backend_name_fn(),
        reading->temperature_c,
        reading->pressure_bar,
        reading->vibration_mm_s,
        reading->motor_rpm
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

    const char *requested_backend = getenv("SENTINEL_EDGE_BACKEND");

    if (requested_backend == NULL || requested_backend[0] == '\0' ||
        strcmp(requested_backend, "simulation") == 0) {
        backend_init_fn = sensor_backend_init;
        backend_read_fn = sensor_backend_read;
        backend_name_fn = sensor_backend_name;
    } else if (strcmp(requested_backend, "modbus") == 0) {
        backend_init_fn = modbus_backend_init;
        backend_read_fn = modbus_backend_read;
        backend_name_fn = modbus_backend_name;
    } else {
        fprintf(stderr, "Unsupported backend: %s\n", requested_backend);
        return EXIT_FAILURE;
    }

    if (backend_init_fn() != 0) {
        perror("Sensor backend initialization failed");
        return EXIT_FAILURE;
    }

    printf("SentinelEdge Industrial Data Acquisition\n");
    printf("Backend: %s\n", backend_name_fn());
    printf("Output: %s\n", output_path);
    printf("Publishing interval: 2 seconds\n");

    while (1) {
        SensorReading reading;

        if (backend_read_fn(&reading) != 0) {
            perror("Sensor acquisition failed");
            sleep(2);
            continue;
        }

        if (publish_telemetry(output_path, &reading) != 0) {
            fprintf(stderr, "Failed to publish telemetry\n");
        } else {
            printf(
                "Temperature: %.1f C | "
                "Pressure: %.1f bar | "
                "Vibration: %.1f mm/s | "
                "Motor: %d RPM\n",
                reading.temperature_c,
                reading.pressure_bar,
                reading.vibration_mm_s,
                reading.motor_rpm
            );
        }

        if (reading.temperature_c > TEMP_LIMIT)
            printf("ALARM: High temperature!\n");

        if (reading.pressure_bar > PRESSURE_LIMIT)
            printf("ALARM: High pressure!\n");

        sleep(2);
    }

    return EXIT_SUCCESS;
}
