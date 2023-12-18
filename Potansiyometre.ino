#define potpin A0
int deger = 0;
float yazilacakGerilim=0;

void setup(){

  Serial.begin(9600);
  Serial.println("Pot Deger Okuma");
}

void loop(){
  
  deger = analogRead(potpin);

  float gerilim=(5.00/1024.00)*deger;

  if(yazilacakGerilim != gerilim){
    Serial.println(gerilim);
    yazilacakGerilim=gerilim;
    delay(1000);
  }
}