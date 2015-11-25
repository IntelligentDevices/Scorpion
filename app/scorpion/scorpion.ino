#include <SoftwareSerial.h>

// #################### CONSTANTS ####################
const int power_trigger = 2;
const int power_position = 6;
const int io_trigger = A1;
const int io_position = A7;
const int motor = A3;

// #################### RUNTIMES ####################
float position;
float trigger;

boolean rearposition = false;
boolean frontposition = false;
boolean cyclecomplete = false;

const boolean debug = true;

// #################### SETUP ####################
void setup() {
  // Start serial monitor
  if(debug)Serial.begin(19200);
  
  // What's this for? Are you starting the motor now?
  // This seems like it might make the gun fire when you boot...
  analogWrite(motor,HIGH);

  // Initialize IO pins
  pinMode(power_trigger,OUTPUT);
  pinMode(power_position, OUTPUT);
  pinMode(io_position,INPUT);
  pinMode(io_trigger,INPUT);

  // Why are you setting these high in setup? Necessary?
  digitalWrite(power_position,HIGH);
  digitalWrite(power_trigger,HIGH);
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
      Serial.println("rear position");
      rearposition = true;
    }
    
    // Check if tappet has already reached rear position 
    // and THEN check if it has hit front position
    // Check if rearposition is true first, because if 
    // rearposition is false we don't bother to do the 
    // analog read. Burns less battery, uses less resources, 
    // doesn't slow down execution speed. 
    if (rearposition && position() >= .50)
    {
      frontposition = true;
      Serial.println("front position");
    }

    // If tappet has fully cycled, shut off motor.
    if (frontposition && rearposition)
    {
      digitalWrite(motor,LOW);
      Serial.println("no more pew pew");
      rearposition = false;
      frontposition = false;
      cyclecomplete = true;
    }
    
  } else if(cyclecomplete && trigger()) {
    // Do nothing, semi auto fired but trigger still pulled
  } else {
    // Trigger not pulled, reset cycle
    cyclecomplete = false;
  }
}
