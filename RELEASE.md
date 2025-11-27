# Release: v0.1.0

Titel: CapacityWakeup v0.1.0

Kurze Beschreibung
- Erstes funktionsfähiges Prototype-Release: Kapazitiver Touch‑Wake via USB‑HID
  für ESP32‑S3 (N16R8). Sendet ein `Space` HID‑Event bei Berührung eines angeschlossenen Metallpads.

Highlights
- Baseline‑Kalibrierung und adaptive Thresholds
- Debounce + Hysterese gegen Fehlauslösungen
- TinyUSB HID Keyboard Device (Space on touch)
- Optionales Onboard‑LED Feedback (`CONFIG_LED_GPIO`, default 48)
- RAW‑Logging (`CONFIG_ENABLE_RAW_LOG`) zur Unterstützung von Tuning/Autotune
- `scripts/flash_mac.sh` (macOS helper) & `scripts/autotune.py` (Autotune helper)

Änderungen / Breaking
- Dies ist ein Initial‑Release / Prototype. APIs und Kconfig‑Namen wurden für
  ESP‑IDF kompatible Varianten erstellt; je nach ESP‑IDF‑Version sind kleine
  Anpassungen möglich.

Install / Build Hinweise
- Siehe `README.md` und `docs/testing.md` für vollständige Installations- und
  Testanweisungen.

Empfohlene Release‑Beschreibung (für GitHub Release body)
```
CapacityWakeup v0.1.0 — Prototype

This release provides a working prototype for using an ESP32‑S3 module as a
capacitive sensor that sends a USB HID Space key to wake a connected PC.

Files of interest:
- `main/main.c` — firmware (touch handling, HID)
- `main/Kconfig` — configurable touch pad number, LED GPIO, raw logging
- `scripts/flash_mac.sh` — macOS helper to detect serial port and flash
- `scripts/autotune.py` — helper to collect RAW values and suggest tuning
- `docs/testing.md` — step‑by‑step testing and tuning guide

Notes for users:
- Enable `CONFIG_ENABLE_RAW_LOG` for autotuning and use `scripts/autotune.py`
  to gather baseline/touch samples.
- See CHANGELOG.md for a summary of this release.

```

Anweisungen zum lokalen Taggen & Pushen
```bash
# Commit release artefact if not yet committed
git add CHANGELOG.md RELEASE.md
git commit -m "chore(release): v0.1.0" || true

# Create annotated tag
git tag -a v0.1.0 -m "CapacityWakeup v0.1.0"

# Push tag to remote
git push origin main --tags
```

Falls du möchtest, kann ich hier im Container versuchen, das Tag zu setzen und
zu pushen — beachte aber, dass der Container möglicherweise keinen Git‑Remote‑Zugriff
auf dein GitHub hat. Ich empfehle, die obigen Befehle auf deinem Mac auszuführen.
