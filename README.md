# SentinelEdge — Industrial Edge Monitoring Platform
SentinelEdge is an embedded Linux industrial edge monitoring platform built with the Yocto Project.
It combines a custom Linux distribution, a C-based telemetry acquisition service, a C++ system monitoring layer, and a Qt 6/QML operations dashboard.
**Technology stack:** Yocto Scarthgap · KAS · BitBake · C · C++ · Qt 6 · QML · systemd
## Project overview
SentinelEdge runs as a custom Yocto Linux image targeting QEMU x86-64.
- Custom Yocto Linux distribution.
- C-based simulated industrial telemetry acquisition service.
- Local JSON telemetry pipeline.
- C++ Linux CPU, memory, and uptime monitoring.
- Qt 6/QML operations dashboard.
- Telemetry range and freshness validation.
**Hardware status:** Industrial measurements are simulated; no physical sensors are connected.
**AI status:** The diagnostics interface exists, but no inference model is currently loaded.
## Architecture
The platform connects Linux system monitoring and simulated industrial acquisition through local C++ monitoring services and a Qt 6/QML dashboard.
```text
Industrial acquisition (C)
          |
    Local JSON file
          |
C++ TelemetryMonitor ---- C++ SystemMonitor (/proc)
          |
      Qt 6 / QML
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



- Successful Yocto image build.

- Successful application recipe builds.

- QEMU boot and Linux userspace startup.

- systemd acquisition service startup.

- Continuous generation of telemetry JSON data.

- Verification of changing telemetry values.

- Linux CPU and memory monitoring.

- Qt dashboard startup through VNC.

- Telemetry connection and display in the dashboard.



## Development status



| Component | Status |

|---|---|

| Yocto distribution and custom layer | Implemented |

| KAS build environment | Implemented |

| C acquisition service | Implemented |

| JSON telemetry pipeline | Implemented |

| C++ system monitoring | Implemented |

| Telemetry validation | Implemented |

| Qt 6/QML dashboard | Implemented |

| QEMU integration | Tested |

| Physical sensor integration | Planned |

| AI inference integration | Planned |



## Limitations



- Industrial measurements are simulated; physical sensor integration has not been performed.

- AI diagnostics are currently a user-interface feature; no inference model is loaded.

- Validation has been performed in QEMU, not on physical industrial hardware.



## Project objective



SentinelEdge explores the development of a complete embedded Linux edge platform, from custom Yocto image construction and system services to telemetry processing and graphical monitoring.



---



**SentinelEdge — Local intelligence. Embedded control. Industrial visibility.**
