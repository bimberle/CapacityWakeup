# CapacityWakeup — ESP32-S3 Capacitive Wake via USB HID

Dieses Projekt zeigt ein kleines ESP-IDF-Projekt für einen ESP32-S3, das
ein angeschlossenes Metallstück als kapazitiven Sensor nutzt und bei Berührung
ein USB-HID-Tastatur-Event (Space) an einen per USB verbundenen Windows-PC
sendet. Ziel ist, den PC aus dem Sleep (S3) zu wecken (sofern USB-Wakeup aktiviert ist).

Wichtig: Dieses Repository liefert ein Basis-Setup. Je nach ESP-IDF-Version und
Board kann es nötig sein, kleine API-Anpassungen vorzunehmen (TinyUSB-/Touch-API).

Hardware (empfohlen):
- ESP32-S3 mit Native USB (z. B. N16R8)
- Metallstück (Sensorpad)
- 1 MΩ Schutzwiderstand zwischen Metall und Touch-GPIO
- Optional: 10 nF Kondensator zwischen Metall und GND zur Stabilisierung

Vorsicht: Kein Netzstrom, kein direkter Kontakt zu gefährlichen Spannungen.

Anschluss-Empfehlung:
- Verbinde das Metallstück über einen 1 MΩ Widerstand mit dem Touch-GPIO.
- GND an Board-GND (Metall kann optional geerdet werden, funktioniert aber
  auch als freistehende Elektrode).

Software (ESP-IDF):
1. Installiere ESP-IDF (empfohlen: aktuelle v4.x oder v5.x zur Zeit dieses Projekts).
2. Clone dieses Repo in dein Workspace (ist bereits hier).

Build & Flash (Beispiel):
```bash
cd /workspaces/CapacityWakeup
idf.py set-target esp32s3
idf.py menuconfig   # optional: TinyUSB/USB configuration anpassen
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Hinweise Windows/Host:
- Unter Windows in den Gerätemanager gehen und bei der HID-Tastatur im
  Eigenschaften -> Energieverwaltung "Gerät kann den Computer aus dem Ruhezustand
  aktivieren" aktivieren.
- Damit Remote-Wakeup funktioniert, muss der Host USB-Suspend/Resume unterstützen
  und das Gerät als Remote-Wakeup-fähig erkannt werden.

Änderungen anpassen:
- `main/main.c`: Standardmäßig nutzt es `TOUCH_PAD_NUM0`. Falls dein Board
  eine andere Pinbelegung hat, kannst du die Touch-Pad-Nummer in `menuconfig`
  unter `CapacityWakeup configuration -> Touch pad number` anpassen (z. B. 0
  für `TOUCH_PAD_NUM0`). Prüfe die Pinbelegung deines spezifischen Moduls/Devboards
  in dessen Dokumentation, um die richtige `TOUCH_PAD_NUMx`-Zuweisung zu finden.

Hinweis zur Auswahl eines Pins:
- Wenn du ein Espressif Devboard verwendest, schaue ins Board-Schema: die
  Touch-Pad-Nummern sind hardware-gebunden (z. B. `TOUCH_PAD_NUM0` usw.).
  Ich habe das Projekt so vorbereitet, dass du die Nummer bequem über
  `idf.py menuconfig` setzen kannst.

Fehlerbehebung:
- Wenn Touch-Werte sehr unstabil sind: Filter-Parameter in `touch_pad_filter_start`
  anpassen oder ein kleines Erdungspad hinzufügen.
- Wenn kein Wake funktioniert: Prüfe ob das Gerät als HID-Keyboard erkannt wird
  und ob unter Windows die Energieverwaltungsoption für das Gerät gesetzt ist.

Wenn du möchtest, passe ich den genauen GPIO-Vorschlag an dein Board-Layout
oder schreibe eine angepasste `sdkconfig.defaults`-Datei. Möchtest du, dass
ich noch ein `sdkconfig.defaults` mit empfohlenen Optionen anlege?

Was ist eine "ESP-IDF-Version"?
- Die ESP-IDF (Espressif IoT Development Framework) ist das offizielle SDK
  für Espressif-Chips (z. B. ESP32, ESP32-S3). Eine "ESP-IDF-Version" ist
  einfach die Veröffentlichungsnummer des SDK (z. B. v4.x, v5.x). APIs und
  Build-Tools können zwischen Versionen leicht variieren, deshalb ist es
  hilfreich, die verwendete Version zu kennen, wenn du Unterstützung beim
  Kompilieren brauchst.

ESP-IDF Installationshinweise für Plattformen

Die plattformspezifischen Installations- und Build-Anleitungen für Linux
und Windows habe ich ausgelagert, damit diese Haupt-README schlank bleibt.

- Linux: siehe `docs/README.linux.md`
- Windows: siehe `docs/README.windows.md`

macOS helper script:
- Ein einfaches Script `scripts/flash_mac.sh` hilft beim automatischen Erkennen
  des seriellen Ports auf macOS und führt `idf.py -p <port> flash monitor` aus.
  Siehe `scripts/flash_mac.sh` für Verwendung und Optionen.

Testing & Tuning
- Ausführliche Test‑ und Feinabstimmungsanweisungen findest du in `docs/testing.md`.
  Dort ist die komplette Schritt‑für‑Schritt‑Anleitung, die relevanten Faktoren
  (`ON_FACTOR`, `OFF_FACTOR`, `DEBOUNCE_COUNT`, u. a.), eine praktische
  Feintuning‑Anleitung und eine Checkliste enthalten.

Live Debug
- Für Live‑Diagnose, Beispiel‑Logs und schnelle Troubleshooting‑Checks siehe
  `docs/live_debug.md`.

macOS (Apple Silicon / Intel)
--------------------------------
Kurze, praktische Anleitung für dein ARM‑MacBook. Die Linux‑Schritte gelten
meistens auch für macOS; hier die wichtigsten Befehle und das Hilfs‑Script:

1) ESP-IDF installieren (Beispiel, Apple Silicon):

```bash
# Clone (einmal)
git clone --recursive https://github.com/espressif/esp-idf.git ~/esp-idf
cd ~/esp-idf
# Optional: konkrete Version auschecken, z. B. v5.1
# git checkout v5.1

