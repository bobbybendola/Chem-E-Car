 /*
Code last updated: 2/3/2026, 10:00 AM
Recent changes:
  - When the light intensity goes under threshold and car stops, motor will also stop
    - Similarly, the switch may be pulled again to reset the cycle without needing to restart the arduino
  - Fixed up false premature-breaking of findColorSensorScale on multiple iterations by reformatting the function
  
  - Possible improvements: Keep checking switch_pin state within case 1 to improve responsiveness of linear actuator (if switch is pulled early)
    - might not really be necessary. at the worst, you just gotta wait for the delays to finish as this is only to do with the sensor calibration
    (lines 141-160)

❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
❗❗❗ NOTICE TO EVERYONE: BEFORE UPLOADING UPDATED CODE FILES TO THE GOOGLE DRIVE, UPDATE THE CHANGELOG HERE ❗❗❗
❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗❗
*/

#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

#define switch_pin 9
#define motor_pin 3
#define unused_pin 4
#define linear_actuator_IN1 5
#define linear_actuator_IN2 6
//#define air_pump_IN1 7 // Mechanical team will be using an air pump soon
//#define air_pump_IN2 8
#define pump_sleep 7     
//PUMP_SLEEP required to be driven HIGH to allow the motor driver for the car to work. 
#define car_sleep 8 

#define ASTEP_VAL 2999
#define ATIME_VAL 50 

//DO NOT USE A4(SDA), A5(SDL)  pins, THEY ARE LEFT FOR THE ADAFRUIT SENSOR ONLY. 

unsigned int measured_intensity;
unsigned int threshold_intensity = 0; // Stopping team, change this depending on your experiment results; 
 //STEVEN : Changed this to 0 based of testing with the Rho Team. 
 //Car Motor cuts of when the threshold returns to 0.   
int switch_state;
unsigned long switchOnTime;
unsigned long currentTime;
int flag = 1; // 1 = flip has been switched to ON, 0 = flip has been switched to OFF
bool stoppingFlag = 0;  // 1 = never rerun anything, 0 = reaction still going, continue operations
bool setupDelay=false;   //used to allow the car 8s only at the start of the set-up, and after initial setup, the loop of the car doesn't cause 8s delay to repeat.




void setup()
{
  pinMode(linear_actuator_IN1, OUTPUT);
  pinMode(linear_actuator_IN2, OUTPUT);
  pinMode(pump_sleep, OUTPUT); 
  pinMode(car_sleep,OUTPUT);
   
  pinMode(motor_pin, OUTPUT);
  pinMode(unused_pin, OUTPUT);
  pinMode(switch_pin, INPUT_PULLUP);
  Serial.begin(115200);

  if (!as7341.begin()) {
    Serial.println("Could not find AS7341");
    while (1) {
      delay(10);
    }
  }

  // Total integration time = (ATIME + 1) * (ASTEP + 1) * 2.78 µS
  // Set initial scale (would be overriden later)
  //STEVEN: USING HARDCODED VALUES FOR ATIME & ASTEP after testing with the Rho Team. 
  as7341.setATIME(ATIME_VAL); // integration time per step in increments of 2.78 us, range: 0 to 255
  as7341.setASTEP(ASTEP_VAL); // number of integration steps, range: 0 to 65535
  as7341.setGain(AS7341_GAIN_512X); // Gain can increase sensitivity, STEVEN: Left unchanged during testing by the Rho Team. 
  Serial.print("setup done");
}

void waitForReswitch() {
  Serial.println("Threshold reached, car is stopping");
  digitalWrite(pump_sleep,LOW);
  digitalWrite(motor_pin, LOW);  // turn OFF motor

  digitalWrite(linear_actuator_IN1, HIGH);  // force linear actuator up
  digitalWrite(linear_actuator_IN2, LOW);

  while (1) {
     Serial.println("CAR Stopped, Waiting for switch to be reset..");
    if (digitalRead(switch_pin) == 1) return;  // breaks loop if switch is reset
  }
}


