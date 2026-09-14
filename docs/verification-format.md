# Verification format

Each exercise contains a `verification` directory:

- `method.md` states the setup, command, success criteria, and recovery steps.
- `expected-output.txt` contains stable reference lines rather than a complete noisy log.
- `actual-output.txt` contains a dated, redacted observation and tool versions.
- `checksums.sha256` records distributable artifacts or explains why no artifact is stored.

The exercise README declares exactly one evidence state: `HARDWARE_VERIFIED`, `BUILD_VERIFIED`, or `DOCUMENTED`. Update the state only when the corresponding evidence exists. A new build does not turn a compile-only exercise into a physical hardware result, and an old observation must keep its original date.

Actual logs remove device addresses, local account paths, credentials, private IP addresses, and unrelated nearby device names. Preserve error codes, chip revisions, image sizes, hashes, and command results when they establish the result.
