#ifndef DIAGNOSTIC_ENGINE_H
#define DIAGNOSTIC_ENGINE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double temperature_c;
    double pressure_bar;
    double vibration_mm_s;
    int motor_rpm;
} DiagnosticInput;

typedef enum {
    DIAGNOSTIC_NORMAL = 0,
    DIAGNOSTIC_WARNING = 1,
    DIAGNOSTIC_CRITICAL = 2
} DiagnosticSeverity;

enum {
    FAULT_NONE        = 0,
    FAULT_TEMPERATURE = 1U << 0,
    FAULT_PRESSURE    = 1U << 1,
    FAULT_VIBRATION   = 1U << 2,
    FAULT_MOTOR_SPEED = 1U << 3
};

typedef struct {
    DiagnosticSeverity severity;
    uint32_t faults;
} DiagnosticResult;

int diagnostic_evaluate(
    const DiagnosticInput *input,
    DiagnosticResult *result
);

#ifdef __cplusplus
}
#endif

#endif
