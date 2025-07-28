/*
--Swarnendu-Rup Repository-->

Receiver's Connections steps

NRF24L01 Module -> ESP32

VCC -> 3.3V
GND -> GND
CE -> GPIO4
CSN -> GPIO5
SCK -> GPIO18
MOSI -> GPIO23
MISO -> GPIO19
L298N Motor Driver -> ESP32
IN1 -> GPIO12
IN2 -> GPIO13
ENA -> GPIO14 (Ensure the jumper is removed from ENA)
IN3 -> GPIO25
IN4 -> GPIO26
ENB -> GPIO27 (Ensure the jumper is removed from ENB)
GND -> GND (This should already be connected from the power step)
Motors -> L298N Motor Driver
Left Motor -> L298N OUT1 & OUT2
Right Motor -> L298N OUT3 & OUT4
Pan Servo -> ESP32 & L298N
Signal (Yellow/Orange wire) -> ESP32 GPIO15
VCC (Red wire) -> L298N 5V Output
GND (Brown/Black wire) -> L298N GND Terminal
Tilt Servo -> ESP32 & L298N
Signal (Yellow/Orange wire) -> ESP32 GPIO2
VCC (Red wire) -> L298N 5V Output
GND (Brown/Black wire) -> L298N GND Terminal
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP32Servo.h>

// --- Pin Configuration (Unchanged) ---
#define NRF_CE_PIN 4
#define NRF_CSN_PIN 5
#define L_MOTOR_IN1 12
#define L_MOTOR_IN2 13
#define L_MOTOR_ENA 14
#define R_MOTOR_IN3 25
#define R_MOTOR_IN4 26
#define R_MOTOR_ENB 27
#define PAN_SERVO_PIN 15
#define TILT_SERVO_PIN 2

// --- Dead Zone for Joysticks ---
#define JOY_DEADZONE_LOW 80   //changed here joyrc
#define JOY_DEADZONE_HIGH 130    //changed here joyrc

// --- RF24 Setup ---
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
const byte address[6] = "00001";

// --- Servo Objects ---
Servo panServo;
Servo tiltServo;

// --- NEW: Servo Position State Variables ---
// These will store the servo's current position. They start at 90 (center).
int currentPanAngle = 90;
int currentTiltAngle = 90;

// Data structure to hold all control values (must match transmitter)
struct Data_Package {
  byte j1_x;
  byte j1_y;
  byte j2_x;
  byte j2_y;
  byte pot_speed;
};

Data_Package data;

void stopMotors() {
    digitalWrite(L_MOTOR_IN1, LOW);
    digitalWrite(L_MOTOR_IN2, LOW);
    digitalWrite(R_MOTOR_IN3, LOW);
    digitalWrite(R_MOTOR_IN4, LOW);
    analogWrite(L_MOTOR_ENA, 0);
    analogWrite(R_MOTOR_ENB, 0);
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Receiver starting up... (Slow Servo Version)");

  // Configure Motor pins
  pinMode(L_MOTOR_IN1, OUTPUT);
  pinMode(L_MOTOR_IN2, OUTPUT);
  pinMode(L_MOTOR_ENA, OUTPUT);
  pinMode(R_MOTOR_IN3, OUTPUT);
  pinMode(R_MOTOR_IN4, OUTPUT);
  pinMode(R_MOTOR_ENB, OUTPUT);

  // Attach servos and set to initial position
  panServo.attach(PAN_SERVO_PIN);
  tiltServo.attach(TILT_SERVO_PIN);
  panServo.write(currentPanAngle);
  tiltServo.write(currentTiltAngle);
  
  Serial.println("Motors and Servos configured.");

  // Initialize NRF24L01 radio
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println("Radio setup complete. Waiting for data...");

  stopMotors();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));

    // --- Motor Control Logic (Unchanged) ---
    int motorSpeed = map(data.pot_speed, 0, 255, 150, 255);
    if (data.j1_y > JOY_DEADZONE_HIGH) { // Forward
        digitalWrite(L_MOTOR_IN1, HIGH); digitalWrite(L_MOTOR_IN2, LOW);
        digitalWrite(R_MOTOR_IN3, HIGH); digitalWrite(R_MOTOR_IN4, LOW);
        analogWrite(L_MOTOR_ENA, motorSpeed); analogWrite(R_MOTOR_ENB, motorSpeed);
    } else if (data.j1_y < JOY_DEADZONE_LOW) { // Backward
        digitalWrite(L_MOTOR_IN1, LOW); digitalWrite(L_MOTOR_IN2, HIGH);
        digitalWrite(R_MOTOR_IN3, LOW); digitalWrite(R_MOTOR_IN4, HIGH);
        analogWrite(L_MOTOR_ENA, motorSpeed); analogWrite(R_MOTOR_ENB, motorSpeed);
    } else {
        if (data.j1_x > JOY_DEADZONE_HIGH) { // Right
            digitalWrite(L_MOTOR_IN1, HIGH); digitalWrite(L_MOTOR_IN2, LOW);
            digitalWrite(R_MOTOR_IN3, LOW); digitalWrite(R_MOTOR_IN4, HIGH);
            analogWrite(L_MOTOR_ENA, motorSpeed); analogWrite(R_MOTOR_ENB, motorSpeed);
        } else if (data.j1_x < JOY_DEADZONE_LOW) { // Left
            digitalWrite(L_MOTOR_IN1, LOW); digitalWrite(L_MOTOR_IN2, HIGH);
            digitalWrite(R_MOTOR_IN3, HIGH); digitalWrite(R_MOTOR_IN4, LOW);
            analogWrite(L_MOTOR_ENA, motorSpeed); analogWrite(R_MOTOR_ENB, motorSpeed);
        } else {
            stopMotors(); // Stop if car joystick is centered
        }
    }

    // --- NEW: Slow Pan/Tilt Control Logic ---
    // Pan Control (Horizontal)
    if (data.j2_x > JOY_DEADZONE_HIGH) { // If joystick is pushed right
      currentPanAngle++; // Increment the angle
    } else if (data.j2_x < JOY_DEADZONE_LOW) { // If joystick is pushed left
      currentPanAngle--; // Decrement the angle
    }
    // If joystick is in the middle, do nothing to the angle

    // Tilt Control (Vertical)
    if (data.j2_y > JOY_DEADZONE_HIGH) { // If joystick is pushed up
      currentTiltAngle++; // Increment the angle
    } else if (data.j2_y < JOY_DEADZONE_LOW) { // If joystick is pushed down
      currentTiltAngle--; // Decrement the angle
    }
    // If joystick is in the middle, do nothing to the angle

    // Constrain the angles to the servo's valid range (0 to 180)
    currentPanAngle = constrain(currentPanAngle, 0, 180);
    currentTiltAngle = constrain(currentTiltAngle, 0, 180);

    // Write the new, slightly changed angles to the servos
    panServo.write(currentPanAngle);
    tiltServo.write(currentTiltAngle);
    
    // --- Serial Debugging Output ---
    Serial.print("Car Y: "); Serial.print(data.j1_y);
    Serial.print(" | Pan Angle: "); Serial.print(currentPanAngle);
    Serial.print(" | Tilt Angle: "); Serial.println(currentTiltAngle);

    // This delay controls the speed of the pan/tilt.
    // Bigger number = Slower movement.
    delay(5); 
  }
}
