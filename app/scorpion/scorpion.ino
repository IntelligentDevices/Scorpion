#include <SoftwareSerial.h>

// #################### BEGIN MODE SETTINGS ####################
// These settings are for configuration and debugging only, turn
// them off when not being used. They will eat battery and 
// cause problems.
// DEBUG MODE SHOULD BE TURNED OFF WHEN NOT ATTACHED TO THE IDE
const boolean debug = true;
// CONFIGURE MODE SHOULD BE TURNED OFF WHEN NOT ATTACHED TO THE IDE
const boolean configure = false;
// #################### BEGIN USER SETTINGS ####################
// For full-auto, use a large number like 9999. Since no magazine 
// has that many rounds in it, it will effectively fire until you
// release the trigger. This number must be smaller than 32,767 or
// it will cause problems when you try to compile.
// THE NUMBER OF ROUNDS TO FIRE IN SEMI-AUTO MODE 
const int semirounds = 1;
// THE NUMBER OF ROUNDS TO FIRE IN FULL-AUTO MODE 
const int autorounds = 3;
// SMALLER NUMBERS MEAN EARLIER TRIGGER BREAK
const int trigger_break = 1100;

// PASTE VALUES FROM CONFIG MODE HERE

//Trigger values:
const int trigger_max = 4457;
const int trigger_min = 3088;
//Selector Values:
const int selector_max = 5800;
const int selector_min = 4550;
//Tappet Values:
const int tappet_max = 6001;
const int tappet_min = 4477;

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
int rounds = 0;
// #################### SETUP ####################
void setup() {
  // Start serial monitor
  if(debug)Serial.begin(9600);
  
  if(debug)Serial.println("Initializing");

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
      digitalWrite(power_position,HIGH);
      digitalWrite(power_selector,HIGH);

  if(configure) pre_configure();
}
// #################### SELECTOR MODE ####################
boolean isauto(){
  //float a = (analogRead(io_selector)*(1.00/1023.00))+selector_offset;
  int av = ((analogRead(io_selector)/1023.00)*10000);

  if(debug){
    //Serial.print("IsAuto: ");
    //Serial.print(av);
    //Serial.print("<");
    //Serial.print(selector_min+100);
    //Serial.print("=");
    //Serial.println((av<selector_min + 100));
  }
  boolean ret = av<selector_min + 100;
  //if(debug) Serial.println(ret);
  return(ret);
  //return (a>.45);
}
// #################### TRIGGER PULL ####################
boolean trigger(){
  //float t = (analogRead(io_trigger) *(1.00/1023.00))+trigger_offset;
  int tr = ((analogRead(io_trigger)/1023.00)*10000);
  // 0.35 and below means trigger is pulled
  if(debug){
    //Serial.print("Trigger: ");
    //Serial.println(tr);
  }
  boolean ret = (tr<=trigger_max-trigger_break);
  //if(ret) Serial.println("trigger");
  return(ret);
  //return (t<= 0.35);
}
// #################### SENSOR POSITION ####################
int position(){
  //float p = (analogRead(io_position) *(1.00/1023.00))+tappet_offset;
  int pv = ((analogRead(io_position)/1023.00)*10000);

  if(debug){
    //Serial.print("Position: ");
    //Serial.println(pv);
  }
  return pv;
}
// #################### WATCH FIRE CYCLE ####################
void watchcycle(){
    int pos = position();
    // Check if tappet has reached rear position
    if (!rearposition && pos <= tappet_max-100)
    {
      if(debug){
        Serial.print("Cycle: Rear Position: ");
        Serial.println(pos);
      }
      rearposition = true;
    }
    // Check if tappet has already reached rear position 
    // and THEN check if it has hit front position
    // Alter front position check for single shots to 
    // adjust for lack of momentum
    if (rearposition && pos >= tappet_max-50)
    {
      if(debug){
        Serial.print("Cycle: Front Position: ");
        Serial.println(pos);
      }
      frontposition = true;
    }

    // tappet has fully cycled, increment cycle
    if (frontposition && rearposition)
    {
      // Delay shutting off motor for single round to 
      //compensate for lack of momentum
      if(rounds==1) delay(15);
      if(debug) Serial.print("Cycle Complete: ");
      if(debug) Serial.println(rounds-cyclecount);
      rearposition = false;
      frontposition = false;
      cyclecount = cyclecount + 1;
    }
}
// #################### LOOP ####################
void loop() {
  if(configure){
    configure_loop();
  }else{
    // Is trigger pulled?
    if(trigger()){
      // turn on selector and tapppet sensors
      // This may need to be moved back up to setup if it acts wonky. 
      // I don't know how long the sensor takes to generate a reading, 
      // or the impact of writing it HIGH repeatedly. 
      //digitalWrite(power_position,HIGH);
      //digitalWrite(power_selector,HIGH);
  
      // determine how many times to fire
      rounds = (isauto()?autorounds:semirounds);
      // Check if enough rounds have been fired
      if(cyclecount>=rounds){
        // cycles meets or exceeds round count for selector position
        // motor off
        //if(debug) Serial.println("Motor Off");
        digitalWrite(motor,LOW);
      }else{
        // listen for cycle count
        watchcycle();
        // cycles has not yet hit desired count
        digitalWrite(motor, HIGH);
        //if(debug) Serial.println("Motor On");
      }
  
    } else {
      // Trigger not pulled
      // reset cycle
      cyclecount = 0;
      // motor off150
      digitalWrite(motor,LOW);
      // Turn off selector and tappet sensors
      //digitalWrite(power_position,LOW);
      //digitalWrite(power_selector,LOW);
    }
  }
}
// ################## CONFIGURATION MODE #################
void pre_configure(){
  Serial.println("========== CONFIGURATION MODE ==========");
  Serial.println("Instructions go here...");
  // Make sure motor is off
  digitalWrite(motor,LOW);
  // Turn sensors on      digitalWrite(power_position,HIGH);
      digitalWrite(power_selector,HIGH);

  digitalWrite(power_selector,HIGH);
  digitalWrite(power_position,HIGH);
}


