#!/usr/bin/env python3
"""
Autotune helper for CapacityWakeup

This script connects to a serial port (the same port used by `idf.py monitor`),
reads RAW lines printed by the firmware (enable CONFIG_ENABLE_RAW_LOG in menuconfig),
collects samples while you follow prompts (rest, touch), and suggests ON/OFF
factors and a debounce count.

Usage:
  ./scripts/autotune.py --port /dev/cu.usbmodemXXX

Requires: pyserial (`pip install pyserial`)
"""
import argparse
import sys
import time
import statistics
import serial


def read_samples(ser, duration_s):
    """Read RAW: <val> lines for duration_s seconds and return list of ints."""
    samples = []
    end = time.time() + duration_s
    ser.timeout = 1.0
    while time.time() < end:
        line = ser.readline().decode(errors='ignore').strip()
        if not line:
            continue
        # Accept lines like: RAW: 1234
        if line.startswith('RAW:'):
            try:
                val = int(line.split(':', 1)[1].strip())
                samples.append(val)
            except Exception:
                pass
    return samples


def suggest_params(rest_vals, touch_vals):
    base = statistics.mean(rest_vals)
    rest_std = statistics.stdev(rest_vals) if len(rest_vals) > 1 else 0.0
    touch_mean = statistics.mean(touch_vals)
    delta = base - touch_mean

    # Suggest ON/OFF factors
    on_factor = (touch_mean + base) / 2.0 / base
    # ensure within reasonable bounds
    on_factor = max(0.5, min(0.9, on_factor))

    off_factor = (base + (base - delta*0.5)) / base
    off_factor = max(on_factor + 0.05, min(0.99, off_factor))

    # Debounce suggestion based on variability
    noise = rest_std
    if noise < 2:
        debounce = 2
    elif noise < 6:
        debounce = 3
    elif noise < 15:
        debounce = 4
    else:
        debounce = 5

    return {
        'baseline': base,
        'rest_std': rest_std,
        'touch_mean': touch_mean,
        'on_factor': round(on_factor, 3),
        'off_factor': round(off_factor, 3),
        'debounce': debounce,
        'delta': delta,
    }


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--port', required=True, help='Serial port (e.g. /dev/cu.usbmodem14101)')
    p.add_argument('--rest', type=int, default=5, help='Seconds to record resting baseline')
    p.add_argument('--touch', type=int, default=5, help='Seconds to record while touching')
    args = p.parse_args()

    try:
        ser = serial.Serial(args.port, 115200)
    except Exception as e:
        print('Failed to open port:', e)
        sys.exit(2)

    print('Connected. Make sure firmware was built with CONFIG_ENABLE_RAW_LOG=y')
    print('You will be prompted to keep the sensor untouched (rest) and then touched.')
    input('Press Enter to start resting measurement (do NOT touch the pad)...')
    rest = read_samples(ser, args.rest)
    print(f'Collected {len(rest)} rest samples. mean={statistics.mean(rest):.1f} std={statistics.stdev(rest) if len(rest)>1 else 0:.2f}')

    input('Now place your finger on the pad and hold for the touch measurement. Press Enter to start...')
    touch = read_samples(ser, args.touch)
    print(f'Collected {len(touch)} touch samples. mean={statistics.mean(touch):.1f} std={statistics.stdev(touch) if len(touch)>1 else 0:.2f}')

    if not rest or not touch:
        print('Not enough samples collected. Ensure firmware prints RAW: lines and try again.')
        sys.exit(2)

    suggestion = suggest_params(rest, touch)
    print('\n=== Autotune suggestion ===')
    print(f"Baseline (mean rest): {suggestion['baseline']:.1f}")
    print(f"Touch mean: {suggestion['touch_mean']:.1f}")
    print(f"Delta (baseline - touch): {suggestion['delta']:.1f}")
    print(f"Rest noise (std): {suggestion['rest_std']:.2f}")
    print(f"Suggested ON_FACTOR: {suggestion['on_factor']}\nSuggested OFF_FACTOR: {suggestion['off_factor']}\nSuggested DEBOUNCE_COUNT: {suggestion['debounce']}")

    print('\nApply these by updating `main.c` constants or translating them to menuconfig where available.')


if __name__ == '__main__':
    main()
