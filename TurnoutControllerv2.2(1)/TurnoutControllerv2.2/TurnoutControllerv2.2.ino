/**********************************************************************
 * Written by Brendan Faherty. This will take the incoming turnout requests
 * and process them. A servo shield will be used and communicates via I2C
 *
 * Turnout Controller v2.0 - Original 
 * Turnout Controller v2.1 - January 11 2017 - Modifications to the parse() routine. JMRI DCC++ does not handle turnout commands as originally thought. Have to allow for "a (ADDR) (SUBA) (STATE)"
 *                                             commands to be processed for JMRI to operate properly. When setting up a turnout, use DIRECT with NoFeedback. JMRI for whatever reason calculates 
 *                                             the Address and SubAddress based on the Turnout#.
 *                                             
 *                                             Turnout#1 - Address 1 SubAddress 0
 *                                             Turnout#2 - Address 1 SubAddress 1
 *                                             Turnout#3 - Address 1 SubAddress 2
 *                                             Turnout#4 - Address 1 SubAddress 3
 *                                             Turnout#5 - Address 2 SubAddress 0  - Each Address only has 4 SubAddresses # 0 - 3
 *                                             Turnout#9 - Address 3 SubAddress 0
 *                                             Turnout#13- Address 4 SubAddress 0
 *                                             Etc...
 *                                             
 *                                             Also, JMRI will query a list of turnouts. It sends a <T> packet TWICE. It causes JMRI to populate the index list with duplicates, as well JMRI appears
 *                                             to ignore the list anyway. The <T> packet will now be ignored and return <X>. The function Turnoutshow() will be commented out to save space 
 *                                             
 * Turnout Controller v2.2 - January 12 2017 - Modifications are primarily code clean up and optimization. The storing of the turnoutstatus in the EEPROM has been added as well. 
 *                                             The EEPROM writes only occur and automatically occur when a turnout is thrown/closed. The recall of the EEPROM occurs in the setup() function.
 *                                             
 */

#include <Wire.h>                                             //Wire library is used to support and handle the I2C communications
#include <EEPROM.h>                                           //EEPROM library is used so we can store the turnoutstatus[] array

#define MAX_COMMAND_LENGTH 30                                 //Size of the buffer to hold serial data
#define SERVOMIN 150                                          //Minimum pulse width for 0 degrees movement    -These will have to be adjusted based on the amount of throw needed
#define SERVOMAX 400                                          //Maximum pulse width for 180 degrees movement
#define ARRAYSIZE 9                                           //Size of the array for the turnoutstatus array -(#Turnouts/8 rounded UP)
#define num_servo_boards 3                                   //How many 16 channel servo boards do we have. Each board controls 16 turnouts, so divide how many
                                                              //turnouts you have by 16 and round UP to the next whole number. I have 70 turnouts, so 70 / 16 = 4.375 so round up to 5.
                                                              //I have 2 there for my testing purposes.

static char commandString[MAX_COMMAND_LENGTH+1];              //Setting up an array to hold what passes through the serial port

unsigned int MAXTURNOUTS = (ARRAYSIZE * 8);                   //Our maximum number of turnouts allowed based on the size of our array

uint8_t SERVOADDRS[num_servo_boards]={0x40, 0x41};
                                                              //List of I2C addresses of the servo boards.The addresses are user configurable.
                                                              //Important info, this software will use the first address for the first 16 turnouts (1 - 16), and the second for the next 16
                                                              //(17 - 32) so on and so forth. So be sure to note which turnouts are connected to which board and output#. If the boards are                                                          
                                                              //STACKED on top of one another, that has no impact on any of this. The addresses are setting by creating a solder jumper on 
                                                              //certain pads on the board. You should refer to the datasheet about how the addresses are set.
                                                              
char c;                                                       //Serial data

unsigned int turnoutstatus[ARRAYSIZE];                        //Array for holding the turnout positions 8bytes x 8 bits = 64 turnouts
                                                              //This can easily be increased as necessary. Increase the size of the array
                                                              //to add more turnouts. Be sure to adjust the maximum count on lines 68,99,103 if
                                                              //increasing the size of the array.

