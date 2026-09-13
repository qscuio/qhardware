# ECG-like waveform simulator

Verification state: `BUILD_VERIFIED`

## Objective

Create a PWM-encoded ECG-like waveform with a synchronized digital R-peak trigger.

## Parts

- Arduino Uno\n- Oscilloscope\n- Optional RC low-pass filter for D3 (for example 3.3 kΩ and 1 µF)

## Wiring

| Output | Signal |\n| --- | --- |\n| D3 | 62.5 kHz PWM carrying ECG-like amplitude |\n| D6 | about 976 Hz 50% PWM filter test |\n| D8 | digital trigger aligned with R peak |\n| GND | probe and filter ground |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

No external library is required.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/ecg-simulator
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/ecg-simulator` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

D8 pulses about every 833 ms (roughly 72 BPM). A low-pass filtered D3 signal shows P, QRS, and T-like features with slow baseline drift.

## Validation

Trigger the oscilloscope from D8, view filtered D3, and confirm a stable repeating complex with a period near 833 ms.

## Recovery

Verify the waveform on raw D3 before debugging the RC filter. This sketch explicitly owns Timer2 and uses Timer0-driven PWM on D6.
