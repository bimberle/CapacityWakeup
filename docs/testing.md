# Testen & Feinabstimmung (Tuning)

Diese Datei fasst die Schritt‑für‑Schritt‑Anleitung zum Testen des Projekts zusammen,
beschreibt die relevanten Parameter (Faktoren) und gibt eine praktische Checkliste
zum Feintuning des kapazitiven Sensors.

=== Schritt‑für‑Schritt Anleitung (wie im README) ===

1) Vorbereitung (einmalig)
- Stelle sicher, dass ESP‑IDF installiert und `idf.py` in deiner Shell verfügbar ist.
- Mach das macOS‑Hilfs‑Script ausführbar, falls nötig:
```bash
chmod +x scripts/flash_mac.sh
```
- Projekt in Target setzen:
```bash
cd /workspaces/CapacityWakeup
idf.py set-target esp32s3
```

2) `menuconfig` prüfen
- Öffne und setze `Touch pad number` und `Onboard LED GPIO` falls nötig:
```bash
idf.py menuconfig
# CapacityWakeup configuration -> Touch pad number
# CapacityWakeup configuration -> Onboard LED GPIO (set -1 to disable)
```

3) Build & flash
- Baue und flashe (macOS helper):
```bash
idf.py build
./scripts/flash_mac.sh
```
- Alternativ manuell (Port bekannt):
```bash
idf.py -p /dev/cu.usbmodemXXXX flash monitor
```

4) Serial‑Logs beobachten
- Suche nach Baseline/Threshold‑Zeilen, z. B.:
  `Baseline: 1234  OnThreshold: 864  OffThreshold: 1049`

5) Erster Funktionstest
- Öffne ein Textfeld auf dem Host und tippe mit leerem Fokus.
- Berühre das Metallpad kurz: du solltest `Touch detected (val=...)` sehen
  und im Textfeld ein `Space` erscheinen (sofern HID erkannt wird).

6) Stabilitätsprüfung
- Berühre mehrfach und prüfe, ob nur ein Trigger pro Berührung erfolgt.

7) Hysterese & Debounce feinjustieren
- Wenn nötig, passe `DEBOUNCE_COUNT`, `ON_FACTOR`, `OFF_FACTOR` in `main.c`
  (oder über `menuconfig` für `LED_GPIO`) an und wiederhole Build/Flash.

8) Test Wake‑From‑Sleep (Windows)
- Stelle sicher, dass der Host USB‑Wake unterstützt und die HID‑Energieoption
  aktiviert ist. Versetze den PC in Sleep und teste den Touch.

=== Relevante Parameter (Faktoren) ===

- `POLL_MS` (z. B. 50–200 ms): Messintervall. Kleinere Werte → schnellere Reaktion,
  höhere CPU‑Nutzung; größere Werte → stabiler gegen kurzzeitiges Rauschen.
- `DEBOUNCE_COUNT` (z. B. 2–6): Anzahl aufeinanderfolgender Messungen, die den
  gleichen Zustand bestätigen müssen, bevor ein Zustandswechsel akzeptiert wird.
- `ON_FACTOR` (z. B. 0.60–0.80): Multiplikator für die Erkennungs‑Schwelle
  (on_threshold = baseline * ON_FACTOR). Kleinere Werte machen das System
  empfindlicher.
- `OFF_FACTOR` (z. B. 0.80–0.95): Multiplikator für die Freigabe‑Schwelle
  (off_threshold = baseline * OFF_FACTOR). Größere Werte erhöhen die Hysterese
  und verhindern schnelles Rückwechseln.
- `touch_pad_filter_start(filter_period)` (Filtergröße): Erhöhen zum Glätten
  schwankender Rohwerte.

=== Feintuning‑Anleitung (praktisch) ===

1) Starte mit Standardwerten
- `POLL_MS = 100`, `DEBOUNCE_COUNT = 3`, `ON_FACTOR = 0.70`, `OFF_FACTOR = 0.85`.

2) Bestimme Baseline
- Schau im Log die Baseline‑Zahl an. Sie ist der Referenzwert für die Faktoren.

3) Empfindlichkeit anpassen
- Wenn Berührungen nicht erkannt werden: verringere `ON_FACTOR` schrittweise
  (z. B. 0.70 → 0.65 → 0.60) und teste erneut.
