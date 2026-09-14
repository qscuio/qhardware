# ESP32-S3 development boards

The physically tested boards use an ESP32-S3 revision 0.2 in a QFN56 package,
16 MB flash, and 8 MB embedded octal PSRAM. ESP-IDF reports two Xtensa LX7
cores, native 2.4 GHz Wi-Fi, and Bluetooth Low Energy. One board enumerated
through a WCH CH343 bridge. The other used an application-defined native USB
CDC port in its factory firmware, then the ESP32-S3 USB Serial/JTAG interface
in download and probe-firmware modes. These observations identify the SoC and
memory configuration, but do not prove a particular development-board product
name.

The software baseline is ESP-IDF 5.5.5. ESP-IDF starts FreeRTOS before calling
`app_main()`.

## Guides

- [Connect and identify the board](docs/connection-and-identification.md)
- [Audit onboard and connected peripherals](docs/peripheral-audit.md)
- [Test and debug the bootloader](docs/bootloader-debugging.md)
- [Build, flash, back up, and restore firmware](docs/firmware-windows-linux.md)

## Exercises

- [Board, memory, FreeRTOS, Wi-Fi, and BLE probe](exercises/board-probe/README.md)

## Identification boundary

`esptool`, eFuse data, boot logs, and USB descriptors can establish the SoC,
revision, package, memory, security state, and active USB interface. Exact board
identity, connector pinout, RGB LED GPIO, and peripheral population require
board silkscreen, module markings, a schematic, or a vendor product page.
