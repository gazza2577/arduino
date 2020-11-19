// ************* from the header file

// Create a pointer to a function that takes references to a Print and two byte types and to an integer array
// What is a Print type?
typedef void (*FILTER_CALLBACK)(Print & stream, byte & opcode, byte & paramCount, int p[]);

struct DCCEXParser
{
   DCCEXParser();
   void parse(Print & stream, const byte * command, bool banAsync);
   static void setFilter(FILTER_CALLBACK filter); // call the callback function "filter()" with setfilter
 
   private:
    static FILTER_CALLBACK  filterCallback;  // filtercallback type. Why do we need this?
};


// ************ from the .cpp file

FILTER_CALLBACK  DCCEXParser::filterCallback=0; // prototype? Why set it to zero?

void DCCEXParser::setFilter(FILTER_CALLBACK filter) {
  filterCallback=filter; 
 }  

void DCCEXParser::parse(Print & stream, const byte *com, bool banAsync) {
  byte opcode=com[0];
     
  if (filterCallback)  filterCallback(stream,opcode,params,p); 
    // Functions return from this switch if complete, break from switch implies error <X> to send
    switch(opcode) {
        case '\0': return;    // filterCallback asked us to ignore 	
	    case 't':       // THROTTLE <t REGISTER CAB SPEED DIRECTION>
        DCC::setThrottle(p[1],p[2],p[3]);
        StringFormatter::send(stream,F("<T %d %d %d>"), p[0], p[2],p[3]);
        return;
	}
}

// Calling it from a sketch

// myFilter is an example of an OPTIONAL command filter used to intercept < > commands from
// the usb or wifi streamm.  It demonstrates how a command may be intercepted
//  or even a new command created without having to break open the API library code.
// The filter is permitted to use or modify the parameter list before passing it on to 
// the standard parser. By setting the opcode to 0, the standard parser will 
// just ignore the command on the assumption that you have already handled it.
//
// The filter must be enabled by calling the DCC EXParser::setFilter method, see use in setup().
 
void myFilter(Print & stream, byte & opcode, byte & paramCount, int p[]) {
    (void)stream; // avoid compiler warning if we don't access this parameter
    switch (opcode) {  
       case 'F': // Invent new command to call the new Loco Function API <F cab func 1|0>
             DIAG(F("Setting loco %d F%d %S"),p[0],p[1],p[2]?F("ON"):F("OFF"));
             DCC::setFn(p[0],p[1],p[2]==1); 
             opcode=0;  // tell parser to ignore this command
             break; 
       case '$':   // Diagnose parser <$....>
            DIAG(F("$ paramCount=%d\n"),paramCount);
            for (int i=0;i<paramCount;i++) DIAG(F("p[%d]=%d (0x%x)\n"),i,p[i],p[i]);
            opcode=0; // Normal parser wont understand $, 
            break;
       default:  // drop through and parser will use the command unaltered.   
            break;  
    }
}

// Callback functions are necessary if you call any API that must wait for a response from the 
// programming track. The API must return immediately otherwise other loop() functions would be blocked.
// Your callback function will be invoked when the data arrives from the prog track.
// See the DCC:getLocoId example in the setup function. 


void myCallback(int result) {
  DIAG(F("\n getting Loco Id callback result=%d"),result); 
}

DCCEXParser  serialParser;

void setup() {

  // 1: Start the usb connection for diagnostics and possible JMRI input
   Serial.begin(115200);
   
   // Responsibility 2: Start the DCC engine.   
   DCC::begin();

    //  This is just for demonstration purposes 
   DIAG(F("\n===== CVReader demonstrating DCC::getLocoId() call ==========\n"));
   DCC::getLocoId(myCallback); // myCallback will be called with the result 
   DIAG(F("\n===== DCC::getLocoId has returned, but the callback wont be executed until we are in loop() ======\n"));
   
   // Optionally tell the command parser to use my example filter.
   // This will intercept JMRI commands from both USB and Wifi 
   DCCEXParser::setFilter(myFilter);
   
   DIAG(F("\nReady for JMRI commands\n"));
   
}
	
    
