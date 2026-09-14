# ESP-IDF and FreeRTOS debugging

## Establish the baseline

Record these before changing code:

```text
idf.py --version
git rev-parse --short HEAD
idf.py size
```

Capture the complete boot log from reset through the first failure. The application name, ESP-IDF version, minimum and actual chip revisions, flash mode, reset reason, and panic text often identify a configuration mismatch without another flash.

## Understand `app_main()`

ESP-IDF starts FreeRTOS and invokes `app_main()` from its main task. Returning from `app_main()` deletes that task, but tasks, timers, event loops, and drivers already created by the application continue running. A long-lived application can either keep `app_main()` blocked on an event or create dedicated tasks and then return.

Create tasks with explicit names and measured stacks. Inspect remaining stack space at the deepest normal workload:

```c
UBaseType_t words = uxTaskGetStackHighWaterMark(NULL);
ESP_LOGI("health", "minimum unused stack words=%u", (unsigned)words);
```

Increase a stack only after measuring it. A stack high-water mark reports words, not bytes, on this target.

## Logs and event boundaries

Use one tag per component and log state transitions, return codes, buffer lengths, and hardware identities:

```c
ESP_LOGI(TAG, "camera PID=0x%04x", sensor_pid);
ESP_LOGE(TAG, "i2s_channel_write failed: %s", esp_err_to_name(err));
```

For multi-component paths such as P4 application to ESP-Hosted to C6 radio to access point, preserve evidence at every boundary: SDIO initialization, C6 capability event, Wi-Fi association, and DHCP. Do not log passwords, reusable tokens, full BLE addresses, or unrelated nearby device names in committed output.

## Queues, semaphores, and timers

- Check every queue, task, semaphore, and timer allocation for `NULL`.
- Use bounded waits and log timeout context.
- Never call blocking APIs from an ISR; use the ISR variants and request a context switch when required.
- Protect shared peripheral state with the correct mutex or a single owner task.
- Keep software-timer callbacks short; hand longer work to a task.

When a consumer sees corrupt data, log the item size at queue creation and both send and receive sites. When a task stalls, log before and after every blocking call to locate the wait.

## Watchdogs

A task watchdog normally indicates that a task failed to yield or a critical section stayed locked. Capture the named tasks and backtrace, then inspect loops, interrupt-disabled regions, and blocking driver calls. Add `vTaskDelay()`, split work, or correct the ownership boundary only after locating the blocking path. Disabling the watchdog hides the evidence and is not a repair.

## Panic and backtrace decoding

Keep the matching `.elf` file for the flashed build. ESP-IDF monitor decodes addresses automatically when it runs from that build directory:

```text
idf.py -p PORT monitor
```

For a saved program counter or backtrace address:

```text
riscv32-esp-elf-addr2line -pfiaC -e build/PROJECT.elf ADDRESS
```

Use the exact ELF whose application SHA matches the boot log.

## Core dumps and GDB

Enable core dumps in `idf.py menuconfig` under **Component config > ESP System Settings > Core dump** and choose UART or flash storage. Decode a flash core dump from the matching build:

```text
idf.py coredump-info
idf.py coredump-debug
```

For supported debug probes, configure OpenOCD for the exact board/target, start it in one terminal, and run `idf.py gdb` in another. Stop all serial programs before opening the same interface through another tool.

## Common failures

| Symptom | First evidence to collect | Likely boundary |
| --- | --- | --- |
| Port missing | USB device list and cable swap | cable, driver, or power |
| Flash connect timeout | BOOT/RESET sequence and port owner | ROM downloader entry |
| Image rejected | chip revision and configured target | sdkconfig/build mismatch |
| Repeated reset | ROM reset reason and first panic | power, watchdog, or exception |
| Wi-Fi reason 204 | authentication configuration without printing it | WPA handshake |
| Wi-Fi reason 205 | preceding association/authentication reason | connection attempt |
| Camera PID mismatch | I2C/SCCB address and ribbon orientation | sensor control bus |
| Audio initializes but is silent | codec route, I2S write count, speaker connector | codec/output path |

After every change, reproduce from reset and retain only the smallest change that explains the result.
