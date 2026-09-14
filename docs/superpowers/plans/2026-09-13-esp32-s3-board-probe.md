# ESP32-S3 Board Probe Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a reproducible ESP32-S3 N16R8 board-identification and radio probe, with Windows/Linux instructions and redacted physical verification evidence.

**Architecture:** A new `esp32-s3` hardware area owns connection, model-identification, peripheral-audit, firmware, and debugging guides. One ESP-IDF probe performs read-only Flash access, destructive-in-allocated-memory PSRAM testing, FreeRTOS scheduling, anonymous Wi-Fi scanning, and BLE advertising; host scripts build, flash, capture evidence, and verify over-the-air BLE discovery.

**Tech Stack:** ESP-IDF 5.5.5, FreeRTOS, ESP Wi-Fi, NimBLE, esptool, PowerShell, POSIX shell, Python unittest.

---

### Task 1: Register ESP32-S3 as a repository hardware target

**Files:**
- Modify: `scripts/validate_repo.py`
- Modify: `tests/test_validate_repo.py`

- [ ] **Step 1: Extend the validator test fixture**

Add `esp32-s3` to the hardware tuple used by the tests so a missing README or exercises directory fails validation.

- [ ] **Step 2: Run the focused test before implementation**

Run: `python -m unittest tests.test_validate_repo -v`

Expected: failure because the production validator does not yet require `esp32-s3`.

- [ ] **Step 3: Extend the production hardware tuple**

Add `esp32-s3` to `HARDWARE_DIRECTORIES` in `scripts/validate_repo.py`.

- [ ] **Step 4: Run the focused test after implementation**

Run: `python -m unittest tests.test_validate_repo -v`

Expected: all validator tests pass.

### Task 2: Add identification, peripherals, bootloader, and firmware guides

**Files:**
- Create: `esp32-s3/README.md`
- Create: `esp32-s3/docs/connection-and-identification.md`
- Create: `esp32-s3/docs/peripheral-audit.md`
- Create: `esp32-s3/docs/bootloader-debugging.md`
- Create: `esp32-s3/docs/firmware-windows-linux.md`

- [ ] **Step 1: Document connection and model identification**

Include Windows and Linux enumeration commands, USB VID/PID interpretation, esptool `chip_id`, `flash_id`, `get_security_info`, package/revision/Flash/PSRAM checks, and the limitation that exact development-board identity requires silkscreen or schematic evidence.

- [ ] **Step 2: Document peripheral auditing**

Separate SoC capabilities from board-mounted hardware. Give safe procedures for GPIO, RGB LED, buttons, USB, camera, display, audio, I2C, SPI, UART, SD, Wi-Fi, and BLE without guessing pins.

- [ ] **Step 3: Document bootloader verification**

Capture ROM and second-stage logs, parse partition tables and application descriptions, verify image hashes, explain BOOT/RESET entry, and preserve a full private Flash backup before replacement.

- [ ] **Step 4: Document Windows and Linux build/flash/restore**

Use parameterized `IDF_PATH`, `PORT`, and backup paths. Include ESP32-S3 Xtensa tool installation and a 16 MB backup-size guard.

### Task 3: Add the ESP32-S3 probe exercise

**Files:**
- Create: `esp32-s3/exercises/board-probe/CMakeLists.txt`
- Create: `esp32-s3/exercises/board-probe/sdkconfig.defaults`
- Create: `esp32-s3/exercises/board-probe/main/CMakeLists.txt`
- Create: `esp32-s3/exercises/board-probe/main/probe_main.c`
- Create: `esp32-s3/exercises/board-probe/scripts/build.ps1`
- Create: `esp32-s3/exercises/board-probe/scripts/build.sh`
- Create: `esp32-s3/exercises/board-probe/scripts/flash.ps1`
- Create: `esp32-s3/exercises/board-probe/scripts/flash.sh`
- Create: `esp32-s3/exercises/board-probe/scripts/capture_serial.py`
- Create: `esp32-s3/exercises/board-probe/scripts/scan_ble.py`
- Create: `esp32-s3/exercises/board-probe/README.md`

- [ ] **Step 1: Add the probe firmware**

Implement a 64 KB Flash read, a 4 MB PSRAM pattern write/read, a FreeRTOS worker notification, an anonymous Wi-Fi scan, and continuous `QHW-ESP32S3-PROBE` BLE advertising. Do not log SSIDs, passwords, unrelated device names, or addresses.

- [ ] **Step 2: Add portable host scripts**

Make builds and flashes accept an explicit ESP-IDF path and port. Make serial capture redact address-shaped values and BLE scanning report only the expected probe name and RSSI.

- [ ] **Step 3: Build the project**

Run: `idf.py -B build set-target esp32s3 && idf.py -B build build`

Expected: the app, partition table, and bootloader binaries are produced for 16 MB DIO Flash with 8 MB octal PSRAM.

### Task 4: Record physical evidence and link the new hardware area

**Files:**
- Create: `esp32-s3/exercises/board-probe/verification/method.md`
- Create: `esp32-s3/exercises/board-probe/verification/expected-output.txt`
- Create: `esp32-s3/exercises/board-probe/verification/actual-output.txt`
- Create: `esp32-s3/exercises/board-probe/verification/checksums.sha256`
- Modify: `README.md`

- [ ] **Step 1: Test both physical boards**

Back up each board privately, flash the probe, capture the five PASS markers, and verify `QHW-ESP32S3-PROBE` from a Windows BLE scanner. Record only non-secret board traits and redacted evidence.

- [ ] **Step 2: Add the root navigation link**

Add ESP32-S3 to the hardware list in `README.md`.

- [ ] **Step 3: Run all repository checks**

Run: `python scripts/validate_repo.py` and `python -m unittest discover -s tests -v`.

Expected: repository validation passes and all tests pass.

### Task 5: Publish the update

**Files:**
- Stage only the files listed above.

- [ ] **Step 1: Inspect the final diff**

Run: `git status -sb` and `git diff --check`.

Expected: only ESP32-S3 probe, documentation, validator, test, plan, and root README changes appear; whitespace check passes.

- [ ] **Step 2: Commit and push**

Run: `git commit -m "feat: add ESP32-S3 board diagnostics"` followed by `git push -u origin codex/initialize-hardware-labs`.

- [ ] **Step 3: Update the draft pull request**

Update PR 1 so its title and description cover the new ESP32-S3 target and list build, repository, and physical validation results.
