const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  delay(500);

  Serial.println("HC-SR04 ultrasonic distance reader");
  Serial.println("TRIG=D7 ECHO=D8");
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);

  if (duration == 0) {
    Serial.println("out of range or no echo");
  } else {
    float distanceCm = duration / 58.0;
    Serial.print(distanceCm, 1);
    Serial.println(" cm");
  }

  delay(300);
}
