# ESP-IDF Installation & Build (Linux)

Diese Datei enthält die Linux-spezifischen Schritte zur Installation von ESP-IDF
und zum Bauen/Flashen des Projekts.

1) ESP-IDF installieren (Linux)

```bash
# Clone ESP-IDF (im Home-Verzeichnis oder einem Entwickler-Ordner)
git clone --recursive https://github.com/espressif/esp-idf.git
cd esp-idf
# Optional: checkout einer stabilen Version, z. B. v5.x
# git checkout v5.x

# Install-Script ausführen (installiert Toolchain und Abhängigkeiten)
./install.sh

# Environment laden (für jede neue Shell-Session wiederholen)
. ./export.sh

# Prüfen
idf.py --version
```

2) Projekt konfigurieren, bauen und flashen

```bash
cd /path/to/CapacityWakeup
idf.py set-target esp32s3
idf.py menuconfig

# In menuconfig: setze 'CapacityWakeup configuration -> Touch pad number'
# und überprüfe TinyUSB/HID Einstellungen unter 'Component config -> TinyUSB'.

idf.py build
# Port finden (Linux): /dev/ttyUSB0, /dev/ttyACM0, /dev/ttyUSB* etc.
idf.py -p /dev/ttyUSB0 flash monitor
```

Hinweise:
- Auf einigen Systemen müssen udev-Rules gesetzt werden, damit der Benutzer
  auf serielle USB-Adapter ohne sudo zugreifen kann. Siehe: https://docs.espressif.com/
- Wenn du ein Devboard mit CP210x oder FTDI verwendest, installiere ggf. die
  entsprechenden Treiber.
