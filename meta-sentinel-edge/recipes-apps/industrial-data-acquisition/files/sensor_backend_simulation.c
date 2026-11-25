#include "sensor_backend.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef enum {
    SIMULATION_NORMAL,
    SIMULATION_TEMPERATURE_FAULT,
    SIMULATION_PRESSURE_FAULT,
    SIMULATION_VIBRATION_FAULT,
    SIMULATION_MOTOR_FAULT
} SimulationProfile;

static SimulationProfile profile = SIMULATION_NORMAL;
static unsigned int sample_count = 0;

/*
 * Deterministic triangular variation.
 * Returns values between -amplitude and +amplitude.
 */
static int triangle_wave(unsigned int sample,
                         unsigned int period,
                         int amplitude)
{
    unsigned int phase = sample % period;
    unsigned int half = period / 2;

    if (phase <= half) {
        return -amplitude +
            (int)((2 * amplitude * phase) / half);
    }

    return amplitude -
        (int)((2 * amplitude * (phase - half)) / half);
}

int sensor_backend_init(void)
{
    const char *requested_profile =
        getenv("SENTINEL_EDGE_SIMULATION_FAULT");

    profile = SIMULATION_NORMAL;
    sample_count = 0;

    if (requested_profile == NULL ||
        requested_profile[0] == '\0' ||
        strcmp(requested_profile, "none") == 0) {
        return 0;
    }

    if (strcmp(requested_profile, "temperature") == 0) {
        profile = SIMULATION_TEMPERATURE_FAULT;
    } else if (strcmp(requested_profile, "pressure") == 0) {
        profile = SIMULATION_PRESSURE_FAULT;
    } else if (strcmp(requested_profile, "vibration") == 0) {
        profile = SIMULATION_VIBRATION_FAULT;
    } else if (strcmp(requested_profile, "motor") == 0) {
        profile = SIMULATION_MOTOR_FAULT;
    } else {
        errno = EINVAL;
        return -1;
    }

    return 0;
}

int sensor_backend_read(SensorReading *reading)
{
    if (reading == NULL) {
        errno = EINVAL;
        return -1;
    }

    unsigned int sample = sample_count++;

    /* Stable normal operating conditions with bounded variation. */
    reading->temperature_c =
        42.0 + triangle_wave(sample, 40, 2) * 0.5;

    reading->pressure_bar =
        5.0 + triangle_wave(sample + 7, 30, 3) * 0.1;

    reading->vibration_mm_s =
        1.5 + triangle_wave(sample + 11, 24, 2) * 0.1;

    reading->motor_rpm =
        1250 + triangle_wave(sample + 5, 36, 50);

    /* Deliberate fault injection for diagnostic validation. */
    switch (profile) {
    case SIMULATION_TEMPERATURE_FAULT:
        reading->temperature_c = 75.0;
        break;

    case SIMULATION_PRESSURE_FAULT:
        reading->pressure_bar = 8.2;
        break;

    case SIMULATION_VIBRATION_FAULT:
        reading->vibration_mm_s = 5.2;
        break;

    case SIMULATION_MOTOR_FAULT:
        reading->motor_rpm = 1850;
        break;

    case SIMULATION_NORMAL:
    default:
        break;
    }

    return 0;
}

const char *sensor_backend_name(void)
{
    return "SIMULATED-C-ACQUISITION";
}
