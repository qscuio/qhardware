# ESP32-S3 board probe

Verification state: `HARDWARE_VERIFIED`

## Objective

Identify and exercise an ESP32-S3 without assuming any board-specific GPIO
mapping. The probe verifies bounded Flash reads, allocated PSRAM writes and
reads, FreeRTOS task scheduling, anonymous Wi-Fi scanning, and BLE advertising.

The expected tested configuration is an ESP32-S3 with 16 MB DIO Flash and 8 MB
octal PSRAM. Confirm those values with esptool before flashing. This exercise
does not identify or drive an onboard RGB LED, camera, display, codec, or header
pin because those connections vary by development-board product.

## Dependencies

- ESP-IDF 5.5.5 with the `esp32s3` Xtensa tools installed
- A data-capable USB cable
- Python 3 with `pyserial` for redacted serial capture
- Windows Python with `bleak` for the supplied over-the-air BLE check

Install the optional host packages in a virtual environment:

```text
python -m pip install pyserial bleak
```

## Build and flash

Windows:

```powershell
.\scripts\build.ps1 -IdfPath C:\esp\esp-idf-v5.5.5
.\scripts\flash.ps1 -IdfPath C:\esp\esp-idf-v5.5.5 -Port COM11
python .\scripts\capture_serial.py --port COM11 --output probe-serial.log
python .\scripts\scan_ble.py
```

Linux:

```sh
./scripts/build.sh /opt/esp-idf-v5.5.5
./scripts/flash.sh /opt/esp-idf-v5.5.5 /dev/ttyACM0
python scripts/capture_serial.py --port /dev/ttyACM0 --output probe-serial.log
```

The BLE script intentionally reports only `QHW-ESP32S3-PROBE`. It does not
print unrelated nearby devices or addresses. The serial-capture script replaces
address-shaped values before saving or printing the log. It opens USB control
lines inactive and waits for the five-second heartbeat, so native USB does not
fall back into ROM download mode. Add `--reset` only for a bridge whose RTS line
is known to reset the board normally.

## Pass criteria

The serial log must contain `FLASH TEST PASS`, `PSRAM TEST PASS`,
`FREERTOS TEST PASS`, `WIFI SCAN PASS`, and `BLE ADVERTISE PASS`. A separate
scanner must receive `QHW-ESP32S3-PROBE` to prove over-the-air BLE operation.

## Recovery

If the board no longer enters the app, hold BOOT, tap RESET/EN, release BOOT,
list ports again, and re-run the flash command on the re-enumerated port. Restore
vendor firmware only from a private, size-checked backup read from that exact
board.
