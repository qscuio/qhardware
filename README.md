# QHardware

QHardware collects repeatable hardware exercises for four hardware families. Each section explains how to identify and connect the board, diagnose common failures, build or update firmware on Windows and Linux, run an exercise, and preserve evidence.

## Hardware

- [Waveshare ESP32-P4-WIFI6](esp32-p4/README.md)
- [ESP32-S3 development boards](esp32-s3/README.md)
- [Raspberry Pi 4](raspberry-pi-4/README.md)
- [Arduino Uno](arduino-uno/README.md)

## Evidence states

Every exercise declares exactly one state:

- `HARDWARE_VERIFIED`: the documented result was observed on the named physical board and has a dated log.
- `BUILD_VERIFIED`: the source compiled with the documented toolchain, but the recorded run did not exercise the physical hardware.
- `DOCUMENTED`: the instructions and source are present, but there is no current build or physical result.

These labels describe the evidence in this repository. They are not general compatibility guarantees.

## Start here

Read [lab conventions](docs/lab-conventions.md) before wiring hardware and [verification format](docs/verification-format.md) before adding a result. Run the static repository checks with:

```text
python scripts/validate_repo.py
python -m unittest discover -s tests -v
```

Local credentials, downloaded SDKs, build trees, device-specific flash backups, and operating-system images are excluded from Git.

## Fundamentals

- [Twenty ESP32 experiments: from LED control to a robot controller](docs/fundamentals/ESP32_20实验_从点灯到机器人主控板.md)
- [Signals, time and frequency domains, digitalization, and carriers](docs/fundamentals/信号_时域_频域_数字化_载波_完整逻辑模型.md)
- [PCB clock systems, PLLs, clock-domain crossing, and high-speed interfaces](docs/fundamentals/pcb_clock_session_notes.md)

## License

Source and documentation in this repository are available under the [MIT License](LICENSE). Referenced vendor assets remain under their upstream licenses.
