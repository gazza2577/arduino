/*
  d. bodnar  revised 9-06-2016  V3.12
  Steve's mod
  Norman's Mods
  MPA GUI Mods
    After a short message to Dave, he graciously endorsed further development of his throttle. Please note that whilst I might add or remove
    functionality to/from Dave's original work he remains the author of this fine work and I am simply trying to extend (or 
    hopefully not destroy) what he has already graciously done:-) I take no credit.
    First release V0.1:-
    Supports animated buttons for pages 1 and 2 (and some additional changes:-))
    Change History
      V0.2 finalized on 25 July 2017
        UpdateBinary - refreshes binary function state for active address
        UpdateButtons - refreshes function and address buttons for active address
        SetActiveAddress - restores address buttons after address change/abort
      V0.3 Started and released on 25 July 2017
        Direction Change Buttons added
        Slider added in parallel with rotary encoder
        scaling routines removed for the time being
        accelerated encoder rate removed for the time being...
      V0.4
        Accelerated rotary encoder code re-instated
        Incremental buttons added to adjust speed (with repeat function:-) (Keith's idea) 
      V0.41
        Changed method of determining Nextion throttle setting to overcome speed randomly jumping to zero
        zeroed speed on direction change
        Tidied up interface
      V0.42
        Added ReverseThreshhold constant to set a point above which speed is reset to zero on direction change (to answer Steve:-)
      V0.43 Hmmm Just when I thought 42 was the answer to the meaning of it all...(the late Douglas Adams)
        Added "On" and "Off" buttons - thanks Keith:-)
        Introduced Functions up to 24 (my original goal being realized sooner than expected, but thanks again Keith:-))
        Added some (more) colour (color) for fun
      v0.44
      V0.45
        Added full support for 24 (25) functions
      V0.50
        Added Turnout Control
      V0.60
        Added Rudimentary Route support
      V4.00
        Cleaned up some inconsistencies
        Changed STOP button to be single event button
        Completed Route page on Nextion
        End of Development:-)
      V4.01
        OK, not quite the end...
        Nextion to Arduino speed to 57600 seems to work fine
        Arduino to Base Station Serial port set to 115200
        Updated Nextion HMI to include 250ms delays between turnout commands
        Moved the Rotary Encoder 'speed-up' code into the interrupt handlers

        TurnOut(Default Orientation, Address, Default State)
        examples:-
          TurnOut(LH, 1, Closed)  - Left Hand turnout with address 1, State is CLOSED
          TurnOut(RH, 2, Closed)  - Right Hand turnout with address 2, State is CLOSED
          TurnOut(LH, 3, Thrown)  - Left Hand turnout with address 3, State is THROWN
          TurnOut(RH, 4, Thrown)  - Right Hand turnout with address 4, State is THROWN
       V5.00
         GUI completely reworked.    
*/
/*
Array for Turnouts on Page 3
Modify to suit your preferences:-
(Orientation (LH/RH), Address, State (0=CLOSED, 1=THROWN))
*/
#define RH 10  //Right Hand Closed .pic
#define LH 12  //Left Hand Closed .pic
#define Closed 0 
#define Thrown 1

int TurnOuts3[25][3] =
{ 
  {LH,1,Closed},
  {LH,2,Closed},
  {LH,3,Closed},
  {LH,4,Closed},
  {LH,5,Closed},
  {LH,6,Closed},
  {LH,7,Closed},
  {LH,8,Closed},
  {LH,9,Closed},
  {LH,10,Closed},
  {LH,11,Closed},
  {LH,12,Closed},
  {LH,13,Closed},
  {LH,14,Closed},
  {LH,15,Closed},
  {LH,16,Closed},
  {LH,17,Closed},
  {LH,18,Closed},
  {LH,19,Closed},
  {LH,20,Closed},
  {LH,21,Closed},
  {LH,22,Closed},
  {LH,23,Closed},
  {LH,24,Closed},
  {LH,25,Closed}
};
/*
Array for Turnouts on Page 4
Modify to suit your preferences:-
(Orientation (LH/RH), Address, State (0=CLOSED, 1=THROWN))
*/
int TurnOuts4[25][3] =
{ 
  {RH,26,Closed},
  {RH,27,Closed},
  {RH,28,Closed},
  {RH,29,Closed},
  {RH,30,Closed},
  {RH,31,Closed},
  {RH,32,Closed},
  {RH,33,Closed},
  {RH,34,Closed},
  {RH,35,Closed},
  {RH,36,Closed},
  {RH,37,Closed},
  {RH,38,Closed},
  {RH,39,Closed},
  {RH,40,Closed},
  {RH,41,Closed},
  {RH,42,Closed},
  {RH,43,Closed},
  {RH,44,Closed},
  {RH,45,Closed},
  {RH,46,Closed},
  {RH,47,Closed},
  {RH,48,Closed},
  {RH,49,Closed},
  {RH,50,Closed}
};
#define ReverseThreshold 15   //value can be between 0 and 127 - nkh addition
#define PowerUpState ("ON")       //change to ON or OFF as preferred required
#define fGroup4 222
#define fGroup5 223
#define incAmount 5   // change this to your preferred acceration rate of the Rotary Encoder

