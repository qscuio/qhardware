# Verification method

1. Install Arduino AVR Boards 1.8.8 and the libraries declared in the README.
2. Compile the sketch for `arduino:avr:uno`.
3. Upload it to an Uno with power removed from external loads during reset.
4. Reconnect the documented hardware safely and perform the observable check below.
5. Record compiler output and, when hardware is present, the observed result without machine-specific paths.

Observe the display for ten seconds and confirm the counter increments once per second without leaving stale digits.
