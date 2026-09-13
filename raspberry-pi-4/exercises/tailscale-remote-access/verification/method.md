# Verification method

1. Run the PowerShell script on a disposable test file and record its original SHA-256.
2. Confirm the metadata byte total and part count.
3. Copy the complete chunk directory to a Linux host or Pi.
4. Run the join script with a new output path.
5. Confirm the script reports the same SHA-256 and `cmp` reports no difference.
6. For a real package, inspect architecture and metadata with `dpkg-deb --info` before installation.

This repository records syntax and deterministic file reconstruction. No Raspberry Pi was reachable for the current record, so remote access itself is not claimed as verified.
