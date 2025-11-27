# Live‑Debug Anleitung

Diese Seite erklärt, wie du während des laufenden Geräts seriell debuggst, die
Werte interpretierst und schnell Probleme (keine Erkennung, viele Falschauslöser,
kein Wake) analysierst.

Voraussetzungen
- Firmware ist geflasht und läuft (siehe `README.md`).
- `idf.py` und das passende Serial‑Device sind verfügbar.

1) Serial‑Monitor starten

```bash
cd /workspaces/CapacityWakeup
idf.py -p /dev/cu.<dein-port> monitor
```

Oder verwende das macOS‑Script, das Port erkennt:

```bash
./scripts/flash_mac.sh
```

2) Wichtige Log‑Zeilen und ihre Bedeutung
- `Baseline: <n>`
  - Der gemessene Referenzwert ohne Berührung. Ausgangspunkt für Thresholds.
- `OnThreshold: <x>` / `OffThreshold: <y>`
  - Schwellen, berechnet aus `baseline * ON_FACTOR` bzw. `baseline * OFF_FACTOR`.
- `RAW: <v>`
  - Rohwert der Touch‑Messung (ein Wert pro Poll‑Intervall, nur bei aktivem RAW‑Log).
- `Touch detected (val=<v>)`
  - Ein Touch‑Event wurde registriert (vor dem Senden von HID‑Space).
- `Touch released (val=<v>)`
  - Das Loslassen wurde erkannt.

Beispiel (Logs):
```
I (1234) capacity_wakeup: Baseline: 2345  OnThreshold: 1641.5  OffThreshold: 1993.25
RAW: 2337
RAW: 2320
RAW: 1600
I (2345) capacity_wakeup: Touch detected (val=1600)
I (2355) capacity_wakeup: Sent HID space press
RAW: 1700
RAW: 2100
I (2600) capacity_wakeup: Touch released (val=2100)
```

3) Live‑Diagnose Checkliste (schnell)
- Siehst du Baseline‑Zeile nach Boot? Wenn nein, Firmware nicht gestartet.
- Beobachte RAW‑Werte einige Sekunden ohne Berührung: sind sie stabil (kleine Schwankung)?
- Lege Finger auf das Blech: RAW‑Wert sollte merklich unter Baseline fallen.
- Wenn RAW‑Wert fällt, aber kein `Touch detected` -> ON_FACTOR zu hoch (weniger empfindlich) oder DEBOUNCE_COUNT zu groß.
- Wenn `Touch detected` mehrfach pro kurzen Zeitraum (Flattern) -> DEBOUNCE_COUNT zu klein oder OFF_FACTOR zu nah an ON_FACTOR.

4) Konkrete Quick‑Fixes
- Keine Touchs erkannt:
  - Prüfe, ob `RAW:`‑Werte bei Berührung fallen. Wenn ja, verringere `ON_FACTOR` (z. B. 0.70 → 0.65).
  - Falls `RAW:` nicht fällt, prüfe Anschluss: Widerstand/Verbindung zum Blech, Kabellänge.
- Zu viele Falschauslösungen:
  - Erhöhe `DEBOUNCE_COUNT` (z. B. 3 → 5) oder setze `OFF_FACTOR` höher (z. B. 0.85 → 0.90).
- LED zeigt nichts an:
  - Prüfe `menuconfig` -> `CONFIG_LED_GPIO` und ob `LED_GPIO` auf ein verfügbares GPIO gesetzt ist.
  - Achte auf Konflikte mit UART/Flash Pins.

5) HID / Host Checks
- Gerät wird nicht als HID erkannt:
  - Prüfe TinyUSB‑Initialisierung in Logs (Fehler beim enumerieren?).
  - In `menuconfig` TinyUSB Device/HID aktivieren.
- HID‑Tasten funktionieren im aktiven Host‑Zustand, aber Wake funktioniert nicht:
  - Prüfe Windows Energieverwaltung (Geräte‑Manager → Tastatur → Energieverwaltung).
  - Prüfe BIOS/UEFI Einstellungen für USB‑Wake.

6) Sammlung von Messwerten für Feintuning
- Aktiviere RAW‑Logging (standardmäßig aktiv) und lasse das Gerät einige Sekunden
  im Normalzustand laufen, dann berühre das Pad für einige Sekunden und notiere Mittelwerte.
- Nutze `scripts/autotune.py` wie in `docs/testing.md` beschrieben.

7) Wenn du Werte posten willst
- Poste die `Baseline:`, ein paar `RAW:`‑Zeilen (ohne persönliche Informationen) und die Zeitangaben.
  Ich kann dann konkrete Empfehlungen für `ON_FACTOR`/`OFF_FACTOR`/`DEBOUNCE_COUNT` geben.

8) Zusätzliche Tools
- `scripts/autotune.py` zum Sammeln und Vorschlagen von Parametern.
- `scripts/apply_suggestion.py` wendet Vorschläge direkt in `main/main.c` an.

Wenn du magst, können wir jetzt live testen: du flashst die Firmware, öffnest den Monitor
und sendest mir die Baseline + ein paar RAW‑Zeilen — ich gebe dir sofort konkrete Werte.
