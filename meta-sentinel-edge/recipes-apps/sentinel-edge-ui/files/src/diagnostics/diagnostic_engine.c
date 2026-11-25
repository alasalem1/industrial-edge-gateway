#include "diagnostic_engine.h"

int diagnostic_evaluate(
    const DiagnosticInput *input,
    DiagnosticResult *result)
{
    if (input == 0 || result == 0)
        return -1;

    result->severity = DIAGNOSTIC_NORMAL;
    result->faults = FAULT_NONE;

    /* Temperature thresholds: warning 65 C, critical 72 C */
    if (input->temperature_c >= 72.0) {
        result->faults |= FAULT_TEMPERATURE;
        result->severity = DIAGNOSTIC_CRITICAL;
    } else if (input->temperature_c >= 65.0) {
        result->faults |= FAULT_TEMPERATURE;
        result->severity = DIAGNOSTIC_WARNING;
    }

    /* Pressure thresholds: warning 7 bar, critical 7.8 bar */
    if (input->pressure_bar >= 7.8) {
        result->faults |= FAULT_PRESSURE;
        result->severity = DIAGNOSTIC_CRITICAL;
    } else if (input->pressure_bar >= 7.0) {
        result->faults |= FAULT_PRESSURE;
        if (result->severity < DIAGNOSTIC_WARNING)
            result->severity = DIAGNOSTIC_WARNING;
    }

    /* Vibration thresholds: warning 4 mm/s, critical 4.8 mm/s */
    if (input->vibration_mm_s >= 4.8) {
        result->faults |= FAULT_VIBRATION;
        result->severity = DIAGNOSTIC_CRITICAL;
    } else if (input->vibration_mm_s >= 4.0) {
        result->faults |= FAULT_VIBRATION;
        if (result->severity < DIAGNOSTIC_WARNING)
            result->severity = DIAGNOSTIC_WARNING;
    }

    /* Motor speed thresholds: warning 1650 RPM, critical 1780 RPM */
    if (input->motor_rpm >= 1780) {
        result->faults |= FAULT_MOTOR_SPEED;
        result->severity = DIAGNOSTIC_CRITICAL;
    } else if (input->motor_rpm >= 1650) {
        result->faults |= FAULT_MOTOR_SPEED;
        if (result->severity < DIAGNOSTIC_WARNING)
            result->severity = DIAGNOSTIC_WARNING;
    }

    return 0;
}
