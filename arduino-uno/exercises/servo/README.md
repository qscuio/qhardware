# Serial servo control

Verification state: `BUILD_VERIFIED`

## Objective

Sweep a hobby servo at startup and accept angles from 0 through 180 over the serial port.

## Parts

- Arduino Uno\n- SG90-compatible servo\n- Regulated 5 V supply suitable for servo stall current

## Wiring

| Servo lead | Connection |\n| --- | --- |\n| Signal | D9 |\n| Positive | external regulated 5 V |\n| Ground | external supply GND and Uno GND |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

Uses the `Servo` library supplied with Arduino AVR Boards 1.8.8.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/servo
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/servo` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

At startup the servo visits 90, 0, 90, 180, and 90 degrees. The 9600-baud monitor prints each angle and accepts newline-terminated values.

## Validation

Power the servo externally, open a 9600-baud monitor, then send `30`, `90`, and `150`. Confirm the printed angle and physical direction change each time.

## Recovery

If the Uno resets while the servo moves, correct the external power and common ground. D9 conflicts with sketches that directly reconfigure Timer1.
