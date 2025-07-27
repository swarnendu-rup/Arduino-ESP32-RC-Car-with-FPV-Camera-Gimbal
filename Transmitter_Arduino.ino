/*
--Swarnendu-Rup--> Repository

Transmitter (Arduino Uno / Nano) Wiring

NRF24L01 Module -> Arduino Uno/Nano
VCC -> 3.3V
GND -> GND
CE -> D7
CSN -> D8
SCK -> D13
MOSI -> D11
MISO -> D12
Joystick 1 (Car Control) -> Arduino Uno/Nano
VCC -> 5V
GND -> GND
VRx -> A0
VRy -> A1
Joystick 2 (Camera Control) -> Arduino Uno/Nano
VCC -> 5V
GND -> GND
VRx -> A2
VRy -> A3
Potentiometer (Speed Control) -> Arduino Uno/Nano
VCC (one of the outer pins) -> 5V
GND (the other outer pin) -> GND
Signal (middle pin) -> A4
Receiver (ESP32 & Car) Wiring
Important Power Note: First, connect your battery pack (e.g., 7.4V) to the L298N motor driver's main power terminals. The L298N will then provide a stable 5V to power the ESP32 and the servos.
Main Power Connections
Battery Positive (+) -> L298N VCC/12V Terminal
Battery Negative (-) -> L298N GND Terminal
L298N 5V Output -> ESP32 VIN Pin
L298N GND Terminal -> ESP32 GND Pin
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// --- Pin Configuration ---
// NRF24L01 Module Connections
#define NRF_CE_PIN 7
#define NRF_CSN_PIN 8

// Main Joystick (Car Control)
#define JOY1_X_PIN A0
#define JOY1_Y_PIN A1

// Second Joystick (Camera Control)
#define JOY2_X_PIN A2
#define JOY2_Y_PIN A3

// Speed Control (Potentiometer)
#define SPEED_POT_PIN A4

// --- RF24 Setup ---
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN); // CE, CSN
const byte address[6] = "00001";

// Data structure to hold all control values
struct Data_Package {
  byte j1_x;
  byte j1_y;
  byte j2_x;
  byte j2_y;
  byte pot_speed;
};

Data_Package data; // Create a variable of the data structure

void setup() {
  // Start serial communication for debugging
  Serial.begin(9600);
  Serial.println("Transmitter starting up...");

  // Initialize NRF24L01 radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN); // Use lower power for testing
  radio.stopListening();
  Serial.println("Radio setup complete. Sending data...");
}

void loop() {
  // Read analog values and map them from 0-1023 to 0-255
  data.j1_x = map(analogRead(JOY1_X_PIN), 0, 1023, 0, 255);
  data.j1_y = map(analogRead(JOY1_Y_PIN), 0, 1023, 0, 255);
  data.j2_x = map(analogRead(JOY2_X_PIN), 0, 1023, 0, 255);
  data.j2_y = map(analogRead(JOY2_Y_PIN), 0, 1023, 0, 255);
  data.pot_speed = map(analogRead(SPEED_POT_PIN), 0, 1023, 0, 255);

  // Send the data package over the radio
  radio.write(&data, sizeof(Data_Package));

  // --- Serial Debugging Output ---
  Serial.print("Car Stick: X=");
  Serial.print(data.j1_x);
  Serial.print(", Y=");
  Serial.print(data.j1_y);

  Serial.print(" | Cam Stick: X=");
  Serial.print(data.j2_x);
  Serial.print(", Y=");
  Serial.print(data.j2_y);

  Serial.print(" | Speed Pot: ");
  Serial.println(data.pot_speed);

  delay(100); // Delay to prevent flooding the receiver
}