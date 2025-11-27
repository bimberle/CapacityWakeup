# CHANGELOG

## v0.1.0 — 2025-11-24

Initial working prototype and tooling for CapacityWakeup.

Notable changes in this release:

- Project scaffolded as an ESP-IDF project (`CMakeLists.txt`, `main/`).
- Capacitive touch sensing using ESP32-S3 touch pad API with baseline
  calibration and adaptive thresholds.
- Debounce and hysteresis implemented to avoid false triggers.
- TinyUSB HID keyboard support: on touch a `Space` HID key is sent to the host.
- `main/Kconfig` entries added to configure touch pad number, LED GPIO and
  enable raw logging.
- Default: `CONFIG_ENABLE_RAW_LOG` = `y` (raw touch values printed as `RAW:`)
  to assist tuning and autotune tooling.
- Optional onboard LED feedback via `CONFIG_LED_GPIO` (default 48).
- `scripts/flash_mac.sh`: macOS helper to auto-detect serial port and run
  `idf.py -p <port> flash monitor`.
- `scripts/autotune.py`: Python helper that reads `RAW:` values from serial and
  suggests `ON_FACTOR`, `OFF_FACTOR`, and `DEBOUNCE_COUNT` tuning values.
- `docs/testing.md`: Detailed step-by-step testing and tuning guide.
- `sdkconfig.defaults`: commented hints for configuring TinyUSB/Remote Wakeup.

Notes for maintainers and users:
- To create a local Git tag for this release and push it to origin:

```bash
git add CHANGELOG.md
git commit -m "chore: add changelog for v0.1.0"
git tag -a v0.1.0 -m "CapacityWakeup v0.1.0"
git push origin main --tags
```

Replace `origin`/`main` with your remote/branch names if different.

If you want, I can also open a minimal GitHub release body or prepare a
`v0.1.0` annotated tag file for you; tell me if you'd like that.
