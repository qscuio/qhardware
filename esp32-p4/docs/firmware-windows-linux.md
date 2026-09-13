# Build, update, back up, and restore ESP32-P4 firmware

The tested toolchain is ESP-IDF 5.5.5. Keep each exercise's generated `build`, `managed_components`, dependency lock, local SDK configuration, and device backup outside Git.

## Windows

Install ESP-IDF 5.5.5 with Espressif's Windows installer or the documented Git/Python flow. Open an ESP-IDF PowerShell or import its environment into the current shell:

```powershell
$IdfPath = 'C:\esp\esp-idf-v5.5.5' # replace with the installation directory
. "$IdfPath\export.ps1"
idf.py --version
```

From an exercise directory:

```powershell
$Port = 'COM5' # replace after checking Device Manager
idf.py set-target esp32p4
idf.py reconfigure
idf.py build
idf.py -p $Port flash
idf.py -p $Port monitor
```

Exit ESP-IDF monitor with `Ctrl+]`. If dependencies are declared in `idf_component.yml`, `idf.py reconfigure` downloads compatible components before compilation.

## Linux

Install the prerequisites for the distribution, clone the `v5.5.5` ESP-IDF tag, install the ESP32-P4 tools, and export the environment:

```sh
git clone --branch v5.5.5 --recursive https://github.com/espressif/esp-idf.git esp-idf-v5.5.5
cd esp-idf-v5.5.5
./install.sh esp32p4
. ./export.sh
idf.py --version
```

From an exercise directory:

```sh
PORT=/dev/ttyUSB0 # replace after checking enumeration
idf.py set-target esp32p4
idf.py reconfigure
idf.py build
idf.py -p "$PORT" flash
idf.py -p "$PORT" monitor
```

## Back up the full 32 MB flash

Back up unknown or vendor firmware before erasing. Close serial monitors and verify the port immediately before reading.

Windows:

```powershell
$Port = 'COM5'
$Backup = Join-Path $PWD 'esp32-p4-full-backup.bin'
python "$env:IDF_PATH\components\esptool_py\esptool\esptool.py" `
    --chip esp32p4 -p $Port read_flash 0 0x2000000 $Backup
Get-FileHash -Algorithm SHA256 $Backup
```

Linux:

```sh
PORT=/dev/ttyUSB0
BACKUP=esp32-p4-full-backup.bin
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" \
  --chip esp32p4 -p "$PORT" read_flash 0 0x2000000 "$BACKUP"
sha256sum "$BACKUP"
```

Store the backup and checksum privately. Raw flash can contain device settings and credentials.

## Erase and restore

An ordinary application update uses `idf.py flash`; a full erase is rarely necessary. Run an erase only after verifying the port and confirming that a usable backup exists:

```text
idf.py -p PORT erase-flash
```

Restore the exact 32 MB backup to the same board with stable power.

Windows:

```powershell
$Port = 'COM5'
$Backup = 'D:\private-backups\esp32-p4-full-backup.bin'
if ((Get-Item $Backup).Length -ne 0x2000000) { throw 'Backup is not 32 MB' }
python "$env:IDF_PATH\components\esptool_py\esptool\esptool.py" `
    --chip esp32p4 -p $Port write_flash 0 $Backup
```

Linux:

```sh
PORT=/dev/ttyUSB0
BACKUP=/media/private/esp32-p4-full-backup.bin
test "$(stat -c %s "$BACKUP")" -eq 33554432
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" \
  --chip esp32p4 -p "$PORT" write_flash 0 "$BACKUP"
```

Reset the board and capture its boot log after a restore. If verification fails, stop and recheck the backup hash, board identity, USB stability, and flash size before another write.