uint8_t read_(uint8_t _i2caddr, uint8_t addr)                 //Routine to read info from one of the Servo driver boards.
{                                                             //Multiple Boards are used, so the address of the board and the address of
      Wire.beginTransmission(_i2caddr);                       //register to be read are passed to the function. It will return the contents
      Wire.write(addr);                                       //of the register.
      Wire.endTransmission();

      Wire.requestFrom((uint8_t)_i2caddr, (uint8_t)1);        //Requesting 1 BYTE of data
      
      return Wire.read();                                     //Return the data requested 
}

void write_(uint8_t _i2caddr, uint8_t addr, uint8_t d)        //Routine used to write to the various registers. Multiple boards are being used             
{                                                             //The address of the board to be addressed, plus the register to be updated, and the value
      Wire.beginTransmission(_i2caddr);                       //are passed to this. It returns no value.
      Wire.write(addr);
      Wire.write(d);
      Wire.endTransmission();
}

void setPWM(uint8_t _i2caddr, uint8_t num, uint16_t on, uint16_t off)
{                                                             //This routine is what makes the servo move. It updates the necessary registers to
      Wire.beginTransmission(_i2caddr);                       //give a smooth movement. 
      Wire.write(0x06+(4*num));
      Wire.write(on);
      Wire.write(on>>8);
      Wire.write(off);
      Wire.write(off>>8);
      Wire.endTransmission();
}

//void Turnout_show()                                           //This function will just pass along the status of each turnout
//{
//    int j,k,ID,sts;
//    
//    ID=1;
//
//    for (j=0; j<ARRAYSIZE; j++)                               //BYTE LOOP
//    {
//      for (k=0; k<8; k++)                                     //BIT LOOP
//      {
//            
//            sts = ((turnoutstatus[j]>>k) & 0x01);             //Using a BIT mask to check the status if each register.
//            
//            Serial.print("<H ");                              //JMRI reads this info in to generate a list of Turnouts. Sadly it doesn't use the list. So
//            Serial.print(ID);                                 //this is only here to satisfy it. The function is currently disabled in the parse() function and
//            Serial.print(" ");                                //isn't called. If the bug in JMRI is ever corrected, then it is simple to fix this.
//            Serial.print(ID);
//            Serial.print(" 0 ");
//            Serial.print(sts);
//            Serial.print(">");
//
//            ID++;
//            sts = 0;
//      }   
//    }
//}

void UpdateBuffer(uint8_t n, uint8_t s, uint8_t p)            //This function handles the updating of the buffer and sets the turnouts
{
    int x,z,temp;                                             //Some generic intergers for holding some values.
    uint8_t addr, servo;
    
    if ((n <= MAXTURNOUTS) && (n>0))                          //Check to make sure that the number passed to the routine is within our memory limits
    {                                                        
        temp = 0;
        
        x = (n-1)/8;                                          //Which byte are we in      <<The divisor on (8) THIS LINE ONLY should match the array size of turnoutstatus[]
        z = n - (8*x)-1;                                      //Which bit to shift into

        temp = (1<<z);                                        //Create a BIT mask

        addr = ((n-1)/16);                                    //Figure out based on the turnout number which board address we want.
        servo = ((n-1)%16);                                   //Calculate which servo number we want
        
        if (s == 0)                                           //De-activate the turnout
        {
            if((turnoutstatus[x] & temp)!=0)                  //Check to make sure the turnout is actually SET before resetting it. Resetting a turnout that isn't set, will end up setting it.
            {

              turnoutstatus[x] = (turnoutstatus[x] ^ temp);   //Update the BIT of the corresponding BYTE

              for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--)
              {
                  setPWM(SERVOADDRS[addr], servo, 0, pulselen);//This FOR loop calls the setPWM function that moves the servo
              }
                        
            }

            if (p == 1)                                       
            {  
                Serial.print("<H ");                          //Tell JMRI that the message was received and that the turnout is being deactivated
                Serial.print(n);
                Serial.print(" 0>");
            }
            
        }
        else if (s == 1)                                      //Activate/Throw the turnout
        {
            
            if((turnoutstatus[x] & temp) == 0)                //Make sure the turnout hasn't already been thrown
            {
              turnoutstatus[x] = (turnoutstatus[x] | temp);   //Update the BIT of the corresponding BYTE

              for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++)
              {
                   setPWM(SERVOADDRS[addr], servo, 0, pulselen);//This FOR loop calls the setPWM function that moves the servo
              }
            }

            if (p == 1)
            {
                Serial.print("<H ");
                Serial.print(n);                              //Tell JMRI that the message was received and that the turnout is being activated
                Serial.print(" 1>");
            }
            
            
        }
        else                                                  //Just incase some other value then 0 or 1 is passed we will do nothing.
        {
            if (p == 1) Serial.print("<X>");                
        }

        EEPROM.write(x, turnoutstatus[x]);                    //Update the EEPROM
    }

    else                                                      //In case a turnout ID greater than what we have allowed for is passed to the function
    {
      Serial.print("<X>");
    }

}

