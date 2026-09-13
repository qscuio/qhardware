# Test and debug the ESP32-S3 bootloader

An ESP32-S3 boot has three observable stages: the mask-ROM downloader/loader,
the second-stage bootloader in flash, and the selected application partition.

## Capture a cold boot

Monitor at 115200 baud and reset without holding BOOT:

```text
idf.py -p PORT monitor
```

Keep the ROM banner, reset reason, boot mode, flash mode/frequency, second-stage
ESP-IDF version, flash size, partition table, selected app offset, app SHA, chip
revision, PSRAM initialization, and first application line. A checksum or SHA
warning belongs to the stage named immediately before it; do not assume the app
is corrupt when the warning names the bootloader.

## Read and inspect without flashing

Read the bootloader, partition table, and factory-app header to private files:

```text
python ESPEFUSE --chip esp32s3 -p PORT summary
python ESPTOOL --chip esp32s3 -p PORT read_flash 0x0 0x8000 bootloader-region.bin
python ESPTOOL --chip esp32s3 -p PORT read_flash 0x8000 0x1000 partition-table.bin
python ESPTOOL --chip esp32s3 image_info bootloader-region.bin
```

Decode the partition table with ESP-IDF's `gen_esp32part.py`, then use the
reported app offset and size for a bounded read. Run `esptool image_info` on the
app image to inspect its project name, build time, ESP-IDF version, configured
flash size, checksum, and appended SHA-256.

## Validate a newly built boot chain

```text
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash
idf.py -p PORT monitor
```

A pass requires all flashed regions to report `Hash of data verified`, a normal
reset into the expected partition, no image checksum/SHA warning, a matching app
project name/SHA, and the expected application pass marker. Keep the exact ELF
for address-to-line and core-dump decoding.

## Failure order

| Symptom | Check next |
| --- | --- |
| No serial data | cable, active USB connector, port owner, console transport |
| Esptool cannot connect | current port, BOOT/RESET sequence, re-enumerated native USB port |
| Native USB stays at `boot:0x0 (DOWNLOAD)` after flashing | close the monitor, release BOOT, then press RESET or power-cycle without touching BOOT |
| Wrong boot mode | strapping pins and external circuits on BOOT GPIO |
| Invalid header | selected offset, flash mode/voltage, damaged image |
| SHA comparison fails | identify whether bootloader or app image emitted it; inspect that region |
| App repeatedly resets | first reset reason, panic, watchdog, supply stability |
| PSRAM boot failure | octal/quad mode, voltage, speed, size, board/module variant |

Never disable verification or watchdogs to hide the first failure. Preserve the
original flash before replacing an unknown bootloader.

When capturing through native USB, open the port with DTR and RTS inactive.
Some serial programs assert both lines while opening and immediately select the
ROM downloader again. The board-probe capture script uses inactive lines by
default and only pulses RTS when explicitly passed `--reset`.
