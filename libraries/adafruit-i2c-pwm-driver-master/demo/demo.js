// Generated by CoffeeScript 1.6.3
var PwmDriver, pwm, servoLoop, servoMax, servoMin, setHigh, setLow, setServoPulse, sleep;

PwmDriver = require("../main");

sleep = require('sleep');

pwm = new PwmDriver(0x40);

servoMin = 150;

servoMax = 600;

setServoPulse = function(channel, pulse) {
  var pulseLength;
  pulseLength = 1000000;
  pulseLength /= 60;
  print("%d us per period" % pulseLength);
  pulseLength /= 4096;
  print("%d us per bit" % pulseLength);
  pulse *= 1000;
  pulse /= pulseLength;
  return pwm.setPWM(channel, 0, pulse);
};

pwm.setPWMFreq(60);

setHigh = function() {
  pwm.setPWM(0, 0, servoMax);
  return setTimeout(setLow, 1000);
};

setLow = function() {
  pwm.setPWM(0, 0, servoMin);
  return setTimeout(setHigh, 1000);
};

servoLoop = function() {
  return setLow();
};

servoLoop();

/*
while true
  # Change speed of continuous servo on channel O
  pwm.setPWM(0, 0, servoMin)
  pwm.setPWM(0, 0, servoMax)
  sleep.sleep(1)
*/


process.on('SIGINT', function() {
  console.log("\nGracefully shutting down from SIGINT (Ctrl-C)");
  pwm.stop();
  return process.exit();
});