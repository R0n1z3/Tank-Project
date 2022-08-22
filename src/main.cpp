#include <Arduino.h>
#include <SPI.h>
#include <printf.h>
#include <RF24.h>
RF24 radio(8, 7, 4000000); // set SPI speed to 4MHz instead of default 10MHz

const byte address[6] = "00001";
int   data[2]         = {0, 0};
byte  motorPins[4]    = {6, 5, 9, 10};
int   motorData[4]    = {0, 0, 0, 0};
float leftWeight      = 0.0;
float rightWeight     = 0.0;

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  for (int count = 0; count < 4; count++) {
    pinMode(motorPins[count], OUTPUT);
  }
}

void input() {
  if (radio.available()) {
    radio.read(data, sizeof(int) * 2);
  }
}

void process() {
  //Serial.println(String(motorData[0]) + "  " + String(motorData[1]) + "  " + String(motorData[2]) + "  " + String(motorData[3]));
  float weight = data[0] / 100.0;
  //Serial.println(weight);
  if (weight > 1.1 || weight < 0.9) {


    leftWeight  = weight;
    rightWeight = 2.0 - weight;

  } else {
    leftWeight  = 1.0;
    rightWeight = 1.0;
  }
  //*this is the updating of the motor data values.

  if (data[1] > 25 || data[1] < -25) {
    if (data[1] > 25) {
;     motorData[0] = leftWeight  * data[1];
      motorData[1] = 0;
      motorData[2] = rightWeight * data[1];
      motorData[3] = 0;
    } else {
      motorData[0] = 0;
      motorData[1] = leftWeight  * abs(data[1]);
      motorData[2] = 0;
      motorData[3] = rightWeight * abs(data[1]);
    }
  } else {
    motorData[0]   = 0;
    motorData[1]   = 0;
    motorData[2]   = 0;
    motorData[3]   = 0;
  }
}

void output() {
  for (int count = 0; count < 4; count++) {
    //Serial.println("motorPin " + String(motorPins[count]) + " Settings " + String(motorData[count]));
    analogWrite(motorPins[count], motorData[count]);
  }
}
void loop() {
  input();
  process();
  output();
}

