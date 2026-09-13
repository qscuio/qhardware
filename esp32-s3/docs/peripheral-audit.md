# Audit ESP32-S3 peripherals

Separate three facts when inventorying a board: capability built into the SoC,
components mounted on the development board, and devices connected externally.
An ESP32-S3 supporting a peripheral does not prove that a connector or sensor
is wired to it.

## Start with evidence

1. Photograph both sides and transcribe silkscreen exactly.
2. Find the matching schematic and board revision.
3. Create a table of connector label, voltage, signal, SoC GPIO, and source page.
4. Power down before continuity testing or changing wiring.
5. Probe one peripheral at a time and save the boot log and pass criterion.

## Audit checklist

| Area | Establish first | Safe functional test |
| --- | --- | --- |
| USB | connector route, native USB or bridge, VBUS path | enumerate, reset, enter ROM downloader |
| Buttons | BOOT/RESET labels and schematic GPIO | log level transitions with internal pull configured as documented |
| RGB/status LED | LED type, supply, GPIO, active level | low-brightness primary-color cycle on the documented pin only |
| GPIO headers | voltage and GPIO/header-number mapping | loopback between two documented free pins through a resistor |
| I2C | voltage, pull-ups, SDA/SCL pins | address scan with known devices disconnected first |
| SPI | CS/clock/data pins and voltage | JEDEC-ID or loopback test at a conservative clock |
| UART | voltage and TX/RX mapping | local loopback; never connect two TX outputs together |
| Camera/display | pitch, contact side, pinout, voltage | identify over SCCB/I2C before enabling high-speed data |
| Audio | codec/amplifier, MIC input, speaker output | codec register read, then a low-amplitude generated tone |
| SD card | socket wiring and supply | mount a disposable card and read/write a temporary file |
| PSRAM | type, voltage, bus mode, size | boot memory test plus allocated pattern write/read |
| Flash | voltage, bus mode, capacity | esptool ID, boot image validation, and bounded read |
| Wi-Fi | antenna path and 2.4 GHz support | anonymous scan; association only with ignored local credentials |
| BLE | controller/host configuration | named advertisement plus reception on a second device |

Never sweep unknown GPIOs by driving them. Pins may control power rails, flash,
PSRAM, USB, strapping, displays, cameras, or amplifiers. When the schematic is
unavailable, restrict the first probe to SoC/memory/radio functions that do not
drive board-specific pins.

## Preserve useful evidence

Record board marking, revision, firmware commit, ESP-IDF version, command, and
the minimum output that proves the test. Redact addresses, SSIDs, nearby device
names, passwords, machine paths, and private backups before committing.