void loop()
{
  if (!as7341.readAllChannels()) {
    Serial.println("Error reading all channels!");
    return;
  }

  // Uncommenting the line below will tell you the integration time in milliseconds
  // regular_print(as7341.getTINT());

  //STEVEN: CHANGED THE CHANNEL TO DETECT THE BLUE WAVELENGTH OF LIGHT, BASED OF THIS YEAR'S REACTION
  //measured_intensity = as7341.getChannel(AS7341_CHANNEL_480nm_F6);
  switch_state = digitalRead(switch_pin);
  

  /*
  Motor pin will go high if the switch is turned on for the linear actuator. To prevent the car motor from prematurely running,
  plug motor into battery only when you want to dispense liquid.
  */

//STEVEN: IMPORTANT NOTE: CAR STARTS MOVING THE MOMENT YOU TURN THE CAR ON. 
  switch (switch_state) {
    case 0:  // switch on

      Serial.println("Starting new reaction protocol:\t"); 
      Serial.println("Linear Actuator: Compressing...");
      digitalWrite(pump_sleep, HIGH);
      // Push down Linear Actuator
      digitalWrite(linear_actuator_IN1, LOW);
      digitalWrite(linear_actuator_IN2, HIGH);
      //Now the reaction has started, BUT we need to avoid the first 0
      //STEVEN: 8s THIS DELAY IS ADDED SO THAT WE CAN AVOID SENSING THE FIRST ZERO FROM THE STOPPING RHO REACTION, 
      //AND ONLY READ THE SECOND ZERO READING FROM THE REACTION.  
      //Car is only given 8s at the start for intial setup
      //Car's motors are only HIGH until the sensor starts to output readings of 0.  
    if(!setupDelay)
    {
     unsigned long int reactionStart=millis(); //holds the startTime of the reaction (specifically holds time since Arduino program started, but placing this in a variable helps us save the time.
     while (millis()-reactionStart<8000)
      {
       //execution gets stuck in this loop till 8000s
        if(digitalRead(switch_pin)==1) {break;}  //Incase we decided to stop the car using the switch
        Serial.println("IN SETUP DELAY");
      }
      //We cannot use delay(8000), since this will stop the arduino and even the linear actuator would stop as well. 
      setupDelay=true; // set to true, so that this SETUP 8s is only given once.
      Serial.println("END OF SETUP DELAY");
    }          
      /*KILL switch feature*/
      if (digitalRead(switch_pin) == 1) {
        break;
      }

      while((digitalRead(switch_pin) == 0)){
        as7341.readAllChannels();  // update channel states
        measured_intensity = as7341.getChannel(AS7341_CHANNEL_480nm_F3);  // fetch intensity value
        Serial.print("Measured Intensity:\t"); Serial.println(measured_intensity); 
        Serial.print("Threshold Intensity:\t"); Serial.println(threshold_intensity);

      if (measured_intensity!=threshold_intensity) {  // break state if switch is pulled mid-cycle  
             digitalWrite(car_sleep, HIGH);
             digitalWrite(motor_pin, HIGH);
             digitalWrite(unused_pin, LOW);
        }

      else if (measured_intensity==threshold_intensity) { /*redundant if statement but guarantees that the sensor reading is equal to the threshold reading*/
          waitForReswitch();  // hold infinite while loop until switch is repulled
          break;
        }
        // if we flip the switch again, the switch will be in 1 state, and return here. It will skip the while statement in the next iteration, and go to next case.
      }

      break; /*for the case=0 switch case statement*/

    case 1:  // switch off
      // stop motor
      digitalWrite(pump_sleep, LOW);  //disconnect H-Bridge of the motor driver
      digitalWrite(motor_pin, LOW);
      digitalWrite(unused_pin, LOW);

      // Extend linear actuator
      Serial.println("Switch OFF");
      digitalWrite(linear_actuator_IN1, HIGH);
      digitalWrite(linear_actuator_IN2, LOW);
      break;
  }
}

