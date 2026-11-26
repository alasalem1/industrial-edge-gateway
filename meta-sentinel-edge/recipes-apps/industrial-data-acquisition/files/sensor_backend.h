#ifndef SENSOR_BACKEND_H
#define SENSOR_BACKEND_H

typedef struct {
    double temperature_c;
    double pressure_bar;
    double vibration_mm_s;
    int motor_rpm;
} SensorReading;

int sensor_backend_init(void);
int sensor_backend_read(SensorReading *reading);
const char *sensor_backend_name(void);

int modbus_backend_init(void);
int modbus_backend_read(SensorReading *reading);
const char *modbus_backend_name(void);

#endif
