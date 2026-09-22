# STERN OS — Pocket Cyberdeck

### A DIY Pocket Cyberdeck built inside a Barkleys tin

* **Schematic & PCB (EasyEDA):** https://oshwlab.com/dmitriyshalagurov/project_itafwezn
* **Build Video:** https://youtu.be/7yzCkS4Cg1M
* **Build Video (FULL VERSION):** https://youtu.be/xNvaoZqZ404

---

## Project

**STERN OS** is a custom pocket cyberdeck built around an ESP32 and designed to fit inside a small **Barkleys tin**.
The goal of the project is to create a compact, fully custom handheld computer with its own keyboard, display, storage, games, terminal, file browser and other software features.
The entire project is built from scratch — including the electronics, custom PCB, keyboard, firmware and enclosure integration.

> **This is NOT an Altoids tin.**
> The project uses a Barkleys tin while keeping the compact metal-tin cyberdeck concept.

---

## Features

* Custom boot animation
* Graphical user interface
* Status bar
* File browser
* Text editor
* Terminal
* Settings interface
* Keyboard input system
* Game of Life
* Dino Game
* NES emulator
* Custom bitmap/image handling
* ESP32 Wi-Fi and Bluetooth
* MicroSD support
* Custom keyboard matrix
* Custom PCB
* Compact pocket-sized enclosure

---

# Hardware

## Main Controller

* **ESP32-WROOM / ESP32-DevKitC-32**: Standard Dual-Core ESP32 (4MB Flash).
* **ESP32-WROVER**: Dual-Core ESP32 mit 4MB/8MB PSRAM für erweiterte Emulation.
* **ESP32-S3**: Dual-Core Xtensa LX7 mit USB-OTG/CDC & PSRAM (z.B. LilyGO T-Deck / Cardputer).
* **ESP32-C3**: Kompakter RISC-V Single-Core Controller.

Das System passt seine Pin-Mappings und Speicherverwaltung über `include/hardware_config.h` automatisch an den gewählten Chip an.

## Custom Keyboard

The keyboard uses a dedicated **TCA8418RTWR — TCA8418** keyboard controller.

* QFN-24 package
* I²C keyboard controller
* Custom keyboard matrix
* Hardware diode per key
  Each key uses a **1N4148 SOD-323** diode.

## Display
 
 Das System unterstützt verschiedene Display-Controller und Auflösungen über `include/display_config.h`:
 * **ST7789**: 240 × 320 (Querformat: 320 × 240) & 240 × 240
 * **ST7735**: 128 × 160 & 128 × 128 (sehr kompakte Mini-Cyberdecks)
 * **ILI9341**: 320 × 240 & 240 × 320
 * **SSD1306**: 128 × 64 (OLED-Modus) & 128 × 32 (sekundäres Status-OLED)
 
 Alle UI-Elemente berechnen ihre Position dynamisch relativ zur tatsächlichen Bildschirmgröße.

## Power System

The power system is based around a **TPS63020** DC/DC buck-boost converter.

### Output

**3.3 V**
The converter provides the regulated 3.3 V supply required by the electronics.
Additional capacitors:

* 470 µF / 10 V bulk capacitor
* 10 µF 0603 X7R 10 V
* 100 nF 1206 50 V
  The main power switch is **SS12D11**.
  The power switch is placed in series with the battery supply so the entire system can be disconnected from the battery.

## FPC Interface

The project uses a **14-pin FPC connector** with:

* 0.5 mm pitch
* 14 contacts
* 10 cm FPC cable
  The project uses a specific FPC connector orientation, so the connector type must be selected carefully.

> **Important:** The first FPC connector purchased for the project turned out to be the wrong type/orientation. When reproducing the project, verify the connector orientation and contact direction before ordering.
> For easier prototyping and testing, a **14-pin FPC/FFC → 2.54 mm adapter** can also be used.

## PCB

The custom PCB for STERN OS was manufactured by **JLCPCB**.
The PCB integrates the electronics required by the cyberdeck and is designed around the extremely limited space available inside the Barkleys tin.
PCB production files and source design files will be provided in the `hardware` directory.
------------------------------------------------------------------------------------------

# Software

STERN OS is developed using:

* C++
* ESP32
* PlatformIO
* Visual Studio Code
  The project is organized into multiple modules rather than keeping the entire operating system inside one source file.

---
# Applications

## Terminal & Linux POSIX Shell

Das Terminal bietet eine vollständige Linux-artige Shell (`root@stern:~# `) mit Verzeichnis-Navigation, Redirection (`>`) und Standard-Tools:
* **Dateisystem:** `cd <dir>`, `pwd`, `ls [-l]`, `cat <file>`, `touch <file>`, `mkdir <dir>`, `rm <file>`, `rmdir <dir>`, `cp <src> <dst>`, `mv <src> <dst>`, `grep <pattern> <file>`, `head <file>`, `df -h`
* **System:** `uname -a`, `uptime`, `free -h`, `ps` / `top`, `whoami`, `date`, `reboot`, `clear`
* **Netzwerk:** `ifconfig`, `scan` (WLAN-Scan), `wifi connect <ssid> <pass>`, `ping <host>`, `wget <url> [file]`, `curl <url>`
* **Shell & Skripte:** Redirection (`echo "Text" > file.txt`), Ausführen von Shell-Skripten (`sh script.sh`), Hilfe (`help`, `man <cmd>`)