# Install-Script (lädt Toolchain und Abhängigkeiten)
./install.sh

# Environment für die Session laden
. ./export.sh

# Prüfen
idf.py --version
```

2) Projekt konfigurieren, bauen und flashen (im Projektverzeichnis):

```bash
cd /workspaces/CapacityWakeup
idf.py set-target esp32s3
idf.py menuconfig   # Touch pad number setzen
idf.py build

# Auto-detect + flash + monitor (Hilfs-Script)
chmod +x scripts/flash_mac.sh
./scripts/flash_mac.sh

# Alternativ manuell (Port ermitteln mit `ls /dev/cu.*`):
idf.py -p /dev/cu.usbmodemXXXX flash monitor
```

3) Hinweise
- Das Script `scripts/flash_mac.sh` versucht, ein passendes `/dev/cu.*`-Device
  zu finden (z. B. `/dev/cu.usbmodem*`, `/dev/cu.SLAB_USBtoUART*`) und ruft
  `idf.py -p <port> flash monitor` auf. Wenn mehrere Geräte angeschlossen sind,
  gib den Port mit `-p` an.

Wenn du willst, mache ich das Script ausführbar im Repository (Commit),
oder ergänze ich noch weitere macOS-spezifische Hinweise (z. B. Homebrew‑Pakete).

Wiring (empfohlen):
- Verbinde das Metallstück über einen hohen Widerstand (~1 MΩ) mit dem
  gewählten Touch-GPIO. Optional: 10 nF zwischen Metall und GND zur Stabilität.

ASCII-Schaltbild:

  [Metall-Sensor]
       |
      1MΩ
       |
   [GPIO TOUCH]
       |
     ESP32-S3
       |
     GND --- optional 10nF --- Metall (oder Kondensator zur Erde)

Hinweis zur Pin-/Touch-Pad-Auswahl:
- Das Modul `ESP32-S3-N16R8` ist ein ESP32-S3-basiertes Modul. Die Zuordnung
  zwischen `TOUCH_PAD_NUMx` und physischen GPIOs hängt vom Chip/Board ab.
  Schau ins Datenblatt / Board-Schema deines Devboards. In `main/Kconfig`
  kannst du die Touch-Pad-Nummer einstellen (z. B. 0..N).

Testen & Troubleshooting (Windows Host):
- Stelle sicher, dass das Board als USB-HID-Gerät erkannt wird (Geräte-Manager).
- In den Geräte-Eigenschaften (Geräte-Manager -> Tastatur -> Eigenschaften ->
  Energieverwaltung) aktiviere "Gerät kann den Computer aus dem Ruhezustand aktivieren".
- Prüfe, ob dein Windows OS und BIOS USB-Remote-Wakeup unterstützen.
- Wenn der Host nicht aufwacht: teste erstmal, ob normale Tasten-Events ankommen
  (z. B. sende mehrere Tasten nach dem Anschluss). Wenn ja, ist das HID-Teil OK
  und das Problem liegt bei Remote-Wakeup/Windows-Konfiguration.

Tipps zur Stabilität des Sensors:
- Erhöhe die Filter-Zeit in `touch_pad_filter_start(...)` wenn Messwerte flott
  schwanken.
- Verwende ein größeres Metallpad oder eine zusätzliche Erdungsfläche, falls
  die Empfindlichkeit zu gering ist.
- Falls das Metall unbeabsichtigte Trigger hat, füge eine Hysterese oder
  längere Bestätigungszeit (z. B. mehrere Poll-Zyklen) hinzu.

Onboard-LED Hinweis:
- Das Projekt verwendet optionales LED-Feedback. In `main/main.c` ist `LED_GPIO`
Onboard-LED Hinweis:
- Das Projekt verwendet optionales LED-Feedback. Voreinstellung: `CONFIG_LED_GPIO`
  ist in `menuconfig` standardmäßig auf `48` gesetzt (RGB-LED). Falls dein Board
  stattdessen die LED auf GPIO `38` hat, ändere `menuconfig` oder setze die
  Option beim ersten Konfigurieren:

```bash
idf.py menuconfig
# CapacityWakeup configuration -> Onboard LED GPIO (set -1 to disable)
```

Alternativ kannst du vorübergehend `LED_GPIO` direkt in `main/main.c` ändern,
die konfigurierte/standardmäßige Methode ist aber `menuconfig`.

Weiteres Vorgehen:
- Wenn du willst, führe ich hier im Container einen Build aus — dazu muss
  ESP-IDF im Container installiert sein. Alternativ kann ich dir eine
  Schritt-für-Schritt Installationsanleitung für deine Plattform erstellen.

# CapacityWakeup
ESP32 Project to wakeup my PC via capacitive Sensor
