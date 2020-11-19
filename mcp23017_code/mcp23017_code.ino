    //  MCP23017 code -- some parts translated from Portugese    //  MCP23017 code -- some parts translated from Portugese
    #include <Adafruit_PWMServoDriver.h>
    #include <CMRI.h>
    #include <Auto485.h>
    #include <Adafruit_MCP23017.h>

    //#define RS485Serial Serial1

    #define CMRI_ADDR 1
    #define DE_PIN 2

    int Tbit[16];           // number of servos
    uint8_t servonum = 0;   // servo counter
    uint8_t lastservo = 16;

    //Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40);    //setup the board address 0
    // Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41);    //setup the board address 1
    Auto485 bus(DE_PIN); // Arduino pin 2 -> MAX485 DE and RE pins
    CMRI cmri(CMRI_ADDR, 24, 48, bus);

    // Initialize I2C MCP23017
    Adafruit_MCP23017 mcp0;
    Adafruit_MCP23017 mcp1;
    Adafruit_MCP23017 mcp2;

    byte UpdateOutputValue=1;
    byte OutputValue[6];   //Each byte is 8 bits - 8x4 32 bits
    byte InputValue[3]; //Each byte is 8 bits - 8x3 24 bits
    unsigned long TimerLoadInput;
     

    void setup() {
      Serial.begin(9600);
      bus.begin(9600, SERIAL_8N2);
     // pwm1.begin();
      // pwm2.begin();
      //pwm1.setPWMFreq(60);
      // pwm2.setPWMFreq(60);

      // Define MCP23017 modules
      mcp0.begin();
      mcp1.begin();
      mcp2.begin();
     
      InitializePortsMCP23017();
     
      Serial.println("setup1 complete");
    }

    void loop() {
     //  Serial.println(bus.read());
       cmri.process();

      // Load Input Values
       if (millis()-TimerLoadInput>500) {
          LoadInputValues();
          TimerLoadInput = millis();
        }
      
       for(servonum = 0 ; servonum < lastservo; servonum ++){

        Tbit[servonum] = (cmri.get_bit(servonum));     
      // Serial.println(servonum, Tbit[servonum]);
        if(servonum <16) {
            if (Tbit[servonum] == 1){
              //  pwm1.setPWM(servonum, 0, 100);
          //      Serial.println("throw");
              }
            if (Tbit[servonum] == 0){
             //   pwm1.setPWM(servonum, 0, 400);
          //      Serial.println("close 1");
              }
          }
      /*
        if(servonum >=16 && servonum <= 31){
            if (Tbit[servonum] == 1){
                pwm2.setPWM(servonum - 16, 0, 100);
                Serial.println("throw2");
              }
            if (Tbit[servonum] == 0){
                pwm2.setPWM(servonum - 16, 0, 400);
                Serial.println("close 2");
              }
          }
        */     
        // add additional lines for additional boards

      }

      // Check for change in output values
      for(int i = 0; i < 6; i++) {
        byte NewValue;
        NewValue = cmri.get_byte(i);
      //  Serial.println(NewValue);
        if (NewValue != OutputValue[i]) {
          CompareOutputBits(NewValue, i);
        }
      }

      // Update of Output Values
      if (UpdateOutputValue>0) UpdateOutput();
    }

    void CompareOutputBits(byte tmpValue, byte bBlock) {
      //Compare the New Value bits
      for(byte i = 0; i < 8; i++) {
        if (bitRead(tmpValue, i) != bitRead(OutputValue[bBlock],i)) {
          if (bBlock<2) {
            i = i;  // This is where the servo code was called
                    // -- review could improve efficiency
                    // -- also a reason for only using one pca9685 pwm board
         //   NovoValueServo(bBlock * 8 + i, bitRead(tmpValue, i)); 
          }
          else
          {
            UpdateOutputValue=1;
          }
        }
      }
      OutputValue[bBlock] = tmpValue;
    }

    void LoadInputValues() {
      for (byte i = 0; i < 2; i++) {
        // Input Values MCP23017
        byte NewValue = ReturnValueEnteredMCP23017(i);
            
        // Check value change
        if (NewValue != InputValue[i]) {
          cmri.set_byte(i, ~NewValue);
          InputValue[i] = NewValue;
        }
      }
    }

    byte ReturnValueEnteredMCP23017(byte bInput) {
      byte bValue;
      // Input ports 0 - MCP23017 0 / Port A
      if (bInput==0) bValue=mcp0.readGPIO(0);
      // Input ports 1 - MCP23017 0 / Port B
      if (bInput==1) bValue=mcp0.readGPIO(1);
      // Returns bValue
      return bValue;
    }

    void InitializePortsMCP23017() {
      //Module 0 - Input
      for (int p=0;p<16;p++) {
        mcp0.pinMode(p, INPUT);
        mcp0.pullUp(p, HIGH); 
      }
      //Module 1 - Output
      for (int p=0;p<16;p++) {
        mcp1.pinMode(p, OUTPUT);
      }
      //Module 2 - Output
      for (int p=0;p<16;p++) {
        mcp2.pinMode(p, OUTPUT);
      }
    }

    void UpdateOutput() {
      int tmpAB;
     
      // Output ports 2 and 3 (17 to 32) - MCP23017 1
      tmpAB = OutputValue[3];
      tmpAB <<= 8;
      tmpAB |= OutputValue[2];
      mcp1.writeGPIOAB(tmpAB);
     
      // Output ports 4 and 5 (32 to 48)- MCP23017 2
      tmpAB = OutputValue[5];
      tmpAB <<= 8;
      tmpAB |= OutputValue[4];
      mcp2.writeGPIOAB(tmpAB);
    }

     
