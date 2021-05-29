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

String okunan_gps = "";

const int hizSens = 2;
unsigned long prevmillis;
unsigned long duration;
int rpm;
int rpmOrt = 0;
boolean currentstate;
boolean prevstate;

const int benzinSens = 5;
int seviye = 0;
int seviyeYeni;

// HX711
long HX711_oku()
{
  long agirlik = scale.read();

  return agirlik;
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

  Serial.println("YAKIT GOZLEM ve TASARRUF CİHAZI");

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

   if(eski - yenizaman = 10000){
   seviyeYeni = digitalRead(benzinSens);
   
   sdkart yaz
   
		10 saniyede 
		ortalama hız 
		tüketilen yakıt seviye - seviyeYeni
		gps bilgisi
   }
  prevstate = currentstate;

  if (digitalRead(HALL) == 0)
  {

    eskiDeger = HX711_oku();

    while (digitalRead(HALL) == 0)
      ;

    yeniDeger = HX711_oku();

    dosya = SD.open("AGIRLIK/data.txt", FILE_WRITE);

    if (dosya)
    {
      dosya.println("Bulunduğunuz konuma dair veriler işleniyor....");
      dosya.println("Konumunuz ...:");
      dosya.println(GPS_oku());
      dosya.println("Eski ağırlık ...:");
      dosya.println(eskiDeger);
      dosya.println("Yeni ağırlık ...:");
      dosya.println(yeniDeger);
      dosya.println("Ağırlık değişimi ...:");
      dosya.println(eskiDeger - yeniDeger);
      dosya.println("--------------------------------------------------");
    }

    dosya.close();
    delay(10000);
  }
}
