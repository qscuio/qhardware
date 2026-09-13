# QHardware Lab Repository Design

## Purpose

QHardware is a reproducible record of hands-on work with three hardware families:

- Waveshare ESP32-P4-WIFI6, including its ESP32-C6 connectivity module
- Raspberry Pi 4
- Arduino Uno

The repository must let a new user identify the board, connect it safely, build the software, update or restore firmware, diagnose failures, run an exercise, and record evidence without depending on this chat history.

## Repository Structure

```text
qhardware/
├── README.md
├── LICENSE
├── .gitignore
├── docs/
│   ├── lab-conventions.md
│   ├── verification-format.md
│   └── superpowers/
│       ├── specs/
│       └── plans/
├── esp32-p4/
│   ├── README.md
│   ├── docs/
│   │   ├── connection.md
│   │   ├── freertos-debugging.md
│   │   └── firmware-windows-linux.md
│   └── exercises/
│       ├── ov5647-camera/
│       └── es8311-speaker/
├── raspberry-pi-4/
│   ├── README.md
│   ├── docs/
│   │   ├── connection-ssh-tailscale.md
│   │   ├── debugging.md
│   │   └── os-eeprom-kernel-update.md
│   └── exercises/
│       └── tailscale-remote-access/
└── arduino-uno/
    ├── README.md
    ├── docs/
    │   ├── connection.md
    │   ├── debugging.md
    │   └── firmware-windows-linux.md
    └── exercises/
        ├── blink/
        ├── servo/
        ├── ultrasonic/
        ├── lcd/
        ├── pir/
        ├── scope-signal/
        ├── ecg-simulator/
        └── pca9685-robot-arm/
```

The root README links directly to the three hardware landing pages and explains the evidence labels. Each hardware landing page identifies the exact tested board, lists prerequisites, and links to connection, debugging, firmware, and exercise documents.

## Exercise Contract

Each exercise is independently understandable and uses this layout when the files are applicable:

```text
exercise-name/
├── README.md
├── src/
├── scripts/
└── verification/
    ├── method.md
    ├── expected-output.txt
    ├── actual-output.txt
    └── checksums.sha256
```

`README.md` states the objective, required hardware, pin mapping or cable orientation, voltage requirements, dependencies, build command, upload command, runtime behavior, and recovery path. It must not tell a user to join bare conductors by hand or connect a signal before its voltage is known.

`src/` contains only source and small required assets. `scripts/` contains repeatable build, flash, upload, or verification commands. Scripts accept paths, ports, and hosts as parameters instead of embedding values from one computer.

`verification/method.md` explains how the result is checked. Expected output is stable, edited reference text; actual output is a dated capture from a real run. Checksums cover distributable firmware or other generated artifacts when such artifacts exist.

## Evidence States

Every exercise declares exactly one current verification state:

- `HARDWARE_VERIFIED`: observed on the named physical board and supported by a dated log.
- `BUILD_VERIFIED`: compiled successfully with the documented toolchain but not exercised on hardware during the recorded run.
- `DOCUMENTED`: instructions and source are present but no current build or hardware evidence is available.

The repository must not convert old observations into fresh results. Existing ESP32 camera and speaker logs from 2026-09-13 can be recorded as hardware evidence because the board, firmware, commands, and serial output are known. Arduino exercises can be marked `BUILD_VERIFIED` only after a fresh Arduino CLI compile; otherwise they remain `DOCUMENTED`. Raspberry Pi procedures remain `DOCUMENTED` unless a Pi 4 is reachable for a current run.

## ESP32-P4 Design

The ESP32 section targets the Waveshare ESP32-P4-WIFI6 revision with ESP32-P4 revision 3.2, 32 MB flash, 32 MB PSRAM, ESP32-C6-MINI-1 connectivity, a MIPI-CSI camera connector, and an ES8311 audio codec. Documentation distinguishes the onboard microphone from the two-pin `SPK` connector.

The software baseline is ESP-IDF 5.5.5. The documentation explains that ESP-IDF applications run on FreeRTOS, that `app_main()` is invoked by the ESP-IDF main task, and that drivers can use FreeRTOS tasks, queues, semaphores, timers, and watchdogs.

The debugging guide covers serial logging, automatic and manual bootloader entry, reset reasons, panic decoding, core dumps, task stack high-water marks, watchdog diagnosis, USB serial driver checks, and safe flash backup/restore.

The firmware guide provides PowerShell commands for Windows and shell commands for Linux. It covers ESP-IDF installation, dependency resolution, configuration, compilation, flashing, monitoring, erasing, and restoring a full backup. Generated build trees, managed dependency downloads, local SDK copies, and the 32 MB backup image do not enter normal Git history.

