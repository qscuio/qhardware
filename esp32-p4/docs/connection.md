# Connect and identify the ESP32-P4-WIFI6

## Required equipment

- Waveshare ESP32-P4-WIFI6
- Data-capable USB-C cable
- Windows 10/11 or a current Linux distribution
- A stable USB port or powered hub

Disconnect external peripherals for the first enumeration test. A charge-only cable can light the power LED while providing no serial port.

## Windows

1. Connect the board's USB-C programming/serial connector.
2. Open Device Manager and inspect **Ports (COM & LPT)**. The tested board enumerates through a WCH CH343 USB serial device.
3. In PowerShell, list candidate ports:

   ```powershell
   Get-CimInstance Win32_SerialPort |
       Select-Object DeviceID, Name, PNPDeviceID
   ```

4. Save the observed port for commands in this repository:

   ```powershell
   $Port = 'COM5' # replace with the port just identified
   ```

5. Open a 115200-baud monitor with ESP-IDF:

   ```powershell
   idf.py -p $Port monitor
   ```

Install the current WCH CH34x driver from Waveshare or WCH if no port appears and Windows shows an unknown USB device. Reconnect the cable after installation and identify the port again.

## Linux

1. Capture the device list before and after connecting the board:

   ```sh
   ls -l /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
   dmesg --follow
   ```

2. Save the newly enumerated port:

   ```sh
   PORT=/dev/ttyUSB0 # replace with the observed device
   ```

3. Check ownership and group access:

   ```sh
   stat "$PORT"
   id
   ```

   Add the current account to the distribution's serial group, commonly `dialout`, only when the device permissions require it. Log out and back in after changing group membership.

4. Monitor at 115200 baud:

   ```sh
   idf.py -p "$PORT" monitor
   ```

## Reset and download mode

Normal `idf.py flash` toggles the USB serial control lines and automatically enters the ROM downloader. If automatic entry fails:

1. Hold **BOOT**.
2. Press and release **RESET**.
3. Release **BOOT**.
4. Run the flash command immediately.

A normal reset without BOOT starts the application from flash. Do not hold BOOT during ordinary runtime tests.

## Diagnostic order

1. Check power LED and cable data capability.
2. Confirm the port appears and disappears with the board.
3. Close SecureCRT, Arduino IDE, serial monitors, and other programs that may own the port.
4. Verify the selected port and 115200 baud.
5. Press RESET and capture the ROM banner and reset reason.
6. Verify the ESP-IDF version and target before changing firmware.
7. Back up the full flash before erasing or replacing unknown firmware.

## Peripheral connection rules

Open an FFC/FPC connector latch before inserting a ribbon and close it evenly after the cable is fully seated. Do not force a camera ribbon of a different pitch or contact orientation into the connector. Use a purpose-built adapter cable when the camera and host connectors differ.

For audio, the onboard microphone is an input and the two-pin `SPK` connector is the speaker output. Power down before connecting or disconnecting either camera or speaker.
