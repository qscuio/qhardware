# Raspberry Pi 4 debugging

Start with evidence that does not change the system.

## Power, temperature, and throttling

```sh
vcgencmd get_throttled
vcgencmd measure_volts core
vcgencmd measure_temp
cat /sys/class/thermal/thermal_zone0/temp
```

Decode the throttling bitmask using the Raspberry Pi documentation. Current or historical undervoltage is evidence to inspect the supply, USB-C cable, connectors, and attached loads before changing software.

## Boot and kernel logs

```sh
journalctl -b -p warning
journalctl -b -1 -p warning
dmesg --level=err,warn
uname -a
cat /proc/cmdline
```

Preserve the first error and surrounding lines. Later service failures may be consequences of an earlier filesystem, voltage, or device error.

## Services and processes

```sh
systemctl --failed
systemctl status SERVICE
journalctl -u SERVICE -b
systemd-analyze critical-chain
```

Check the unit's effective configuration with `systemctl cat SERVICE` before editing overrides.

## Network

```sh
ip -brief link
ip -brief address
ip route
resolvectl status
ss -lntup
ping -c 3 GATEWAY
```

For Wi-Fi, add `iw dev` and the network manager's status command. For Tailscale, add `tailscale status` and `tailscale netcheck`. Redact private addresses and peer names from published logs.

## Storage

```sh
lsblk -o NAME,SIZE,FSTYPE,MOUNTPOINTS,MODEL
df -hT
df -ih
journalctl -b | grep -Ei 'mmc|nvme|usb|I/O error|filesystem'
```

Unmount suspect removable storage before an offline filesystem check. Image failing media from another computer before attempting repairs when the data matters.

## GPIO

Use libgpiod's current interface instead of assuming legacy sysfs GPIO numbers:

```sh
gpiodetect
gpioinfo
```

Confirm the header pin, Broadcom GPIO name, voltage, direction, and shared function before driving a line. Raspberry Pi GPIO uses 3.3 V logic and is not 5 V tolerant.

## Recovery from another computer

If the Pi does not boot, power it down, remove the boot media, and inspect it from another machine. Back up important data before editing boot files or running repairs. Re-image only after identifying the correct removable device and saving any required configuration.
