# QHardware Lab Repository Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Publish a reproducible hardware-practice repository for the Waveshare ESP32-P4-WIFI6, Raspberry Pi 4, and Arduino Uno, with connection, debugging, firmware, code, and evidence for every exercise.

**Architecture:** The root provides shared conventions and a static repository validator. Each hardware family owns its setup guides and self-contained exercises; every exercise declares one evidence state and keeps its source, scripts, and verification evidence together. Scripts accept machine-specific values as parameters, and secrets, SDK downloads, build trees, and device backups stay outside Git.

**Tech Stack:** Markdown, Python 3, PowerShell 7/Windows PowerShell, POSIX shell, ESP-IDF 5.5.5, ESP-Hosted/NimBLE, Arduino CLI, Raspberry Pi OS tools, GitHub Actions.

---

## File map

- `README.md`, `LICENSE`, `.gitignore`: repository entry point, license, and artifact exclusions.
- `docs/lab-conventions.md`, `docs/verification-format.md`: safety, parameterization, and evidence rules shared by all hardware.
- `scripts/validate_repo.py`, `tests/test_validate_repo.py`: enforce directory, README, evidence-state, secret, and absolute-path requirements.
- `.github/workflows/validate.yml`: run static validation and Python tests on pushes and pull requests.
- `esp32-p4/`: board identity, ESP-IDF/FreeRTOS connection and recovery guides, and camera, speaker, Wi-Fi, and BLE exercises.
- `raspberry-pi-4/`: SSH/Tailscale connection, diagnostics, OS/EEPROM/kernel update guides, and parameterized Tailscale package transfer.
- `arduino-uno/`: AVR connection/debug/firmware guides plus eight independent sketches and their verification records.

### Task 1: Shared repository contract and validator

**Files:**
- Create: `README.md`, `LICENSE`, `.gitignore`
- Create: `docs/lab-conventions.md`, `docs/verification-format.md`
- Create: `scripts/validate_repo.py`, `tests/test_validate_repo.py`
- Create: `.github/workflows/validate.yml`

- [ ] **Step 1: Write validator tests** covering a valid minimal fixture, a missing `verification/method.md`, an invalid evidence state, a Windows user path, a Unix home path, and credential assignments.
- [ ] **Step 2: Verify the tests fail** with `python -m unittest tests.test_validate_repo -v`; expect import failure for `scripts.validate_repo`.
- [ ] **Step 3: Implement the validator** with exported functions `validate_repository(root: Path) -> list[str]` and `main() -> int`; scan tracked text files while excluding `.git`, build outputs, and test fixtures.
- [ ] **Step 4: Add the root documents and CI**. The README must link all three hardware landing pages and explain `HARDWARE_VERIFIED`, `BUILD_VERIFIED`, and `DOCUMENTED`.
- [ ] **Step 5: Verify and commit** with `python -m unittest tests.test_validate_repo -v` and `python scripts/validate_repo.py`; expect all tests to pass and `Repository validation passed.` Then commit as `feat: add repository conventions and validation`.

### Task 2: ESP32-P4 platform guides

**Files:**
- Create: `esp32-p4/README.md`
- Create: `esp32-p4/docs/connection.md`
- Create: `esp32-p4/docs/freertos-debugging.md`
- Create: `esp32-p4/docs/firmware-windows-linux.md`

- [ ] **Step 1: Document the tested board** as Waveshare ESP32-P4-WIFI6 with ESP32-P4 revision 3.2, 32 MB flash, 32 MB PSRAM, ESP32-C6-MINI-1, OV5647 CSI camera support, ES8311 codec, onboard MIC, and two-pin SPK output.
- [ ] **Step 2: Document Windows and Linux connection flows** using parameter variables such as `$Port` and `${PORT}`; include CH343 enumeration, 115200 baud monitoring, automatic reset, and BOOT/RESET fallback.
- [ ] **Step 3: Document FreeRTOS debugging** including `app_main`, tasks, queues, semaphores, timers, stack high-water marks, watchdogs, panic decoding, and core dumps.
- [ ] **Step 4: Document firmware lifecycle** with ESP-IDF 5.5.5 install, `idf.py set-target esp32p4`, build, flash, monitor, erase, full-flash backup, and restore commands for both operating systems. Backup and erase commands must require an explicit port and size.
- [ ] **Step 5: Run `python scripts/validate_repo.py` and commit** as `docs: add ESP32-P4 setup and recovery guides`.

