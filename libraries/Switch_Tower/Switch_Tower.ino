#include <NmraDcc.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <turnout.h>

// ******** UNLESS YOU WANT ALL CV'S RESET UPON EVERY POWER UP
// ******** AFTER THE INITIAL DECODER LOAD REMOVE THE "//" IN THE FOOLOWING LINE!!
//#define DECODER_LOADED
#define SERVOMIN  300 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  700 // this is the 'maximum' pulse length count (out of 4096)
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);// add additional variables for more boards 
//next board is 0x42, incrementing for each additional board
const uint8_t Board_Count = 2; // change this for the number of boards
#define This_Decoder_Address 40     //ACCESSORY DECODER ADDRESS
                                    //Start of SWITCHES RANGE
uint8_t CV_DECODER_MASTER_RESET = 120;
#define CV_To_Store_SET_CV_Address  121
#define CV_Accessory_Address CV_ACCESSORY_DECODER_ADDRESS_LSB

NmraDcc  Dcc ;
DCC_MSG  Packet ;

struct CVPair
{
  uint16_t  CV;
  uint8_t   Value;
};
CVPair FactoryDefaultCVs [] =
{
  {CV_ACCESSORY_DECODER_ADDRESS_LSB, 1},
  {CV_ACCESSORY_DECODER_ADDRESS_MSB, 0},
};

uint8_t FactoryDefaultCVIndex = 0;

void notifyCVResetFactoryDefault()
{
  // Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset 
  // to flag to the loop() function that a reset to Factory Defaults needs to be done
  FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs)/sizeof(CVPair);
};

uint8_t servonum = 0;// our servo # counter
uint8_t lastservo = 31;
struct turnout TurnOut[32];
void setup() {
  Serial.begin(115200);
  Dcc.pin(0, 2, 1);    // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up 
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );  // Call the main DCC Init function to enable the DCC Receiver
  pwm1.begin();// start pwm board 1
  pwm2.begin();//start pwm board 2
  pwm1.setPWMFreq(50);  //run at 50 Hz updates
  pwm2.setPWMFreq(50);  //run at 50 Hz updates
// iniatialize all turnouts to start at normal position, set normal (min) and diverge(max) positions     
  for (int serv = 0;serv < lastservo;serv ++)
  {
    TurnOut[serv].min_pos= SERVOMIN;
    TurnOut[serv].max_pos=SERVOMAX;
    TurnOut[serv].current_pos = TurnOut[serv].min_pos + 100;
    TurnOut[serv].target_pos = TurnOut[serv].min_pos;
  }
  for(int serv = 0;serv<16;serv++){
    pwm1.setPWM(serv, 0, TurnOut[serv].min_pos);//set first 16 servos to min
    pwm2.setPWM(serv + 16, 0, TurnOut[serv+16].min_pos);}//set servos 16-31 to min
    // add additional lines for additional boards
}

void loop() {
  Dcc.process();  // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation

  if( FactoryDefaultCVIndex && Dcc.isSetCVReady())
  {
    FactoryDefaultCVIndex--; // Decrement first as initially it is the size of the array 
    Dcc.setCV( FactoryDefaultCVs[FactoryDefaultCVIndex].CV, FactoryDefaultCVs[FactoryDefaultCVIndex].Value);
  }
  UpdatePosition();

}

void UpdatePosition()
{   
    for(servonum = 0 ; servonum < lastservo; servonum ++){
    if (TurnOut[servonum].current_pos!= TurnOut[servonum].target_pos){
      if(TurnOut[servonum].current_pos<TurnOut[servonum].target_pos)TurnOut[servonum].current_pos++;
      else TurnOut[servonum].current_pos --;
    }
    if(servonum <16)pwm1.setPWM(servonum, 0, TurnOut[servonum].current_pos);
    if(servonum >=16 && servonum <= 32)pwm2.setPWM(servonum - 16, 0, TurnOut[servonum - 16].current_pos);
// add additional lines for additional boards 
  }
}
// This function is called whenever a normal DCC Turnout Packet is received
void notifyDccAccTurnoutBoard( uint16_t BoardAddr, uint8_t OutputPair, uint8_t Direction, uint8_t OutputPower )
{
  Serial.print("notifyDccAccTurnoutBoard: ") ;
  Serial.print(BoardAddr,DEC) ;
  Serial.print(',');
  Serial.print(OutputPair,DEC) ;
  Serial.print(',');
  Serial.print(Direction,DEC) ;
  Serial.print(',');
  Serial.println(OutputPower, HEX) ;
  int index = (BoardAddr*4)+OutputPair;
  if(index<=lastservo) //check for valid address
  {
    Serial.print("index valid = ");
    Serial.println(index, DEC);
    if (Direction>0)
    {TurnOut[index].target_pos=TurnOut[index].max_pos;
     Serial.print("target = ");
     Serial.println(TurnOut[index].target_pos);
     }
    else {
      TurnOut[index].target_pos=TurnOut[index].min_pos;
      Serial.print("target = ");
      Serial.println(TurnOut[index].target_pos);
      }
  }    
}
