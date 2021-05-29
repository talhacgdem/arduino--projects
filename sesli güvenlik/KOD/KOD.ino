#include <SoftwareSerial.h> // GSM Seri haberleşmesi için kütüphanemiz
SoftwareSerial gsm(3, 2); // GSM Seri haberleşmesi için yazılımsal haberleşme pinleri
String numara = "+905550005500"; // SMS ile bilgi göndereceğimiz telefon numarası
int gsmPower = 5; // GSM Modülünü yalnızca gerektiğinde kullanmak için güç alacağı pini numarası

int LDR = A0; // LDR sensörün pin numarası
int kapiMotor = 4; // Kapıyı açmak için gerekli motor pin numarası

byte ses = 0; // Ses tanıma modülünden gelen verinin tutulacağı değişken
byte sesKapiAcilsin = 0x13; // Ses tanıma için eşleşme gereken ifade

void setup() {
  Serial.begin(9600); // Seri haberleşme için bant genişliğinin ayarlanması
  delay(2000); // Bekleme süresi
  /*
     Ses tanıma modülünün kendi  dökümanlarında bulunan
     başlatma ifadelerinin kodları
  */
  Serial.write(0xAA);
  Serial.write(0x37);
  delay(1000);
  Serial.write(0xAA);
  Serial.write(0x21);
  /*
     Ses tanıma modülü tanımlamaları sonu
  */

  pinMode(kapiMotor, OUTPUT); // Kapı açılışı için kullanılacak motor pini çıkış olarak ayarlandı

  pinMode(gsmPower, OUTPUT); // GSM modülünü açıp kapatmak için kullanacağımız pin çıkış olarak ayarlandı
  gsm.begin(19200); // GSM modülünün seri haberleşme bant genişliği değeri
}

void loop() {
  if (analogRead(LDR) <= 50) { // Eğer lazer ile LDR sensörü arasına engel gireerse LDR sensörü değeri düşük olacaktır
    sesOku(); // Engel girmesi durumunda ses ile doğrtulama için yazdığımız fonksiyonu çağırıyoruz
  }
}


void sesOku() { // Ses doğrulama fonksiyonu
  while (Serial.available()) // Ses tanıma modülü işlem yapabildiği sürece bu kod bloğu çalışacak
  {
    ses = Serial.read(); // Ses tanıma modülünden veriyi alıyoruz
    if (ses != sesKapiAcilsin) { // Eğer gelen ses verisi elimizdeki değerle eşleşmezse bu kod bloğu çalışacak
      delay(200); // 200ms bekleme süresi atıyoruz
      digitalWrite(9, HIGH); // GSM modülünü başlatmak için pini HIGH yapıyoruz
      mesajGonder(); // Mesaj gönderme fonksiyonumuzu çağırıyoruz
      delay(2000); // işlem sonrası 2sn bekleme atıyoruz
      digitalWrite(9, LOW); // GSM modülünü gereksiz kullanım için kapatıyoruz
      break; // Döngüyü kırıp WHILE içerisinden çıkıyoruz
    } else { // Eğer gelen ses verisi elimizdekiyle eşleşirse, yani ev sahibi tarafından gelen ses ise çalışacak kod bloğu
      kapiAc(); // Kapının açılması için gerekli fonksiyonu çağırıyoruz
      break; // Döngüden çıkmak için kırıyoruz
    }
  }
}

void mesajGonder() { // Belirlediğimiz numaraya mesaj göndermek için yazdığımız fonksiyon
  gsm.println("AT+CMGF=1"); // GSM modülü için gerekli AT komutlaı modüle gönderiliyor
  delay(1000); // 1sn bekleme süresi
  gsm.println("AT+CMGS=\"" + numara + "\"\r"); // Mesaj gönderilecek telefon numarası AT komutları ile GSM modülüne iletiliyor
  delay(1000); // 1sn bekleme süresi
  gsm.println("Bir yabancı şu anda k1apınızda... 155'i aramanız önerilir."); // Gerekli mesajı GSM modülüne yönlendiriyoruz
  delay(100); // 100ms bekleme süresi
  gsm.println((char)26); // GSM modülüne mesajın gönderildiğini bildirmek için bildirim gönderiyoruz
  delay(1000); // 1sn bekleme süresi
}

void kapiAc() { // Kapının açılması için yazdığımız fonksiyon
  digitalWrite(kapiMotor, HIGH); // Kapı motorunun çalışması için pini HIGH yapıyoruz
  delay(10000); // Kapı 10sn açık kalıyor
  digitalWrite(kapiMotor, LOW); // Kapıyı tekrar kilitlemek için motoru durduruyoruz, pin LOW
}
