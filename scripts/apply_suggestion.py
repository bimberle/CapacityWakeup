#!/usr/bin/env python3
"""
Apply tuning suggestions to main.c

Usage:
  ./scripts/apply_suggestion.py --on 0.70 --off 0.85 --debounce 3

This will update the constants `DEBOUNCE_COUNT`, `ON_FACTOR` and `OFF_FACTOR`
in `main/main.c`. A backup `main.c.bak` is created.
"""
import argparse
import re
from pathlib import Path

MAIN_C = Path('main/main.c')

def replace_constant(content, name, value):
    # name is macro name like DEBOUNCE_COUNT or ON_FACTOR
    pattern = re.compile(r'(#define\s+' + re.escape(name) + r'\s+).+')
    repl = r'\1' + str(value)
    new, n = pattern.subn(repl, content)
    if n == 0:
        raise RuntimeError(f'Constant {name} not found in {MAIN_C}')
    return new

def main():
    p = argparse.ArgumentParser()
    p.add_argument('--on', type=float, required=True, help='ON_FACTOR (e.g. 0.70)')
    p.add_argument('--off', type=float, required=True, help='OFF_FACTOR (e.g. 0.85)')
    p.add_argument('--debounce', type=int, required=True, help='DEBOUNCE_COUNT (e.g. 3)')
    args = p.parse_args()

    if not MAIN_C.exists():
        print('Error: main/main.c not found. Run this from project root.')
        return

    text = MAIN_C.read_text()
    backup = MAIN_C.with_suffix('.c.bak')
    backup.write_text(text)
    print(f'Backup written to {backup}')

    text = replace_constant(text, 'DEBOUNCE_COUNT', args.debounce)
    # Format floats with 'f' suffix to match C code
    text = replace_constant(text, 'ON_FACTOR', f'{args.on}f')
    text = replace_constant(text, 'OFF_FACTOR', f'{args.off}f')

    MAIN_C.write_text(text)
    print(f'Updated {MAIN_C} with ON_FACTOR={args.on}, OFF_FACTOR={args.off}, DEBOUNCE_COUNT={args.debounce}')
    print('Now rebuild and flash: idf.py build && ./scripts/flash_mac.sh')

if __name__ == "__main__":
    main()
