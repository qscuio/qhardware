# Arduino Uno

This section targets the classic 5 V ATmega328P Arduino Uno. Each exercise keeps its sketch, wiring, build command, expected behavior, and verification evidence together.

## Start here

1. Follow [connection and wiring](docs/connection.md).
2. Install Arduino CLI and the AVR core with [firmware build and upload](docs/firmware-windows-linux.md).
3. Use [debugging](docs/debugging.md) when upload or runtime behavior differs from the exercise README.
4. Compile every sketch with `scripts/compile-all.ps1` on Windows or `scripts/compile-all.sh` on Linux.

## Exercises

| Exercise | Main hardware | Observable result |
| --- | --- | --- |
| `blink` | onboard LED | three short flashes followed by a pause |
| `servo` | SG90-compatible servo | startup sweep and serial angle control |
| `ultrasonic` | HC-SR04 | distance values on the serial monitor |
| `lcd` | HD44780 16x2 LCD | greeting and running seconds counter |
| `pir` | PIR sensor and 16x2 LCD | motion state and event count |
| `scope-signal` | oscilloscope or logic analyzer | sweep, heartbeat, and PWM signals |
| `ecg-simulator` | RC filter and oscilloscope | ECG-like PWM waveform and trigger |
| `pca9685-robot-arm` | PCA9685 and four servos | four-channel binary serial control |

The Uno uses 5 V GPIO. Protect 3.3 V-only peripherals with a level shifter, share grounds, and power motors or servos from a suitable external supply.
