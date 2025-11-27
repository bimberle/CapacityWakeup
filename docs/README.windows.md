# ESP-IDF Installation & Build (Windows)

Kurzanleitung für Windows. Empfohlen: Verwende den offiziellen ESP-IDF Tools
Installer oder arbeite in einer WSL2-Umgebung und folge den Linux-Schritten.

Option A — Native Windows (ESP-IDF Tools Installer):
- Lade den ESP-IDF Tools Installer von der Espressif-Dokumentation herunter
  und folge dem Installationsassistenten (PowerShell + Tools).
- Öffne die "ESP-IDF Command Prompt" oder eine PowerShell mit den ESP-IDF
  Umgebungsvariablen geladen.
- Wechsle in dein Projekt-Directory und führe `idf.py set-target esp32s3`,
  `idf.py menuconfig`, `idf.py build` und `idf.py -p <COMx> flash monitor` aus.

Option B — WSL2 (empfohlen, wenn du Linux-Tooling bevorzugst):
- Installiere WSL2 (Ubuntu) und folge der Linux-Anleitung in `README.linux.md`
  innerhalb der WSL2-Shell. Achte darauf, dass dein USB-Gerät durch WSL zugänglich ist
  (Windows 11 unterstützt USB-Zugriff in WSL; ggf. zusätzliche Einstellungen nötig).

Port/COM Hinweise:
- Unter Windows heißen serielle Ports typischerweise `COM3`, `COM4` etc.
- Beispiel Flash-Befehl:
```powershell
idf.py -p COM3 flash monitor
```

Treiber:
- Bei einigen Boards (ältere) musst du CP210x- oder FTDI-Treiber installieren.
