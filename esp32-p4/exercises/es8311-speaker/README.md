# ES8311 speaker output

Verification state: `HARDWARE_VERIFIED`

## Objective

Initialize the board's ES8311 output codec and I2S path, then generate a low-volume 440 Hz square-wave tone in memory and repeatedly write exactly 640,000 bytes to the speaker path.

## Hardware and wiring

- Waveshare ESP32-P4-WIFI6
- A suitable small passive speaker connected to the two-pin `SPK` socket

The onboard `MIC` is an input and is not the speaker connector. Power down before attaching the speaker. Do not connect either SPK pin to a GPIO or ground unless the board schematic explicitly identifies it that way; the two pins form the amplified output pair.

## Dependencies

- ESP-IDF 5.5.5
- Waveshare `esp32_p4_platform` 2.0.1
- Espressif `esp_codec_dev` 1.5 or newer

## Build and flash

Use `scripts/build.ps1` and `scripts/flash.ps1` on Windows, or the matching `.sh` scripts on Linux. Both scripts require an ESP-IDF path, and the flash script also requires the serial port.

The source generates its own test tone and contains no copyrighted music. The Waveshare upstream `12_I2SCodec` example demonstrates the same codec path with its separately licensed Canon PCM asset: <https://github.com/waveshareteam/ESP32-P4-Platform/tree/main/examples/esp-idf/12_I2SCodec>.

Monitor at 115200 baud. Confirm codec initialization, audible low-volume tone output, and repeated `wrote 640000 bytes` lines.

## Recovery

If initialization succeeds but the speaker is silent, verify the SPK connector, passive speaker impedance, amplifier route, and volume. Stop immediately if the speaker or board becomes hot or the sound is severely distorted.