void Turnout_parse(char *c)                                   //This function takes the string received via our interface and figures out what was sent based on the format of the string
{
    int n,s,m,y;                                              //Some generic intergers for holding some values.
    uint8_t addr, servo;

    switch(sscanf(c,"%d %d %d",&n,&s,&m))                     //Scan the info to see how many numbers were placed. sscanf() returns how many were placed.
    {

        case 2:                                               //Argument is "T [ID] [THROW]" 1 - Throw 0 - Close
    
        UpdateBuffer(n, s, 1);                                //Pass the Turnout #, Throw/Close, and Enable a Response for JMRI
        break;

        case 3:                                               //Argument to handle "a [ADDRESS] [SUBADDRESS] [STATE]"
                                                              //With this argument, JMRI does not expect any responses back. It just assumes whatever was supposed to have
                                                              //happened actually does. 

        y = ((n-1)*4)+(s+1);                                  //Converting the Address and Subaddress into one number

        UpdateBuffer(y, m, 0);                                //Pass the Turnout #, Throw/Close, and No Response (0) to JMRI
        break;
    
        case 1:                                               //Old argument for deleting turnouts. No longer supported or needed. Just break from the switch/case
        break;
    
        case -1:                                              //No arguments, so display the status of each turnout
        //Turnout_show();           
        Serial.print("<X>");
        break;
    }
}

void setPWMFreq(uint8_t _i2caddr, float freq)                 //This routine sets the PWM frequency. We pass to it our desired frequency
{                                                             //and it will calculate the desired PRESCALER and update the register
  
    freq *= 0.9;                                              //Correct for overshoot in the frequency setting
    float prescaleval = 25000000;
    prescaleval /= 4096;                                      //This formula comes from the datasheet for the PCA9685 which is the
    prescaleval /= freq;                                      //chip on the Adafruit board.
    prescaleval -= 1;
  
    uint8_t prescale = floor(prescaleval + 0.5);              

    write_(_i2caddr, 0x00, 0x10);                             //Update the MODE1 register with this value so it goes to SLEEP
    write_(_i2caddr, 0xFE, prescale);                         //Set the PRESCALER with our new calculated value (can only be done while in SLEEP mode)

    write_(_i2caddr, 0x00, 0x00);                             //Re-start the oscillator
    delay(600);                                               //Datasheet says the oscillator needs 500uS to get going so delay for a little

    write_(_i2caddr, 0x00, 0xA0);                             //This sets the MODE1 register to turn on auto increment of the control register

}

void setup()                                                  //Get things initialized and configured
{

      char j;

      for(j = 0; j<=ARRAYSIZE; j++)                           //Load our turnoutstatus buffer from the EEPROM
      {
        turnoutstatus[j] = EEPROM.read(j);
      }

      Serial.begin(115200);                                   //Get the serial port up and running

      Wire.begin();                                           //Get the I2C bus up and running

      for(j = 0; j<=num_servo_boards; j++)
      {
        setPWMFreq(SERVOADDRS[j], 60);                        //Set the frequency of our PWM and get it up and running
      }
      
      yield();

}

void loop()
{

      while(Serial.available()>0)
      {                                                       //Process any serial data
        c=Serial.read();
        if(c=='<')                                            //Look for the start of a new command
          sprintf(commandString,"");
        else if(c=='>')                                       //End of new command received, send the command to the parsing routine to update the BIT
          Turnout_parse(commandString);                                          
        else if(strlen(commandString)<MAX_COMMAND_LENGTH)     //If comandString still has space, append character just read from serial line
          sprintf(commandString,"%s%c",commandString,c);      //otherwise, character is ignored (but continue to look for '<' or '>')
      }
      
}
