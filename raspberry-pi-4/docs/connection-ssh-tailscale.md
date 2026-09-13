# Connect to Raspberry Pi 4

## First boot on a local console

Connect HDMI, keyboard, supported storage, and a suitable USB-C power supply. Use Raspberry Pi Imager's customization screen to set the account, locale, network, and SSH preference before writing media. Do not store those values in this repository.

After boot, identify the machine and addresses locally:

```sh
hostnamectl
ip -brief address
ip route
```

## LAN and mDNS

Prefer Ethernet for initial diagnosis. If mDNS is enabled, test `HOSTNAME.local`; otherwise find the DHCP lease from the router or local console. Before sending credentials, confirm the address belongs to this Pi.

Windows PowerShell and Linux use the same OpenSSH syntax:

```text
ssh USER@PI_HOST
```

On first connection, compare the displayed host-key fingerprint with a fingerprint obtained locally on the Pi:

```sh
ssh-keygen -lf /etc/ssh/ssh_host_ed25519_key.pub
```

A changed key can be a reinstalled Pi, a reused address, or an interception. Verify it locally before removing a saved key.

## SSH keys

Create a dedicated local key if needed and copy only its public half to the Pi. Keep private keys outside the repository. After key login works in a second terminal, password authentication may be disabled according to the owner's access policy.

## Tailscale

Install Tailscale using its current Raspberry Pi OS instructions or the package-transfer exercise in this repository. Authenticate interactively on the Pi; never put a reusable auth key in a script or log.

Verify state and routing:

```sh
tailscale status
tailscale ip -4
ip route
```

Then connect using the MagicDNS name or assigned Tailscale address. Confirm the SSH host key again because this is a new network name or address.

## Connection diagnostic order

1. Confirm stable power and local boot output.
2. Confirm link state and an address with `ip -brief address`.
3. Confirm the default route and DNS resolution.
4. Check `systemctl status ssh` and the SSH listening socket.
5. Test ping or TCP port 22 from the client.
6. Confirm the account and host-key fingerprint.
7. For Tailscale, inspect `tailscale status` on both peers and check ACL policy outside this repository.
