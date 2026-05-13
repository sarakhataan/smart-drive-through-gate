/*
  Smart Drive-Through Gate
  Components: Arduino Uno, HC-SR04 Ultrasonic Sensor,
              Servo Motor, 16x2 LCD Display, Breadboard

  Wiring Summary:
    HC-SR04:
      VCC  -> 5V (via breadboard)
      GND  -> GND (via breadboard)
      Trig -> D2
      Echo -> D3

    Servo Motor:
      Signal (orange/yellow) -> D9
      VCC (red)              -> 5V (via breadboard)
      GND (brown/black)      -> GND (via breadboard)

    LCD 16x2 (4-bit mode, no I2C):
      VSS  -> GND
      VDD  -> 5V
      VO   -> GND (or potentiometer for contrast)
      RS   -> D12
      RW   -> GND
      EN   -> D11
      D4   -> D10
      D5   -> D8
      D6   -> D7
      D7   -> D6
      A    -> 5V (backlight+)
      K    -> GND (backlight-)
*/

#include <LiquidCrystal.h>
#include <Servo.h>

// --- Pin Definitions ---
const int TRIG_PIN  = 7;
const int ECHO_PIN  = 6;
const int SERVO_PIN = 9;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo gateServo;

// --- Settings ---
const int TRIGGER_DISTANCE = 30;  // cm — car must be closer than this
const int GATE_OPEN_ANGLE  = 90;  // servo angle for open gate
const int GATE_CLOSE_ANGLE = 0;   // servo angle for closed gate
const int GATE_OPEN_DELAY  = 3000; // ms to keep gate open

// --- State ---
bool gateOpen = false;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  gateServo.attach(SERVO_PIN);
  gateServo.write(GATE_CLOSE_ANGLE);  // Start closed

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("  Smart Gate  ");
  lcd.setCursor(0, 1);
  lcd.print(" System Ready ");
  delay(2000);

  displayStatus("Gate Closed", "Waiting...");
  Serial.begin(9600);
}

void loop() {
  long distance = measureDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 0 && distance < TRIGGER_DISTANCE) {
    if (!gateOpen) {
      openGate();
    }
  } else {
    if (gateOpen) {
      closeGate();
    }
  }

  delay(200);  // polling interval
}

// --- Measure distance via ultrasonic sensor ---
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  if (duration == 0) return -1;  // no echo received

  long distance = duration * 0.034 / 2;  // convert to cm
  return distance;
}

// --- Open the gate ---
void openGate() {
  gateOpen = true;
  displayStatus("Gate Open", "Welcome!");
  gateServo.write(GATE_OPEN_ANGLE);
  Serial.println("Gate OPENED");
  delay(GATE_OPEN_DELAY);  // hold open
}

// --- Close the gate ---
void closeGate() {
  gateOpen = false;
  gateServo.write(GATE_CLOSE_ANGLE);
  displayStatus("Gate Closed", "Waiting...");
  Serial.println("Gate CLOSED");
}

// --- Update LCD ---
void displayStatus(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}