#include "Arduino.h"
#include <SoftwareSerial.h>
#include <Nextion.h>
#include <EEPROM.h>
SoftwareSerial nextion(4, 5);// Nextion TX to pin 4 and RX to pin 5 of Arduino

Nextion myNextion(nextion, 57600); //create a Nextion object named myNextion using the nextion serial port @ 9600bps
String message;
int ActiveAddress = 0; // make address1 active
int counter = 0;
char key ;
unsigned long lastTime;
unsigned long elapsedTime;
unsigned long DCCtime;
unsigned long DCCcurrentTime;
const int buttonPin = 8;     // the number of the pushbutton pin on encoder
const int ledPin =  13;      // the number of the LED pin
boolean encA;
boolean encB;
boolean lastA = false;
unsigned long number = 0;
unsigned int lowest = 0;
unsigned int highest = 126; //126;
byte Fx = 0;
int maxLocos = 3;// number of loco addresses
byte LocoDirection[3] = {1, 1, 1};
int LocoSpeed[3] = {0, 0, 0};
byte LocoFN0to4[3] = {128, 128, 128};
byte LocoFN5to8[3] = {176, 176, 176};
byte LocoFN9to12[3] = {160, 160, 160};
byte LocoFN13to20[3] = {0, 0, 0};
byte LocoFN21to28[3] = {0, 0, 0};
char VersionNum[] = "5.00";
boolean DCCflag = 0;
boolean ledPin_state;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long currentMillis = millis();
const long interval = 300;
int saveAddress = 0;
int LocoAddress[4] = {000, 1830, 3, 456};
int FNbutton = 0;
volatile byte aFlag = 0;
volatile byte bFlag = 0;
volatile byte encoderPos = 0;
volatile byte oldEncPos = 0;
volatile byte reading = 0;
int old_pos = 0;  //encoderPos;
boolean dir = 0; // direction
boolean buttonState = 0;
unsigned long interruptTime;
boolean FwdRev = 0; // 0 for backwards, 1 for forwards
static int pinA = 2; // Our first hardware interrupt pin is digital pin 2
static int pinB = 3; // Our second hardware interrupt pin is digital pin 3
boolean directionFlag = 0;

String changeButton = "";
int fGroup = 1;
int nextionPage = 0;      //Current Page displayed on Nextion
boolean PowerState = 1;
boolean Processed = 0;
int TOAddress = 0;
int TOSubAddress = 0;
boolean TOState = 0;
boolean REFlag = 0;

/*
 * Array of Turnouts
 * 25 Turnout slots on Page 3 numbered 1 to 25
 * TurnOut(Slot, Orientation Pic, Address, State)
 */

void setup() 
{
  pinMode(ledPin, OUTPUT);
  pinMode(pinA, INPUT_PULLUP); // set pinA as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(pinB, INPUT_PULLUP); // set pinB as an input, pulled HIGH to the logic voltage (5V or 3.3V for most cases)
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(0, PinA, RISING); //interrupt on PinA, rising edge signal & executing the "PinA" Interrupt Service Routine (below)
  attachInterrupt(1, PinB, RISING); // same for B
  pinMode(ledPin, OUTPUT);
  Serial.begin (115200);
  Serial.print("02-15-2018  version ");
  for (int i = 0; i < 4; i++) 
  {
    Serial.print(VersionNum[i]);
  }
  Serial.println("");
  Serial.println("<0>");// power off to DCC++ unit for now
  digitalWrite(ledPin, HIGH);           // Turn the LED on.
  ledPin_state = digitalRead(ledPin);   // Store initial LED state. HIGH when LED is on.
  myNextion.init(); // send the initialization commands for Page 0
  myNextion.sendCommand("page 0" );
  nextionPage = 0;
  myNextion.setComponentText("Version", "5.00");
  delay(2000); //show title screen for minimum of 2 seconds
  myNextion.sendCommand("page 2" );
  nextionPage = 2;
  dir = 1; //forward
  getAddresses(); // read from EEPROM
  updateDCCAddresses();
  setActiveAddress();
  if (PowerUpState == "ON")
  {
    Serial.print("<1>");
    PowerState = 1;
  }
  else
  {
    Serial.print("<0>");
    PowerState = 0;
  }
  setActivePowerState();
} 

// END SETUP

void loop() 
{
  encoderInterrupt(); // check rotary encoder
  DCCcurrentTime = millis(); //send DCC every 30000 ms
  if (DCCcurrentTime - DCCtime >= 30000 | DCCflag == 1) 
  {
    DCCflag = 0;
    DCCtime = DCCcurrentTime;
    doDCC();
  }
  checkButton();  // check for change in direction
  processNextionPage();  // check for DCC address change, page 2 button pressed or Accessory activity
}   

