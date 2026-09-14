# Raspberry Pi 4

This section targets Raspberry Pi 4 without assuming its RAM size, storage medium, hostname, or network. It separates connection and recovery from four update layers: Raspberry Pi OS packages and kernels, bootloader EEPROM, boot-media images, and optional custom kernel builds.

## Guides

- [Connect with local console, SSH, or Tailscale](docs/connection-ssh-tailscale.md)
- [Debug power, boot, services, network, storage, and GPIO](docs/debugging.md)
- [Update the OS, EEPROM, boot media, or kernel](docs/os-eeprom-kernel-update.md)

## Exercises

- [Transfer and install a Tailscale package in verified chunks](exercises/tailscale-remote-access/README.md)

Commands use `PI_HOST`, account names, paths, and devices as caller-supplied values. No authentication key, Wi-Fi credential, private SSH key, private host inventory, or Raspberry Pi OS image belongs in this repository.
