#include "sensor_backend.h"

#include <modbus/modbus.h>

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_PORT 502
#define DEFAULT_UNIT_ID 1

#define REGISTER_START 0
#define REGISTER_COUNT 4

static modbus_t *context = NULL;
static int connected = 0;

static int read_integer_environment(
    const char *name,
    int default_value,
    int minimum,
    int maximum,
    int *result)
{
    const char *value = getenv(name);

    if (value == NULL || value[0] == '\0') {
        *result = default_value;
        return 0;
    }

    char *end = NULL;
    errno = 0;
    long parsed = strtol(value, &end, 10);

    if (errno != 0 || end == value || *end != '\0' ||
        parsed < minimum || parsed > maximum) {
        errno = EINVAL;
        return -1;
    }

    *result = (int)parsed;
    return 0;
}

int modbus_backend_init(void)
{
    const char *host = getenv("SENTINEL_EDGE_MODBUS_HOST");

    if (host == NULL || host[0] == '\0')
        host = DEFAULT_HOST;

    int port;
    int unit_id;

    if (read_integer_environment(
            "SENTINEL_EDGE_MODBUS_PORT",
            DEFAULT_PORT, 1, 65535, &port) != 0)
        return -1;

    if (read_integer_environment(
            "SENTINEL_EDGE_MODBUS_UNIT_ID",
            DEFAULT_UNIT_ID, 0, 247, &unit_id) != 0)
        return -1;

    context = modbus_new_tcp(host, port);

    if (context == NULL)
        return -1;

    if (modbus_set_slave(context, unit_id) == -1 ||
        modbus_set_response_timeout(context, 1, 0) == -1) {
        modbus_free(context);
        context = NULL;
        return -1;
    }

    connected = 0;
    return 0;
}

int modbus_backend_read(SensorReading *reading)
{
    if (reading == NULL || context == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (!connected) {
        if (modbus_connect(context) == -1)
            return -1;

        connected = 1;
    }

    uint16_t registers[REGISTER_COUNT];

    int count = modbus_read_registers(
        context,
        REGISTER_START,
        REGISTER_COUNT,
        registers
    );

    if (count != REGISTER_COUNT) {
        modbus_close(context);
        connected = 0;
        return -1;
    }

    /*
     * SentinelEdge simulated PLC register map:
     * Address 0: temperature in 0.1 degrees C
     * Address 1: pressure in 0.1 bar
     * Address 2: vibration in 0.1 mm/s
     * Address 3: motor speed in RPM
     */
    reading->temperature_c = registers[0] / 10.0;
    reading->pressure_bar = registers[1] / 10.0;
    reading->vibration_mm_s = registers[2] / 10.0;
    reading->motor_rpm = registers[3];

    return 0;
}

const char *modbus_backend_name(void)
{
    return "MODBUS-TCP";
}
