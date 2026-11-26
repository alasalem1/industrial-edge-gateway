# SentinelEdge — Industrial Edge Monitoring Platform
SentinelEdge is an embedded Linux industrial edge monitoring platform built with the Yocto Project.
It combines a custom Linux distribution, a C-based telemetry acquisition service, a C++ system monitoring layer, a deterministic C rule-based diagnostic engine, a native C Isolation Forest anomaly detector, and a Qt 6/QML operations dashboard.
**Technology stack:** Yocto Scarthgap · KAS · BitBake · C · C++ · Qt 6 · QML · systemd
## Project overview
SentinelEdge runs as a custom Yocto Linux image targeting QEMU x86-64.
- Custom Yocto Linux distribution.
- C-based simulated industrial telemetry acquisition service.
- Local JSON telemetry pipeline.
- C++ Linux CPU, memory, and uptime monitoring.
- Qt 6/QML operations dashboard.
- Telemetry range and freshness validation.
- Deterministic C diagnostic engine with NORMAL, WARNING, and CRITICAL severity levels.
- Native C Isolation Forest inference using temperature, pressure, vibration, and motor RPM.
- AI anomaly status and score displayed in the Qt/QML dashboard.
**Hardware status:** Industrial measurements are simulated; no physical sensors are connected.
**Diagnostics status:** Deterministic C rule-based diagnostics and native C Isolation Forest anomaly inference are implemented and tested using simulated telemetry.
## Architecture
The platform separates sensor acquisition from telemetry publishing through a modular C backend interface. The current QEMU implementation uses a simulated sensor backend. Published JSON telemetry is consumed by the C++ monitoring layer and Qt 6/QML dashboard.

```text
       Sensor Backend Interface (C)
                   |
          Simulation Backend
                   |
       Industrial Data Acquisition
                   |
          JSON Telemetry Publisher
                   |
       /run/sentinel-edge/telemetry.json
                   |
          C++ TelemetryMonitor
                   |
C++ SystemMonitor (/proc) ---- Qt 6 / QML
                                      |
                            SentinelEdge Dashboard
```

## Repository structure
```text
industrial-edge-gateway/
├── docker/                 # Docker and KAS helper script
├── kas/                    # Yocto build configuration
├── meta-sentinel-edge/     # Custom Yocto layer and recipes
├── sentinel-edge-ui/       # Qt 6/QML application source
├── docs/                   # Project documentation
├── evidence/               # Validation evidence
└── README.md
```

## Build environment



The project uses KAS inside Docker to manage the Yocto build environment.



### Prerequisites



- Linux or Windows with WSL2 and Ubuntu

- Docker Desktop or Docker Engine

- Git

- Sufficient free disk space and memory for a Yocto build



### Checkout the build environment



```bash

git clone <YOUR-GITHUB-REPOSITORY-URL>

cd industrial-edge-gateway

chmod +x docker/sentinel-edge.sh

./docker/sentinel-edge.sh checkout

```



### Build the image



```bash

./docker/sentinel-edge.sh build

```



The build target is `core-image-minimal`, configured for `qemux86-64`.



### Run the QEMU image



```bash

./docker/sentinel-edge.sh qemu

```



Inside the KAS shell, launch QEMU:



```bash

export QB_SLIRP_OPT="-netdev user,id=net0,hostfwd=tcp:0.0.0.0:5901-:5900,hostfwd=tcp:127.0.0.1:2222-:22"

runqemu qemux86-64 nographic slirp

```



After Linux boots, launch the dashboard:



```bash

export QT_QPA_PLATFORM=vnc

export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/lib/plugins/platforms

sentinel-edge-ui

```



Connect a VNC viewer to `127.0.0.1::5901` from the host.

## Validation and testing



The platform has been built and tested in the QEMU x86-64 environment.

