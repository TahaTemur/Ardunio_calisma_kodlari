#include<LiquidCrystal.h>
LiquidCrystal lcd(7,6,9,10,11,12);

int tonePin = 2;
int toneFreq = 1000;
int ledPin = 13;
int deLedPin = 4; // Silme Butonu Işığı
int buttonPin = 8;
int deletePin = 3; // Silme Butonu
int debounceDelay = 30;
int dotLength = 240; 


  int dotSpace = dotLength;
  int dashLength = dotLength*3;
  int letterSpace = dotLength*3;
  int wordSpace = dotLength*7; 
  float wpm = 1200./dotLength;
  
int t1, t2, onTime, gap;
bool newLetter, newWord, letterFound, keyboardText;
int lineLength = 0;
int maxLineLength = 16; 

bool lcdAvailable = false; // Ekran Yazı Kontrolü

char* letters[] = 
{
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." 
};

char* numbers[] = 
{
"-----", ".----", "..---", "...--", "....-", 
".....", "-....", "--...", "---..", "----." 
};

String dashSeq = "";
char keyLetter, ch;
int i, index;

void setup() 
{
  lcd.begin(16,2); //LCD Ekran boyutu tanımlar
  delay(500);
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(deletePin, INPUT_PULLUP);
  Serial.begin(9600);

  lcd.clear(); //Eğer Başlangıçta Ekranda Yazı Olursa Siler
  lcd.setCursor(0, 0); // Ekranın 1. Satırı
  lcd.print("Mors Kod Ceviri");
  lcd.setCursor(0, 1); // Ekranın 2. Satırı
  lcd.print("Aktif");
  lcdAvailable = true; // Ekranda Yazı Olduğunu Belirler
  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("Merhaba Ben Sanal Morse Çevirmeniyim");
  Serial.println("Kelime Veya Morse Kodu Giriniz : ");
  Serial.println("-------------------------------");
      
  newLetter = false; 
  newWord = false;  
  keyboardText = false; 
}

void loop(){
// Silme Butonu
  if (digitalRead(deletePin) == LOW ) {
    digitalWrite(deLedPin, HIGH);
    delay(300);
    digitalWrite(deLedPin, LOW);
    Serial.println("         ");
    lcd.clear();
  }
  
  if (Serial.available() > 0)
  {
    //Ekranda yazı mevcut ise silme işlemi gerçekleştirir
     if (lcdAvailable == true) {
    lcd.clear();
    lcdAvailable = false;
    }

    if (keyboardText == false) 
    {
      Serial.println();
      Serial.println("-------------------------------");
    }
    keyboardText = true;
    ch = Serial.read();
    if (ch >= 'a' && ch <= 'z')
    { ch = ch-32; }
    
    if (ch >= 'A' && ch <= 'Z')
    {
      Serial.print(ch); 
      lcd.print(ch);
      Serial.print(" ");
      Serial.println(letters[ch-'A']);
      flashSequence(letters[ch-'A']);
      delay(letterSpace);
    }
    if (ch >= '0' && ch <= '9')
    {
      Serial.print(ch);
      lcd.print(ch);
      Serial.print(" ");
      Serial.println(numbers[ch-'0']);
      flashSequence(numbers[ch-'0']);
      delay(letterSpace);
    }

    if (ch == ' ')
    {
      lcd.print(" ");
      Serial.println("_");
      delay(wordSpace);    
    } 


     if (Serial.available() <= 0) 
     {
      Serial.println();
      Serial.println("Morse Kodu Çevirildi.");
      Serial.println("-------------------------------");
      keyboardText = false;
      delay(3000); //Yazı görüntülemek için süre
      lcdAvailable = true; // Ekrana yazı yazıldıktan bir süre sonra aktif olur ve yazı silinir
     }
      
      //Ekranı Temizler
      if (lcdAvailable == true) {
    lcd.clear();
    lcdAvailable = false;
    }
      
  }
  

  if (digitalRead(buttonPin) == LOW ) 
  {
    newLetter = true; 
    newWord = true;
    t1=millis(); 
    digitalWrite(ledPin, HIGH); 
    tone(tonePin, toneFreq);
    delay(debounceDelay);     
    while (digitalRead(buttonPin) == LOW ) 
      {delay(debounceDelay);}
      delay(debounceDelay);
       
    t2 = millis();  
    onTime=t2-t1;  
    digitalWrite(ledPin, LOW); 
    noTone(tonePin); 
    


    if (onTime <= dotLength*1.5) 
      {dashSeq = dashSeq + ".";} 
    else 
      {dashSeq = dashSeq + "-";}
  }  
  


  gap=millis()-t2; 
  if (newLetter == true && gap>=letterSpace)  
  { 
    if (lcdAvailable == true) {
    lcd.clear();
    lcdAvailable = false;
    }

    letterFound = false; keyLetter = 63; 
    for (i=0; i<=25; i++)
    {
      if (dashSeq == letters[i]) 
      {
        keyLetter = i+65;
        letterFound = true;   
        break ;    
      }
    }
    if(letterFound == false) 
    {
      for (i=0; i<=10; i++)
      {
      if (dashSeq == numbers[i]) 
        {
          keyLetter = i+48;
          letterFound = true;   
          break ;     
        }
      }
    }   
     

    lcd.print(keyLetter);
    Serial.print(keyLetter);

    if(letterFound == false) 
    {
      tone(tonePin, 100, 500);
    }  
    newLetter = false; 
    dashSeq = "";
    lineLength=lineLength+1;
  }  
  

  if (newWord == true && gap>=wordSpace*1.5)
    { 
     newWord = false; 
     lcd.print("_");
     Serial.print("_");  
     lineLength=lineLength+1;
     


    digitalWrite(ledPin, HIGH);
    delay(25);
    digitalWrite(ledPin, LOW);       
    } 



  if (lineLength >= maxLineLength) 
    {
      Serial.println();
      lineLength = 0;
    }      
} 

void flashSequence(char* sequence)
{
  int i = 0;
  while (sequence[i] == '.' || sequence[i] == '-')
  {
    flashDotOrDash(sequence[i]);
    i++;
  }
}

void flashDotOrDash(char dotOrDash)
{
  digitalWrite(ledPin, HIGH);
  tone(tonePin, toneFreq);
  if (dotOrDash == '.')
   { delay(dotLength); }
     else
   { delay(dashLength); }

  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(dotLength); 
}
