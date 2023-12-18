int LED_Pin=13;
void setup() {
  Serial.begin(9600);
  pinMode(LED_Pin,OUTPUT);

}


void loop() {
  if(Serial.available()){
    char gelen_bilgi=Serial.read();
    if(gelen_bilgi=='1'){
      
      digitalWrite(LED_Pin,HIGH);
    }
    else if(gelen_bilgi=='0'){
      digitalWrite(LED_Pin,LOW);
    }
  }
  delay(100);
}
