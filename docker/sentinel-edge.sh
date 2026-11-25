
#!/usr/bin/env bash

# ============================================================
# SentinelEdge - Docker / Yocto Development Environment
# Platform : Yocto Scarthgap
# Machine  : qemux86-64
# KAS      : ghcr.io/siemens/kas/kas:5.5
# ============================================================

set -Eeuo pipefail

# ------------------------------------------------------------
# Project configuration
# ------------------------------------------------------------

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PROJECT_NAME="SentinelEdge"

KAS_IMAGE="ghcr.io/siemens/kas/kas:5.5"
KAS_CONFIG="kas/sentinel-edge.yml"

CONTAINER_WORKDIR="/work"
VNC_PORT="5904"

# ------------------------------------------------------------
# Terminal colors
# ------------------------------------------------------------

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# ------------------------------------------------------------
# Logging
# ------------------------------------------------------------

log_info() {
    echo -e "${BLUE}[INFO]${NC} $*"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $*"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $*"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $*" >&2
}

# ------------------------------------------------------------
# Help
# ------------------------------------------------------------

show_help() {
    cat <<EOF

${PROJECT_NAME} - Development Environment

Usage:
    ./docker/sentinel-edge.sh <command>

Commands:
    shell       Open a non-root KAS development shell
    build       Build the SentinelEdge Linux image
    checkout    Download/update configured Yocto repositories
    qemu        Open a root KAS shell for QEMU execution
    status      Check Docker, project and build environment
    help        Display this help message

Examples:
    ./docker/sentinel-edge.sh shell
    ./docker/sentinel-edge.sh build
    ./docker/sentinel-edge.sh checkout
    ./docker/sentinel-edge.sh qemu
    ./docker/sentinel-edge.sh status

QEMU:
    After entering the QEMU KAS shell, run:

    export QB_SLIRP_OPT="-netdev user,id=net0,hostfwd=tcp:0.0.0.0:${VNC_PORT}-:5900,hostfwd=tcp:127.0.0.1:2225-:22"

    runqemu qemux86-64 nographic slirp

VNC Viewer:
    127.0.0.1::${VNC_PORT}

EOF
}

# ------------------------------------------------------------
# Environment validation
# ------------------------------------------------------------

check_docker() {
    if ! command -v docker >/dev/null 2>&1; then
        log_error "Docker is not installed or WSL integration is disabled."
        exit 1
    fi

    if ! docker info >/dev/null 2>&1; then
        log_error "Docker Desktop is not running."
        exit 1
    fi

    log_success "Docker is available."
}

check_project() {
    if [[ ! -f "${PROJECT_DIR}/${KAS_CONFIG}" ]]; then
        log_error "KAS configuration not found: ${KAS_CONFIG}"
        exit 1
    fi

    if [[ ! -d "${PROJECT_DIR}/meta-sentinel-edge" ]]; then
        log_error "SentinelEdge layer not found."
        exit 1
    fi

    log_success "SentinelEdge project directory verified."
}

check_environment() {
    check_docker
    check_project
}

# ------------------------------------------------------------
# Docker container configuration
# ------------------------------------------------------------

run_kas_shell() {
    log_info "Starting non-root KAS development shell..."

    docker run --rm -it \
        --user 0:0 \
        -e HOST_UID="$(id -u)" \
        -e HOST_GID="$(id -g)" \
        -v "${PROJECT_DIR}:${CONTAINER_WORKDIR}" \
        -w "${CONTAINER_WORKDIR}" \
        --entrypoint /bin/bash \
        "$KAS_IMAGE" \
        -lc '
            if ! getent group "$HOST_GID" >/dev/null; then
                groupadd -g "$HOST_GID" hostdev
            fi

            GROUP_NAME="$(getent group "$HOST_GID" | cut -d: -f1)"

            if ! getent passwd "$HOST_UID" >/dev/null; then
                useradd -m -u "$HOST_UID" -g "$HOST_GID" \
                    -s /bin/bash ala
            fi

            exec su -s /bin/bash \
                "$(getent passwd "$HOST_UID" | cut -d: -f1)" \
                -c "cd /work && kas shell kas/sentinel-edge.yml"
        '
}

# ------------------------------------------------------------
# Build SentinelEdge Linux
# ------------------------------------------------------------

build_image() {
    log_info "Starting SentinelEdge image build..."
    log_info "BitBake will run as a non-root user."

    docker run --rm -it \
        --user "$(id -u):$(id -g)" \
        -e HOME=/tmp \
        -v "${PROJECT_DIR}:${CONTAINER_WORKDIR}" \
        -w "${CONTAINER_WORKDIR}" \
        --entrypoint /bin/bash \
        "$KAS_IMAGE" \
        -lc "kas shell ${KAS_CONFIG} -c 'bitbake core-image-minimal'"

    log_success "Build command completed."
}