- Successful Yocto image and acquisition recipe builds.
- Modular C sensor backend interface and simulation backend compiled successfully.
- Acquisition package verified in the generated image manifest.
- QEMU boot and Linux userspace startup.
- systemd acquisition service startup and enabled state verified.
- Continuous JSON telemetry publishing with changing timestamps and simulated measurements.
- Manual service stop/start recovery verified.
- Automatic systemd recovery after forced process termination (`SIGKILL`) verified.
- New process PID and telemetry publishing confirmed after automatic restart.
- Linux CPU and memory monitoring.
- Qt dashboard startup through VNC.
- Telemetry connection and display in the dashboard.
- Standalone C diagnostic-engine tests covering normal operation, warnings, multiple faults, and critical conditions.
- Dashboard warning-state validation using manually supplied telemetry.




- Native C Isolation Forest inference integrated with the Qt 6/QML dashboard.
- Automated C/Python inference parity test using 1,600 samples.

### AI inference regression test

Install the pinned Python test dependencies:

```bash
python3 -m venv "$HOME/sentinel-edge-ml-venv"
"$HOME/sentinel-edge-ml-venv/bin/python" -m pip install -r ml/requirements-test.txt
```

Run the automated test:

```bash
./ml/run_c_inference_test.sh
```

The script compiles the native C inference library into a temporary file,
runs the Python/C parity test, and removes the temporary library afterward.

**Latest parity results:**

| Metric | Result |
|---|---:|
| Test samples | 1,600 |
| Maximum score difference | 1.11e-16 |
| Mean score difference | 3.54e-18 |
| Classification mismatches | 0 |
| Result | PASS |

See [AI anomaly detection validation](docs/AI_ANOMALY_VALIDATION.md)
for QEMU fault-injection results and model limitations.


## Development status



| Component | Status |
|---|---|
| Yocto distribution and custom layer | Implemented |
| KAS build environment | Implemented |
| C acquisition service | Implemented |
| Modular sensor backend interface | Implemented |
| Simulation sensor backend | Implemented |
| JSON telemetry pipeline | Implemented |
| C++ system monitoring | Implemented |
| Telemetry validation | Implemented |
| C rule-based diagnostic engine | Implemented and tested |
| Qt 6/QML dashboard | Implemented |
| QEMU integration | Tested |
| Physical sensor integration | Planned |
| Native C AI anomaly inference | Implemented and tested (QEMU) |

## Rule-based diagnostic thresholds

The C diagnostic engine evaluates four simulated industrial measurements:

| Measurement | Warning | Critical |
|---|---:|---:|
| Temperature | >= 65 °C | >= 72 °C |
| Pressure | >= 7.0 bar | >= 7.8 bar |
| Vibration | >= 4.0 mm/s | >= 4.8 mm/s |
| Motor speed | >= 1650 RPM | >= 1780 RPM |

The engine reports NORMAL, WARNING, or CRITICAL severity and a fault bitmask.
These are demonstration thresholds, not certified industrial safety limits.

## Diagnostic engine unit tests

Compile and run the standalone C tests with GCC:

```bash
gcc -Wall -Wextra -Werror -std=c11 -Isentinel-edge-ui/src/diagnostics tests/test_diagnostic_engine.c sentinel-edge-ui/src/diagnostics/diagnostic_engine.c -o /tmp/test_diagnostic_engine
/tmp/test_diagnostic_engine
```

The suite covers normal operation, all warning and critical threshold boundaries, simultaneous faults, severity priority, and null-pointer handling.

**Latest results:** 14 diagnostic tests + 23 simulator tests = **37 passed, 0 failed**.

## Limitations



- Industrial measurements are simulated; physical sensor integration has not been performed.

- AI anomaly inference uses a prototype Isolation Forest model trained and evaluated with synthetic data.
- Some individual simulated fault scenarios, including the tested temperature and pressure profiles, did not cross the model anomaly threshold.
- The AI model has not been validated with physical industrial sensors or real-world machinery.

- Validation has been performed in QEMU, not on physical industrial hardware.



## Project objective



SentinelEdge explores the development of a complete embedded Linux edge platform, from custom Yocto image construction and system services to telemetry processing and graphical monitoring.



---



**SentinelEdge — Local intelligence. Embedded control. Industrial visibility.**

## Persistent fault management

The Qt application records fault events in `/var/lib/sentinel-edge/fault-history.json`. The fault manager tracks active and cleared states, acknowledgement status, and event timestamps. QEMU validation covered all four simulated fault profiles and recovery to normal measurements.
