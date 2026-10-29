#include <Servo.h>
Servo my_servo;  


int minim = 10;
int maxim = 170;


void setup() {
  my_servo.attach(2);  
  my_servo.write(minim);                  
delay(3000);
  my_servo.write(maxim); 
delay(3000);
}

void loop() {
  my_servo.write(90);                
  
}
