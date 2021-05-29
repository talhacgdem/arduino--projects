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

String okunan_sicaklik = "", okunan_gps = ""; // String türünde iki adet değişken tanımlanıyor

// Hastaya göre normal seviyenin altı
// ve üstü nabız değerleri tanımlanıyor
int nabizAlt = 60;
int nabizUst = 120;

int LM35 = A1;
int nabizPin = A0;

// Nabız ölçüm fonksiyonu
int NABIZ_oku()
{
  int nabiz = analogRead(nabizPin);
  return nabiz;
}

String GPS_oku()
{
  okunan_gps = "";
  if (gps.location.isValid())
  {
    okunan_gps += "Enlem: " + String(gps.location.lat()) + " --- ";
    okunan_gps += "Boylam: " + String(gps.location.lng()) + " --- ";
  }
  else
  {
    okunan_gps += "Konum sağlanamadı";
  }

  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10)
      okunan_gps += gps.time.hour() + ":";
    if (gps.time.minute() < 10)
      okunan_gps += gps.time.minute();
  }
  else
  {
    okunan_gps += "Zaman Saptanamadı...";
  }

  return okunan_gps;
}

// Sıcaklık ölçümü fonksiyonu
String SICAKLIK_oku()
{
  okunan_sicaklik = "";
  int bilgi = analogRead(LM35);
  float sicaklik = bilgi * 0.48828125;
  okunan_sicaklik = String(sicaklik);
  return okunan_sicaklik;
}

void setup()
{
  Serial.begin(115200); // 115200baud hızında serial iletişim başlatılıyor

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  Serial.println("------------- TARIM PROJESİ -------------");

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

  SD.mkdir("EKG GOZLEM"); // Proje  sonuçlarının kaydedileceği klasör oluşturuluyor
}

void loop()
{

  if (NABIZ_oku() < nabizAlt || NABIZ_oku()>nabizUst)
  {

    dosya = SD.open("EKG GOZLEM/data.txt", FILE_WRITE);

    if (dosya)
    {
      dosya.println("Nabız verileri normalin dışında izlendi....");
      dosya.println("Konum ve zaman bilgisi ...:");
      dosya.println(GPS_oku());
      dosya.println("Nabız ...:");
      dosya.println(String(NABIZ_oku()));
      dosya.println("Sıcaklık değeri ...:");
      dosya.println(SICAKLIK_oku());
      dosya.println("--------------------------------------------------");
    }

    dosya.close();
  }
}
