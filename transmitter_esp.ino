/*
-- Swarnendu-Rup Repository -->
-- MODIFIED ESP32 TRANSMITTER --

This code is for an ESP32.
It uses one joystick for car control, a potentiometer for speed,
and FOUR PUSH BUTTONS for camera pan/tilt control.
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// --- Pin Configuration (ESP32) ---
// NRF24L01 Module Connections (Standard VSPI)
#define NRF_CE_PIN 4
#define NRF_CSN_PIN 5
// SCK: 18, MOSI: 23, MISO: 19 - these are set by the library

// Joystick (Car Control) - Use ADC1 pins
#define JOY1_X_PIN 34
#define JOY1_Y_PIN 35

// Potentiometer (Speed Control) - Use an ADC1 pin
#define SPEED_POT_PIN 32

// NEW: Push Buttons for Camera Control
#define PAN_LEFT_PIN  13
#define PAN_RIGHT_PIN 12
#define TILT_UP_PIN   14
#define TILT_DOWN_PIN 27

// --- RF24 Setup ---
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN); // CE, CSN
const byte address[6] = "00001";

// Data structure to hold all control values (UNCHANGED to match receiver)
struct Data_Package {
  byte j1_x;
  byte j1_y;
  byte j2_x;    // This will now be controlled by buttons
  byte j2_y;    // This will now be controlled by buttons
  byte pot_speed;
};

Data_Package data; // Create a variable of the data structure

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  Serial.println("ESP32 Transmitter starting up...");

  // NEW: Configure button pins with internal pull-ups
  // This means the pin is HIGH by default, and LOW when the button is pressed.
  pinMode(PAN_LEFT_PIN, INPUT_PULLUP);
  pinMode(PAN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(TILT_UP_PIN, INPUT_PULLUP);
  pinMode(TILT_DOWN_PIN, INPUT_PULLUP);
  Serial.println("Button pins configured.");

  // Initialize NRF24L01 radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  Serial.println("Radio setup complete. Sending data...");
}

void loop() {
  // Read analog values and map them from 0-4095 (ESP32 12-bit ADC) to 0-255
  data.j1_x = map(analogRead(JOY1_X_PIN), 0, 4095, 0, 255);
  data.j1_y = map(analogRead(JOY1_Y_PIN), 0, 4095, 0, 255);
  data.pot_speed = map(analogRead(SPEED_POT_PIN), 0, 4095, 0, 255);

  // --- NEW: Read buttons to simulate the second joystick ---
  // Pan Control (j2_x)
  if (digitalRead(PAN_LEFT_PIN) == LOW) { // Button is pressed
    data.j2_x = 0; // Simulate joystick pushed left
  } else if (digitalRead(PAN_RIGHT_PIN) == LOW) { // Button is pressed
    data.j2_x = 255; // Simulate joystick pushed right
  } else {
    data.j2_x = 127; // Simulate joystick centered
  }

  // Tilt Control (j2_y)
  if (digitalRead(TILT_UP_PIN) == LOW) { // Button is pressed
    data.j2_y = 255; // Simulate joystick pushed up
  } else if (digitalRead(TILT_DOWN_PIN) == LOW) { // Button is pressed
    data.j2_y = 0; // Simulate joystick pushed down
  } else {
    data.j2_y = 127; // Simulate joystick centered
  }

  // Send the data package over the radio
  radio.write(&data, sizeof(Data_Package));

  // --- Serial Debugging Output ---
  Serial.print("Car Stick: X="); Serial.print(data.j1_x);
  Serial.print(", Y="); Serial.print(data.j1_y);
  Serial.print(" | Cam Buttons: X="); Serial.print(data.j2_x);
  Serial.print(", Y="); Serial.print(data.j2_y);
  Serial.print(" | Speed Pot: "); Serial.println(data.pot_speed);

  delay(100); // Delay to prevent flooding the receiver
}