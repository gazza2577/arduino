//Tutorial: https://www.electronoobs.com/eng_arduino_tut90.php
//Schematic: https://www.electronoobs.com/eng_arduino_tut90_sch1.php

//LCD config
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>      //http://www.electronoobs.com/eng_arduino_liq_crystal.php
LiquidCrystal_I2C lcd(0x27,20,4);  //sometimes the LCD adress is not 0x3f. Change to 0x27 if it dosn't work.


//inputs/outputs
int left = 2;
int right = 4;
int up = 5;
int down = 6;
int sel = 7;
int back = 8;
int pot = A0;
int pwm = 9;

///Variables
uint8_t retarrow[8] = {  
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000};
int menu_level = 0;
int menu_0_level = 0;
int menu_1_level = 0;
int menu_4_level = 0;
int menu_8_level = 0;
int menu_11_level = 0;
bool up_state = true;
bool down_state = true;
bool left_state = true;
bool right_state = true;
bool sel_state = true;
bool back_state = true;

int loop_counter = 0;
unsigned long previousMillis = 0; 

int deg_val = 0;
int width = 1000;
int width_min = 1000;
int width_max = 2000;
int frequency = 50;

int servo_freq = 20000;
int esc_freq = 20000; 

int calib_min = 1000;
int calib_max = 2000;


int esc_width = 1000;
int esc_width_min = 1000;
int esc_width_max = 2000;
int esc_frequency = 50;

void setup() {
  pinMode(left,INPUT_PULLUP);
  pinMode(right,INPUT_PULLUP);
  pinMode(up,INPUT_PULLUP);
  pinMode(down,INPUT_PULLUP);
  pinMode(sel,INPUT_PULLUP);
  pinMode(back,INPUT_PULLUP);
  pinMode(pot,INPUT);  
  pinMode(pwm, OUTPUT);
  
  lcd.begin();                 //Init the LCD
  lcd.backlight();            //Activate backl
  lcd.createChar(1, retarrow);
  
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.write(1);  
  lcd.print(" Servo control");
  lcd.setCursor(0,1);
  lcd.print("  ESC control");

  TCCR1A = 0;
  TCCR1A = (1 << COM1A1) | (1 << WGM11);  
  TCCR1B = 0;
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11); 
  ICR1 = 20000;   //Freq= 50Hz
  OCR1A = 0;      //PWM->OFF pulse width = 0;

}

