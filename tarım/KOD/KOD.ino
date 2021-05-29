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

String okunan_nem = "", okunan_isik = "", okunan_gps = ""; // String türünde üç adet değişken tanımlanıyor

unsigned long timeOld;       // delay olmadan bekleme oluşturmak için gerekli değişkenler
const long timeWait = 10000; // 10 saniye yani 10000 milisaniyelik bekleme süresi tanımlanıyor

#include <dht11.h>
#define DHT11PIN 7 // dht11 pinimiz
dht11 DHT11;       // DHT kutuphanesinden obje olusturduk.

int LDR = A1; // analog pin 1 LDR bağlanıyor

// HAVA analog ölçüm fonksiyonu
String NEM_oku()
{
  okunan_nem = "";

  int value = DHT11.read(DHT11PIN); // dht11 sensorumuzu okuduk
  float nem = DHT11.humidity;       // float olarak nem degerimizi aldik.

  okunan_nem += String(nem);
  return okunan_nem;
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

// IŞIK ölçümü fonksiyonu
long LDR_oku()
{
  int bilgi = analogRead(LDR);
  float isik = bilgi;
  return isik;
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

  SD.mkdir("TARIM PROJESI"); 
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

    dosya = SD.open("TARIM PROJESI/data.txt", FILE_WRITE);
    if (dosya)
    {
      dosya.println("Bulunduğunuz konuma dair veriler işleniyor....");
      dosya.println("Konumunuz ...:");
      dosya.println(GPS_oku());
      dosya.println("Gün ışığı değeri ...:");
      dosya.println(LDR_oku());
      dosya.println("Nem değeri ...:");
      dosya.println(NEM_oku());
      dosya.println("--------------------------------------------------");
    }
    dosya.close();
  }
}
