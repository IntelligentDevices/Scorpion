#include <SoftwareSerial.h>

// #################### CONSTANTS ####################
const int power_trigger = 2;
const int power_position = 6;
const int power_selector = 3;
const int io_selector = A2;
const int io_trigger = A1;
const int io_position = A7;
const int motor = A3;

// #################### RUNTIMES ####################


boolean rearposition = false;
boolean frontposition = false;
boolean cyclecomplete = false;

const boolean debug = true;

// #################### SETUP ####################
void setup() {
  // Start serial monitor
  if(debug)Serial.begin(19200);
  
  // Initializes motor. Doesn't cause it to start firing.
    analogWrite(motor,HIGH);

  // Initialize IO pins
  pinMode(power_selector,OUTPUT);
  pinMode(power_trigger,OUTPUT);
  pinMode(power_position, OUTPUT);
  pinMode(io_position,INPUT);
  pinMode(io_trigger,INPUT);
  pinMode(io_selector,INPUT);

  // Turns on the hall sensors. Later we can set it so only the trigger is on in idle 
  //and the other hall sensors come on when the trigger is pulled to save battery
  digitalWrite(power_position,HIGH);
  digitalWrite(power_trigger,HIGH);
  digitalWrite(power_selector,HIGH);
}
// #################### Selector ####################
float selector(){
  return analogRead(io_selector) *(1.00/1023.00);
  //semi reads between 0.27 and 0.28
  // auto reads 0.36 
}
// #################### TRIGGER ####################
boolean trigger(){
  float t = analogRead(io_trigger) *(1.00/1023.00);
  return (t<= 0.35);
}
// #################### POSITION ####################
float position(){
  return analogRead(io_position) *(1.00/1023.00);
}
// #################### LOOP ####################
void loop() {
 
  // Is trigger pulled on a new cycle?
  if(!cyclecomplete && trigger())
  {
    // Motor on
    digitalWrite(motor, HIGH);
    
    // Check if tappet has reached rear position
    if (position() <= .48)
    {
      if(debug) Serial.println("rear position");
      rearposition = true;
    }
    
    // Check if tappet has already reached rear position 
    // and THEN check if it has hit front position
    // Check if rearposition is true first, because if 
    // rearposition is false we don't bother to do the 
    // analog read. Burns less battery, uses less resources, 
    // doesn't slow down execution speed. 
    if (rearposition && position() >= .49)
    {
      frontposition = true;
      if(debug) Serial.println("front position");
    }

    // If tappet has fully cycled, shut off motor.
    if (frontposition && rearposition)
    {
      digitalWrite(motor,LOW);
      if(debug) Serial.println("no more pew pew");
      rearposition = false;
      frontposition = false;
      cyclecomplete = true;
    }
    
  } else if(cyclecomplete && trigger()) {
    // Do nothing, semi auto fired but trigger still pulled
    digitalWrite(motor,LOW);
    //added this cause it would sometimes burst
  } else {
    digitalWrite(motor,LOW);// Trigger not pulled, reset cycle
    //same thing here as the line above
    cyclecomplete = false;
  }
}

