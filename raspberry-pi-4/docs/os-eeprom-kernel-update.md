# Raspberry Pi 4 update layers

These procedures affect different storage and should be diagnosed separately.

## 1. Raspberry Pi OS packages and packaged kernel

```sh
sudo apt update
apt list --upgradable
sudo apt full-upgrade
sudo reboot
uname -a
```

Read package prompts and preserve local configuration intentionally. A normal package upgrade may install a packaged kernel and boot files; verify the running kernel only after reboot.

## 2. Bootloader EEPROM

Inspect before updating:

```sh
sudo rpi-eeprom-update
vcgencmd bootloader_version
vcgencmd bootloader_config
```

Apply an available EEPROM update only with stable power and a recovery SD card available:

```sh
sudo rpi-eeprom-update -a
sudo reboot
sudo rpi-eeprom-update
```

Do not interrupt power during the update boot.

## 3. SD or USB boot-media image

On Windows, use Raspberry Pi Imager, inspect the selected drive capacity and model, then write and verify. On Linux, Raspberry Pi Imager is preferred. A raw write is destructive; inspect the exact block device immediately before the command:

```sh
lsblk -o NAME,SIZE,MODEL,TRAN,MOUNTPOINTS
sudo umount /dev/DEVICE?*
sudo dd if=raspios.img of=/dev/DEVICE bs=4M conv=fsync status=progress
sync
```

Replace `/dev/DEVICE` only after matching model and capacity. Never use a mounted partition path as the output. This repository does not store OS images.

## 4. Optional custom Linux kernel

Use a native 64-bit Raspberry Pi OS build or a Linux/WSL2 cross-build environment. Start from the Raspberry Pi kernel repository and the branch documented for the intended OS release. Record the source commit, compiler version, defconfig, local configuration changes, and installed module directory.

The high-level native flow is:

```sh
make bcm2711_defconfig
make -j"$(nproc)" Image.gz modules dtbs
sudo make modules_install
```

Install the kernel image, DTBs, overlays, and boot configuration using the layout required by the active Raspberry Pi OS release. Keep the previous known-good kernel selectable until the new kernel boots and devices are verified.

On Windows, compile inside WSL2 or another Linux environment. Windows alone does not supply the kernel's normal build tools, file modes, or module-install layout.
