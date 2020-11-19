#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <CMRI.h>
#include <Auto485.h>

#define CMRI_ADDR 1
#define DE_PIN 2
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);
int Tbit[2];
Auto485 bus(DE_PIN); // Arduino pin 2 -> MAX485 DE and RE pins
CMRI cmri(CMRI_ADDR, 24, 48, bus);

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel PWM test!");
  bus.begin(9600);
  pwm1.begin();
  pwm1.setPWMFreq(60);

  pwm2.begin();
  pwm2.setPWMFreq(60);



}
void loop(){
 Serial.begin(9600);
 cmri.process();{
for (int e = 0; e < 16; e++)

  for (int i = 0; i < 16; i++)



    for (int c = 0; c < 16; c++)

      if (Tbit[c] == 1)
        pwm1.setPWM(i, 0, 170);
Serial.println("throw");
Serial.println("i");
for (int e = 0; e < 16; e++)

  for (int i = 0; i < 16; i++)
for (int c = 0; c < 16; c++)
if (Tbit[c] == 0)
  pwm1.setPWM(i, 0, 150);
Serial.println("close");
Serial.println("i");

 for (int e = 0; e < 16; e++)

  for (int i = 0; i < 16; i++)
    for (int c = 0; c < 16; c++)
      if (Tbit[c] == 1)
        pwm2.setPWM(i, 0, 170);
Serial.println("throw");}
Serial.println("i");


for (int e = 0; e < 16; e++)

  for (int i = 0; i < 16; i++)

    for (int c = 0; c < 16; c++)
      if (Tbit[c] == 0)
        pwm2.setPWM(i, 0, 150);
Serial.println("close");{
Serial.println("i");

}
}