// *** END LOOP ***

void encoderInterrupt() 
{
  if (oldEncPos != encoderPos) 
  {    
    oldEncPos = encoderPos;
    myNextion.setComponentValue("throttle", encoderPos);
    myNextion.setComponentValue("throttleNum", encoderPos);
    LocoSpeed[ActiveAddress] = encoderPos;
    doDCC();
  }
}

void checkButton() 
{  // Checks button on encoder to change direction
  buttonState = digitalRead(buttonPin);
  directionFlag = 0;
  currentMillis = millis();
  if (buttonState == LOW) 
  {
    do 
    {
      if (millis() - currentMillis > 200) 
      {
      }
      buttonState = digitalRead(buttonPin);
      if (millis() - currentMillis >= 1000) 
      {
        LocoSpeed[ActiveAddress] = 0;
        encoderPos = 0;
        oldEncPos = 0;
        directionFlag = 1;
      }
    }
    while (buttonState == LOW);
    if (directionFlag == 0) 
    {
      directionFlag = 0;
      dir = !dir;
      if (dir) 
      {
        digitalWrite(ledPin, HIGH);
        if (encoderPos >= ReverseThreshold)
        {
          LocoSpeed[ActiveAddress] = 0;
          encoderPos = 0;
          oldEncPos = 0;
        }
        makeFWD();
      }
      else 
      {
        digitalWrite(ledPin, LOW);
        if (encoderPos >= ReverseThreshold)
        {
          LocoSpeed[ActiveAddress] = 0;
          encoderPos = 0;
          oldEncPos = 0;
        }
        makeBKW();
      }
      currentMillis = millis();
      LocoDirection[ActiveAddress] = dir;
      doDCC();
    }
  }
}

