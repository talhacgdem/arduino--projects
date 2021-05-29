int echo = 9;  // Mesafe sensörünün Echo pini
int trig = 10; // Mesafe sensörünün Trig pini
int mesafeSonuc;

int buzzerPin = 2; //Buzzerın takılacağı pin
int role = 12; // Yağmur sensörüne bağlı olarak güç ileteceğimiz ve silecekleri çalıştıracağımız röle pini

int carpismaSensoru = A1; // Yaylı darbe sensörü için pin
int carpismaDegeri; // Yaylı darbe sensörünün değerini tutacağımız değişken
int carpismaEsik = 100; // Yaylı darbe sensörü için eşik değeri

int yagmurSensoru = A0; // Yağmur sensörü için pin
int yagmurDegeri; // Yağmur sensörünün değerini tutacağımız değişken
int yagmurEsik = 100; // Yağmur sensörü için eşik değeri

long mesafeOlcum() { // Mesafe ölçüm fonksiyonu, sabit bir fonksiyon internette her yerde bu fonksiyon kullanılır
  long sure, mesafe;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  sure = pulseIn(echo, HIGH);
  mesafe = (sure / 2) / 29.1;
  return mesafe;
}

void setup() {
  pinMode(buzzerPin, OUTPUT); // Buzzer pini çıkış
  pinMode(role, OUTPUT); // Role pini çıkış
  pinMode(echo, INPUT); // Mesafe sensörünün ECHO pini giriş
  pinMode(trig, OUTPUT); // Mesafe sensörünün TRIG pini çıkış
}

void loop() {
  carpismaDegeri = analogRead(carpismaSensoru); // Sensörden çarpışma değerini okuyoruz
  if (carpismaDegeri > carpismaEsik) { // Eğer çarpışma durumu eşiğin üstündeyse buzzer çalışacak
    digitalWrite(buzzerPin, HIGH);
  } else { // Eğer çarpışma durumu eşiğin altındaysa buzzer çalışmayacak
    digitalWrite(buzzerPin, LOW);
    mesafeSonuc = mesafeOlcum(); // Sensörden mesafe bilgisi alıyoruz
    
    // ALTTAKİ İF BLOĞU MESAFE DEĞERİNE GÖRE BUZZER'dan BELİRLİ SİNYAL SESLERİNİ ÇIKARMAK İÇİN
    // EĞER ÇOK YAKINDA HIZLI, UZAKSA YAVAŞ ÇALMASI İÇİN
    if (mesafeSonuc < 40) { 
      digitalWrite(buzzerPin, HIGH);
      delay(200);
      digitalWrite(buzzerPin, LOW);
      delay(200);
    } else if (mesafeSonuc < 20) {
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
    } else if (mesafeSonuc < 10) {
      digitalWrite(buzzerPin, HIGH);
      delay(20);
      digitalWrite(buzzerPin, LOW);
      delay(20);
    } else { // Ve mesafe aralığında değilse buzzer çalışmayacak
      digitalWrite(buzzerPin, LOW);
    }
  }

  yagmurDegeri = analogRead(yagmurSensoru); // Sensörden yağmur verisini alıyoruz
  if (yagmurDegeri > yagmurEsik) { //Eğer yağmur eşiğini aşıyorsak sileceklerin çalışması için RÖLEye güç veriyoruz
    digitalWrite(role, HIGH);
  } else {  //Eğer yağmur eşiğini aşmıyorsak sileceklerin durması için RÖLEye giden gücü kesiyoruz
    digitalWrite(role, LOW);
  }
}