The camera exercise contains the custom OV5647 MIPI-CSI probe source and documents the verified sensor PID `0x5647`, an 800 by 800 RGB565 frame, a 1,280,000-byte frame, and checksum change across captures. The speaker exercise documents ES8311 and I2S initialization and verifies repeated 640,000-byte writes. The vendor-supplied Canon PCM asset is referenced through the Waveshare source repository and is not copied into QHardware.

## Raspberry Pi 4 Design

The Raspberry Pi section targets Raspberry Pi 4 without assuming a RAM size or storage medium. It separates four update layers:

1. Raspberry Pi OS packages and kernel packages
2. Bootloader EEPROM through `rpi-eeprom-update`
3. SD card or USB boot media images
4. Optional custom Linux kernel compilation

Windows instructions use Raspberry Pi Imager for boot media, PowerShell or OpenSSH for remote access, and WSL2 when a Linux build environment is required. Linux instructions use Raspberry Pi Imager or a carefully verified block-device command, OpenSSH, native package tools, and native or cross-compiled kernel builds. Any destructive imaging command requires the user to verify the selected block device immediately before writing.

Connection documentation covers local HDMI/keyboard setup, Ethernet or Wi-Fi, mDNS, SSH host-key verification, and Tailscale. Debugging covers power and undervoltage indicators, `vcgencmd`, `journalctl`, `dmesg`, systemd service status, network routes, storage health, temperatures, GPIO discovery with libgpiod, and recovery using another computer.

The Tailscale exercise preserves the useful ARMHF package-transfer lesson while replacing absolute local paths with parameters. It never stores reusable authentication keys, Wi-Fi credentials, private SSH keys, public IP addresses, or private host inventory.

## Arduino Uno Design

The Arduino section targets the classic AVR ATmega328P Uno. Connection documentation covers USB serial drivers, board and port selection, 5 V logic, 3.3 V peripheral limits, shared grounds, and external-power rules for servos and larger loads.

Build and upload instructions use Arduino CLI on Windows and Linux, with `arduino:avr:uno` as the fully qualified board name. The debugging guide covers serial monitoring, compile verbosity, `avrdude` errors, loopback testing, bootloader recovery through another Arduino or an ISP programmer, memory reports, timer conflicts, and measurement with a multimeter or oscilloscope.

The exercise set preserves the existing LED, servo, ultrasonic, LCD, PIR, oscilloscope signal, ECG simulator, and PCA9685 robot-arm work. Each exercise receives its own wiring table and validation steps. Third-party Arduino libraries are declared by name and version rather than copied from the user's global Arduino library directory.

## Firmware and Artifact Policy

Git stores source, configuration, scripts, documentation, small input assets with clear provenance, verification logs, and SHA-256 manifests. Build directories, downloaded SDKs, managed components, credentials, machine-specific settings, and large binary backups are ignored.

Reproducible small firmware images may be attached to a tagged GitHub Release after their source revision, toolchain, build command, target board, verification state, and SHA-256 checksum are recorded. A release must not contain the Raspberry Pi OS image, vendor assets without redistribution permission, private credentials, or a raw flash backup that may contain device-specific data.

## Validation

A repository validation script checks required directories, required README sections, evidence-state spelling, absence of absolute user paths, absence of common credential patterns, and presence of verification methods. Hardware-specific scripts perform available builds:

- ESP32: `idf.py build` for each self-contained ESP-IDF exercise
- Raspberry Pi: shell syntax checks and Python compilation for included utilities; kernel build commands are documented but are not run on Windows without WSL2
- Arduino Uno: `arduino-cli compile --fqbn arduino:avr:uno` for every sketch

Hardware tests are separate from static checks because flashing one ESP32 exercise replaces the currently running exercise, physical peripherals may be disconnected, and a Raspberry Pi or Uno may not be attached. Validation output reports these limitations directly.

## Failure Handling and Recovery

Build and upload scripts stop on errors and print the failing command. They do not automatically erase devices, overwrite full flash, write block devices, or update EEPROM. Recovery instructions explain how to identify the correct serial port or block device before retrying.

Each hardware section includes a diagnostic order that starts with power, cable, USB or network enumeration, and toolchain identity before changing firmware. Logs redact secrets and use stable relative paths where possible.

## Publication

Work is prepared on a `codex/initialize-hardware-labs` branch, validated locally, committed with explicit file selection, and pushed to `qscuio/qhardware`. The initial pull request is created as a draft. The PR describes which exercises have physical hardware evidence, which have compile-only evidence, and which remain documentation-only.

