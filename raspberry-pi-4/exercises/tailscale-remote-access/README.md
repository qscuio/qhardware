# Tailscale package transfer

Verification state: `DOCUMENTED`

## Objective

Split a Raspberry Pi OS `.deb` package into transport-friendly chunks on Windows, transfer the directory, reconstruct the package on Raspberry Pi 4, and verify SHA-256 before installation.

Use a package whose architecture matches the installed OS. Check with `dpkg --print-architecture` on the Pi and inspect the package with `dpkg-deb --info PACKAGE` before installation.

## Windows split

```powershell
./scripts/split_tailscale.ps1 `
    -InputPath D:\packages\tailscale_VERSION_ARCH.deb `
    -OutputDirectory D:\transfer\tailscale-chunks `
    -ChunkSizeMiB 8
```

Transfer the entire output directory through the approved channel. The directory contains numbered parts, metadata, and the source package hash; it contains no auth key.

## Raspberry Pi join and verify

```sh
./scripts/join_tailscale.sh ./tailscale-chunks ./tailscale.deb
dpkg-deb --info ./tailscale.deb
sudo apt install ./tailscale.deb
```

Authenticate Tailscale interactively and verify `tailscale status`. Do not commit or paste reusable authentication URLs or keys.

## Recovery

If the checksum differs, delete the reconstructed output, compare the part count and sizes with `metadata.txt`, and transfer the damaged or missing parts again. Do not install a package that failed hash verification.
