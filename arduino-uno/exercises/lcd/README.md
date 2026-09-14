# 16x2 LCD counter

Verification state: `BUILD_VERIFIED`

## Objective

Drive an HD44780-compatible 16x2 display in four-bit parallel mode and update a seconds counter.

## Parts

- Arduino Uno\n- HD44780-compatible 16x2 LCD\n- 10 kΩ contrast potentiometer\n- 220 Ω backlight resistor when required by the module

## Wiring

| LCD signal | Uno |\n| --- | --- |\n| RS | D12 |\n| E | D11 |\n| D4, D5, D6, D7 | D5, D4, D3, D2 |\n| RW | GND |\n| VSS/VDD | GND/5 V |\n| VO | contrast potentiometer wiper |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

Uses the `LiquidCrystal` library supplied with Arduino AVR Boards 1.8.8.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/lcd
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/lcd` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

The display shows `Hello Arduino` and a seconds counter. The 9600-baud monitor prints matching `seconds=N` lines.

## Validation

Observe the display for ten seconds and confirm the counter increments once per second without leaving stale digits.

## Recovery

If the backlight is on but text is absent, adjust contrast, tie RW to ground, and recheck the four data wires in order.
