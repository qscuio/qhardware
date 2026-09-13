# Firmware build, upload, and recovery

These commands use Arduino CLI and keep board and port choices explicit.

## Install the toolchain

Install Arduino CLI from its official release or package for the operating system, then install the AVR platform:

```sh
arduino-cli config init
arduino-cli core update-index
arduino-cli core install arduino:avr@1.8.8
arduino-cli lib install "Adafruit PWM Servo Driver Library@3.0.3"
```

`Servo`, `Wire`, `EEPROM`, and `LiquidCrystal` are supplied by the AVR core. The PCA9685 exercise also needs Adafruit BusIO, which Arduino CLI resolves when installing the PWM driver library.

## Compile

From the repository root on Windows:

```powershell
.\arduino-uno\scripts\compile-all.ps1 -BuildCache "$env:TEMP\qhardware-arduino-cache"
```

On Linux:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
```

The scripts stage each sketch in a temporary matching-name directory because Arduino CLI requires the primary `.ino` name to match its containing sketch directory. To upload the Blink result after the Linux build:

```sh
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/blink
```

## Upload and monitor

Replace `PORT` with the value from `arduino-cli board list`:

```sh
arduino-cli monitor -p PORT --config baudrate=9600
```

On Windows a port resembles `COM4`; on Linux it commonly resembles `/dev/ttyACM0` or `/dev/ttyUSB0`.

## Core and library updates

List available updates first, then rebuild all exercises before uploading:

```sh
arduino-cli core update-index
arduino-cli core list
arduino-cli lib update-index
arduino-cli lib outdated
```

This repository pins known versions in the install example. Update a pin deliberately, compile all sketches, and record the new result rather than silently changing the environment.

## Bootloader recovery

If USB serial works but a verified Blink upload repeatedly fails, use another Arduino as ISP or an AVR programmer. In the IDE select the target Uno and programmer, wire ICSP signals plus 5 V/GND, then choose **Burn Bootloader**. With Arduino CLI, derive the exact programmer ID from `arduino-cli upload --help` and use the supported programmer command for the installed CLI version. Remove all shields and external loads during recovery.
