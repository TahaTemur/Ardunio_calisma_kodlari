#include <Servo.h>
#define pot_pin A0
#define servo_pin 5

Servo servo_motor;

int pot_deger;

 
void setup() {
  servo_motor.attach(servo_pin);

}

void loop() {
  pot_deger = analogRead(pot_pin);
  pot_deger = map(pot_deger,0,1023,0,180);

  servo_motor.write(pot_deger);
  delay(15);

}
