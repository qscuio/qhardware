# Connect and identify an ESP32-S3 board

Use a data-capable USB cable and disconnect external peripherals during the
first inspection. A power LED does not prove the cable carries data.

## Find the port on Windows

Capture the list before and after connecting the board:

```powershell
Get-CimInstance Win32_SerialPort |
    Select-Object DeviceID, Name, PNPDeviceID
Get-PnpDevice -PresentOnly |
    Where-Object Class -eq 'Ports' |
    Select-Object Status, FriendlyName, InstanceId
```

Common observations include a bridge such as WCH CH343
(`VID_1A86&PID_55D3`) or an Espressif native USB interface (`VID_303A`). The
USB identity describes the active transport, not the complete board model.

```powershell
$Port = 'COM11' # replace with the newly observed port
```

## Find the port on Linux

```sh
ls -l /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
dmesg --follow
```

A bridge normally appears as `/dev/ttyUSB0`; native USB commonly appears as
`/dev/ttyACM0`. Verify the actual device each time:

```sh
PORT=/dev/ttyACM0
udevadm info --query=property --name="$PORT"
```

## Enter the ROM downloader

Close all serial monitors, then try an esptool query. If automatic reset fails,
hold **BOOT**, press and release **RESET/EN**, wait one second, then release
**BOOT**. A board without RESET can be connected while BOOT is held. Native USB
may disconnect and re-enumerate with a new port in download mode, so list ports
again before continuing.

## Identify the SoC and memory

With ESP-IDF exported and `PORT` set:

Windows:

```powershell
python "$env:IDF_PATH\components\esptool_py\esptool\esptool.py" `
    --chip auto -p $Port chip_id
python "$env:IDF_PATH\components\esptool_py\esptool\esptool.py" `
    --chip esp32s3 -p $Port flash_id
python "$env:IDF_PATH\components\esptool_py\esptool\esptool.py" `
    --chip esp32s3 -p $Port get_security_info
```

Linux:

```sh
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" \
  --chip auto -p "$PORT" chip_id
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" \
  --chip esp32s3 -p "$PORT" flash_id
python "$IDF_PATH/components/esptool_py/esptool/esptool.py" \
  --chip esp32s3 -p "$PORT" get_security_info
```

Record the chip family, package, silicon revision, crystal frequency, physical
flash size, flash voltage, and reported embedded PSRAM. Do not publish device
addresses. Confirm PSRAM size and mode again in an ESP-IDF boot log because the
second-stage bootloader initializes and tests it.

## Determine the exact board product

Use evidence in this order:

1. Read the development-board and RF-module silkscreen on both sides.
2. Count and label USB connectors, buttons, FPC connectors, audio jacks, and pin
   headers.
3. Match the markings and connector layout to a vendor product page and
   schematic.
4. Compare the schematic's USB path, flash/PSRAM part, RGB LED, and button GPIOs
   with measured behavior.

Do not infer an exact product solely from `ESP32-S3`, `N16R8`, a USB VID/PID, or
a similar-looking board. Multiple boards share those attributes.