# ------------------------------------------------------------
# Checkout Yocto repositories
# ------------------------------------------------------------

checkout_repositories() {
    log_info "Checking out configured Yocto repositories..."

    docker run --rm -it \
        --user "$(id -u):$(id -g)" \
        -e HOME=/tmp \
        -v "${PROJECT_DIR}:${CONTAINER_WORKDIR}" \
        -w "${CONTAINER_WORKDIR}" \
        --entrypoint /bin/bash \
        "$KAS_IMAGE" \
        -lc "kas checkout ${KAS_CONFIG}"

    log_success "Repository checkout completed."
}

# ------------------------------------------------------------
# QEMU development environment
# ------------------------------------------------------------

run_kas_qemu() {
    log_info "Starting root KAS shell for QEMU..."
    log_warning "Use this shell for QEMU only, not BitBake builds."

    echo
    echo "After the KAS shell opens, run these commands:"
    echo
    echo "export QB_SLIRP_OPT=\"-netdev user,id=net0,hostfwd=tcp:0.0.0.0:${VNC_PORT}-:5900,hostfwd=tcp:127.0.0.1:2225-:22\""
    echo
    echo "runqemu qemux86-64 nographic slirp"
    echo
    echo "Connect TightVNC Viewer to: 127.0.0.1::${VNC_PORT}"
    echo

    docker run --rm -it \
        --device=/dev/net/tun \
        --cap-add=NET_ADMIN \
        --user 0:0 \
        -p "${VNC_PORT}:${VNC_PORT}" \
        -v "${PROJECT_DIR}:${CONTAINER_WORKDIR}" \
        -w "${CONTAINER_WORKDIR}" \
        --entrypoint /bin/bash \
        "$KAS_IMAGE" \
        -lc "
            git config --system --add safe.directory '*'
            kas shell ${KAS_CONFIG}
        "
}

# ------------------------------------------------------------
# Development environment status
# ------------------------------------------------------------

show_status() {
    echo
    echo "=============================================="
    echo "          SENTINELEDGE STATUS"
    echo "=============================================="

    echo
    echo "[1] Project"
    echo "Project directory: ${PROJECT_DIR}"

    if [[ -f "${PROJECT_DIR}/${KAS_CONFIG}" ]]; then
        log_success "KAS configuration found."
    else
        log_error "KAS configuration missing."
    fi

    echo
    echo "[2] Docker"

    if command -v docker >/dev/null 2>&1; then
        docker --version

        if docker info >/dev/null 2>&1; then
            log_success "Docker daemon is running."
        else
            log_error "Docker daemon is unavailable."
        fi
    else
        log_error "Docker command not found."
    fi

    echo
    echo "[3] KAS image"

    if docker image inspect "$KAS_IMAGE" >/dev/null 2>&1; then
        log_success "KAS Docker image is available."
    else
        log_warning "KAS Docker image is not downloaded yet."
    fi

    echo
    echo "[4] Latest QEMU image"

    IMAGE_DIR="${PROJECT_DIR}/build/tmp-glibc/deploy/images/qemux86-64"

    if [[ -d "$IMAGE_DIR" ]]; then
        find "$IMAGE_DIR" -maxdepth 1 \
            -type f \
            -name 'core-image-minimal-qemux86-64.rootfs-*.ext4' \
            -printf '%T@ %f\n' |
            sort -nr |
            head -1 |
            cut -d' ' -f2- || true
    else
        log_warning "No image deployment directory found."
    fi

    echo
    echo "[5] Running Docker containers"
    docker ps --format "table {{.ID}}\t{{.Image}}\t{{.Status}}\t{{.Ports}}"

    echo
    echo "=============================================="
    echo "             STATUS COMPLETE"
    echo "=============================================="
}

# ------------------------------------------------------------
# Command dispatcher
# ------------------------------------------------------------

main() {
    local command="${1:-help}"

    case "$command" in
        shell)
            check_environment
            run_kas_shell
            ;;

        build)
            check_environment
            build_image
            ;;

        checkout)
            check_environment
            checkout_repositories
            ;;

        qemu)
            check_environment
            run_kas_qemu
            ;;

        status)
            show_status
            ;;

        help|-h|--help)
            show_help
            ;;

        *)
            log_error "Unknown command: ${command}"
            show_help
            exit 1
            ;;
    esac
}

main "$@"
