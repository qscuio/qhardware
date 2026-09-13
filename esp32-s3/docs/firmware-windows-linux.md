# Build, flash, back up, and restore ESP32-S3 firmware

The tested toolchain is ESP-IDF 5.5.5. The tested memory configuration is 16 MB
flash with 8 MB octal PSRAM. Confirm those values on each board before using the
backup or restore sizes below.

## Windows

Install ESP-IDF and the ESP32-S3 Xtensa tools, then export the environment:

```powershell
$IdfPath = 'C:\esp\esp-idf-v5.5.5' # replace with the installation directory
& "$IdfPath\install.ps1" esp32s3
. "$IdfPath\export.ps1"
idf.py --version
```

From an exercise directory:

```powershell
$Port = 'COM11' # replace after checking enumeration
idf.py set-target esp32s3
idf.py reconfigure
idf.py build
idf.py -p $Port flash
idf.py -p $Port monitor
```

## Linux

```sh
git clone --branch v5.5.5 --recursive https://github.com/espressif/esp-idf.git esp-idf-v5.5.5
cd esp-idf-v5.5.5
./install.sh esp32s3
. ./export.sh
idf.py --version
```

From an exercise directory:

```sh
PORT=/dev/ttyACM0 # replace after checking enumeration
idf.py set-target esp32s3
idf.py reconfigure
idf.py build
idf.py -p "$PORT" flash
idf.py -p "$PORT" monitor
```

## Back up a confirmed 16 MB board

Close monitors and verify the port immediately before reading. Raw flash can
contain credentials, calibration data, and device identifiers; keep it outside
Git.

Windows:

```powershell
$Port = 'COM11'
$Backup = Join-Path $PWD 'esp32-s3-private-full-backup.bin'
python "$env:IDF_PATH\components\esptool_py\esptool\esptool.py" `
    --chip esp32s3 -p $Port read_flash 0 0x1000000 $Backup
if ((Get-Item -LiteralPath $Backup).Length -ne 0x1000000) { throw 'Backup is not 16 MB' }
Get-FileHash -Algorithm SHA256 -LiteralPath $Backup
```

Linux:

```sh
PORT=/dev/ttyACM0
BACKUP=esp32-s3-private-full-backup.bin
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" \
  --chip esp32s3 -p "$PORT" read_flash 0 0x1000000 "$BACKUP"
test "$(stat -c %s "$BACKUP")" -eq 16777216
sha256sum "$BACKUP"
```

## Restore the same board

Verify the saved checksum and exact size before writing. Restore only to the
board from which the private image was read.

Windows:

```powershell
if ((Get-Item -LiteralPath $Backup).Length -ne 0x1000000) { throw 'Backup is not 16 MB' }
python "$env:IDF_PATH\components\esptool_py\esptool\esptool.py" `
    --chip esp32s3 -p $Port write_flash 0 $Backup
```

Linux:

```sh
test "$(stat -c %s "$BACKUP")" -eq 16777216
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" \
  --chip esp32s3 -p "$PORT" write_flash 0 "$BACKUP"
```

Reset and capture a complete boot log after restoring.
