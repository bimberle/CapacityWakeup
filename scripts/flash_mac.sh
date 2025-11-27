#!/usr/bin/env bash
# Simple helper to detect a serial device on macOS and run idf.py flash+monitor
set -euo pipefail

PROG="$0"

usage() {
  cat <<EOF
Usage: $PROG [-p port] [--] [idf-args...]

If -p is not given, the script will try to auto-detect a likely /dev/cu.*
device (usbmodem*, usbserial*, SLAB_USBtoUART*, etc.) and use the first match.
Any extra arguments are forwarded to idf.py.
EOF
}

PORT=""
EXTRA=()

while [[ $# -gt 0 ]]; do
  case "$1" in
    -p|--port)
      PORT="$2"; shift 2;;
    --)
      shift; EXTRA+=("$@"); break;;
    -h|--help)
      usage; exit 0;;
    *)
      EXTRA+=("$1"); shift;;
  esac
done

if [[ -z "$PORT" ]]; then
  # try to find likely macOS serial devices
  candidates=(/dev/cu.usbmodem* /dev/cu.usbserial* /dev/cu.SLAB_USBtoUART* /dev/cu.usb* )
  for pat in "${candidates[@]}"; do
    for f in $pat; do
      if [[ -e "$f" ]]; then
        PORT="$f"
        break 2
      fi
    done
  done
fi

if [[ -z "$PORT" ]]; then
  echo "No serial port found. Specify with -p /dev/cu.xxx" >&2
  exit 2
fi

echo "Using port: $PORT"

# Run idf.py with port and any extras
cmd=(idf.py -p "$PORT" flash monitor "${EXTRA[@]}")
echo "Running: ${cmd[*]}"
exec "${cmd[@]}"
