/*

Code Version 9.2:- implement Interrupt for reading digital pin 10 (Works in combination with "digital_write_001.ino")
Code Version 9.1:- Read Digital Read Pin From other Arduino Nano (Which is Reading PWM from FC or Reciver)
Code Version 9 :- Falling back to general (not using accel lib)
Code Version 8 :- Facing an issue with reading PWM , implementing knob(Hw 040) for increasing and decreasing the pwm val manually.
                  Code implementations from "dsr_nema23_002_2.ino"
Code Version 7 :- Activate the seed picker only when pwm read is above 1200pwm
Code Version 6 :- Avoid using pulsein and implement ISR
Code Version 5 :- Recheck accel lib , rpm calculation in loop
Code Version 4 :- Torque calculation on bench (Running Torque 2kgcm @3200SPR @1.5amp @93.75rpm )
Code Version 3 :- micro step selection based in range 5rpm to 100rpm
Code Version 2 :- implement RPM calculator in setup section for the micro step and dt selected
Code Version 1 :- Check accel lib and micro stepping 
---------------- Components Used ----------------
Driver Used  = TB6600
Stepper Used = Nema 17, 42HS48-1684, 
Gear Box = No
Micro Step used = 1/16 (Full Step Mode - 200SPR) S1 = 1, S2 = 1, S3 = 0
Current Set = 1.5Amp  S4 = 1, S5 = 1, S6 = 0
Input Volatge = 12v (8amp) for driver 
Arduino = Nano original
12v to 5v buck (for powering Nano)
Rotary encoder input = Hw040 30pulses per 1 complete rotation
--------------------------------------------------

-------------- Connections -------------------
TB6600 to Nano
 ENA- , DIR- and PUL-  ->   gnd of arduino
 ENA+                  ->   D7
 DIR+                  ->   D8
 PUL+                  ->   D9 

Encode Knob to nano 
Clk O/p A             ->    D2
Dt  O/p B             ->    NC
Sw                    ->    NC
5v                    ->    5v
GND                   ->    GND
-----------------------------------------------


Created on :- 09-02-2026
Created by :- K Madhu Mohan Chary 
For code enquiry :- madhu@marutdrones.com 

*/
#include <AccelStepper.h>

// Used Common cathode setup on driver
const byte pul_pin = 9; //pulse pin 
const byte dir_pin = 8; //direction pin
const byte ena_pin = 7; //enable pin


//micro step selection based in range 5rpm to 100rpm
unsigned long SPR = 3200; //Steps per rotation (unsigned long for rpm calculation)
unsigned long dt = 100; //micro seconds
int dt_min = 100; //100 -> 93.75rpm or 1.5625rps (0.5kgcm holding , 2kgcm running torque) @3200SPR @1.5amp (or)   200 -> 93.75 @1600SPR
int dt_max = 1000; //1000 -> 9.38rpm or 0.1563rps (1.5kgcm holding, 2.5kgcm running torque) @3200SPR @1.5amp (or)  2000 -> 9.38 @1600SPR
float rpm ; //rpm must be float or double 


//Torque 
int read_pos = 0; // for incoming serial data

//accel
float accel_lib_rpm ;
int set_step_per_sec = 0;
int min_step_per_sec = 500; // 93.75 RPM @3200 SPR
int max_step_per_sec = 5000;// 9.38 RPM @3200 SPR

//picker
//int min_seeder_pwm = 1051; Updated in knob section
int max_seeder_pwm = 1951;
unsigned long read_seeder_pwm = 0;


//cutoff pwm
unsigned long cut_off_pwm = 1200;


//knob 
volatile byte en_clk_pin = 2;//interrupt pin
volatile int pwm_incre_decre_factor = 50;
volatile int pwm_incre_decre = 0;
volatile int min_seeder_pwm = 1051;



//micro
unsigned long curr_micro = 0;
unsigned long prev_micro = 0;
volatile bool state = HIGH;

volatile int dt_micro = 100;
volatile int dt_micro_min = 100;
volatile int dt_micro_max = 1000;
volatile int dt_micro_factor = 10;

//digital Read from other (nano)
volatile byte digital_read_pin = 3;
volatile bool digital_read_val = LOW;


void change_dt_micro()
{
    dt_micro = dt_micro + dt_micro_factor;

    if(dt_micro > dt_micro_max)
    {
      dt_micro = dt_micro_min;
    }
}

void read_digital_pin()
{
  digital_read_val = digitalRead(digital_read_pin);
}

void setup() {
  pinMode(pul_pin,OUTPUT);
  pinMode(dir_pin,OUTPUT);
  pinMode(ena_pin,OUTPUT);
  pinMode(en_clk_pin,INPUT);
  pinMode(digital_read_pin,INPUT);

  digitalWrite(ena_pin,LOW);
  digitalWrite(dir_pin,LOW);
  Serial.begin(9600);

  delay(100);
  
  //seed_stepper.setMaxSpeed(max_step_per_sec);// in steps per second = 3200 * 1.5625 = 5000 (max steps per second) @93.75rpm
  // 3200 * 0.1563 = 500 (min steps per second) @9.38rpm
  rpm = 30000000.0 / (SPR * dt); //importent
  
  Serial.print("With SPR = ");
  Serial.print(SPR);
  Serial.print(" and dt = ");
  Serial.print(dt);
  Serial.print(" Calculated RPM = ");
  Serial.println(rpm);
  attachInterrupt(digitalPinToInterrupt(en_clk_pin),change_dt_micro,RISING); 
  attachInterrupt(digitalPinToInterrupt(digital_read_pin),read_digital_pin,HIGH);  //Dont Change "HIGH"         
                
}

void loop()
{
    
   if(digital_read_val == HIGH)
   { 
    curr_micro = micros();
    if((curr_micro - prev_micro) >= dt_micro)
    {
        state = !state;
        digitalWrite(pul_pin,state);
        //Serial.println(state);
        prev_micro = curr_micro;
    }
   }

}





