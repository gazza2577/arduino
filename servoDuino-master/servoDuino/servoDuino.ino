/*
 * USB-to-I2C servo control
 * (c) 2018 Peter Habermehl
 */
 
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);

#define CODENAME "servoDuino"
#define CODEVERS "1.1.1"
#define MAX_CMD 142

char rxCur = 0;
char rxBuf[MAX_CMD+1];

// servo min & max pwm, < 4096
int smin = 120;
int smax = 550;

// pwm frequency in Hz
int freq = 60;


// ---------------------------------------------------------

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(115200);  // seriellen USB-Port öffnen

  // warte auf PC-Kommunikation
  while(!Serial) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(450);
  }

  // init servo driver
  pwm1.begin();
  pwm1.setPWMFreq(freq); // 1600 is max.

  /* permanent sweep on servo 0
    while(true) {
    pwm1.setPWM(0, 0, 145);
    delay(1000);
    pwm1.setPWM(0, 0, 555);
    delay(1000); 
  } 
  */
  
  /*90 deg sweep for marble separator
  
    while(true) {
    pwm1.setPWM(8, 0, 600);
    delay(1000);
    pwm1.setPWM(4, 0, 170);
    delay(1000);
    pwm1.setPWM(8, 0, 170);
    delay(1000);
    pwm1.setPWM(4, 0, 370);
    delay(1000);
  }
  
   */
  
 }
 
// --------------------------------------------------------

void loop() {
  static unsigned char pwm = 0;
  int success;
  int n;
  
  // lies alle Bytes, die per USB/Serial empfangen wurden
  
  while(Serial.available()) {
    char c = Serial.read();

    // "return" beendet ein Kommando
    if((c == '\n') || (c == '\r')) {
      if(rxCur > 0) {
        rxBuf[rxCur] = '\0';

        // separiere bei erstem leerzeichen, wenn vorhanden
        char *parm = NULL;        
        if(strchr(rxBuf, ' ')) {
          parm = strchr(rxBuf, ' ')+1;
          *strchr(rxBuf, ' ') = '\0';
        }

        success=-1;
        
        // below commands with case sensitive parameters
        
        String cmd = String(rxBuf);
        cmd.toLowerCase();
             
        if((cmd == "led_set") && parm) {
          int c = atoi(parm);
          if(c == 0) {
            digitalWrite(LED_BUILTIN, LOW);
            success=1;
          }
          else {
            digitalWrite(LED_BUILTIN, HIGH);
            success=1;
          }
        }

        else if((cmd == "report_code")) {
          Serial.println(CODENAME);
          success=2;
        }
        
        else if((cmd == "report_version")) {
          Serial.println(CODEVERS);
          success=2;
        }

        // Servo - Funktionen

        else if(cmd == "pwm_halt") {
          for (int i=0; i<15; i++) {
             pwm1.setPWM(i, 4096, 0);
          }
          success=1;
        }

        else if((cmd == "pwm_min_set") && parm) {
          smin = min( max (0, atoi(parm)), 4095);
          Serial.println(smin);
          success=2;
        }
         
        else if((cmd == "pwm_max_set") && parm) {
          smax = min( max (0, atoi(parm)), 4095);
          Serial.println(smax);
          success=2;
        }

        else if((cmd == "pwm_freq_set") && parm) {
          freq = min( max (40, atoi(parm)), 1000);
          pwm1.setPWMFreq(freq);
          Serial.println(freq);
          success=2;
        }

        else if(cmd == "pwm_min_get") {
          Serial.println(smin);
          success=2;
        }
         
        else if(cmd == "pwm_max_get") {
          Serial.println(smax);
          success=2;
        }
        
        else if(cmd == "pwm_freq_get") {
          Serial.println(freq);
          success=2;
        }

        else if((cmd == "pwm_set") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
         
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
           
              pwm1.setPWM(max(min(atoi(parm),15),0),
                max(min(atoi(parm2),4096),0),
                max(min(atoi(parm3),4096),0) );
     
              success=1;
            }
          }
        }   

        else if((cmd == "servo_trans") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
          char *parm4 = NULL;
         
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
                if(strchr(parm3, ' ')) {
                  parm4 = strchr(parm3, ' ')+1;
                  *strchr(parm3, ' ') = '\0';          
                  //int c = pulselength( (min( max (0, atoi(parm)), 180)), smin, smax ); 
           
                  pwm1.setPWM(max(min(atoi(parm),15),0),
                    0,
                    pulselength( (min( max (0, atoi(parm4)), 180)),
                     (min( max (0, atoi(parm2)), 4096)),
                     (min( max (0, atoi(parm3)), 4096)) ) );
     
                  success=1;
              }
            }
          }
        }

        else if((cmd == "servo_set") && parm) {
          char *parm2 = NULL;
          
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0'; 
          
            int c = pulselength( (min( max (0, atoi(parm2)), 180)), smin, smax ); 
                         
            pwm1.setPWM(max(min(atoi(parm),15),0),
                0,
                c );
            
            Serial.println(c);
            success=2;
          }
        }
        
        else if((cmd == "servo_sweep") && parm) {
          char *parm2 = NULL;
          char *parm3 = NULL;
         
          if(strchr(parm, ' ')) {
            parm2 = strchr(parm, ' ')+1;
            *strchr(parm, ' ') = '\0';
            if(strchr(parm2, ' ')) {
              parm3 = strchr(parm2, ' ')+1;
              *strchr(parm2, ' ') = '\0';
          
              //int c = pulselength( (min( max (0, atoi(parm)), 180)), smin, smax ); 

              for(int i=atoi(parm2);i<=atoi(parm3);i++) {
              
                pwm1.setPWM(max(min(atoi(parm),15),0),
                  0,
                  i );
                delay(5);
              }
              for(int i=atoi(parm3);i>=atoi(parm2);i--) {
              
                pwm1.setPWM(max(min(atoi(parm),15),0),
                  0,
                  i );
                delay(5);
              }  
              success=1;
            }
          }
        }
        
        //
        // allgemeine I2C-Kommunikation
        //

        else if((cmd == "i2c_write") && parm) {
          char * ppt;

          ppt = strtok(parm," ");
          Wire.beginTransmission(atoi(ppt));
          ppt=strtok(NULL, " ");
                    
          while(ppt!=NULL) {
            // Serial.println(ppt);
            Wire.write(atoi(ppt));
            ppt=strtok(NULL, " ");      
          }
          Wire.endTransmission(); 
          success=1;       
        }        

        else if((cmd == "i2c_read") && parm) {
          char * ppt;
          uint8_t  i;
          uint8_t  j;
          
          ppt = strtok(parm," ");
          i=atoi(ppt);
          ppt=strtok(NULL, " ");
          j=atoi(ppt);
          
          Wire.requestFrom(i,j);
          delay(50);
          // Serial.println("I2C read:");
          while(Wire.available()) {
            uint8_t ans = Wire.read();
            Serial.print(ans);
            Serial.print(" ");
          }
          Serial.println();
          success=2;
        }

                 
        // Abschluss eines Durchlaufs
        
        if(success!=2) {
          Serial.println(success);
        }
                
        Serial.flush(); // warte auf Leeren des Schreibbuffers
        rxCur = 0;
      }
    } else if(rxCur < MAX_CMD) {
      rxBuf[rxCur++] = c;
    }
  }
} 

//
// some helpers
// 

int pulselength(int deg,  int servo_min, int servo_max) {
      return map(deg, 0, 180, servo_min, servo_max);
}