### Task 3: ESP32-P4 camera and speaker exercises

**Files:**
- Create: `esp32-p4/exercises/ov5647-camera/{README.md,CMakeLists.txt,sdkconfig.defaults}`
- Create: `esp32-p4/exercises/ov5647-camera/main/{CMakeLists.txt,idf_component.yml,camera_probe.c}`
- Create: `esp32-p4/exercises/ov5647-camera/scripts/{build.ps1,build.sh,flash.ps1,flash.sh}`
- Create: `esp32-p4/exercises/ov5647-camera/verification/{method.md,expected-output.txt,actual-output.txt,checksums.sha256}`
- Create: `esp32-p4/exercises/es8311-speaker/{README.md,CMakeLists.txt,sdkconfig.defaults}` and matching `main/`, `scripts/`, and `verification/` files.

- [ ] **Step 1: Copy the custom camera probe source** from the local tested project and replace machine paths with component-manager dependencies. Preserve checks for PID `0x5647`, 800 by 800 RGB565, 1,280,000-byte frames, and changing frame checksums.
- [ ] **Step 2: Add parameterized build and flash scripts**. PowerShell syntax is `./scripts/build.ps1 -IdfPath C:\path\to\esp-idf`; POSIX syntax is `./scripts/build.sh /opt/esp/idf`; flash scripts require the port as a positional or named argument.
- [ ] **Step 3: Record the dated camera hardware log** with local paths removed and mark the exercise `HARDWARE_VERIFIED`.
- [ ] **Step 4: Add the ES8311/I2S speaker source and scripts** without copying the vendor Canon PCM asset. Reference the Waveshare upstream asset and verify ES8311 initialization plus repeated 640,000-byte I2S writes.
- [ ] **Step 5: Record the dated speaker hardware log**, mark it `HARDWARE_VERIFIED`, run both ESP-IDF builds, update SHA-256 manifests for generated application images, and commit as `feat: add verified ESP32 camera and speaker labs`.

### Task 4: ESP32-P4 Wi-Fi and BLE exercises

**Files:**
- Create: `esp32-p4/exercises/wifi-station/` with source, scripts, README, `sdkconfig.defaults.example`, and verification files.
- Create: `esp32-p4/exercises/nimble-ble-peripheral/` with source, scripts, README, safe defaults, and verification files.

- [ ] **Step 1: Add the ESP-Hosted Wi-Fi station source** with disconnect reason logging. Read SSID and password from an untracked `sdkconfig.local` or environment-generated SDK config; the committed example contains only `YOUR_SSID` and `YOUR_PASSWORD`.
- [ ] **Step 2: Add the NimBLE hosted-HCI peripheral** using ESP-Hosted 1.4.x and ESP Wi-Fi Remote 0.14.x, advertising as `nimble-bleprph` through the onboard ESP32-C6.
- [ ] **Step 3: Add secret-safe verification logs**. Wi-Fi evidence records association and DHCP success with the address redacted; BLE evidence records HCI enabled, the NimBLE host task, and advertising, with device addresses redacted.
- [ ] **Step 4: Build both projects with ESP-IDF 5.5.5**, update application-image SHA-256 files, mark successful physical runs `HARDWARE_VERIFIED`, and commit as `feat: add verified ESP32 Wi-Fi and BLE labs`.

### Task 5: Raspberry Pi 4 guides and Tailscale exercise

**Files:**
- Create: `raspberry-pi-4/README.md`
- Create: `raspberry-pi-4/docs/{connection-ssh-tailscale.md,debugging.md,os-eeprom-kernel-update.md}`
- Create: `raspberry-pi-4/exercises/tailscale-remote-access/{README.md,scripts/split_tailscale.ps1,scripts/join_tailscale.sh,verification/method.md,verification/expected-output.txt,verification/actual-output.txt,verification/checksums.sha256}`

