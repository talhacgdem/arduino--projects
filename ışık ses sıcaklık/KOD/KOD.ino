#include <TinyGPS++.h>
#include <SoftwareSerial.h>
int RXPin = 2;
int TXPin = 3;
int GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

#include <SPI.h> // SPI kütüphanesi tanımlanıyor
#include <SD.h>  // SD kart kütüphanesi tanımlanıyor

File dosya; // dosya adında bir File nesnesi oluşturuluyor

String okunan_lm35 = "", okunan_ldr = "", okunan_gps = ""; // String türünde üç adet değişken tanımlanıyor

int LM35 = A0; // analog pin 0 LM35 bağlanıyor
int LDR = A1;  // analog pin 1 LDR bağlanıyor

unsigned long eski_zaman;          // delay olmadan bekleme oluşturmak için gerekli değişkenler
const long bekleme_suresi = 10000; // 10 saniye yani 10000 milisaniyelik bekleme süresi tanımlanıyor

// LM35 sıcaklık ölçümü fonksiyonu
String LM35_oku()
{
  okunan_lm35 = "";
  int bilgi = analogRead(LM35);
  float sicaklik = bilgi * 0.48828125;
  okunan_lm35 = String(sicaklik);
  return okunan_lm35;
}

// LDR analog ölçüm fonksiyonu
long LDR_oku()
{
  okunan_ldr = "";
  int bilgi = analogRead(LDR);
  float isik = bilgi;
  okunan_ldr = String(isik);
  return okunan_ldr;
}

String GPS_oku()
{
  String gpsVeri = "";
  if (gps.location.isValid())
  {
    gpsVeri += "Enlem: " + String(gps.location.lat()) + " --- ";
    gpsVeri += "Boylam: " + String(gps.location.lng()) + " --- ";
  }
  else
  {
    gpsVeri += "Konum sağlanamadı";
  }

  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    gpsVeri += gps.time.hour() + ":";
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    gpsVeri += gps.time.minute();
  }
  else
  {
    gpsVeri += "Zaman Saptanamadı...";
  }

  return gpsVeri;
}

void setup()
{
  Serial.begin(115200); // 115200baud hızında serial iletişim başlatılıyor

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  Serial.println("SD KART DATALOGGER UYGULAMASI");

  // SD kartın takılıp takılmadığını kontrol eden program bloğu
  // takılı değilse SD kart takılana kadar bekler
  // takılınca program kaldığı yerden devam eder
  // SD.begin(4) yazıldığı için 4 nolu dijital pin SD kart modülünün CS bacağına bağlanmalıdır
  Serial.println("SD kart hazirlaniyor...");
  if (!SD.begin(4))
  {
    Serial.println("SD kart takili degil!!!");
    Serial.println("Lutfen SD kartinizi takiniz!!!");
    while (!SD.begin(4))
      ;
    Serial.println("SD kart kullanima hazir!!!");
    delay(1000);
  }
  else
  {
    Serial.println("SD kart kullanima hazir!!!");
    delay(1000);
  }

  SD.mkdir("DATALOGGER"); // LM35 ölçüm sonuçlarının kaydedileceği klasör oluşturuluyor
}

void loop()
{
  // kod bloğu bekleme işlemini kontrol etmektedir.
  // belirlenen bekleme süresi dolduğunda Ölçümler her üç sensör için
  // bir kez alınır ve SD kartta ilgili klasörde kayıt edilir

  unsigned long simdiki_zaman = millis();
  if (simdiki_zaman - eski_zaman >= bekleme_suresi)
  {
    eski_zaman = simdiki_zaman;

    dosya = SD.open("DATALOGGER/data.txt", FILE_WRITE);

    if (dosya)
    {
      dosya.println("Bulunduğunuz konuma dair veriler işleniyor....");
      dosya.println("Konumunuz ...:");
      dosya.println(GPS_oku());
      dosya.println("Gün ışığı değeri ...:");
      dosya.println(LDR_oku());
      dosya.println("Sıcaklık değeri ...:");
      dosya.println(LM35_oku());
      dosya.println("--------------------------------------------------");
    }

    dosya.close();
  }
}
