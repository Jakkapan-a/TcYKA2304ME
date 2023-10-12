// For example 01 use mega2560 board
// This example shows how to use the library to control the motor
// The motor is connected to the pins 5, 22, 25
#include <TcYKA2304ME.h>
#include <TcBUTTON.h>
// ------------------  INPUT      ------------------ //
#define MOTOR1_START_PIN 30
void OnSensor1Start(bool);
TcBUTTON SENSOR1_MOTOR_START(MOTOR1_START_PIN, false);
#define MOTOR1_END_PIN 31
void OnSensor1End(bool);
TcBUTTON SENSOR1_MOTOR_END(MOTOR1_END_PIN, false);

#define MOTOR2_START_PIN 32
void OnSensor2Start(bool);
TcBUTTON SENSOR2_MOTOR_START(MOTOR2_START_PIN, false);
#define MOTOR2_END_PIN 33
void OnSensor2End(bool);
TcBUTTON SENSOR2_MOTOR_END(MOTOR2_END_PIN, false);


#define PU1_PIN 5
#define DR1_PIN 10
#define MF1_PIN 23

TcYKA2304ME motor1(PU1_PIN, DR1_PIN, MF1_PIN);
void OnMotor1Learned(unsigned long position, unsigned long minPosition, unsigned long maxPosition);


void setup() {
  Serial.begin(115200);
  motor1.setOnLearned(OnMotor1Learned);
}

void loop() {
  motor1.update();
}

void OnMotor1Learned(unsigned long position, unsigned long minPosition, unsigned long maxPosition) {
  Serial.print("Motor 1 learned: ");
  Serial.print(position);
  Serial.print(" min: ");
  Serial.print(minPosition);
  Serial.print(" max: ");
  Serial.println(maxPosition);
}

// ------------------  FUNCTIONS SENSOR 1  ------------------ //
void OnSensor1Start(bool state) {
  Serial.println("OnSensor1 Start");
  // byte data[] = {0x02, 0x55, 0x4C, 0x31, 0x00, 0x00, 0x00, 0x03};
  // if(state){
  //     data[6] = 0x01;
  // }
  // Serial.write(data, sizeof(data));
  motor1.setStateStart(!state);
}

void OnSensor1End(bool state) {
  Serial.println("OnSensor1 End");
  // byte data[] = {0x02, 0x55, 0x4C, 0x32, 0x00, 0x00, 0x00, 0x03};
  // if(state){
  //     data[6] = 0x01;
  // }
  // Serial.write(data, sizeof(data));
  motor1.setStateEnd(!state);
}