# SentinelEdge — AI Anomaly Detection Validation

## 1. Overview

This report documents functional validation of the SentinelEdge
industrial edge monitoring platform.

The validation was performed using the Yocto-built Linux image
running on QEMU (`qemux86-64`).

The application uses a native C Isolation Forest inference engine
integrated with the Qt/QML operations dashboard.

## 2. AI Configuration

| Parameter | Value |
|---|---|
| Inference implementation | Native C |
| Algorithm | Isolation Forest |
| Estimators | 128 |
| Maximum samples | 256 |
| Contamination | Auto |
| Random state | 42 |
| Anomaly threshold | -0.60417195075477803 |
| Input features | Temperature, pressure, vibration, motor RPM |

A sample is classified as an anomaly when its score is below
the configured threshold.

## 3. Test Environment

- Platform: SentinelEdge Industrial Edge Monitoring Platform
- Embedded Linux: Yocto Scarthgap
- Target machine: QEMU x86-64 (`qemux86-64`)
- Sensor source: Simulated C acquisition backend
- Inference: Local native C Isolation Forest
- Visualization: Qt 6 / QML dashboard
- Test date: 2026-11-26

No physical industrial sensors or machinery were used.

## 4. Fault Injection Results

Fault profiles were activated through the
`SENTINEL_EDGE_SIMULATION_FAULT` systemd environment variable.

| Test | Injected measurement | AI score | Dashboard result |
|---|---:|---:|---|
| Temperature | 75.0 °C | -0.5681 | NORMAL |
| Pressure | 8.2 bar | -0.5633 | NORMAL |
| Vibration | 5.2 mm/s | -0.6059 | ANOMALY |
| Motor speed | 1850 RPM | -0.6160 | ANOMALY |

### Observations

- All four fault profiles successfully changed the simulated telemetry.
- Vibration and motor faults crossed the configured anomaly threshold.
- Temperature and pressure faults did not cross the threshold at the
  tested values.
- The dashboard displayed the corresponding inference result.

## 5. Normal Operation and Recovery

After removing the temporary systemd fault override, the acquisition
service was restarted.

The dashboard returned to:

- Inference engine: NORMAL
- Diagnostic output: NO ANOMALY DETECTED
- Anomaly score: -0.5125

The normal-operation score was above the configured anomaly threshold.

This confirms recovery of the tested application pipeline after
fault injection was removed.

## 6. Validation Summary

| Validation item | Result |
|---|---|
| Yocto image boots in QEMU | PASS |
| Acquisition service runs | PASS |
| Simulated fault injection | PASS |
| Native C inference integration | PASS |
| Vibration anomaly detection | PASS |
| Motor anomaly detection | PASS |
| Temperature anomaly detection at tested value | NOT DETECTED |
| Pressure anomaly detection at tested value | NOT DETECTED |
| Dashboard returns to normal after fault removal | PASS |

## 7. Limitations and Next Work

These tests demonstrate functional integration using simulated
sensor data. They do not establish industrial field performance.

The current model did not detect the tested temperature and pressure
fault profiles. Further work should include:

1. Investigating feature distributions and anomaly scores.
2. Expanding representative normal and fault training datasets.
3. Evaluating detection sensitivity and false-positive rates.
4. Revalidating the model against a separate test dataset.
5. Testing with physical sensors and representative equipment.

The anomaly threshold should not be changed solely to force these
individual tests to pass. Any model or threshold adjustment must be
supported by measured validation results.

## 8. Conclusion

SentinelEdge successfully demonstrated an end-to-end embedded AI
diagnostic pipeline in a Yocto-based QEMU environment.

The simulated vibration and motor faults were detected by the
native C Isolation Forest and displayed by the Qt/QML dashboard.
The application also returned to normal status after fault injection
was removed.

This is functional prototype validation, not certification or
proof of production industrial reliability.
