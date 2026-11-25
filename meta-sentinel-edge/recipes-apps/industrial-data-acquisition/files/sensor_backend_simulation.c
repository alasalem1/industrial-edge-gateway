#include "sensor_backend.h"

#include <stdlib.h>
#include <time.h>
#include <errno.h>

int sensor_backend_init(void)
{
    srand((unsigned int)time(NULL));
    return 0;
}

int sensor_backend_read(SensorReading *reading)
{
    if (reading == NULL) {
        errno = EINVAL;
        return -1;
    }

    reading->temperature_c = 30.0 + (rand() % 451) / 10.0;
    reading->pressure_bar = 2.0 + (rand() % 61) / 10.0;
    reading->vibration_mm_s = 0.2 + (rand() % 49) / 10.0;
    reading->motor_rpm = 900 + (rand() % 901);

    return 0;
}

const char *sensor_backend_name(void)
{
    return "SIMULATED-C-ACQUISITION";
}
