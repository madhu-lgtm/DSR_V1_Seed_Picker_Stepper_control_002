/*
Version 2:- Try to read PWM From Servo tester or FC or Reciver
Version 1:- Produce a fixed delay in between HIGH and LOW digital Write (Works with "dsr_nema17_42hs48_1684_009_02.ino")

------------- Connections ------------------------
nano (pwm Reader)               nano (Seed picker)
GND                    ->       GND
D11 - 10k resistor     ->       D3      

nano (pwm Reader)               Servo Tester / FC / Reciver
D9(pwm read pin)       ->       Out Signal (Use pump Signal)
GnD                    ->       Gnd

nano (pwm Reader)               5v UBEC 
Vin                    ->       5v pin
GND                    ->       Gnd
------------------------------------------------------------

*/

int digital_write_pin = 11;
int dt = 1000;
int pwm_read_pin = 9;
int pwm_val ;

void setup() {
  // put your setup code here, to run once:
  pinMode(digital_write_pin,OUTPUT);
  pinMode(pwm_read_pin,INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  pwm_val = pulseIn(pwm_read_pin,HIGH);
  if(pwm_val > 1500)
  {
    digitalWrite(digital_write_pin,HIGH);
  }
  else
  {
    digitalWrite(digital_write_pin,LOW);
  }

}
