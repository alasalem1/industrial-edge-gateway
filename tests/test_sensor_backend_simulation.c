#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "sensor_backend.h"
#include "diagnostic_engine.h"

static int tests_run = 0;
static int tests_failed = 0;

static void check(int condition, const char *name)
{
    tests_run++;

    if (condition) {
        printf("PASS: %s\n", name);
    } else {
        printf("FAIL: %s\n", name);
        tests_failed++;
    }
}

static void test_normal_profile(void)
{
    SensorReading reading;
    DiagnosticInput input;
    DiagnosticResult result;

    unsetenv("SENTINEL_EDGE_SIMULATION_FAULT");

    check(sensor_backend_init() == 0,
          "Normal profile initializes");

    int all_readings_normal = 1;
    int all_readings_bounded = 1;

    for (int i = 0; i < 60; i++) {
        if (sensor_backend_read(&reading) != 0) {
            all_readings_normal = 0;
            all_readings_bounded = 0;
            break;
        }

        input.temperature_c = reading.temperature_c;
        input.pressure_bar = reading.pressure_bar;
        input.vibration_mm_s = reading.vibration_mm_s;
        input.motor_rpm = reading.motor_rpm;

        if (diagnostic_evaluate(&input, &result) != 0 ||
            result.faults != FAULT_NONE) {
            all_readings_normal = 0;
        }

        if (reading.temperature_c < 40.0 ||
            reading.temperature_c > 44.0 ||
            reading.pressure_bar < 4.6 ||
            reading.pressure_bar > 5.4 ||
            reading.vibration_mm_s < 1.2 ||
            reading.vibration_mm_s > 1.8 ||
            reading.motor_rpm < 1200 ||
            reading.motor_rpm > 1300) {
            all_readings_bounded = 0;
        }
    }

    check(all_readings_normal,
          "60 normal samples produce no diagnostic faults");

    check(all_readings_bounded,
          "Normal samples stay within bounded ranges");
}

static void test_fault_profile(const char *profile,
                               uint32_t expected_fault,
                               const char *name)
{
    SensorReading reading;
    DiagnosticInput input;
    DiagnosticResult result;

    int environment_ok =
        setenv("SENTINEL_EDGE_SIMULATION_FAULT",
               profile, 1) == 0;

    check(environment_ok, name);

    if (!environment_ok)
        return;

    int init_ok = sensor_backend_init() == 0;
    check(init_ok, "Fault profile initializes");

    if (!init_ok)
        return;

    int read_ok = sensor_backend_read(&reading) == 0;
    check(read_ok, "Fault profile produces a reading");

    if (!read_ok)
        return;

    input.temperature_c = reading.temperature_c;
    input.pressure_bar = reading.pressure_bar;
    input.vibration_mm_s = reading.vibration_mm_s;
    input.motor_rpm = reading.motor_rpm;

    int evaluation_ok =
        diagnostic_evaluate(&input, &result) == 0;

    check(evaluation_ok, "Fault reading evaluates successfully");

    if (evaluation_ok) {
        check(result.faults == expected_fault &&
              result.severity == DIAGNOSTIC_CRITICAL,
              "Injected fault triggers expected critical diagnostic");
    }
}

int main(void)
{
    test_normal_profile();

    test_fault_profile("temperature", FAULT_TEMPERATURE,
                       "Temperature fault profile selected");

    test_fault_profile("pressure", FAULT_PRESSURE,
                       "Pressure fault profile selected");

    test_fault_profile("vibration", FAULT_VIBRATION,
                       "Vibration fault profile selected");

    test_fault_profile("motor", FAULT_MOTOR_SPEED,
                       "Motor fault profile selected");

    unsetenv("SENTINEL_EDGE_SIMULATION_FAULT");

    printf("\nTests: %d | Passed: %d | Failed: %d\n",
           tests_run, tests_run - tests_failed, tests_failed);

    return tests_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
