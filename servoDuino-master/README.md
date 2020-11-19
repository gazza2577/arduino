# servoDuino
An [Arduino sketch](https://github.com/PeterDHabermehl/servoDuino/raw/master/servoDuino/servoDuino.ino) to serve as an USB-I2C-bridge for PCA9685 servo shields

This is mainly intended to control [marble runs standalone](https://www.youtube.com/watch?v=XlLrBHuRTXs) (then with a dedicated sketch) or to serve in conjunction with [startIDE](https://github.com/PeterDHabermehl/startIDE) on a [TX-Pi](https://github.com/harbaum/tx-pi).

Discussion about the servoDuino may take place here in the [ftCommunity forums](https://forum.ftcommunity.de/viewtopic.php?f=8&t=5079).

See this [vid](https://www.youtube.com/watch?v=aEfqSiFy-tU) for a startIDE tool to calibrate servos.

![servoDuino](https://github.com/PeterDHabermehl/servoDuino/blob/master/Bilder/DSC_1339.JPG)

[stl-files to print the case available here!](https://github.com/PeterDHabermehl/servoDuino/tree/master/stl)

Parts and wiring:
![wiring](https://github.com/PeterDHabermehl/servoDuino/blob/master/Bilder/DSC_1334_Wiring.jpg)


a) The diode I used was a 1N4004


b) Adjust the output voltage of the step down converter to ~5V **before** connecting


c) You might adjust the voltage to slightly above 5V, so that the voltage drop at the diode will be compensated for the arduino and the servo shield power terminals are still below 6V


d) You'll need 2 "Bundhülsen" (flush sleeves) 8,4mm long for the power input. Just solder them to the input terminals of the step down converter. If you just get longer ones, carefully shorten them to fit.

e) If you intend to control your servoDuino via startIDE, you have to check the vid:pid of the Arduino. startIDE uses 1a86:7523 by default. If the vid:pid of your device differs (under the ft community firmware, use the App "USBList" to check for your connected USB devices), start your startIDE program code with a comment "# SRDVIDPID xxxx:yyyy" where xxxx:yyyy is the vid:pid of your device.

Good luck!

BTW, have a look at the [LEDs](https://github.com/PeterDHabermehl/servoDuino/blob/master/Doku/LEDs.pdf)

**Commands on USB port:

## Servo/Pwm control

Only I2C address 0x40 supported, only one shield. pwm frequency is 60Hz fixed. 
The new command to address a PWM port: **pwm_set \<channel\> \<pulse high start\> \<pulse high end\>**


with

    <channel>           Number of the pwm/servo port of the shield, starts at 0 up to num.ports-1, so 0..15 for 16 channels.

    <pulse high start>  Start of the high pulse in x/4096, usually "0"
        
    <pulse high end>    End of the high pulse in x/4096
        

As the servo shield resolves a pwm pulse in 12 bit and the high level of the pwm signal usually starts at 0, a duty cycle of 5% on channel 0 could be set like:
    
    pwm_set 0 0 205 because 4096 * 0,05 = 204,8

Accordingly 12.5%:

    pwm_set 0 0 512 (4096 * 0.125 = 512)

To turn on a channel with 100%, issue
    
    pwm_set <channel> 4096 0

to turn off

    pwm_set <channel> 0 4096
    
**The command "pwm_halt" turns off all 16 channels at once.**

## I2C communication

To access the ftDuino I2C port, use

    I2C_Write <device address> <data byte 0> .. <data byte 31>
    
    I2C_Read <device address> <number of bytes to read>
    
where all parameters are expected to be space-separated 8bit unsigned integer numbers.

I2CRead then returns the data also as a sequence of space-separated numbers.