void processNextionPage() 
{
  message = myNextion.listen(); //check for message
  if (message != "") 
  {
    if (nextionPage == 2)
    {
      char mostSignificantDigit = message.charAt(5);    // for function numbers
      char throttle = message.charAt(4);
      String myString;
      myString = mostSignificantDigit;
      FNbutton = myString.toInt();
      if (message.startsWith("65 2"))                                   //These are all Function Button data streams
      {
        if (myString == "a") FNbutton = 9;
        if (myString == "b") FNbutton = 0;
        if (FNbutton >= 0 && FNbutton <= 9)
        {
          doFunction();               //Process Function Buttons 
        }
      }
      if (message.startsWith("da"))                                     //Address Change Button Presssed - Move to Page 1
      {
        getLocoAddress();  // get new DCC address #
      }
      if (message.startsWith("dg1"))                                   //Functions 1 to 8 selected
      {
        fGroup = 1;
        setActiveFunctionGroup();
      }
      if (message.startsWith("dg2"))                                   //Functions 9 to 16 selected
      {
        fGroup = 2;
        setActiveFunctionGroup();
      }
      if (message.startsWith("dg3"))                                   //Functions 17 to 24 selected
      {
        fGroup = 3;
        setActiveFunctionGroup();
      }
      if (message.startsWith("df"))                                   //direction forward button
      {
        digitalWrite(ledPin, HIGH);
        dir = 1;
        LocoDirection[ActiveAddress] = dir;
        if (encoderPos >= ReverseThreshold)
        {
          LocoSpeed[ActiveAddress] = 0;
          encoderPos = 0;
          oldEncPos = 0;
        }
        makeFWD();
        doDCC();
      }
      if (message.startsWith("dr"))                                   //direction reverse button
      {
        digitalWrite(ledPin, LOW);
        dir = 0;
        LocoDirection[ActiveAddress] = dir;
        if (encoderPos >= ReverseThreshold)
        {
          LocoSpeed[ActiveAddress] = 0;
          encoderPos = 0;
          oldEncPos = 0;
        }
        makeBKW();
        doDCC();
      }
      if (message.startsWith("dt"))                                   //throttle slider action
      {
        encoderPos = throttle;
        oldEncPos = encoderPos;
        LocoSpeed[ActiveAddress] = encoderPos;
        doDCC();
      }
      if (message.startsWith("du"))                                   //increase throttle value
      {
        encoderPos = encoderPos+1;
        if (encoderPos > highest) encoderPos = 126;
        LocoSpeed[ActiveAddress] = encoderPos;
      }
      if (message.startsWith("dd"))                                   //decrease throttle value
      {
        if (encoderPos == 0) encoderPos = 1;
        encoderPos = encoderPos-1;
        LocoSpeed[ActiveAddress] = encoderPos;
      }
      if (message.startsWith("AllStop"))                              // STOP button page 2 - Stop all locos, but leave power on
      {
        Serial.println("STOP STOP STOP");
        LocoSpeed[ActiveAddress] = 0;
        encoderPos = 0;     //encoderPos only applies to active address
        oldEncPos = 0;      //oldEncPos only applies to active address
        myNextion.setComponentValue("throttle", 0);
        myNextion.setComponentValue("throttleNum", 0);
      }
      if (message.startsWith("l1"))                                   // Address of Loco1 pressed
      {
        encoderPos = LocoSpeed[0];
        oldEncPos = encoderPos;
        updateDCC1();
      }
      if (message.startsWith("l2"))                                   // Address of Loco2 pressed
      { 
        encoderPos = LocoSpeed[1];
        oldEncPos = encoderPos;
        updateDCC2();
      }
      if (message.startsWith("l3"))                                   // Address of Loco3 pressed
      {
        encoderPos = LocoSpeed[2];
        oldEncPos = encoderPos;
        updateDCC3();
      }
      if (message.startsWith("don"))                                   //Power "On" button pressed
      {
        Serial.print("<1>");
        myNextion.sendCommand("don.picc=4");
        myNextion.sendCommand("doff.picc=3");
        PowerState = 1;
      }
      if (message.startsWith("doff"))                                  //Power "Off" button pressed
      {
        Serial.print("<0>");
        myNextion.sendCommand("don.picc=3");
        myNextion.sendCommand("doff.picc=4");
        PowerState = 0;
      }
      if (message.startsWith("acc"))
      {
        nextionPage = 3;
        myNextion.sendCommand("page 3");
        initNextionPage3();
        Processed = 1;
      }
    }
    if(Processed==0)
    {
      if (nextionPage == 3)                                               // Busy with Turnouts on Page 3
      {
        uint8_t turnoutSlot = message.toInt();
        turnoutSlot = (turnoutSlot - 1);
        if(message.startsWith("done3"))
        {
          Processed = 1;
          nextionPage = 2;
          myNextion.sendCommand("page 2");
          updateButtons();
          setActivePowerState();
          setActiveFunctionGroup();
          setActiveAddress();       
          updateDCCAddresses();                
        }
        if(message.startsWith("routes"))
        {
          Processed = 1;
          nextionPage = 5;
          myNextion.sendCommand("page 5");
        }
        if(message.startsWith("acc2"))
        {
          Processed = 1;
          nextionPage = 4;
          myNextion.sendCommand("page 4");
          initNextionPage4();
        }
        if(Processed == 0)
        {
          TOAddress = int(((TurnOuts3[turnoutSlot][1]) - 1) / 4) + 1;
          TOSubAddress = ((TurnOuts3[turnoutSlot][1]) - 1) % 4;
          TOState = TurnOuts3[turnoutSlot][2];
          TOState = !TOState;
          TurnOuts3[turnoutSlot][2] = TOState;
          int PIC = (TurnOuts3[turnoutSlot][0] + TOState);
          String(nextionCommand1) = ("TO" + String(turnoutSlot+1));
          String(nextionCommand2) = (".picc=" + String(PIC));
          String(nextionCommand) = (nextionCommand1 + nextionCommand2);
          myNextion.sendCommand(nextionCommand.c_str());
          doDCCTO();
        }
      }
    }
    if(Processed == 0)
    {
      if (nextionPage == 4)
      {
        uint8_t turnoutSlot = 0;
        turnoutSlot = message.toInt();
        turnoutSlot = (turnoutSlot - 1);
        if(message.startsWith("done"))
        {
          Processed = 1;
          nextionPage = 2;
          myNextion.sendCommand("page 2");
          updateButtons();
          setActivePowerState();
          setActiveFunctionGroup();
          setActiveAddress();       
          updateDCCAddresses();                
        }
        if(message.startsWith("routes"))
        {
          Processed = 1;
          nextionPage = 5;
          myNextion.sendCommand("page 5");
        }
        if(message.startsWith("acc1"))
        {
          Processed = 1;
          nextionPage = 3;
          myNextion.sendCommand("page 3");
          initNextionPage3();
        }
        if(Processed == 0)
        {
          TOAddress = int(((TurnOuts4[turnoutSlot][1]) - 1) / 4) + 1;
          TOSubAddress = ((TurnOuts4[turnoutSlot][1]) - 1) % 4;
          TOState = TurnOuts4[turnoutSlot][2];
          TOState = !TOState;
          TurnOuts4[turnoutSlot][2] = TOState;
          int PIC = (TurnOuts4[turnoutSlot][0] + TOState);
          String(nextionCommand1) = ("TO" + String(turnoutSlot+1));
          String(nextionCommand2) = (".picc=" + String(PIC));
          String(nextionCommand) = (nextionCommand1 + nextionCommand2);
          myNextion.sendCommand(nextionCommand.c_str());
          doDCCTO();
        }
      }
    }
    if(Processed==0)
    {
      if (nextionPage == 5)
      {
        if(message.startsWith("done5"))
        {
          Processed = 1;  
          nextionPage = 2;
          myNextion.sendCommand("page 2");
          updateButtons();
          setActivePowerState();
          setActiveFunctionGroup();
          setActiveAddress();       
          updateDCCAddresses();                
        }
        if(message.startsWith("acc1"))
        {
          Processed = 1;  
          nextionPage = 3;
          myNextion.sendCommand("page 3");
          initNextionPage3();
        }
        if(message.startsWith("acc2"))
        {
          Processed = 1;  
          nextionPage = 4;
          myNextion.sendCommand("page 4");
          initNextionPage4();
        }
        if(Processed == 0)
        {
          Serial.print(message);
        }
      }
    }
    Processed = 0;
  }
}
//START DO FUNCTION BUTTONS

