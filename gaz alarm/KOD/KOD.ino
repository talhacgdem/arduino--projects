int esikgazDegeri = 400; //Gaz eşik değerini belirliyoruz.
int gazDeger; //Sensörden okunan değer
int gazSensor = A0; // Gaz Sensörü için pin numarası

int alevSensor = 4; // Alev Sensörü için pin numarası
int alevDeger = 0; // Alev sensöründen gelen değer

int yagmurPin = A1; //Sensörü bağlayacağımız pin
int sondurmeEsikDegeri = 100; //Su miktarı için eşik değeri
int sondurmeVeri; //Sensörden okuduğumuz değer

int buzzerPin = 6; //Buzzerın takılacağı pin
int suMotoru = 5; //Su motorunun takılacağı pin

void setup() {
  pinMode(buzzerPin, OUTPUT); //Buzzer pinimizi çıkış olarak ayarlıyoruz.
  pinMode(alevSensor, INPUT); // Alev sensör pinimizi giriş olarak ayarladık
  pinMode(suMotoru, OUTPUT); // Su motoru pinimizi çıkış olarak ayarladık
}

void loop() {

  // EĞER GAZ SEVİYESİ EŞİK DEĞERİNİ AŞARSA BUZZER İLE SİNYAL ALARMI VERİLİR
  // KOŞUL SAĞLANMAZSA BUZZER ÇALIŞMAZ
  gazDeger = analogRead(gazSensor);           //Sensörden analog değer okuyoruz.
  if (gazDeger > esikgazDegeri) {         //Sensörden okunan değer eşik değerinden büyükse çalışır.
    digitalWrite(buzzerPin, HIGH);
    delay(100);
    digitalWrite(buzzerPin, LOW);
    delay(100);
    // Burada buzzer sinyal sesi gibi duraklayarak çalışmakta
  }
  else {                            //Sensörden okunan değer eşik değerinin altındaysa çalışır.
    digitalWrite(buzzerPin, LOW);
  }


  // EĞER ALEV ALGILANIRSA SU MOTORU ÇALIŞIR,
  // SU SEVİYESİ EŞİK DEĞERİNE ULAŞINCAYA KADAR BUZZER ALARM VERİR - SU MOTORU ÇALIŞIR,
  // EĞER SU SEVİYESİ EŞİK DEĞERİNE ULAŞIRSA ALEV SÖNMÜŞ OLUR - BUZZER SUSTURULUR - SU MOTORU DURUR
  alevDeger = digitalRead(alevSensor);
  delay(200);
  if (alevDeger == 0) {
    digitalWrite(buzzerPin, HIGH); // Burada buzzer acil durum sesi gibi sürekli çalışmakta

    sondurmeVeri = analogRead(yagmurPin); //Sensörden analog veriyi okuyoruz.
    if (sondurmeVeri > sondurmeEsikDegeri) { //Sensör verisi eşik değerini geçerse if içerisindeki kodlar uygulanır.
      digitalWrite(buzzerPin, LOW);
      digitalWrite(suMotoru, LOW);
    }
    else {                           //Sensör verisi eşik değerinden küçük olursa if içerisindeki kodlar uygulanır.
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(suMotoru, HIGH);
    }

  }
}