void loop() {
  if(digitalRead(up))
  {    up_state = true;  }
  if(digitalRead(down))
  {    down_state = true;  }
  if(digitalRead(left))
  {    left_state = true;  }
  if(digitalRead(right))
  {    right_state = true;  }
  if(digitalRead(sel))
  {    sel_state = true;  }
  if(digitalRead(back))
  {    back_state = true;  }  


  if(!(menu_level == 2 || menu_level == 3 || menu_level == 9 || menu_level == 10 || menu_level == 17 ))
  {
    OCR1A = 0;      //PWM->OFF pulse width = 0;
    //digitalWrite(9,LOW);
  }
    
  
  ///////////////////////////////MENU 0////////////////////////////////
  if(menu_level == 0 && !digitalRead(up) && up_state == true)
  {    
    up_state = false;
    menu_0_level = menu_0_level - 1;
    if(menu_0_level < 0)
    {
      menu_0_level = 2;
    }
    if(menu_0_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Servo control");
      lcd.setCursor(0,1);
      lcd.print("  ESC control");
    }
    if(menu_0_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Servo control");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" ESC control");
    }
    if(menu_0_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  ESC control");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" ESC calibrate");
    }
  }

  if(menu_level == 0 && !digitalRead(down) && down_state == true)
  {    
    down_state = false;
    menu_0_level = menu_0_level + 1;
    if(menu_0_level > 2)
    {
      menu_0_level = 0;
    }
     if(menu_0_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Servo control");
      lcd.setCursor(0,1);
      lcd.print("  ESC control");
    }
    if(menu_0_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Servo control");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" ESC control");
    }
    if(menu_0_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  ESC control");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" ESC calibrate");
    }
  }
  ///////////////////////////////MENU 0////////////////////////////////






  ///////////////////////////////MENU CHANGE////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  if(!digitalRead(back) && back_state == true)
  {
    back_state = false;
    if(menu_level == 1 || menu_level == 8 || menu_level == 15)
    {
      menu_level = 0;
      menu_0_level = 0; 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Servo control");
      lcd.setCursor(0,1);
      lcd.print("  ESC control");
    }
    if(menu_level == 2 || menu_level == 3 || menu_level == 4)
    {
      menu_level = 1;
      menu_1_level = 0; 
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Servo pot");
      lcd.setCursor(0,1);
      lcd.print("  Servo fine");
    }
    if(menu_level == 1)
    {
      menu_level = 0;
      menu_0_level = 0; 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Servo control");
      lcd.setCursor(0,1);
      lcd.print("  ESC control");
    }
    if(menu_level == 5 || menu_level == 6 || menu_level == 7)
    {
      menu_level = 4;
      menu_4_level = 0;     
      lcd.clear();    
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Min");
      lcd.setCursor(0,1);
      lcd.print("  Max");
    }     
    if(menu_level == 9 || menu_level == 10 || menu_level == 11)
    {
      menu_level = 8;
      menu_8_level = 0; 
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" ESC pot");
      lcd.setCursor(0,1);
      lcd.print("  ESC fine");
    }
    if(menu_level == 12 || menu_level == 13 || menu_level == 14)
    {
      menu_level = 11;
      menu_11_level = 0;     
      lcd.clear();    
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Min");
      lcd.setCursor(0,1);
      lcd.print("  Max");
    }  
  }
  /////////////////////////////////////////////////////////////////////////////////


  

  
  /////////////////////////////////////////////////////////////////////////////////
  if(menu_level == 0 && menu_0_level == 0 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 1;    
    ICR1 = servo_freq;
    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.write(1);  
    lcd.print(" Servo pot");
    lcd.setCursor(0,1);
    lcd.print("  Servo fine");
  }
  if(menu_level == 0 && menu_0_level == 1 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 8;   
    ICR1 = esc_freq; 
    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.write(1);  
    lcd.print(" ESC pot");
    lcd.setCursor(0,1);
    lcd.print("  ESC fine");
  }
  if(menu_level == 0 && menu_0_level == 2 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 15;    
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Set min value:");
    lcd.setCursor(0,1);
    lcd.print(calib_min);
  }
  /////////////////////////////////////////////////////////////////////////////////


  
  /////////////////////////////////////////////////////////////////////////////////
  if(menu_level == 1 && menu_1_level == 0 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    previousMillis = millis();
    menu_level = 2;    
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Deg ");
    lcd.print(deg_val);    
  }
  if(menu_level == 1 && menu_1_level == 1 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    previousMillis = millis();
    menu_level = 3;    
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Deg ");
    lcd.print(deg_val);    
  }
  if(menu_level == 1 && menu_1_level == 2 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 4;    
    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.write(1);  
    lcd.print(" Min");
    lcd.setCursor(0,1);
    lcd.print("  Max");    
  }
  /////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////
  if(menu_level == 4 && menu_4_level == 0 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 5;    
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set min:");
    lcd.setCursor(0,1);
    lcd.print(width_min);    
  }
  if(menu_level == 4 && menu_4_level == 1 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 6;    
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set max:");
    lcd.setCursor(0,1);
    lcd.print(width_max);     
  }
  if(menu_level == 4 && menu_4_level == 2 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 7;    
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set frequency:");
    lcd.setCursor(0,1);
    lcd.print(frequency);     
  }
  /////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////
  if(menu_level == 8 && menu_8_level == 0 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    previousMillis = millis();
    menu_level = 9;    
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("us: ");
    lcd.print(esc_width);    
  }
  if(menu_level == 8 && menu_8_level == 1 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 10;   
    previousMillis = millis(); 
    esc_width = esc_width_min;
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("us: ");
    lcd.print(esc_width);    
  }
  if(menu_level == 8 && menu_8_level == 2 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 11;    
    lcd.clear();  
    lcd.setCursor(0,0);
    lcd.write(1);  
    lcd.print(" Min");
    lcd.setCursor(0,1);
    lcd.print("  Max");    
  }
  /////////////////////////////////////////////////////////////////////////////////




  /////////////////////////////////////////////////////////////////////////////////
  if(menu_level == 11 && menu_11_level == 0 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 12;    
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set min:");
    lcd.setCursor(0,1);
    lcd.print(width_min);    
  }
  if(menu_level == 11 && menu_11_level == 1 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 13;    
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set max:");
    lcd.setCursor(0,1);
    lcd.print(width_max);     
  }
  if(menu_level == 11 && menu_11_level == 2 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 14;    
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set frequency:");
    lcd.setCursor(0,1);
    lcd.print(frequency);     
  }
  /////////////////////////////////////////////////////////////////////////////////


  
  



  
  ///////////////////////////////MENU CHANGE////////////////////////////////





  ///////////////////////////////MENU 1////////////////////////////////
  if(menu_level == 1 && !digitalRead(up) && up_state == true)
  {
    up_state = false;
    menu_1_level = menu_1_level - 1;
    if(menu_1_level < 0)
    {
      menu_1_level = 2;
    }
    if(menu_1_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Servo pot");
      lcd.setCursor(0,1);
      lcd.print("  Servo fine");
    }
    if(menu_1_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Servo pot");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" Servo fine");
    }
    if(menu_1_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  Servo fine");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" Servo config");
    }    
  }

  if(menu_level == 1 && !digitalRead(down) && down_state == true)
  {
    OCR1A = 0;      //PWM->OFF pulse width = 0;
    down_state = false;
    menu_1_level = menu_1_level + 1;
    if(menu_1_level > 2)
    {
      menu_1_level = 0;
    }
    if(menu_1_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Servo pot");
      lcd.setCursor(0,1);
      lcd.print("  Servo fine");
    }
    if(menu_1_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Servo pot");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" Servo fine");
    }
    if(menu_1_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  Servo fine");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" Servo config");
    } 
  }
  ///////////////////////////////MENU 1////////////////////////////////





    ///////////////////////////////MENU 2////////////////////////////////
  if(menu_level == 2)
  {
    width = map(analogRead(pot),0,1024,width_min,width_max);
    deg_val = map(width,width_min,width_max,0,181);
    OCR1A = width;

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= 100)
    {
      previousMillis += 100;
      lcd.clear();  
      lcd.setCursor(0,0);    
      lcd.print("Deg: ");
      lcd.print(deg_val);      
    }      
  }
  ///////////////////////////////MENU 2////////////////////////////////

  

        
  ///////////////////////////////MENU 3////////////////////////////////
  if(menu_level == 3)
  {
    if(!digitalRead(left) && left_state==true)
    {
      deg_val = deg_val - 1;
      left_state = false;
    }
    if(!digitalRead(right) && right_state==true)
    {
      deg_val = deg_val + 1;
      right_state = false;
    }
    if(deg_val > 180) deg_val = 180;
    if(deg_val < 0) deg_val = 0;    
    
    width = map(deg_val, 0, 190,width_min, width_max);
    OCR1A = width;

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= 100)
    {
      previousMillis += 100;
      lcd.clear();  
      lcd.setCursor(0,0);    
      lcd.print("Deg: ");
      lcd.print(deg_val);      
    }     
  }
  ///////////////////////////////MENU 3////////////////////////////////





  ///////////////////////////////MENU 4////////////////////////////////
  if(menu_level == 4 && !digitalRead(up) && up_state == true)
  {
    OCR1A = 0;      //PWM->OFF pulse width = 0;
    up_state = false;
    menu_4_level = menu_4_level - 1;
    if(menu_4_level < 0)
    {
      menu_4_level = 2;
    }
    if(menu_4_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Min");
      lcd.setCursor(0,1);
      lcd.print("  Max");
    }
    if(menu_4_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Min");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" Max");
    }
    if(menu_4_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  Max");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" Frequency");
    }    
  }

  if(menu_level == 4 && !digitalRead(down) && down_state == true)
  {
    OCR1A = 0;      //PWM->OFF pulse width = 0;
    down_state = false;
    menu_4_level = menu_4_level + 1;
    if(menu_4_level > 2)
    {
      menu_4_level = 0;
    }
    if(menu_4_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Min");
      lcd.setCursor(0,1);
      lcd.print("  Max");
    }
    if(menu_4_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Min");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" Max");
    }
    if(menu_4_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  Max");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" Frequency");
    } 
  }
  ///////////////////////////////MENU 4////////////////////////////////



  ///////////////////////////////MENU 5////////////////////////////////
  if(menu_level == 5 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    width_min = width_min - 5;
    if(width_min < 100)
    {
      width_min = 100;
    }
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set min:");
    lcd.setCursor(0,1);
    lcd.print(width_min);    
  }
  if(menu_level == 5 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    width_min = width_min + 5;
    if(width_min > width_max)
    {
      width_min = width_max;
    }   
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set min:");
    lcd.setCursor(0,1);
    lcd.print(width_min); 
  }
  ///////////////////////////////MENU 5////////////////////////////////


  

  ///////////////////////////////MENU 6////////////////////////////////
  if(menu_level == 6 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    width_max= width_max - 5;
    if(width_max < width_min)
    {
      width_max = width_min;
    }   
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set max:");
    lcd.setCursor(0,1);
    lcd.print(width_max); 
  }
  if(menu_level == 6 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    width_max = width_max + 5;
    if(width_max > 4000)
    {
      width_max = 40000;
    }   
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set max:");
    lcd.setCursor(0,1);
    lcd.print(width_max); 
  }
  ///////////////////////////////MENU 5////////////////////////////////


  ///////////////////////////////MENU 7////////////////////////////////
  if(menu_level == 7 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    frequency= frequency - 1;    
    if(frequency < 10)
    {
      frequency = 10;
    } 
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set frequency:");
    lcd.setCursor(0,1);
    lcd.print(frequency);     
    servo_freq = 16000000/(frequency*16);
  }
  if(menu_level == 7 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    frequency = frequency + 1;   
    if(frequency > 100)
    {
      frequency = 100;
    }        
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set frequency:");;
    lcd.setCursor(0,1);
    lcd.print(frequency); 
    servo_freq = 16000000/(frequency*16);
  }
  ///////////////////////////////MENU 7////////////////////////////////




  ///////////////////////////////MENU 8////////////////////////////////
  if(menu_level == 8 && !digitalRead(up) && up_state == true)
  {
    up_state = false;
    menu_8_level = menu_8_level - 1;
    if(menu_8_level < 0)
    {
      menu_8_level = 2;
    }
    if(menu_8_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" ESC pot");
      lcd.setCursor(0,1);
      lcd.print("  ESC fine");
    }
    if(menu_8_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  ESC pot");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" ESC fine");
    }
    if(menu_8_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  ESC fine");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" ESC config");
    }    
  }

  if(menu_level == 8 && !digitalRead(down) && down_state == true)
  {    
    down_state = false;
    menu_8_level = menu_8_level + 1;
    if(menu_8_level > 2)
    {
      menu_8_level = 0;
    }
    if(menu_8_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" ESC pot");
      lcd.setCursor(0,1);
      lcd.print("  ESC fine");
    }
    if(menu_8_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  ESC pot");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" ESC fine");
    }
    if(menu_8_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  ESC fine");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" ESC config");
    }    
  }
  ///////////////////////////////MENU 8////////////////////////////////



  ///////////////////////////////MENU 9////////////////////////////////
  if(menu_level == 9)
  {
    esc_width = map(analogRead(pot),0,1024,esc_width_min,esc_width_max);    
    OCR1A = esc_width;

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= 100)
    {
      previousMillis += 100;
      lcd.clear();  
      lcd.setCursor(0,0);    
      lcd.print("us: ");
      lcd.print(esc_width);      
    }      
  }
  ///////////////////////////////MENU 9////////////////////////////////

  

        
  ///////////////////////////////MENU 10////////////////////////////////
  if(menu_level == 10)
  {
    if(!digitalRead(left))
    {
      esc_width = esc_width - 1;
      left_state = false;
      delay(10);
    }
    if(!digitalRead(right))
    {
      esc_width = esc_width + 1;
      right_state = false;
      delay(10);
    }
    if(esc_width > esc_width_max) esc_width = esc_width_max;
    if(esc_width < esc_width_min) esc_width = esc_width_min;     
 
    OCR1A = esc_width;

    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= 100)
    {
      previousMillis += 100;
      lcd.clear();  
      lcd.setCursor(0,0);    
      lcd.print("us: ");
      lcd.print(esc_width);      
    }     
  }
  ///////////////////////////////MENU 10////////////////////////////////




  ///////////////////////////////MENU 11////////////////////////////////
  if(menu_level == 11 && !digitalRead(up) && up_state == true)
  {
    up_state = false;
    menu_11_level = menu_11_level - 1;
    if(menu_11_level < 0)
    {
      menu_11_level = 2;
    }
    if(menu_11_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Min");
      lcd.setCursor(0,1);
      lcd.print("  Max");
    }
    if(menu_11_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Min");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" Max");
    }
    if(menu_11_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  Max");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" Frequency");
    }    
  }

  if(menu_level == 11 && !digitalRead(down) && down_state == true)
  {    
    down_state = false;
    menu_11_level = menu_11_level + 1;
    if(menu_11_level > 2)
    {
      menu_11_level = 0;
    }
    if(menu_11_level == 0)
    {
      lcd.clear();  
      lcd.setCursor(0,0);
      lcd.write(1);  
      lcd.print(" Min");
      lcd.setCursor(0,1);
      lcd.print("  Max");
    }
    if(menu_11_level == 1)
    {
      lcd.clear();  
      lcd.setCursor(0,0);      
      lcd.print("  Min");
      lcd.setCursor(0,1);
      lcd.write(1); 
      lcd.print(" Max");
    }
    if(menu_11_level == 2)
    {
      lcd.clear();  
      lcd.setCursor(0,0);          
      lcd.print("  Max");
      lcd.setCursor(0,1);    
      lcd.write(1);   
      lcd.print(" Frequency");
    } 
  }
  ///////////////////////////////MENU 11////////////////////////////////




  ///////////////////////////////MENU 12////////////////////////////////
  if(menu_level == 12 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    esc_width_min = esc_width_min - 5;
    if(esc_width_min < 100)
    {
      esc_width_min = 100;
    }
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set min:");
    lcd.setCursor(0,1);
    lcd.print(esc_width_min);    
  }
  if(menu_level == 12 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    esc_width_min = esc_width_min + 5;
    if(esc_width_min > esc_width_max)
    {
      esc_width_min = esc_width_max;
    }   
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set min:");
    lcd.setCursor(0,1);
    lcd.print(esc_width_min); 
  }
  ///////////////////////////////MENU 12////////////////////////////////


  

  ///////////////////////////////MENU 13////////////////////////////////
  if(menu_level == 13 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    esc_width_max= esc_width_max - 5;
    if(esc_width_max < esc_width_min)
    {
      esc_width_max = esc_width_min;
    }   
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set max:");
    lcd.setCursor(0,1);
    lcd.print(esc_width_max); 
  }
  if(menu_level == 13 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    esc_width_max = esc_width_max + 5;
    if(esc_width_max > 4000)
    {
      esc_width_max = 40000;
    }   
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set max:");
    lcd.setCursor(0,1);
    lcd.print(esc_width_max); 
  }
  ///////////////////////////////MENU 13////////////////////////////////


  ///////////////////////////////MENU 14////////////////////////////////
  if(menu_level == 14 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    frequency= frequency - 1;    
    if(frequency < 10)
    {
      frequency = 10;
    } 
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set frequency:");
    lcd.setCursor(0,1);
    lcd.print(frequency);     
    esc_freq = 16000000/(frequency*16);
  }
  if(menu_level == 14 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    frequency = frequency + 1;   
    if(frequency > 100)
    {
      frequency = 100;
    }        
    lcd.clear();  
    lcd.setCursor(0,0);     
    lcd.print("Set frequency:");;
    lcd.setCursor(0,1);
    lcd.print(frequency); 
    esc_freq = 16000000/(frequency*16);
  }
  ///////////////////////////////MENU 14////////////////////////////////



  ///////////////////////////////MENU 15////////////////////////////////
  if(menu_level == 15 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    calib_min= calib_min - 5;    
    if(calib_min < 100)
    {
      calib_min = 100;
    } 
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Set min value:");
    lcd.setCursor(0,1);
    lcd.print(calib_min);
  }

  if(menu_level == 15 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    calib_min= calib_min + 5;    
    if(calib_min > calib_max)
    {
      calib_min = calib_max;
    } 
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Set min value:");
    lcd.setCursor(0,1);
    lcd.print(calib_min);
  }
  if(menu_level == 15 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 16;
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Set max value:");
    lcd.setCursor(0,1);
    lcd.print(calib_max);
  }

   if(menu_level == 16 && !digitalRead(left) && left_state == true)
  {
    left_state = false;
    calib_max= calib_max - 5;    
    if(calib_max < calib_min)
    {
      calib_max = calib_min;
    } 
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Set max value:");
    lcd.setCursor(0,1);
    lcd.print(calib_max);
  }

  if(menu_level == 16 && !digitalRead(right) && right_state == true)
  {
    right_state = false;
    calib_max= calib_max + 5;    
    if(calib_max > 4000)
    {
      calib_max = 4000;
    } 
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Set max value:");
    lcd.setCursor(0,1);
    lcd.print(calib_max);
  }
  if(menu_level == 16 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;
    menu_level = 17;
    OCR1A = calib_max;
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Connect  battery");
    lcd.setCursor(0,1);
    lcd.print("And press select");
  }
  if(menu_level == 17 && !digitalRead(sel) && sel_state == true)
  {
    sel_state = false;  
    OCR1A = calib_min;
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("Calibrating");
    delay(1000);   
    
    lcd.clear();  
    lcd.setCursor(0,0);    
    lcd.print("ESC configured");
    lcd.setCursor(0,1);
    lcd.print("after beep");
    delay(2000);
    menu_level = 0;
    menu_0_level = 0;    
    lcd.setCursor(0,0);
    lcd.write(1);  
    lcd.print(" Servo control");
    lcd.setCursor(0,1);
    lcd.print("  ESC control");
  }
  ///////////////////////////////MENU 15,16,17////////////////////////////////


  



  
  

}//end void loop
