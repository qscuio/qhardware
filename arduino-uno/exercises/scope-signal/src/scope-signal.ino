// Arduino Uno oscilloscope signal show:
// D9: 100 Hz to 10 kHz triangle sweep, 50% duty cycle.
// D8: double-pulse heartbeat, once per second.
// D6: approximately 980 Hz PWM with a slowly changing duty cycle.
void setup() {
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(6, OUTPUT);

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  // Toggle OC1A on compare match in CTC mode, with an 8x prescaler.
  OCR1A = 9999;
  TCCR1A |= _BV(COM1A0);
  TCCR1B |= _BV(WGM12) | _BV(CS11);
  interrupts();
}

void loop() {
  const unsigned long now = millis();

  // Sweep up for two seconds, then back down for two seconds.
  const unsigned long sweepPosition = now % 4000UL;
  const unsigned long ramp =
      sweepPosition < 2000UL ? sweepPosition : 4000UL - sweepPosition;
  const unsigned long frequency = 100UL + (ramp * 9900UL) / 2000UL;
  OCR1A = (1000000UL / frequency) - 1UL;

  // A stylized heartbeat: short pulse, pause, stronger pulse, long rest.
  const unsigned int beatPosition = now % 1000UL;
  const bool heartbeat = beatPosition < 35U ||
                         (beatPosition >= 120U && beatPosition < 200U);
  digitalWrite(8, heartbeat ? HIGH : LOW);

  // Triangle-wave envelope encoded as PWM duty cycle.
  const unsigned int breathPosition = now % 4000UL;
  const unsigned int brightness = breathPosition < 2000U
      ? (breathPosition * 255UL) / 2000UL
      : ((4000UL - breathPosition) * 255UL) / 2000UL;
  analogWrite(6, brightness);
}
