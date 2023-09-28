// For example 01 use mega2560 board
// This example shows how to use the library to control the motor
// The motor is connected to the pins 5, 22, 25

#include <TcYKA2304ME.h>
#define PU1_PIN 5
#define DR1_PIN 22
#define MF1_PIN 25

TcYKA2304ME motor1(PU1_PIN,DR1_PIN,MF1_PIN);
void OnMotor1Learned(unsigned long position, unsigned long minPosition, unsigned long maxPosition);


void setup() {
  Serial.begin(9600);
  motor1.setOnLearned(OnMotor1Learned);
}

void loop() {
   motor1.update();
}

void OnMotor1Learned(unsigned long position, unsigned long minPosition, unsigned long maxPosition)
{
    Serial.print("Motor 1 learned: ");
    Serial.print(position);
    Serial.print(" min: ");
    Serial.print(minPosition);
    Serial.print(" max: ");
    Serial.println(maxPosition);
}