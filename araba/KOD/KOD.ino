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

String okunan_gps = ""; // String türünde üç adet değişken tanımlanıyor

unsigned long eski_zaman;          // delay olmadan bekleme oluşturmak için gerekli değişkenler
const long bekleme_suresi = 10000; // 10 saniye yani 10000 milisaniyelik bekleme süresi tanımlanıyor

const int hizSens = 5;
unsigned long prevmillis;
unsigned long duration;
int rpm;
int rpmOrt = 0;
boolean currentstate;
boolean prevstate;

const int benzinSens = 6;
int seviye = 0;
int seviyeYeni;

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

void rpmHesap(){
    currentstate = digitalRead(benzinSens);
  if (prevstate != currentstate)
  {
    if (currentstate == HIGH)
    {
      duration = (micros() - prevmillis);
      rpm = (6000000 / duration);
      rpmOrt = (rpm + rpmOrt) / 2;
      prevmillis = micros();
    }
  }
  prevstate = currentstate;
}

void setup()
{
  Serial.begin(115200); // 115200baud hızında serial iletişim başlatılıyor

  // Start the software serial port at the GPS's default baud
  gpsSerial.begin(GPSBaud);

  Serial.println("YAKIT GOZLEM ve TASARRUF IZLEME");

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

  SD.mkdir("YAKIT GOZLEM");

  pinMode(benzinSens, INPUT);
  pinMode(hizSens, INPUT);
  prevmillis = 0;
  prevstate = LOW;

  seviye = digitalRead(benzinSens);
}

void loop()
{

rpmHesap();

  // kod bloğu bekleme işlemini kontrol etmektedir.
  // belirlenen bekleme süresi dolduğunda Ölçümler her üç sensör için
  // bir kez alınır ve SD kartta ilgili klasörde kayıt edilir

  unsigned long simdiki_zaman = millis();
  if (simdiki_zaman - eski_zaman >= bekleme_suresi)
  {
    eski_zaman = simdiki_zaman;

    seviyeYeni = digitalRead(5);

    dosya = SD.open("YAKIT GOZLEM/data.txt", FILE_WRITE);

    if (dosya)
    {
      dosya.println("Bulunduğunuz konuma dair veriler işleniyor....");
      dosya.println("Konumunuz ...:");
      dosya.println(GPS_oku());
      dosya.println("Ortalama hız değeri ...:");
      dosya.println(rpmOrt);
      dosya.println("Tüketilen yakıt miktarı ...:");
      dosya.println(seviye - seviyeYeni);
      dosya.println("--------------------------------------------------");
      rpmOrt = 0;
      seviye = seviyeYeni;
    }

    dosya.close();
  }
}
