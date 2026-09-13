# Verification method

1. Photograph the board and record its silkscreen outside the public log.
2. Identify the current port by disconnecting and reconnecting the board.
3. Enter the ROM downloader and run `chip_id`, `flash_id`, and
   `get_security_info`; remove device addresses from retained output.
4. Confirm ESP32-S3 revision, package, 16 MB Flash, 8 MB embedded PSRAM, and
   disabled/enabled security features before changing firmware.
5. Read the full 16 MB Flash to private storage, verify its byte length, and
   save a SHA-256 checksum outside Git.
6. Build and flash the probe with ESP-IDF 5.5.5. Confirm esptool verifies the
   hash of the bootloader, partition table, and application writes.
7. Reset and capture the serial log through all five PASS markers. Confirm the
   new second-stage bootloader starts without an image checksum or SHA warning.
   For native USB, power-cycle without holding BOOT and run the capture script
   without `--reset`; wait for the repeated heartbeat if initial lines passed
   before the port opened.
8. Scan for `QHW-ESP32S3-PROBE` from a second Bluetooth device for at least 15
   seconds and record only its name and RSSI.
9. Redact addresses, SSIDs, nearby names, private paths, and backup hashes from
   public runtime evidence when they identify a particular device.

Flash and PSRAM size assertions apply to the tested N16R8-class boards. Change
the configuration and backup length only after identifying a different variant.
