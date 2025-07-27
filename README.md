# Arduino-ESP32-RC-Car-with-FPV-Camera-Gimbal
 A versatile RC car controlled by a custom-built Arduino transmitter. This project features an ESP32 receiver for robust motor control and a 2-axis servo gimbal for a first-person view (FPV) camera. Enjoy precise, slow-speed camera movement for smooth tracking while driving your wirelessly controlled vehicle.

Here we have used Arduino as a transmitter and esp32 as a receiver with nrf24L01+PA+LNA as transmitter and regular nrf24L01 as receiver

Hardware Required

For the Transmitter:

1x Arduino Nano: The brain of the transmitter.
1x NRF24L01 Radio Module: For wireless communication.
1x NRF24L01 Adapter: (Highly Recommended) Provides stable 3.3V power and makes wiring easier.
2x Joystick Modules: For controlling the car and the camera gimbal.
1x 10k Ohm Potentiometer: To adjust the speed of the car.
1x Breadboard and Jumper Wires: For connecting all the components.
1x Power Source for Arduino Nano: Such as a 9V battery with a barrel jack connector or a USB power bank.

For the Receiver (on the RC Car):

1x ESP32 Development Board: To receive commands and control the motors and servos.
1x NRF24L01 Radio Module: To receive data from the transmitter.
1x L298N Motor Driver: To control the two DC motors of the car.
1x RC Car Chassis: With two DC motors and wheels.
2x Servo Motors (e.g., SG90 or MG90S): For the pan and tilt camera gimbal.
1x Pan/Tilt Gimbal Bracket: To mount the servos and a camera.
1x Power Source for ESP32 and Motors: A LiPo battery pack (7.4V) or a battery holder with AA batteries is a good option. Important: The L298N will regulate the voltage down for the ESP32.
Jumper Wires: To connect all components.
Wiring Connections - Transmitter (Arduino Nano)

This setup uses an Arduino Uno/Nano to read the joystick and potentiometer inputs and sends them via the NRF24L01 module.

Transmitter (Arduino Uno / Nano) Wiring

NRF24L01+PA+LNA Module -> Arduino Uno/Nano
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

Receiver Connections- 

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
