#include <mcp_can.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <CMRI.h>
#include <Auto485.h>
#include <EEPROM.h>
#include "Adafruit_MCP23017.h" 


#define CMRI_ADDR 1
#define DE_PIN 2
#define LED01 7 // LED code left in to try running pca9685 and LEDs on same mega.
#define button 3 // sensor for the LED


#define I2CAdd 0x40
#define I2Cadd 0x41
#define I2Cadd 0x42

int buttonState = 0;// set a variable to store the button state 
int Tbit[32];
byte i2c_rcv; // data received from I2C bus
unsigned long time_start; //start time in milliseconds
Adafruit_MCP23017 mcp0;
Adafruit_MCP23017 mcp1;
Adafruit_MCP23017 mcp2;
Adafruit_MCP23017 mcp3;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);
Auto485 bus(DE_PIN); // Arduino pin 2 -> MAX485 DE and RE pins
CMRI cmri(CMRI_ADDR, 24, 48, bus);

void setup() {
  
  // LIGHTS
pinMode(LED01, OUTPUT);
pinMode(button, INPUT_PULLUP);

// SERVOS
 
  mcp0.begin(0);
  mcp1.begin(1);
  mcp2.begin(2);
  mcp3.begin(3);
    

  
  Serial.begin(9600);

  Wire.begin();
  
  Wire.setClock(200000);
  
  // initialize global variables  
  i2c_rcv = 255;
  time_start = millis();
  
  bus.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);
  pwm1.begin();
  pwm1.setPWMFreq(60);  // This is the maximum PWM frequency
  pwm2.begin();
  pwm2.setPWMFreq(60);  // This is the maximum PWM frequency
  pwm.setPWM(0, 0, 300);
  pwm.setPWM(1, 0, 300);
  pwm.setPWM(2, 0, 300);
  pwm.setPWM(3, 0, 300);
  pwm.setPWM(4, 0, 300);
  pwm.setPWM(5, 0, 300);
  pwm.setPWM(6, 0, 300);
  pwm.setPWM(7, 0, 300);
  pwm.setPWM(8, 0, 300);
  pwm.setPWM(9, 0, 300);
  pwm.setPWM(10, 0, 300);
  pwm.setPWM(11, 0, 300);
  pwm.setPWM(12, 0, 300);
  pwm.setPWM(13, 0, 300);
  pwm.setPWM(14, 0, 300);
  pwm.setPWM(15, 0, 300);
  
  pwm1.setPWM(0, 0, 300);
  pwm1.setPWM(1, 0, 300);
  pwm1.setPWM(2, 0, 300);
  pwm1.setPWM(3, 0, 300);
  pwm1.setPWM(4, 0, 300);
  pwm1.setPWM(5, 0, 300);
  pwm1.setPWM(6, 0, 300);
  pwm1.setPWM(7, 0, 300);
  
  pwm2.setPWM(0, 0, 300);
  pwm2.setPWM(1, 0, 300);
  pwm2.setPWM(2, 0, 300);
  pwm2.setPWM(3, 0, 300);
  pwm2.setPWM(4, 0, 300);
  pwm2.setPWM(5, 0, 300);
  pwm2.setPWM(6, 0, 300);



}