- [ ] **Step 1: Write connection and diagnostics guides** for local setup, mDNS, SSH host-key checking, Tailscale, undervoltage, logs, services, routes, storage, temperature, and libgpiod.
- [ ] **Step 2: Separate update layers** for apt/kernel packages, EEPROM, boot-media images, and optional custom kernels. Any image write must show a device inspection command immediately before the write command.
- [ ] **Step 3: Refactor the package splitter** to accept `-InputPath`, `-OutputDirectory`, and `-ChunkSizeMiB`, emit a SHA-256 manifest, and never embed a user directory.
- [ ] **Step 4: Add a POSIX join-and-verify script** that concatenates ordered chunks to a caller-supplied output path and validates the manifest before package installation.
- [ ] **Step 5: Run PowerShell parser validation and `bash -n` where available**, mark the exercise `DOCUMENTED` because the Pi is not currently reachable, and commit as `feat: add Raspberry Pi setup and Tailscale lab`.

### Task 6: Arduino Uno platform guides

**Files:**
- Create: `arduino-uno/README.md`
- Create: `arduino-uno/docs/{connection.md,debugging.md,firmware-windows-linux.md}`
- Create: `arduino-uno/scripts/{compile-all.ps1,compile-all.sh}`

- [ ] **Step 1: Document the classic ATmega328P Uno** with USB driver checks, board/port selection, 5 V logic, 3.3 V peripheral limits, shared grounds, and external servo/load power.
- [ ] **Step 2: Document Arduino CLI install, core install, compile, upload, monitor, verbose avrdude diagnostics, loopback testing, memory reports, timer conflicts, and bootloader recovery** for Windows and Linux.
- [ ] **Step 3: Add compile-all scripts** that discover exercise sketches, invoke `arduino-cli compile --fqbn arduino:avr:uno`, stop on failure, and accept a build-cache directory.
- [ ] **Step 4: Run `python scripts/validate_repo.py` and commit** as `docs: add Arduino Uno setup and debugging guides`.

### Task 7: Arduino Uno exercises

**Files:**
- Create under `arduino-uno/exercises/`: `blink`, `servo`, `ultrasonic`, `lcd`, `pir`, `scope-signal`, `ecg-simulator`, and `pca9685-robot-arm`.
- In every directory create: `README.md`, `src/<exercise-name>.ino`, and `verification/{method.md,expected-output.txt,actual-output.txt,checksums.sha256}`.

- [ ] **Step 1: Copy each existing sketch into its own matching `src` directory** and preserve its current pin assignments and behavior.
- [ ] **Step 2: Write each README** with objective, parts, wiring table, voltage notes, declared library names and versions, build/upload commands, observable behavior, validation, and recovery.
- [ ] **Step 3: Install the AVR core and declared libraries** using Arduino CLI, then run the compile-all script against `arduino:avr:uno`.
- [ ] **Step 4: Record fresh compiler output and source checksums**. Mark only successful builds `BUILD_VERIFIED`; leave exercises with unavailable dependencies as `DOCUMENTED` and state the exact missing dependency in `actual-output.txt`.
- [ ] **Step 5: Commit** as `feat: add Arduino Uno practice exercises`.

### Task 8: Final verification and draft publication

**Files:**
- Modify verification states or logs only when required by observed results.
- Create no credential, downloaded SDK, build directory, raw flash backup, or Raspberry Pi image files.

- [ ] **Step 1: Run all static checks** with `python -m unittest discover -s tests -v` and `python scripts/validate_repo.py`; expect zero failures.
- [ ] **Step 2: Run available platform builds**: ESP-IDF builds for self-contained ESP32 exercises, PowerShell/bash syntax checks for Pi scripts, and Arduino CLI compile for every Uno sketch.
- [ ] **Step 3: Review the staged file list and secret scan** with `git diff --check`, `git status --short`, and the validator; verify no local SSID, password, private IP, user profile path, SDK tree, or backup is present.
- [ ] **Step 4: Push `codex/initialize-hardware-labs`** to `origin` and create a draft pull request titled `Initialize reproducible hardware practice labs` whose body lists hardware-verified, build-verified, and documented exercises separately.
- [ ] **Step 5: Open the draft PR for review** and report its URL plus any tests that could not run because hardware was unavailable.
