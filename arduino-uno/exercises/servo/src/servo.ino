#include <Servo.h>

const int SERVO_PIN = 9;

Servo servo;
String input;

void moveTo(int angle) {
  angle = constrain(angle, 0, 180);
  servo.write(angle);
  Serial.print("angle=");
  Serial.println(angle);
}

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN);

  moveTo(90);
  delay(600);
  moveTo(0);
  delay(600);
  moveTo(90);
  delay(600);
  moveTo(180);
  delay(600);
  moveTo(90);

  Serial.println("Send an angle from 0 to 180.");
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      input.trim();
      if (input.length() > 0) {
        moveTo(input.toInt());
        input = "";
      }
    } else if (isDigit(c)) {
      input += c;
    }
  }
}
