# PCA9685 four-servo robot arm

Verification state: `BUILD_VERIFIED`

## Objective

Control four servo channels through a PCA9685 and store the latest integer positions in Uno EEPROM.

## Parts

- Arduino Uno\n- PCA9685 servo driver at address 0x40\n- Four servos\n- External regulated servo supply sized for combined stall current

## Wiring

| PCA9685 | Connection |\n| --- | --- |\n| VCC | Uno 5 V logic supply |\n| GND | Uno GND and servo-supply GND |\n| SDA | A4 |\n| SCL | A5 |\n| V+ | external servo supply positive |\n| Channels 0..3 | base, shoulder, elbow, gripper servos |

Disconnect power while wiring. The classic Uno uses 5 V GPIO; share grounds and follow the external-load rules in `../../docs/connection.md`.

## Libraries

Requires `Adafruit PWM Servo Driver Library` 3.0.3 and its resolved `Adafruit BusIO` 1.17.4 dependency. `Wire` and `EEPROM` come with Arduino AVR Boards 1.8.8.

## Build and upload

From the repository root:

```sh
./arduino-uno/scripts/compile-all.sh /tmp/qhardware-arduino-cache
arduino-cli upload -p PORT --fqbn arduino:avr:uno --input-dir /tmp/qhardware-arduino-cache/build/pca9685-robot-arm
```

On Windows run `compile-all.ps1` as shown in the platform guide and pass its corresponding `build/pca9685-robot-arm` directory to `--input-dir`. Replace `PORT` with the value reported by `arduino-cli board list`.

## Expected behavior

At 9600 baud the sketch prints `INIT:a,b,c,d` or `ERROR:PCA9685_NOT_FOUND`. Each command is eight big-endian bytes containing four angles in centidegrees.

## Validation

Start with servo horns or linkages disconnected. Send four valid centidegree values, confirm channels move, reset the Uno, and confirm the stored integer positions appear in `INIT`.

## Recovery

Disconnect servo power if motion is unexpected. Verify I2C address 0x40 and logic power first, then test one channel with the mechanism unloaded.
