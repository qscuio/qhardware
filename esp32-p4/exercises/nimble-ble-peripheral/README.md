# ESP-Hosted NimBLE peripheral

Verification state: `HARDWARE_VERIFIED`

## Objective

Run the NimBLE host on ESP32-P4 while using the onboard ESP32-C6 as the BLE controller over ESP-Hosted HCI/SDIO. The application advertises the connectable name `nimble-bleprph` and exposes the example GATT services.

This hardware path supports Bluetooth Low Energy. The tested C6 firmware reports `BLE only`; Bluetooth Classic profiles are outside this exercise.

## Hardware and dependencies

- Waveshare ESP32-P4-WIFI6 and USB data cable
- ESP-IDF 5.5.5
- ESP-Hosted 1.4.x and ESP Wi-Fi Remote 0.14.x
- A phone or computer with a BLE scanner for over-the-air confirmation

No external wiring is required.

## Build, flash, and inspect

Run the supplied build and flash scripts, reset the board, and monitor at 115200 baud. The serial pass criteria are hosted Bluetooth enabled, VHCI selected, NimBLE host task started, and GAP advertising initiated.

Use a BLE scanner to look for `nimble-bleprph`. Device addresses may be randomized or cached by the scanner; identify the device by its advertised name and expected GATT service rather than storing its address.

## Recovery

If hosted initialization fails, confirm the C6 capability event and the SDIO pins in the boot log. If the board reports advertising but another scanner sees nothing, verify that the scanner radio is on, scan from a second phone, move both devices away from USB 3 cables or metal, and confirm that no stale connection has stopped advertising.