## File Browser

Allows files stored on the device to be browsed through a graphical interface.

## Text Editor

A lightweight text editor designed specifically for the pocket cyberdeck.

## Game of Life

A playable implementation of Conway's Game of Life.

## Dino Game

A small built-in arcade game.

## Multi-Emulator System
 
 STERN OS enthält ein modulares Emulator-Framework mit einheitlicher Steuerung und Launcher:
 * **NES (Nintendo Entertainment System)**: Lädt `.nes`-ROMs (`nes`-Befehl oder über Dateibrowser).
 * **Game Boy (DMG / Peanut-GB)**: Lädt `.gb`-ROMs (`gb`-Befehl oder über Dateibrowser).
 * **CHIP-8 / SuperChip**: Ultra-kompakter Emulator mit vorinstallierten Retro-Klassikern im Flash (z.B. Pong, Brix) sowie Unterstützung für `.ch8`-Dateien (`chip8`-Befehl).
 * **Universelles Spielemenü**: Aufrufbar über `emu` oder `games`.
 
 ### Einheitliche Emulator-Steuerung:
 * **D-Pad (Richtung):** `S` (Oben), `X` (Unten), `Z` (Links), `C` (Rechts)
 * **Aktionstasten:** `;` (A), `?` (B)
 * **System:** `Space` (Start), `Enter` (Select)
 * **Beenden / Zurück:** `ESC`
 
 ## Settings
 
 System settings are managed through a dedicated graphical settings interface (`gui`).

# Getting Started

## Requirements

* ESP32 development board
* PlatformIO
* Visual Studio Code
* USB Type-C cable
* STERN OS source code

Clone the repository:

```bash
git clone https://github.com/DmitriyShalagurov/STERN_OS-FOR-CYBERDECK---BARKLEYS-POCKET-TIN-.git
```


# Bill of Materials

| Component            | Part / Specification           |
| -------------------- | ------------------------------ |
| MCU                  | ESP32-WROOM / ESP32-DevKitC-32 |
| USB-UART             | CH340C                         |
| Keyboard Controller  | TCA8418RTWR                    |
| Keyboard Diodes      | 1N4148 SOD-323                 |
| Display              | 2.4" TFT SPI                   |
| Display Controller   | ST7735 / ST7789                |
| DC/DC Converter      | TPS63020                       |
| Power Switch         | SS12D11                        |
| Bulk Capacitor       | 470 µF / 10 V                  |
| Decoupling Capacitor | 10 µF 0603 X7R 10 V            |
| Decoupling Capacitor | 100 nF 1206 50 V               |
| FPC Connector        | 14P / 0.5 mm                   |
| FPC Cable            | 14P / 0.5 mm                   |
| FPC Adapter          | 14P → 2.54 mm                  |
| PCB Manufacturer     | JLCPCB                         |
| Enclosure            | Barkleys tin                   |

---

# Design Philosophy

The main challenge of this project is fitting a complete custom cyberdeck into an extremely small enclosure.
The design prioritizes:

* Compact dimensions
* Low power consumption
* Custom hardware
* Modular firmware
* Physical keyboard
* Expandability
* Repairability
* Easy experimentation
  The project intentionally uses commonly available components where possible.

---

# Photos

Project photos will be added here as development continues.

### Final Assembly

*Coming soon.*

### Custom PCB

*Coming soon.*

### Keyboard

*Coming soon.*

### Internal Layout

*Coming soon.*

# Roadmap

* [x] ESP32 firmware
* [x] Custom keyboard controller
* [x] TFT display
* [x] Custom PCB
* [x] Boot animation
* [x] GUI
* [x] File browser
* [x] Text editor
* [x] Terminal
* [x] Game of Life
* [x] Dino Game
* [x] NES emulator
* [ ] Final hardware revision
* [ ] Final FPC connector revision
* [ ] Complete hardware documentation
* [ ] Complete schematic documentation
* [ ] PCB source files
* [ ] Manufacturing files
* [ ] Full build guide
* [ ] Final enclosure documentation

---

# Project Status

**STERN OS is an experimental DIY project and is still under active development.**
Hardware and software may change between revisions.
The current repository represents the development version of the project and may contain unfinished or experimental features.
-----------------------------------------------------------------------------------------------------------------------------

# Contributing

Suggestions, bug reports, improvements and hardware modifications are welcome.
If you build your own version of STERN OS, feel free to share your build and modifications.
-------------------------------------------------------------------------------------------

# License

## License information will be added once the project license is finalized.

# Support the Project

If you find this project interesting, consider giving the repository a on GitHub.
It helps the project get discovered by other hardware and cyberdeck enthusiasts.
--------------------------------------------------------------------------------

**STERN OS**
*Pocket computer. Custom hardware. Built from scratch.*