int doFunction()
{
  key = FNbutton-1;               // already 0 - 9?  changes to -1 to 8 
/*
 * First Process F0 for all Groups
*/
  if (key == 8)
  {
    if (bitRead(LocoFN0to4[ActiveAddress], 4) == 0)
    {
      bitWrite(LocoFN0to4[ActiveAddress], 4, 1); 
    }
    else
    {
      bitWrite(LocoFN0to4[ActiveAddress], 4, 0);
    }
    doDCCfunction04();
  }
  switch (fGroup)
  {
    case 1:
      if (key >= 0 && key <= 3) 
      {
        if (bitRead(LocoFN0to4[ActiveAddress], key) == 0 ) 
        {
          bitWrite(LocoFN0to4[ActiveAddress], key, 1);
        }
        else 
        {
          bitWrite(LocoFN0to4[ActiveAddress], key, 0);
        }
        doDCCfunction04();
      }
      if (key >= 4 && key <= 7) 
      {
        key = key - 4;
        if (bitRead(LocoFN5to8[ActiveAddress], key) == 0 ) 
        {
          bitWrite(LocoFN5to8[ActiveAddress], key, 1);
        }
        else 
        {
          bitWrite(LocoFN5to8[ActiveAddress], key, 0);
        }
        doDCCfunction58();
      }
      if (key == -1)
      {
        key = 0;
        LocoFN0to4[ActiveAddress] = B10000000; //clear variables for which functions are set
        LocoFN5to8[ActiveAddress] = B10110000;
        LocoFN9to12[ActiveAddress] = B10100000;
        LocoFN13to20[ActiveAddress] = B00000000;
        LocoFN21to28[ActiveAddress] = B00000000;
        doDCCfunction04();
        doDCCfunction58();
        doDCCfunction912();
        doDCCfunction1320();
        doDCCfunction2128();
      }
      break;
    case 2:
      if (key >=0 && key <=3)
      {
        if (bitRead(LocoFN9to12[ActiveAddress], key) == 0 )
        {
          bitWrite(LocoFN9to12[ActiveAddress], key, 1);
        }
        else 
        {
          bitWrite(LocoFN9to12[ActiveAddress], key, 0);
        }
        doDCCfunction912();
      }
      if (key >= 4 && key <= 7) 
      {
        key = key - 4;
        if (bitRead(LocoFN13to20[ActiveAddress], key) == 0 ) 
        {
          bitWrite(LocoFN13to20[ActiveAddress], key, 1);
        }
        else 
        {
          bitWrite(LocoFN13to20[ActiveAddress], key, 0);
        }
        doDCCfunction1320();
      }
      if (key == -1)
      {
        key = 0;
        LocoFN9to12[ActiveAddress] = B10100000;
        LocoFN13to20[ActiveAddress] = LocoFN13to20[ActiveAddress] & B11110000;
        doDCCfunction912();
        doDCCfunction1320();
      }
      key = 0;
      break;
    case 3:
      if (key >=0 && key <=3)
      {
        key = key + 4;
        if (bitRead(LocoFN13to20[ActiveAddress], key) == 0 )
        {
          bitWrite(LocoFN13to20[ActiveAddress], key, 1);
        }
        else 
        {
          bitWrite(LocoFN13to20[ActiveAddress], key, 0);
        }
        key = key - 4;
        doDCCfunction1320();
      }
      if (key >= 4 && key <= 7) 
      {
        key = key - 4;
        if (bitRead(LocoFN21to28[ActiveAddress], key) == 0 ) 
        {
          bitWrite(LocoFN21to28[ActiveAddress], key, 1);
        }
        else 
        {
          bitWrite(LocoFN21to28[ActiveAddress], key, 0);
        }
        doDCCfunction2128();
      }
      if (key == -1)
      {
        key = 0;
        LocoFN13to20[ActiveAddress] = LocoFN13to20[ActiveAddress] & B00001111;
        LocoFN21to28[ActiveAddress] = LocoFN21to28[ActiveAddress] & B11110000;
        doDCCfunction1320();
        doDCCfunction2128();
      }
      key = 0;
      break;
    }
    updateButtons();                                                                   
    setActiveAddress();
}