- Wenn zu viele Falschauslösungen: erhöhe `ON_FACTOR` (z. B. 0.70 → 0.75) oder
  erhöhe `DEBOUNCE_COUNT`.

4) Hysterese anpassen
- Wenn das System beim Loslassen zu leicht wieder auslöst (flattert), erhöhe
  `OFF_FACTOR` (z. B. 0.85 → 0.90). Das sorgt dafür, dass der Sensor klarer
  aus dem Berührungszustand zurückspringen muss.

5) Filter und Poll‑Intervall
- Bei stark schwankenden Messwerten: erhöhe den Filter‑Parameter und/oder
  das `POLL_MS`‑Intervall (z. B. 100 → 150 ms). Das reduziert kurzzeitiges Rauschen.

6) LED als Feedback nutzen
- Aktiviere `LED_GPIO` und beobachte visuelles Feedback beim Berühren, das hilft
  beim schnellen Abschätzen, ob Debounce/Hysterese richtig arbeiten.

=== Checkliste (schnell) ===

- [ ] ESP‑IDF installiert und `idf.py` verfügbar
- [ ] `idf.py set-target esp32s3` ausgeführt
- [ ] `idf.py menuconfig`: Touch‑Pad + LED GPIO gesetzt
- [ ] `idf.py build` erfolgreich
- [ ] Gerät geflasht / `monitor` läuft
- [ ] Baseline/Thresholds im Log sichtbar
- [ ] Kurzberührung erzeugt `Touch detected` und HID‑Space beim Host
- [ ] Mehrfaches Berühren → stabiler Einzel‑Trigger (Debounce OK)
- [ ] Wenn Wake‑from‑Sleep testen: Windows Energieverwaltung für HID aktiviert

Wenn du möchtest, nutze das mitgelieferte Auto‑Tuning‑Hilfsprogramm:

- `scripts/autotune.py`: Verbindet sich mit der seriellen Konsole und liest RAW‑Werte
  (Firmware muss `CONFIG_ENABLE_RAW_LOG=y` haben). Das Script sammelt Ruhe‑ und
  Berührungsproben und schlägt `ON_FACTOR`, `OFF_FACTOR` und `DEBOUNCE_COUNT` vor.

Benutzung (Kurzform):
```bash
# In menuconfig: CapacityWakeup configuration -> Enable raw touch value logging -> Yes
idf.py build
./scripts/flash_mac.sh

# In neuem Terminal (installiere pyserial falls nötig):
pip install pyserial
./scripts/autotune.py --port /dev/cu.usbmodemXXXX --rest 5 --touch 5
```

Das Script zeigt eine empfohlene Konfiguration an, die du anschließend in
`main.c` oder deinen `menuconfig`-Einstellungen übernehmen kannst.

=== Autotune → automatisch anwenden ===

Um das Ergebnis des Autotune‑Scripts automatisch in die Firmware zu übernehmen,
ist das Hilfs‑Tool `scripts/apply_suggestion.py` enthalten. Es ersetzt die
Konstanten `DEBOUNCE_COUNT`, `ON_FACTOR` und `OFF_FACTOR` in `main/main.c`.

Beispiel‑Workflow:

1) Firmware mit RAW‑Logging bauen und flashen (Firmware läuft):
```bash
idf.py menuconfig   # ensure Enable raw logging = Yes
idf.py build
./scripts/flash_mac.sh
```

2) Autotune ausführen und Vorschlag lesen:
```bash
pip install pyserial
./scripts/autotune.py --port /dev/cu.usbmodemXXXX --rest 5 --touch 5
```
Merke dir die vorgeschlagenen Werte `ON_FACTOR`, `OFF_FACTOR` und `DEBOUNCE_COUNT`.

3) Die Vorschläge automatisch übernehmen:
```bash
./scripts/apply_suggestion.py --on 0.70 --off 0.85 --debounce 3
```
Das Script legt eine Sicherung `main.c.bak` an und schreibt die neuen Werte in
`main/main.c`.

4) Neu bauen & flashen:
```bash
idf.py build
./scripts/flash_mac.sh
```

Hinweis: Du kannst die Werte auch manuell in `main/main.c` anpassen, wenn du
lieber erst testen willst. Das Autotune‑Tool liefert nur einen Vorschlag –
Feintuning ist oft noch nötig.
