#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "MQ135.h"
#include "MQ7.h"

MQ7 mq7(35, 5.0);

char ssid[] = "TEKNOLAB Office";
char pass[] = "selamatdatang";
const char* host = "192.168.1.18";

byte customChar[] = {
  B00100,
  B01010,
  B01010,
  B01010,
  B11011,
  B10101,
  B10001,
  B11111
};
byte customChar1[] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte customChar2[] = {
  B00000,
  B00010,
  B11100,
  B00001,
  B11110,
  B00001,
  B11110,
  B00000
};
byte customChar3[] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B10001,
  B01110
};
byte customChar4[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B01110,
  B01010,
  B01110
};
byte customChar5[] = {
  B00000,
  B01110,
  B01110,
  B00000,
  B00000,
  B01110,
  B01110,
  B00000
};



// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     
const int Sensor = 34;

int maxudarabersih = 100;

// Change this to your LCD module's address
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

long RL = 1000; // 1000 Ohm
long Ro = 830; // 830 ohm ( SILAHKAN DISESUAIKAN)

float air_quality;
float temperatureC;
float ppm;

void setup() {
  Serial.begin(115200); /* Baud rate untuk komunikasi serial */
  // Inisialisasi WiFi
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");


  lcd.init();
  lcd.backlight();
  lcd.clear();
  pinMode(Sensor, INPUT);

  lcd.createChar(0, customChar);
  lcd.createChar(1, customChar1);
  lcd.createChar(2, customChar2);
  lcd.createChar(3, customChar3);
  lcd.createChar(4, customChar4);
  lcd.createChar(5, customChar5);

}

void loop() {
//  lcd.clear();
    lcd.setCursor(0, 0); // Set the cursor to the first row, first column
    lcd.print("MONITORING JIIS  1.");
//  lcd.write(4);
    lcd.write(3);

//    ds18();
//    brasi();
//    mq77();
    sendtodatabase();
    delay(1000);
}

void brasi(){
    MQ135 gasSensor =MQ135(34);
    air_quality = gasSensor.getPPM();
    Serial.println(String() + "Air Quality: " + air_quality +"  PPM"); 

  lcd.setCursor(0, 2); // Set the cursor to the first row, first column
  lcd.write(2);
  lcd.print(" AQI    ");
  lcd.write(5);
  lcd.print(" ");
  lcd.print(air_quality);
  lcd.print(" AQI ");
  }

void ds18()
{
  sensors.requestTemperatures(); 
  temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);
  Serial.println(String() + "Temperatur: " + temperatureC + "ºC");
  Serial.println(String() + "Farenheat :" + temperatureF + "ºF");
  

  lcd.setCursor(0, 1); // Set the cursor to the second row, first column
  lcd.write(0);
  lcd.print(" Heat   ");
  lcd.write(5);
  lcd.print(" ");
  lcd.print(temperatureC);
  lcd.print(" ");
  lcd.write(1);
  lcd.print("C ");
  
  }
void mq77(){
  Serial.print("Karbon Monoksida : ");
  Serial.println(mq7.getPPM());

  lcd.setCursor(0, 3); // Set the cursor to the fourth row, first column
  lcd.write(2);
  lcd.print(" Polusi ");
  lcd.write(5);
  lcd.print(" ");
  lcd.print(mq7.getPPM());
  lcd.print(" ppm");
}


void sendtodatabase(){
    Serial.print("connecting to ");
    Serial.println(host);
    
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }
    
    ds18();
    brasi();
    mq77();
   
    // This will send the request to the server
    client.print(String("GET http://192.168.1.18/JIIS_F/input.php?") + 
                          ("&aqi=") +  air_quality +   
                          ("&suhu=") +  temperatureC + 
                          ("&polusi=") + mq7.getPPM() +
                          " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 1000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }
    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
        
    }
    Serial.println();
    Serial.println("closing connection");
}
  
 
