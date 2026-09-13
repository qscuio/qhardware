# Connection and wiring

## USB connection

Use a data-capable USB cable. A genuine Uno normally appears as an Arduino USB serial device; many compatible boards use a CH340/CH341 USB-to-serial chip and may need its vendor driver on Windows.

Windows:

```powershell
Get-CimInstance Win32_SerialPort | Select-Object DeviceID, Name
arduino-cli board list
```

Linux:

```sh
ls -l /dev/ttyACM* /dev/ttyUSB* 2>/dev/null
arduino-cli board list
```

On Linux, add the current user to the serial-port group if access is denied, then sign out and in:

```sh
sudo usermod -aG dialout "$USER"
```

## Electrical rules

- Classic Uno GPIO and I2C operate at 5 V.
- Never feed more than 5 V into a GPIO pin.
- Use a level shifter or divider when a peripheral input is limited to 3.3 V.
- Join the Uno ground to every external supply ground.
- Do not power several servos, a robot arm, or other inductive loads from the Uno 5 V pin. Use a regulated external supply sized for stall current.
- Disconnect power before moving wires. Verify pin labels instead of relying on wire colors.

Select `arduino:avr:uno` for the classic ATmega328P board. Keep the port as a command-line parameter; ports differ by computer.
