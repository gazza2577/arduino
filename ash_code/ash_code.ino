#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <CMRI.h>
#include <Auto485.h>

#define CMRI_ADDR 1   // unique for each Arduino
#define DE_PIN 2

int Tbit[32];
uint8_t servonum = 0;   // servo counter
uint8_t lastservo = 32;

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);    //setup the board address 0
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);    //setup the board address 1
Auto485 bus(DE_PIN); // Arduino pin 2 -> MAX485 DE and RE pins
CMRI cmri(CMRI_ADDR, 24, 48, bus);


void setup() {
  Serial.begin(9600);
  bus.begin(9600, SERIAL_8N2);
  pwm1.begin();
  pwm2.begin();
  pwm1.setPWMFreq(60);
  pwm2.setPWMFreq(60);   
}

void loop() {
   cmri.process();
   for(servonum = 0 ; servonum < lastservo; servonum ++){

    Tbit[servonum] = (cmri.get_bit(servonum));     

    if(servonum <16) {
        if (Tbit[servonum] == 1){
            pwm1.setPWM(servonum, 0, 100);
          }
        if (Tbit[servonum] == 0){
            pwm1.setPWM(servonum, 0, 400);
          }
      }
      
    if(servonum >=16 && servonum <= 31){
        if (Tbit[servonum] == 1){
            pwm2.setPWM(servonum - 16, 0, 100);
          }
        if (Tbit[servonum] == 0){
            pwm2.setPWM(servonum - 16, 0, 400);
          }
      }   
    // add additional lines for additional boards

  }

}