void updateButtons()
{
  // Update F0 to F4 Nextion Buttons

  uint8_t i = 0;
  uint8_t fkey = 49;

  if (bitRead(LocoFN0to4[ActiveAddress],4) == 0) 
  {
    myNextion.sendCommand("f0.picc=3");
  }  
  else
  {
    myNextion.sendCommand("f0.picc=4");
  }
  switch (fGroup)
  {
    case 1:
      for(i=0 ; i<4 ; i++)
      {
        if (bitRead(LocoFN0to4[ActiveAddress],i) == 0) 
        {
          changeButton = "f" + String(char(fkey)) + ".picc=3";
        }  
        else
        {
          changeButton = "f" + String(char(fkey)) + ".picc=4";
        }
        myNextion.sendCommand(changeButton.c_str());
        fkey = fkey+1;
      }
      for(i=0 ; i<4 ; i++)
      {
        if (bitRead(LocoFN5to8[ActiveAddress],i) == 0) 
        {
          changeButton = "f" + String(char(fkey)) + ".picc=3";
        }
        else
        {
          changeButton = "f" + String(char(fkey)) + ".picc=4";
        }
        myNextion.sendCommand(changeButton.c_str());
        fkey = fkey+1;
      }
      break; 
    case 2:
      for(i=0 ; i<4 ; i++)
      {
        if (bitRead(LocoFN9to12[ActiveAddress],i) == 0)
        {
          changeButton = "f" + String(char(fkey)) + ".picc=5";
        }
        else
        {
          changeButton = "f" + String(char(fkey)) + ".picc=6";
        }
        myNextion.sendCommand(changeButton.c_str());
        fkey = fkey+1;
      }
      for(i=0 ; i<4 ; i++)
      {
        if (bitRead(LocoFN13to20[ActiveAddress],i) == 0)
        {
          changeButton = "f" + String(char(fkey)) + ".picc=5";
        }
        else
        {
          changeButton = "f" + String(char(fkey)) + ".picc=6";
        }
        myNextion.sendCommand(changeButton.c_str());
        fkey = fkey+1;
      }
      break;
    case 3:
      for(i=4 ; i<8 ; i++)
      {
        if (bitRead(LocoFN13to20[ActiveAddress],i) == 0)
        {
          changeButton = "f" + String(char(fkey)) + ".picc=7";
        }
        else
        {
          changeButton = "f" + String(char(fkey)) + ".picc=8";
        }
        myNextion.sendCommand(changeButton.c_str());
        fkey = fkey+1;
      }
      for(i=0 ; i<4 ; i++)
      {
        if (bitRead(LocoFN21to28[ActiveAddress],i) == 0)
        {
          changeButton = "f" + String(char(fkey)) + ".picc=7";
        }
        else
        {
          changeButton = "f" + String(char(fkey)) + ".picc=8";
        }
        myNextion.sendCommand(changeButton.c_str());
        fkey = fkey+1;
      }
    break;
  }
}

void setActiveAddress()
{
  if (ActiveAddress==0)
  {
    myNextion.sendCommand("l0.picc=4");
    myNextion.sendCommand("l1.picc=3");
    myNextion.sendCommand("l2.picc=3");
  }
  if (ActiveAddress==1)
  {
    myNextion.sendCommand("l0.picc=3");
    myNextion.sendCommand("l1.picc=4");
    myNextion.sendCommand("l2.picc=3");
  }
  if (ActiveAddress==2)
  {
    myNextion.sendCommand("l0.picc=3");
    myNextion.sendCommand("l1.picc=3");
    myNextion.sendCommand("l2.picc=4");
  }
}

void setActiveFunctionGroup()
{
  switch (fGroup)
  {
    case 1:
      myNextion.sendCommand("fa.picc=4");
      myNextion.sendCommand("fb.picc=3");
      myNextion.sendCommand("fc.picc=3");
      break;
    case 2:
      myNextion.sendCommand("fa.picc=3");
      myNextion.sendCommand("fb.picc=4");
      myNextion.sendCommand("fc.picc=3");
      break;
    case 3:
      myNextion.sendCommand("fa.picc=3");
      myNextion.sendCommand("fb.picc=3");
      myNextion.sendCommand("fc.picc=4");
  }
  updateButtons();
}

void doDCC() 
{
  Serial.print("<t ");
  Serial.print(ActiveAddress + 1);
  Serial.print(" ");
  Serial.print(LocoAddress[ActiveAddress] );//locoID);
  Serial.print(" ");
  Serial.print(LocoSpeed[ActiveAddress] );
  Serial.print(" ");
  Serial.print(LocoDirection[ActiveAddress] );
  Serial.println(">");
  number = LocoSpeed[ActiveAddress];
  if (dir) 
  {
    makeFWD();
  }
  else 
  {
    makeBKW();
  }
}