boolean trigger_test_complete = false;
boolean trigger_high = false;
boolean trigger_low = false;
int trigger_test_value = 0;
int trigger_high_value = 0;
int trigger_low_value = 10000;
int trigger_half_cycle_count = 0;

void test_trigger(){
  trigger_test_value = ((analogRead(io_trigger)/1023.00)*10000);
  if(trigger_test_value>trigger_high_value){
    trigger_high_value = trigger_test_value;
  }
  if(trigger_test_value<trigger_low_value){
    trigger_low_value = trigger_test_value;
  }

  if(trigger_high_value-trigger_low_value>100){
    if(trigger_test_value>=trigger_high_value){
      trigger_high = true;
    }
    if(trigger_test_value<=trigger_low_value){
      trigger_low = true;
    }
  }
  
  Serial.print("High: ");
  Serial.print(trigger_high_value);
  Serial.print(" Low: ");
  Serial.print(trigger_low_value);
  Serial.print(" Count: ");
  Serial.print(trigger_half_cycle_count);
  Serial.print(" Current: ");
  Serial.println(trigger_test_value);
  
  if(trigger_high && trigger_low){
    trigger_half_cycle_count = trigger_half_cycle_count + 1;
    trigger_high = false;
    trigger_low = false;
  }

  if(trigger_half_cycle_count >= 6){
    // done with trigger test
    finish_trigger();
  }

}
void finish_trigger(){
  Serial.println("//Trigger values:");
  Serial.print("const int trigger_max = ");
  Serial.print(trigger_high_value);
  Serial.println(";");
  Serial.print("const int trigger_min = ");
  Serial.print(trigger_low_value);
  Serial.println(";");
  trigger_test_complete = true;
}


boolean tappet_test_complete = false;
boolean tappet_high = false;
boolean tappet_low = false;
int tappet_test_value = 0;
int tappet_high_value = 0;
int tappet_low_value = 10000;
int tappet_half_cycle_count = 0;

