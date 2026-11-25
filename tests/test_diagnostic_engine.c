#include <stdio.h>
#include <stdint.h>
#include "diagnostic_engine.h"

static int tests_run = 0;
static int tests_failed = 0;

static void check(
    const char *name,
    DiagnosticInput input,
    DiagnosticSeverity expected_severity,
    uint32_t expected_faults)
{
    DiagnosticResult result;

    tests_run++;

    if (diagnostic_evaluate(&input, &result) != 0 ||
        result.severity != expected_severity ||
        result.faults != expected_faults) {
        printf("FAIL: %s (severity=%d, faults=0x%02X)\n",
               name, result.severity, result.faults);
        tests_failed++;
        return;
    }

    printf("PASS: %s\n", name);
}

int main(void)
{
    DiagnosticInput normal = {40.0, 5.0, 2.0, 1200};

    check("Normal operation", normal,
          DIAGNOSTIC_NORMAL, FAULT_NONE);

    normal.temperature_c = 64.9;
    check("Temperature below warning", normal,
          DIAGNOSTIC_NORMAL, FAULT_NONE);

    normal.temperature_c = 65.0;
    check("Temperature warning boundary", normal,
          DIAGNOSTIC_WARNING, FAULT_TEMPERATURE);

    normal.temperature_c = 72.0;
    check("Temperature critical boundary", normal,
          DIAGNOSTIC_CRITICAL, FAULT_TEMPERATURE);

    normal = (DiagnosticInput){40.0, 7.0, 2.0, 1200};
    check("Pressure warning boundary", normal,
          DIAGNOSTIC_WARNING, FAULT_PRESSURE);

    normal.pressure_bar = 7.8;
    check("Pressure critical boundary", normal,
          DIAGNOSTIC_CRITICAL, FAULT_PRESSURE);

    normal = (DiagnosticInput){40.0, 5.0, 4.0, 1200};
    check("Vibration warning boundary", normal,
          DIAGNOSTIC_WARNING, FAULT_VIBRATION);

    normal.vibration_mm_s = 4.8;
    check("Vibration critical boundary", normal,
          DIAGNOSTIC_CRITICAL, FAULT_VIBRATION);

    normal = (DiagnosticInput){40.0, 5.0, 2.0, 1650};
    check("Motor speed warning boundary", normal,
          DIAGNOSTIC_WARNING, FAULT_MOTOR_SPEED);

    normal.motor_rpm = 1780;
    check("Motor speed critical boundary", normal,
          DIAGNOSTIC_CRITICAL, FAULT_MOTOR_SPEED);

    normal = (DiagnosticInput){65.0, 7.0, 4.0, 1650};
    check("Multiple simultaneous warnings", normal,
          DIAGNOSTIC_WARNING,
          FAULT_TEMPERATURE | FAULT_PRESSURE |
          FAULT_VIBRATION | FAULT_MOTOR_SPEED);

    normal = (DiagnosticInput){72.0, 5.0, 2.0, 1200};
    check("Critical severity takes priority", normal,
          DIAGNOSTIC_CRITICAL, FAULT_TEMPERATURE);

    tests_run++;
    if (diagnostic_evaluate(NULL, &(DiagnosticResult){0}) != -1) {
        printf("FAIL: Null input handling\n");
        tests_failed++;
    } else {
        printf("PASS: Null input handling\n");
    }

    tests_run++;
    if (diagnostic_evaluate(&normal, NULL) != -1) {
        printf("FAIL: Null result handling\n");
        tests_failed++;
    } else {
        printf("PASS: Null result handling\n");
    }

    printf("\nTests: %d | Passed: %d | Failed: %d\n",
           tests_run, tests_run - tests_failed, tests_failed);

    return tests_failed == 0 ? 0 : 1;
}
