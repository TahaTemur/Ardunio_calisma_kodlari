#include<LiquidCrystal.h>          // LiquidCrystal kütüphanesini programa ekler. Bu kütüphane, LCD ekran ile etkileşim sağlamak için kullanılır.
LiquidCrystal lcd(7,6,9,10,11,12); // lcd adında bir LiquidCrystal nesnesi oluşturur ve LCD ekranın bağlı olduğu pinleri tanımlar.

int tonePin = 2;		   // Ses tonu için kullanılacak pinin numarasını belirtir.
int toneFreq = 1000;		   // Çıkacak olan ses tonunun frekansını belirtir.
int ledPin = 13;		   // LED'in bağlı olduğu pinin numarasını belirtir.
int deLedPin = 4;                  // Silme butonu ışığının bağlı olduğu pinin numarasını belirtir.
int buttonPin = 8;                 // Ana butonun bağlı olduğu pinin numarasını belirtir.
int deletePin = 3; 		   // Silme butonunun bağlı olduğu pinin numarasını belirtir.
int debounceDelay = 30;	           // Butonlardan gelen gürültüyü önlemek için kullanılan debounce süresini belirtir.
int dotLength = 200;               // Bir "dot" (nokta) süresini belirtir; Morse kodunda temel bir zaman birimidir.

int dotSpace = dotLength;	   // Nokta aralığını belirtir.
int dashLength = dotLength*3;      // Tire (dash) süresini belirtir.
int letterSpace = dotLength*3;	   // Harfler arasındaki boşluğu belirtir.
int wordSpace = dotLength*7;       // Kelimeler arasındaki boşluğu belirtir.
float wpm = 1200./dotLength;       // Kelimeleri dakikada ifade edilen hızı belirtir.
  
int t1, t2, onTime, gap;           // Zaman ölçümleri için değişkenler.
bool newLetter, newWord, letterFound, keyboardText; //Kontrol flag'leri
int lineLength = 0;                // Satırdaki karakter sayısını takip etmek için kullanılır.
int maxLineLength = 16;            // Bir satırdaki maksimum karakter sayısını belirtir.

bool lcdAvailable = false;         // LCD ekranın üzerine yazı yazıldığını kontrol etmek için kullanılır.

char* letters[] =                  // Harflerin Morse kodlarını içeren bir dizi.
{
".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", 
".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", 
"...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.." 
};

char* numbers[] =                  // Rakamların Morse kodlarını içeren bir dizi.
{
"-----", ".----", "..---", "...--", "....-", 
".....", "-....", "--...", "---..", "----." 
};

String dashSeq = "";               // Dash ve dot'ların bir araya geldiği diziyi temsil eden bir String.
char keyLetter, ch; 		   // Karakter ve tuş girişini temsil eden değişkenler.	
int i, index; 			   // Döngü içinde kullanılan indis değişkenleri.	

void setup() 
{
  lcd.begin(16,2);                 // LCD ekranın boyutunu belirler. 16 sütun, 2 satır olduğunu ifade eder.
  delay(500);		           // Program başladığında 500 milisaniye (yarım saniye) boyunca bekler. Bu süre içinde ekranın başlangıç mesajları okunabilir.	 
  pinMode(ledPin, OUTPUT);         // ledPin olarak adlandırılan pini çıkış moduna ayarlar. Bu pin genellikle bir LED'i kontrol etmek için kullanılır.
  pinMode(tonePin, OUTPUT);        // tonePin olarak adlandırılan pini çıkış moduna ayarlar. Bu pin genellikle bir hoparlörü veya ses üreten bir cihazı kontrol etmek için kullanılır.
  pinMode(buttonPin, INPUT_PULLUP);// buttonPin olarak adlandırılan pini giriş moduna ayarlar ve dahili pull-up direncini kullanır. Bu genellikle bir butonun durumunu okumak için kullanılır.
  pinMode(deletePin, INPUT_PULLUP);// deletePin olarak adlandırılan pini giriş moduna ayarlar ve dahili pull-up direncini kullanır. Bu genellikle bir silme butonunun durumunu okumak için kullanılır.
  Serial.begin(9600);              // Seri haberleşmeyi başlatır. Bu, Arduino'nun USB portu üzerinden bilgisayarla iletişim kurmasına olanak tanır. 9600, baud hızını temsil eder.

  lcd.setCursor(0, 0);             				// LCD ekranın imlecinin başlangıç pozisyonunu belirler (1. satır, 1. sütun).
  lcd.print("Mors Kod Ceviri");    				// LCD ekranın 1. satırına "Mors Kod Ceviri" metnini yazar.
  lcd.setCursor(0, 1); // Ekranın 2. Satırı 			// LCD ekranın imlecinin 2. satırın başlangıç pozisyonunu belirler.
  lcd.print("Aktif");              				// LCD ekranın 2. satırına "Aktif" metnini yazar.
  lcdAvailable = true; // Ekranda Yazı Olduğunu Belirler  	// lcdAvailable değişkenini true (doğru) olarak ayarlar. Bu, ekranda yazı olduğunu belirtir.
  Serial.println();						// Bir satır boşluk bırakarak bir satırı bitirir.
  Serial.println("-------------------------------");		// Bir ayırıcı çizgi ve bir satır boşluk ekleyerek bir ayırıcı metni yazdırır.
  Serial.println("Merhaba Ben Sanal Morse Çevirmeniyim");  	// Başlangıç mesajını yazdırır.
  Serial.println("Kelime Veya Morse Kodu Giriniz : ");     	// Kullanıcıdan kelime veya Morse kodu girmesini isteyen bir mesaj yazdırır.
  Serial.println("-------------------------------");
  
  newLetter = false; 						// newLetter değişkenini false olarak ayarlar. Bu değişken, yeni bir harfin algılanıp algılanmadığını takip eder.
  newWord = false;  						// newWord değişkenini false olarak ayarlar. Bu değişken, yeni bir kelimenin algılanıp algılanmadığını takip eder.
  keyboardText = false; 					// keyboardText değişkenini false olarak ayarlar. Bu değişken, klavyeden girişin devam edip etmediğini takip eder.
}

