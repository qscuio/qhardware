# Oscilloscope signal generator

Verification state: `BUILD_VERIFIED`

## Objective

Generate three repeatable signals for learning oscilloscope triggering and measurements.

## Parts

- Arduino Uno\n- Oscilloscope or logic analyzer\n- Probe ground connected to Uno GND

## Wiring

| Output | Signal |\n| --- | --- |\n| D9 | 100 Hz to 10 kHz sweep |\n| D8 | double-pulse heartbeat once per second |\n| D6 | about 980 Hz PWM with changing duty cycle |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

No external library is required.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/scope-signal
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/scope-signal` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

D9 sweeps up and down over four seconds, D8 emits a double pulse every second, and D6 duty cycle rises and falls.

## Validation

Measure all three pins. Confirm D8 is a stable trigger source and that D9 frequency and D6 duty cycle vary over the documented periods.

## Recovery

Use a shared probe ground and start with D8 at a 500 ms/div timebase. This sketch owns Timer1, so do not combine it with `Servo`.