void loop(){
   cmri.process();
   Tbit[0] = (cmri.get_bit(0)); //Turnout01
   Tbit[1] = (cmri.get_bit(1)); //Turnout02
   Tbit[2] = (cmri.get_bit(2)); //Turnout03
   Tbit[3] = (cmri.get_bit(3)); //Turnout04
   Tbit[4] = (cmri.get_bit(4)); //Turnout05
   Tbit[5] = (cmri.get_bit(5)); //Turnout06
   Tbit[6] = (cmri.get_bit(6)); //Turnout07
   Tbit[7] = (cmri.get_bit(7)); //Turnout08
   Tbit[8] = (cmri.get_bit(8)); //Turnout09
   Tbit[9] = (cmri.get_bit(9)); //Turnout10
   Tbit[10] = (cmri.get_bit(10)); //Turnout11
   Tbit[11] = (cmri.get_bit(11)); //Turnout12
   Tbit[12] = (cmri.get_bit(12)); //Turnout13
   Tbit[13] = (cmri.get_bit(13)); //Turnout14
   Tbit[14] = (cmri.get_bit(14)); //Turnout15
   Tbit[15] = (cmri.get_bit(15)); //Turnout16
   
   Tbit[16] = (cmri.get_bit(16)); //Turnout17
   Tbit[17] = (cmri.get_bit(17)); //Turnout18
   Tbit[18] = (cmri.get_bit(18)); //Turnout19
   Tbit[19] = (cmri.get_bit(19)); //Turnout20
   Tbit[20] = (cmri.get_bit(20)); //Turnout21
   Tbit[21] = (cmri.get_bit(21)); //Turnout22
   Tbit[22] = (cmri.get_bit(22)); //Turnout23
   Tbit[23] = (cmri.get_bit(23)); //Turnout24
   Tbit[24] = (cmri.get_bit(24)); //Turnout25
   
   Tbit[25] = (cmri.get_bit(25)); //Turnout26
   Tbit[26] = (cmri.get_bit(26)); //Turnout27
   Tbit[27] = (cmri.get_bit(27)); //Turnout28
   Tbit[28] = (cmri.get_bit(28)); //Turnout29
   Tbit[29] = (cmri.get_bit(29)); //Turnout30
   Tbit[30] = (cmri.get_bit(30)); //Turnout31
   
   
// TURNOUT01, ADDRESS 1001, BIT 0
if (Tbit[0] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(0, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[0] == 0){
pwm.setPWM(0, 0, 150);
}

// TURNOUT02, ADDRESS 1002, BIT 1
if (Tbit[1] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(1, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[1] == 0){
pwm.setPWM(1, 0, 150);
}

// TURNOUT03, ADDRESS 1003, BIT 2
if (Tbit[2] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(2, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[2] == 0){
pwm.setPWM(2, 0, 150);
}

// TURNOUT04, ADDRESS 1004, BIT 3
if (Tbit[3] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(3, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[3] == 0){
pwm.setPWM(3, 0, 150);
}

// TURNOUT05, ADDRESS 1005, BIT 4
if (Tbit[4] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(4, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[4] == 0){
pwm.setPWM(4, 0, 150);
}

// TURNOUT06, ADDRESS 1006, BIT 5
if (Tbit[5] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(5, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[5] == 0){
pwm.setPWM(5, 0, 150);
}

// TURNOUT07, ADDRESS 1007, BIT 6
if (Tbit[6] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(6, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[6] == 0){
pwm.setPWM(6, 0, 150);
}

// TURNOUT08, ADDRESS 1008, BIT 7
if (Tbit[7] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(7, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[7] == 0){
pwm.setPWM(7, 0, 200);
}

// TURNOUT09, ADDRESS 1009, BIT 8
if (Tbit[8] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(8, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[8] == 0){
pwm.setPWM(8, 0, 150);
}

// TURNOUT10, ADDRESS 1010, BIT 9
if (Tbit[9] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(9, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[9] == 0){
pwm.setPWM(9, 0, 150);
}

// TURNOUT11, ADDRESS 1011, BIT 10
if (Tbit[10] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(10, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[10] == 0){
pwm.setPWM(10, 0, 150);
}

// TURNOUT12, ADDRESS 1012, BIT 11
if (Tbit[11] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(11, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[11] == 0){
pwm.setPWM(11, 0, 150);
}

// TURNOUT13, ADDRESS 1013, BIT 12
if (Tbit[12] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(12, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[12] == 0){
pwm.setPWM(12, 0, 150);
}

// TURNOUT14, ADDRESS 1014, BIT 13
if (Tbit[13] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(13, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[13] == 0){
pwm.setPWM(13, 0, 150);
}

// TURNOUT15, ADDRESS 1015, BIT 14
if (Tbit[14] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(14, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[14] == 0){
pwm.setPWM(14, 0, 150);
}

// TURNOUT16, ADDRESS 1016, BIT 15
if (Tbit[15] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm.setPWM(15, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[15] == 0){
pwm.setPWM(15, 0, 150);
}
  //Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x41);


// TURNOUT17, ADDRESS 1017, BIT 1
if (Tbit[16] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm1.setPWM(0, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[16] == 0){
pwm1.setPWM(0, 0, 150);
}

// TURNOUT18, ADDRESS 1018, BIT 2
 if (Tbit[17] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm1.setPWM(1, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[17] == 0){
pwm1.setPWM(1, 0, 150);
}

// TURNOUT19, ADDRESS 1019, BIT 3
if (Tbit[18] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm1.setPWM(2, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[8] == 0){
pwm1.setPWM(2, 0, 150);
}

// TURNOUT20, ADDRESS 1020, BIT 4
if (Tbit[19] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm1.setPWM(3, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[19] == 0){
pwm1.setPWM(3, 0, 150);
}

// TURNOUT21, ADDRESS 1021, BIT 5
 if (Tbit[20] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm1.setPWM(4, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
 if (Tbit[20] == 0){
 pwm1.setPWM(4, 0, 150);
 }

// TURNOUT22, ADDRESS 1022, BIT 6
 if (Tbit[21] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm1.setPWM(5, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
 }
 if (Tbit[21] == 0){
 pwm1.setPWM(5, 0, 150);
 }

// TURNOUT23, ADDRESS 1023, BIT 7
 if (Tbit[22] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm1.setPWM(6, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
 }
 if (Tbit[22] == 0){
 pwm1.setPWM(6, 0, 150);
 }

// TURNOUT24, ADDRESS 1024, BIT 8
 if (Tbit[23] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm1.setPWM(7, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
 }
 if (Tbit[23] == 0){
 pwm1.setPWM(7, 0, 150);
 /////Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);
// TURNOUT25, ADDRESS 1025, BIT 0 
 if (Tbit[24] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm2.setPWM(0, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[24] == 0){
pwm2.setPWM(0, 0, 150);}
/////Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x42);
// TURNOUT26, ADDRESS 1026, BIT 1
if (Tbit[25] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm2.setPWM(1, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[25] == 0){
pwm2.setPWM(1, 0, 150);
}

// TURNOUT27, ADDRESS 1027, BIT 2
 if (Tbit[26] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm2.setPWM(2, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
 if (Tbit[26] == 0){
 pwm2.setPWM(2, 0, 150);
 }

// TURNOUT28, ADDRESS 1028, BIT 3
 if (Tbit[27] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm2.setPWM(3, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
 }
 if (Tbit[27] == 0){
 pwm2.setPWM(3, 0, 150);
 }

// TURNOUT29, ADDRESS 1029, BIT 4
 if (Tbit[28] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm2.setPWM(4, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
 }
 if (Tbit[28] == 0){
 pwm2.setPWM(4, 0, 150);
 }

// TURNOUT30, ADDRESS 1030, BIT 5
 if (Tbit[29] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
 pwm2.setPWM(5, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
 }
 if (Tbit[29] == 0){
 pwm2.setPWM(5, 0, 150);
 
// TURNOUT31, ADDRESS 1031, BIT 6 
 if (Tbit[30] == 1){ //if Turnoutbit n == 1 (to throw) or 0 (to close)
pwm2.setPWM(6, 0, 300); //set servo n (0-15), board n (0,1,2,3,4,5,etc), angle = n
}
if (Tbit[30] == 0){
pwm2.setPWM(6, 0, 150);} 
 }
}
}