void test_tappet(){
  tappet_test_value = ((analogRead(io_position)/1023.00)*10000);
  if(tappet_test_value>tappet_high_value){
    tappet_high_value = tappet_test_value;
  }
  if(tappet_test_value<tappet_low_value){
    tappet_low_value = tappet_test_value;
  }

  if(tappet_high_value-tappet_low_value>100){
    if(tappet_test_value>=tappet_high_value){
      tappet_high = true;
    }
    if(tappet_test_value<=tappet_low_value){
      tappet_low = true;
    }
  }
  
  if(tappet_high && tappet_low){
    tappet_half_cycle_count = tappet_half_cycle_count + 1;
    tappet_high = false;
    tappet_low = false;
  }

  //Serial.print("High: ");
  //Serial.print(tappet_high_value);
  //Serial.print(" Low: ");
  //Serial.print(tappet_low_value);
  //Serial.print(" Count: ");
  //Serial.print(tappet_half_cycle_count);
  //Serial.print(" Current: ");
  //Serial.println(tappet_test_value);

  if(tappet_half_cycle_count >= 6){
    // done with tappet test
    finish_tappet();
  }

}
void finish_tappet(){
  Serial.println("//Tappet Values:");
  Serial.print("const int tappet_max = ");
  Serial.print(tappet_high_value);
  Serial.println(";");
  Serial.print("const int tappet_min = ");
  Serial.print(tappet_low_value);
  Serial.println(";");
  tappet_test_complete = true;
}


boolean selector_test_complete = false;
boolean selector_high = false;
boolean selector_low = false;
int selector_test_value = 0;
int selector_high_value = 0;
int selector_low_value = 10000;
int selector_half_cycle_count = 0;
int selector_test_run = 0;

void test_selector(){
  if(selector_test_run++<500){
    selector_test_value = ((analogRead(io_selector)/1023.00)*10000);
    if(selector_test_value>selector_high_value){
      selector_high_value = selector_test_value;
    }
    if(selector_test_value<selector_low_value){
      selector_low_value = selector_test_value;
    }

    if(selector_high_value-selector_low_value>1000){
      if(selector_test_value>=selector_high_value){
        selector_high = true;
      }
      if(selector_test_value<=selector_low_value){
        selector_low = true;
      }
    }
    
    if(selector_high && selector_low){
      selector_half_cycle_count = selector_half_cycle_count + 1;
      selector_high = false;
      selector_low = false;
    }

    //Serial.print("High: ");
    //Serial.print(selector_high_value);
    //Serial.print(" Low: ");
    //Serial.print(selector_low_value);
    //Serial.print(" Count: ");
    //Serial.print(selector_half_cycle_count);
    //Serial.print(" Current: ");
    //Serial.println(selector_test_value);

    if(selector_half_cycle_count >= 6){
      // done with selector test
      finish_selector();
    }
  }else{
    selector_high_value = selector_high_value - 100;
    selector_low_value = selector_low_value + 100;
    selector_test_run = 0;
  }
}
void finish_selector(){
  Serial.println("//Selector Values:");
  Serial.print("const int selector_max = ");
  Serial.print(selector_high_value);
  Serial.println(";");
  Serial.print("const int selector_min = ");
  Serial.print(selector_low_value);
  Serial.println(";");
  selector_test_complete = true;
}
//       tappet_test_value = ((analogRead(io_position)*(1.00/1023.00))*10000);

void configure_loop(){
  Serial.print("Trigger: ");
  Serial.print(trigger());
  Serial.print("    Is Auto: ");
  Serial.print(isauto());
  Serial.print("    Position: ");

  int pos = position();

  if(pos <= tappet_max-100){
    Serial.print("rear  ");
  }
  if(pos >= tappet_max-50){
    Serial.print("front  ");
  }
  Serial.println(pos);
  
  //if(!trigger_test_complete) test_trigger();
  //if(!selector_test_complete) test_selector();
  //if(!tappet_test_complete) test_tappet();

}