void loop(){
    
  if (digitalRead(deletePin) == LOW ) {			        // Silme butonuna basılırsa.
    digitalWrite(deLedPin, HIGH);				// deLedPin üzerindeki LED'i yakar (HIGH).
    delay(300);                                              	// 300 milisaniye bekler.
    digitalWrite(deLedPin, LOW);    			  	// deLedPin üzerindeki LED'i söndürür (LOW).
    Serial.println("         ");				// Seri monitöre boş bir satır yazılır.
    lcd.clear();						// LCD ekran temizlenir.
  }
  
  if (Serial.available() > 0)					// Seri haberleşme portunda veri varsa:
  {
    
    if (lcdAvailable == true) {					// Eğer ekranda yazı varsa, LCD ekran temizlenir ve lcdAvailable değişkeni false olarak ayarlanır.
      lcd.clear();
      lcdAvailable = false;
    }

    if (keyboardText == false) 					// Eğer keyboardText değişkeni false ise, bir boş satır ve ayırıcı çizgi yazılır.
    {
      Serial.println();
      Serial.println("-------------------------------");
    }
    keyboardText = true;					// keyboardText değişkeni true olarak ayarlanır.
    ch = Serial.read();						// Seri haberleşme portundan bir karakter (ch) okunur.
    if (ch >= 'a' && ch <= 'z')					// Eğer okunan karakter küçük bir harfse, büyük harfe dönüştürülür.
    { 
      ch = ch-32; 						
    }
    
    if (ch >= 'A' && ch <= 'Z')					// Eğer okunan karakter bir harf ise, Seri monitöre harfi, LCD ekranına harfi, Morse kodunu ve ışık efektini yazdırılır.
    {
      Serial.print(ch);    
      lcd.print(ch);
      Serial.print(" ");
      Serial.println(letters[ch-'A']);
      flashSequence(letters[ch-'A']);
      delay(letterSpace);
    }
    if (ch >= '0' && ch <= '9')					// Eğer okunan karakter bir rakam ise, Seri monitöre rakamı, LCD ekranına rakamı, Morse kodunu ve ışık efektini yazdırılır.
    {
      Serial.print(ch);
      lcd.print(ch);
      Serial.print(" ");
      Serial.println(numbers[ch-'0']);
      flashSequence(numbers[ch-'0']);
      delay(letterSpace);
    }

    if (ch == ' ')						// Eğer okunan karakter bir boşluk ise, LCD ekranına boşluk ve alt tire yazılır.
    {
      lcd.print(" ");
      Serial.println("_");
      delay(wordSpace);    
    } 

    if (Serial.available() <= 0) 				// Eğer seri haberleşme portunda giriş yoksa
    {
      Serial.println();						// Seri monitöre bir boş satır ve "Morse Kodu Çevirildi." mesajı yazılır.
      Serial.println("Morse Kodu Çevirildi.");
      Serial.println("-------------------------------");
      keyboardText = false;					// keyboardText değişkeni false olarak ayarlanır.
      delay(3000); 						// Bu süre, yazıyı bir süre göstermek için beklenir.
      lcdAvailable = true; 					// Ekrana yazı yazıldıktan bir süre sonra aktif olur ve yazı silinir
    }
    
    if (lcdAvailable == true) {					// Eğer ekran aktifse, LCD ekran temizlenir ve lcdAvailable değişkeni false olarak ayarlanır.
      lcd.clear();
      lcdAvailable = false;
    }
  }
  
  if (digitalRead(buttonPin) == LOW ) 				// Eğer ana butona basılırsa:
  {
    newLetter = true; 						// newLetter ve newWord değişkenleri true olarak ayarlanır.
    newWord = true;
    t1=millis(); 						// t1 zamanı kaydedilir.
    digitalWrite(ledPin, HIGH); 				// ledPin üzerindeki LED açılır.			
    tone(tonePin, toneFreq);					// tonePin üzerindeki ton çalınmaya başlanır.
    delay(debounceDelay);     					// debounceDelay süresi kadar beklenir.				
    while (digitalRead(buttonPin) == LOW ) 			// Buton tekrar serbest bırakılana kadar beklenir.
    {
      delay(debounceDelay);					// Bir süre daha beklenir (debounceDelay).
    }
    delay(debounceDelay);
       
    t2 = millis();  						// t2 zamanı kaydedilir.
    onTime=t2-t1;  						// onTime hesaplanır (t2 - t1).
    digitalWrite(ledPin, LOW); 					// LED kapatılır.
    noTone(tonePin); 						// Ton durdurulur.
    
    if (onTime <= dotLength*1.5) 				// onTime süresine göre "." veya "-" karakteri dashSeq dizisine eklenir.
    {
      dashSeq = dashSeq + ".";
    } 
    else 
    {
      dashSeq = dashSeq + "-";
    }
  }  
  
  gap=millis()-t2; 
  if (newLetter == true && gap>=letterSpace)  			// Eğer yeni bir harf bekleniyorsa ve geçen süre (gap) harf aralığından büyükse
  { 
    if (lcdAvailable == true) {					// Eğer LCD ekran aktifse, ekran temizlenir.
      lcd.clear();
      lcdAvailable = false;					// Yeni harf beklentisi kapatılır (newLetter = false).
    }

    // Bu kod bloğu, bir Morse kodunu çözerek karşılık gelen harfi veya karakteri bulur, bu karakteri ekrana ve seri monitöre yazdırır, eğer karakter bulunamazsa bir uyarı tonu çalar.
    letterFound = false; 
    keyLetter = 63; // "?" karakteri				// letterFound ve keyLetter değişkenleri başlangıçta false ve 63 olarak ayarlanır (ASCII'de "?" karakteri)
    for (i=0; i<=25; i++)
    {
      if (dashSeq == letters[i]) 
      {
        keyLetter = i+65;					// Morse kodu, harf dizisinde (letters) aranır.							 
        letterFound = true;   					// Eğer bulunursa, keyLetter değişkenine karşılık gelen ASCII karakteri atanır ve letterFound true yapılır.
        break ;    
      }
    }
    if(letterFound == false) 					// Eğer harf dizisinde bulunamazsa, rakam dizisinde (numbers) aranır.
    {								// Eğer bulunursa, keyLetter değişkenine karşılık gelen ASCII karakteri atanır ve letterFound true yapılır.
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
    
    lcd.print(keyLetter);					// LCD ekran ve seri monitöre çözülen karakter (keyLetter) yazdırılır.
    Serial.print(keyLetter);

    if(letterFound == false) 					// Eğer karakter bulunamamışsa
    {
      tone(tonePin, 100, 500);					// uyarı tonu çalınır (tone(tonePin, 100, 500)).
    }  
    newLetter = false; 						// newLetter false yapılır.
    dashSeq = "";						// dashSeq (Morse kodunu tutan dizi) sıfırlanır.
    lineLength=lineLength+1;					// lineLength bir artırılır.
  }  
  
  if (newWord == true && gap>=wordSpace*1.5)			// Eğer yeni bir kelime bekleniyorsa ve geçen süre (gap) kelime aralığından büyükse:
  { 
    newWord = false; 						// newWord kapatılır (false).
    lcd.print("_");						// LCD ekranına ve seri monitöre alt tire ("_") yazılır.
    Serial.print("_");  
    lineLength=lineLength+1;					// lineLength bir artırılır.
	
    digitalWrite(ledPin, HIGH);					// ledPin üzerindeki LED yakılır (HIGH).
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
