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
#define car_sleep 8 

#define ASTEP_VAL 2999
#define ATIME_VAL 50 


void setup()
{
  pinMode(linear_actuator_IN1, OUTPUT);
  pinMode(linear_actuator_IN2, OUTPUT);
  pinMode(pump_sleep,OUTPUT);
  pinMode(motor_pin, OUTPUT);
  pinMode(unused_pin, OUTPUT);
  //pinMode(switch_pin, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.print("SetUp");
}

void loop() 
{
    Serial.println("IN Main Loop");
    //digitalWrite(motor_pin, HIGH);
    //digitalWrite(unused_pin, LOW);

    // Push down Linear Actuator
    //RUN #1
    digitalWrite(pump_sleep,HIGH);
    digitalWrite(linear_actuator_IN1, LOW);
    digitalWrite(linear_actuator_IN2, HIGH);

   delay (3000); //wait for 3s before next RUN#1
    //RUN #2: Opposite Direction
    //digitalWrite(pump_sleep,HIGH);
    digitalWrite(linear_actuator_IN1, HIGH);
    digitalWrite(linear_actuator_IN2, LOW); 
}