void doDCCfunction04() 
{
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  Serial.print(LocoFN0to4[ActiveAddress]);
  Serial.print(" >");
}
void doDCCfunction58() 
{
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  Serial.print(LocoFN5to8[ActiveAddress]);
  Serial.print(" >");
}
void doDCCfunction912()          
{
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  Serial.print(LocoFN9to12[ActiveAddress]);
  Serial.print(" >");
}
void doDCCfunction1320()         
{
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress] );
  Serial.print(" ");
  Serial.print(fGroup4);
  Serial.print(" ");
  Serial.print(LocoFN13to20[ActiveAddress]);
  Serial.print(" >");
}
void doDCCfunction2128()         
{
  Serial.write("<f ");
  Serial.print(LocoAddress[ActiveAddress]);
  Serial.print(" ");
  Serial.print(fGroup5);
  Serial.print(" ");
  Serial.print(LocoFN13to20[ActiveAddress]);
  Serial.print(" >");
}

// <a addr subaddr 1|0
void doDCCTO()            //Send DCC TurnOut command
{
  Serial.write("<a ");
  Serial.print(TOAddress);
  Serial.print(" ");
  Serial.print(TOSubAddress);
  Serial.print(" ");
  Serial.print(TOState); 
  Serial.println(">"); 
}

void getLocoAddress() 
{
  myNextion.sendCommand("page 1" );
  nextionPage = 1;
  myNextion.setComponentValue("AddrNew", ActiveAddress + 1);
  myNextion.setComponentValue("DCCnew", 0);
  saveAddress =   LocoAddress[ActiveAddress];
  int total = 0;
  counter = 0;
  do 
  {
    do 
    {
      message = myNextion.listen(); //check for message
      if (message != "") 
      {
      }
    }
    while (message == "");
    getNumber();
    if (key == 98 ) 
    { //"done" button hit - less than 4 digits
      break;// exit routine if # button pressed - ABORT new address
    }
    if (key == 99) 
    { //"abort" button hit
      break;
    }
    counter++;
    int number =  key;
    total = total * 10 + number;
    if (total == 0) 
    {   // print multiple zeros for leading zero number
      for (int tempx = 1; tempx <= counter; tempx++) 
      {
      }
    }
    myNextion.setComponentValue("DCCnew", total);
  }
  while (counter <= 3); //  collect exactly 4 digits
  LocoAddress[ActiveAddress] = total;
  if (key != 99)
  {
    LocoFN0to4[ActiveAddress] = B10000000; //clear variables for which functions are set    //nkh mod
    LocoFN5to8[ActiveAddress] = B10110000;                                                  //nkh mod
    LocoSpeed[ActiveAddress] == 0;
    encoderPos = 0;
    dir = 1;
    total = 0;
    counter = 0;
  }
  myNextion.sendCommand("page 2" );
  nextionPage = 2;
  if (key == 99)    //abort
  {
    LocoAddress[ActiveAddress] = saveAddress;
  }

  switch (ActiveAddress) 
  {
    case 0:
      updateDCC1();
      break;
    case 1:
      updateDCC2();
      break;
    default:
      updateDCC3();
  }
  saveAddresses();
  updateButtons();                                                                        //nkh mod
  setActivePowerState();
  setActiveFunctionGroup();
  setActiveAddress();                                                                     //nkh mod
}

void getAddresses() 
{  // from EEPROM
  int xxx = 0;
  for (int xyz = 0; xyz <= maxLocos - 1; xyz++) 
  {
    LocoAddress[xyz] = EEPROM.read(xyz * 2) * 256;
    LocoAddress[xyz] = LocoAddress[xyz] + EEPROM.read(xyz * 2 + 1);
    if (LocoAddress[xyz] >= 10000) LocoAddress[xyz] = 3;
  }
  maxLocos = EEPROM.read(20);
  if (maxLocos >= 4) maxLocos = 4;
}

void setActivePowerState()
{
  if (PowerState == 1)
  {
    myNextion.sendCommand("don.picc=4");
    myNextion.sendCommand("doff.picc=3");
  }
  else
  {
    myNextion.sendCommand("don.picc=3");
    myNextion.sendCommand("doff.picc=4");
  }
}

void saveAddresses()     // TO EEPROM
{
  int xxx = 0;
  for (int xyz = 0; xyz <= maxLocos ; xyz++) 
  {
    xxx = LocoAddress[xyz] / 256;
    EEPROM.write(xyz * 2, xxx);
    xxx = LocoAddress[xyz] - (xxx * 256);
    EEPROM.write(xyz * 2 + 1, xxx);
  }
  EEPROM.write(20, maxLocos);
}

