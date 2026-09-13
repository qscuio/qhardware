# OV5647 MIPI-CSI camera probe

Verification state: `HARDWARE_VERIFIED`

## Objective

Initialize the Waveshare ESP32-P4-WIFI6 CSI interface, identify an OV5647 sensor, map V4L2 capture buffers, and verify that a complete frame reaches application memory.

## Hardware and wiring

- Waveshare ESP32-P4-WIFI6 revision using ESP32-P4 revision 3.2
- OV5647 camera module
- The Waveshare camera adapter ribbon supplied for this connector combination

Power off the board. Open each connector latch, insert the correct end of the adapter fully with the contact side matching the Waveshare board documentation, and close the latch evenly. The sensor control bus is routed internally as SDA GPIO7 and SCL GPIO8. Do not join two different camera ribbons by pressing their exposed contacts together.

## Dependencies

- ESP-IDF 5.5.5
- Espressif `esp_video` component 2.2.x

## Build and flash

Windows:

```powershell
./scripts/build.ps1 -IdfPath C:\esp\esp-idf-v5.5.5
./scripts/flash.ps1 -IdfPath C:\esp\esp-idf-v5.5.5 -Port COM5
```

Linux:

```sh
./scripts/build.sh /opt/esp/esp-idf-v5.5.5
./scripts/flash.sh /opt/esp/esp-idf-v5.5.5 /dev/ttyUSB0
```

Monitor at 115200 baud. A pass requires OV5647 PID `0x5647`, an 800 by 800 RGB565 output frame, 1,280,000 captured bytes, and the pass line in [expected output](verification/expected-output.txt).

## Recovery

If sensor initialization fails, disconnect power and reseat both ribbon ends. Confirm the cable pitch and contact orientation before another attempt. If the image fails to boot, confirm ESP32-P4 revision 3.x and rebuild after deleting only the generated `build` directory.

See [the verification method](verification/method.md) and [the dated hardware log](verification/actual-output.txt).
