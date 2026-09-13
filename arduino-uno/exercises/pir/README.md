# PIR motion counter

Verification state: `BUILD_VERIFIED`

## Objective

Warm up a PIR sensor, show motion state on a 16x2 LCD, and count rising motion events.

## Parts

- Arduino Uno\n- digital-output PIR sensor\n- HD44780-compatible 16x2 LCD and contrast potentiometer

## Wiring

| Signal | Uno |\n| --- | --- |\n| PIR OUT | D7 |\n| PIR VCC/GND | 5 V/GND |\n| LCD RS/E | D12/D11 |\n| LCD D4..D7 | D5, D4, D3, D2 |\n| LCD RW | GND |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

Uses the `LiquidCrystal` library supplied with Arduino AVR Boards 1.8.8.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/pir
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/pir` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

The LCD displays a 30-second warm-up, then `Idle` or `Motion` plus a count. The 9600-baud monitor logs state changes.

## Validation

After warm-up, wait for idle, move a hand across the sensor, then wait for idle again. Confirm one new count and matching serial transitions.

## Recovery

Allow the full warm-up, reduce sensor sensitivity if it chatters, and keep moving heat sources away during the idle check.