// FROM NEXTION CODE
void getNumber() 
{   // note that the 2nd digit (after the 65) was changed from page 0 to page 1
  if (message.startsWith("65 1 a")) 
  {
    key = 0;
  }
  if (message.startsWith("65 1 1")) 
  {
    key = 1;
  }
  if (message.startsWith("65 1 2")) 
  {
    key = 2;
  }
  if (message.startsWith("65 1 3")) 
  {
    key = 3;
  }
  if (message.startsWith("65 1 4")) 
  {
    key = 4;
  }
  if (message.startsWith("65 1 5")) 
  {
    key = 5;
  }
  if (message.startsWith("65 1 6")) 
  {
    key = 6;
  }
  if (message.startsWith("65 1 7")) 
  {
    key = 7;
  }
  if (message.startsWith("65 1 8")) 
  {
    key = 8;
  }
  if (message.startsWith("65 1 9")) 
  {
    key = 9;
  }
  if (message.startsWith("65 1 b")) 
  {   //done
    key = 98;
  }
  if (message.startsWith("65 1 c")) 
  {   //abort
    key = 99;
  }
}

void makeFWD() 
{
  myNextion.sendCommand("forward.picc=4");
  myNextion.sendCommand("reverse.picc=3");
  myNextion.setComponentValue("throttle", encoderPos);
  myNextion.setComponentValue("throttleNum", encoderPos);
}

void makeBKW() 
{
  myNextion.sendCommand("forward.picc=3");
  myNextion.sendCommand("reverse.picc=4");
  myNextion.setComponentValue("throttle", encoderPos);
  myNextion.setComponentValue("throttleNum", encoderPos);
}

void updateDCCAddresses() 
{
  myNextion.setComponentText("l0", String(LocoAddress[0]));
  myNextion.setComponentText("l1", String(LocoAddress[1]));
  myNextion.setComponentText("l2", String(LocoAddress[2]));
  DCCflag = 1;
}

void updateDCC1() 
{
  ActiveAddress = 0;
  number =  LocoSpeed[ActiveAddress];
  dir = LocoDirection[ActiveAddress];
  updateButtons();
  setActiveAddress();
  updateDCCAddresses();
}

void updateDCC2() 
{
  ActiveAddress = 1;
  number =  LocoSpeed[ActiveAddress];
  dir = LocoDirection[ActiveAddress];
  updateButtons();
  setActiveAddress();
  updateDCCAddresses();
}

void updateDCC3() 
{
  ActiveAddress = 2;
  number =  LocoSpeed[ActiveAddress];
  dir = LocoDirection[ActiveAddress];
  updateButtons();
  setActiveAddress();
  updateDCCAddresses();
}

void PinA() 
{
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && aFlag) 
  { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    if (encoderPos >= 1) 
    {
      elapsedTime = (millis() - lastTime);
      encoderPos --;
      if (elapsedTime <= 100)
      {
        if (encoderPos >= incAmount)
        {
          encoderPos = (encoderPos - (incAmount - 1));    //decrement the encoder's position count
        }
      }
    }
    lastTime = millis();
    FwdRev = 0;
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void PinB() 
{
  cli(); //stop interrupts happening before we read pin values
  reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  if (reading == B00001100 && bFlag) //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
  { 
    if (encoderPos < highest) 
    {
      elapsedTime = (millis() - lastTime);
      encoderPos ++;      //increment the encoder's position count
      if (elapsedTime <= 100)
      {
        if (encoderPos <= (highest - incAmount))
        {
          encoderPos = (encoderPos + (incAmount - 1));
        }
      }
    } 
    lastTime = millis();
    FwdRev = 1;
    bFlag = 0; //reset flags for the next turn
    aFlag = 0; //reset flags for the next turn
  }
  else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  sei(); //restart interrupts
}

void initNextionPage3()
{
  int p;
  for(p=0; p<25; p++)
  {
    TOState = (TurnOuts3[p][2]);              //Closed or Thrown
    int TOAddr = (TurnOuts3[p][1]);           //Turnout Address
    int TOPic = (TurnOuts3[p][0] + TOState);  //RH or LH and include Closed or Thrown
    int slotNum = (p + 1);
    String(nextionCommand1) = ("TO" + String(slotNum));
    String(nextionCommand2) = (".picc=" + String(TOPic));
    String(nextionCommand) = (nextionCommand1 + nextionCommand2);
    myNextion.sendCommand(nextionCommand.c_str());
    nextionCommand1 = ("TA" + String(slotNum));
    myNextion.setComponentText(nextionCommand1,String(TOAddr));
    slotNum++;
  }
}
void initNextionPage4()
{
  int p;
  for(p=0; p<25; p++)
  {
    TOState = (TurnOuts4[p][2]);              //Closed or Thrown
    int TOAddr = (TurnOuts4[p][1]);           //Turnout Address
    int TOPic = (TurnOuts4[p][0] + TOState);  //RH or LH
    int slotNum = (p + 1);
    String(nextionCommand1) = ("TO" + String(slotNum));
    String(nextionCommand2) = (".picc=" + String(TOPic));
    String(nextionCommand) = (nextionCommand1 + nextionCommand2);
    myNextion.sendCommand(nextionCommand.c_str());
    nextionCommand1 = ("TA" + String(slotNum));
    myNextion.setComponentText(nextionCommand1,String(TOAddr));
    slotNum++;
  }
}

