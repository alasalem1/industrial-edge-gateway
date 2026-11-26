#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PYTHON="${SENTINEL_AI_PYTHON:-$HOME/sentinel-edge-ml-venv/bin/python}"
SOURCE="$ROOT_DIR/sentinel-edge-ui/src/ml/anomaly_detector.c"

if [[ ! -x "$PYTHON" ]]; then
    echo "ERROR: Python executable not found: $PYTHON"
    exit 1
fi

if ! command -v gcc >/dev/null 2>&1; then
    echo "ERROR: gcc is required."
    exit 1
fi

LIBRARY="$(mktemp /tmp/libsentinel-ai-XXXXXX.so)"
trap 'rm -f "$LIBRARY"' EXIT

echo "===== BUILD NATIVE C INFERENCE ====="
gcc -shared -fPIC -O2 -o "$LIBRARY" "$SOURCE" -lm

echo
echo "===== RUN C / PYTHON PARITY TEST ====="
cd "$ROOT_DIR"
SENTINEL_AI_LIBRARY="$LIBRARY" "$PYTHON" ml/test_c_inference.py

echo
echo "PASS: Automated C inference validation completed."
