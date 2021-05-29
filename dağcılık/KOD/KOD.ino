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

String okunan_bar = "", okunan_hava = "", okunan_gps = ""; // String türünde üç adet değişken tanımlanıyor

const int AOUTpin = A0; // Analog çıkış pini
const int DOUTpin = 10; //  Dijital çıkış pini
int esik;               // Eşik değeri
int ppm;

unsigned long timeOld;       // delay olmadan bekleme oluşturmak için gerekli değişkenler
const long timeWait = 10000; // 10 saniye yani 10000 milisaniyelik bekleme süresi tanımlanıyor

#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;

// HAVA analog ölçüm fonksiyonu
String HAVA_oku()
{
  okunan_hava = "";
  ppm = analogRead(AOUTpin);   // ppm değerini Analog pinden oku
  esik = digitalRead(DOUTpin); // eşik değerinin aşılıp aşılmadığını oku

  okunan_hava += " Hava kirliliği: ";
  okunan_hava += ppm;
  okunan_hava += "ppm. ----- ";
  // Eşik değerinin aşılıp aşılmadığını kontrol et ve seri porta yaz
  if (esik == 1)
  {
    okunan_hava += "Kirlilik eşik değerinin altında";
  }
  else
  {
    okunan_hava += "Kirlilik eşik değerinin altında";
  }
  return okunan_hava;
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
      gpsVeri += gps.time.hour() + ":";
    if (gps.time.minute() < 10)
      gpsVeri += gps.time.minute();
  }
  else
  {
    gpsVeri += "Zaman Saptanamadı...";
  }

  return gpsVeri;
}

// BAR ölçümü fonksiyonu
String BAR_oku()
{
  okunan_bar = "";
  char status;
  double T, P, p0, a;

  // You must first get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          okunan_bar = String(P);
          return okunan_bar;
        }
        else
          Serial.println("HATA..! retrieving pressure measurement\n");
      }
      else
        Serial.println("HATA..! starting pressure measurement\n");
    }
    else
      Serial.println("HATA..! retrieving temperature measurement\n");
  }
  else
    Serial.println("HATA..! starting temperature measurement\n");
}

void setup()
{
  Serial.begin(115200); // 115200baud hızında serial iletişim başlatılıyor

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  Serial.println("------------- DAĞCILIK PROJESİ  -------------");

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

  SD.mkdir("DAGCI PROJESI"); // BAR ölçüm sonuçlarının kaydedileceği klasör oluşturuluyor

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail (disconnected?)\n\n");
    while (1); // REBOOT REQUIRED
  }

  pinMode(DOUTpin, INPUT);
}

void loop()
{
  // kod bloğu bekleme işlemini kontrol etmektedir.
  // belirlenen bekleme süresi dolduğunda Ölçümler her üç sensör için
  // bir kez alınır ve SD kartta ilgili klasörde kayıt edilir

  unsigned long timeNew = millis();
  if (timeNew - timeOld >= timeWait)
  {
    timeOld = timeNew;

    dosya = SD.open("DAGCI PROJESI/data.txt", FILE_WRITE);

    if (dosya)
    {
      dosya.println("Bulunduğunuz konuma dair veriler işleniyor....");
      dosya.println("Konumunuz ...:");
      dosya.println(GPS_oku());
      dosya.println("Basınç değerleri ...:");
      dosya.println(BAR_oku());
      dosya.println("Hava kalitesi değerleri ...:");
      dosya.println(HAVA_oku());
      dosya.println("--------------------------------------------------");
    }

    dosya.close();
  }
}
