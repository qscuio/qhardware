# Waveshare ESP32-P4-WIFI6

The tested board is a Waveshare ESP32-P4-WIFI6 with an ESP32-P4 revision 3.2, 32 MB flash, 32 MB PSRAM, an ESP32-C6-MINI-1 wireless companion, a MIPI-CSI camera connector, and an ES8311 audio codec. The software baseline is ESP-IDF 5.5.5.

ESP32-P4 has no native Wi-Fi or Bluetooth radio. On this board, ESP-Hosted carries Wi-Fi and BLE traffic between the P4 and the onboard C6 over SDIO. The tested C6 firmware reports WLAN and BLE support; it does not report Bluetooth Classic support.

## Guides

- [Connect and identify the board](docs/connection.md)
- [Debug ESP-IDF and FreeRTOS](docs/freertos-debugging.md)
- [Build, flash, back up, and restore firmware](docs/firmware-windows-linux.md)

## Exercises

- [OV5647 MIPI-CSI camera probe](exercises/ov5647-camera/README.md)
- [ES8311 speaker output](exercises/es8311-speaker/README.md)
- [ESP-Hosted Wi-Fi station](exercises/wifi-station/README.md)
- [ESP-Hosted NimBLE peripheral](exercises/nimble-ble-peripheral/README.md)

## Audio connector note

The onboard `MIC` is an input to the ES8311 codec. The two-pin `SPK` connector is the amplified speaker output. Connect a suitable small passive speaker to `SPK`; do not connect a speaker to a GPIO, the microphone input, or a line-level input without checking the schematic and electrical requirements.

## Runtime model

ESP-IDF applications run on FreeRTOS. ESP-IDF creates a main task and calls `app_main()` from it. Applications may create their own tasks, queues, semaphores, event groups, software timers, and watchdog subscriptions. See the debugging guide before increasing task stacks or disabling watchdogs.
