#include <SoftwareSerial.h>

// #################### BEGIN USER SETTINGS ####################
// For full-auto, use a large number like 9999. Since no magazine 
// has that many rounds in it, it will fire effectively fire until 
// you release the trigger. 
// THE NUMBER OF ROUNDS TO FIRE IN SEMI-AUTO MODE 
const int semirounds = 1;
// THE NUMBER OF ROUNDS TO FIRE IN FULL-AUTO MODE 
const int autorounds = 3;
// #################### END USER SETTINGS ####################



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
int cyclecount = 0;
const boolean debug = true;
// #################### SETUP ####################
void setup() {
  // Start serial monitor
  if(debug)Serial.begin(9600);
  
  // Initializes motor. Doesn't cause it to start firing.
  analogWrite(motor,HIGH);

  // Initialize IO pins
  pinMode(power_selector,OUTPUT);
  pinMode(power_trigger,OUTPUT);
  pinMode(power_position, OUTPUT);
  pinMode(io_position,INPUT);
  pinMode(io_trigger,INPUT);
  pinMode(io_selector,INPUT);

  // Turn on trigger sensor
  digitalWrite(power_trigger,HIGH);

  // Initialize selector and tappet sensors as OFF
  //digitalWrite(power_position,LOW);
  //digitalWrite(power_selector,LOW);
  Serial.println("Initializing");
  digitalWrite(power_position,HIGH);
  digitalWrite(power_selector,HIGH);

}
// #################### SELECTOR MODE ####################
boolean isauto(){
  // semi reads between 0.32 and 0.34
  // auto reads 0.47 
  // adjust the >0.40 below to set the breakpoint 
  // where it should mean full auto
//  Serial.println(analogRead(io_selector) *(1.00/1023.00));
  return ((analogRead(io_selector) *(1.00/1023.00))>0.40);
}
// #################### TRIGGER PULL ####################
boolean trigger(){
  float t = analogRead(io_trigger) *(1.00/1023.00);
  // 0.35 and below means trigger is pulled
  return (t<= 0.35);
}
// #################### SENSOR POSITION ####################
float position(){
  return analogRead(io_position) *(1.00/1023.00);
}
// #################### WATCH FIRE CYCLE ####################
void watchcycle(){
    // Check if tappet has reached rear position
    if (position() >= .49)
    {
      //Serial.println("Cycle: Rear Position");
      rearposition = true;
    }
    // Check if tappet has already reached rear position 
    // and THEN check if it has hit front position
    if (rearposition && position() <= .46)
    {
      //Serial.println("Cycle: Front Position");
      frontposition = true;
    }

    // tappet has fully cycled, increment cycle
    if (frontposition && rearposition)
    {
      Serial.println("Cycle Complete");
      rearposition = false;
      frontposition = false;
      cyclecount = cyclecount + 1;
    }
}
// #################### LOOP ####################
void loop() {
  // Is trigger pulled?
  if(trigger()){
    // turn on selector and tapppet sensors
    // This may need to be moved back up to setup if it acts wonky. 
    // I don't know how long the sensor takes to generate a reading, 
    // or the impact of writing it HIGH repeatedly. 

    // determine how many times to fire
    int rounds = (isauto()?autorounds:semirounds);
    // listen for cycle count
    watchcycle();

    // Check if enough rounds have been fired
    if(cyclecount>=rounds){
      // cycles meets or exceeds round count for selector position
      // motor off
      Serial.println("Motor Off");
      digitalWrite(motor,LOW);
    }else{
      // cycles has not yet hit desired count
      digitalWrite(motor, HIGH);
    }

  } else {
    // Trigger not pulled
    // reset cycle
    cyclecount = 0;
    // motor off
    digitalWrite(motor,LOW);
    // Turn off selector and tappet sensors
    //digitalWrite(power_position,LOW);
    //digitalWrite(power_selector,LOW);
  }
}

