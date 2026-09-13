# HC-SR04 distance reader

Verification state: `BUILD_VERIFIED`

## Objective

Measure echo pulse duration and report distance in centimetres.

## Parts

- Arduino Uno\n- HC-SR04 ultrasonic module

## Wiring

| HC-SR04 | Uno |\n| --- | --- |\n| VCC | 5 V |\n| GND | GND |\n| TRIG | D7 |\n| ECHO | D8 |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

No external library is required.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/ultrasonic
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/ultrasonic` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

The 9600-baud monitor prints a distance about every 300 ms, or `out of range or no echo` after a 30 ms timeout.

## Validation

Place a flat target at two clearly different distances. Confirm the reported value changes in the expected direction and remains reasonably stable.

## Recovery

Check VCC, shared ground, and TRIG/ECHO order. Aim at a flat target within the sensor range and move it away from acoustic dead zones.
