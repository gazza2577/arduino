

#include <Servo.h>

#include <stdio.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4

int potPin = 2;
int val = 0;

Adafruit_SSD1306 display(OLED_RESET);

Servo myservo;  // create servo object to control a servo

int potpin = 0;  // analog pin used to connect the potentiometer
int val1;    // variable to read the value from the analog pin

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
   display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

void loop() {
  val1 = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val1 = map(val1, 0, 1023, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  myservo.write(val1);                  // sets the servo position according to the scaled value
  delay(15);                           // waits for the servo to get there
  display.setCursor(0,0);
    display.setTextSize(1);
    display.println("      Servo Vinkel");
    display.drawFastHLine(0, 10, 128, 1);
    display.setCursor(46,17);
    display.setTextSize(2);
    val = analogRead(potPin);
    display.println(val/5-1);
    display.drawRect(0, 38, 128, 24, 1);
    display.fillRect(2, 40, val/7-1, 20, 1);
    display.display();

    display.clearDisplay();  // Clear display must be used to clear text etc
}
