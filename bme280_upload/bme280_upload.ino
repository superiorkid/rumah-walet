#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "secret.h"
#include <LiquidCrystal_I2C.h>
 
char ssid[] = _SSID;
char pass[] = PASSWORD;
WiFiClient  client;
Adafruit_BME280 bme;
LiquidCrystal_I2C lcd(0x27, 16, 2);  
 
unsigned long channel_id = CH_ID;
const char * write_api = WRITE_APIKEY;
float temperature, humidity, pressure, altitude;
 
void setup() {
  Serial.begin(115200);  // Initialize serial

  bme.begin(0x76);
  lcd.begin();
  lcd.backlight();
 
  WiFi.mode(WIFI_STA);
  lcd.setCursor(0, 0);
  lcd.println("Konek ke ");
  lcd.setCursor(0,1);
  lcd.println(_SSID);
  delay(6000);
  lcd.clear();

  WiFi.begin(_SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(6000);
    lcd.print(".");
  }
  lcd.clear();

//  Serial.println("");
  lcd.setCursor(0,0);
  lcd.println("Wifi tersambung..!");
  delay(6000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IP:");
  lcd.println(WiFi.localIP());

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}
 
void loop() { 
  
// set the fields with the values
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, pressure);
  ThingSpeak.setField(4, altitude);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(channel_id, write_api);
  if(x == 200){
    Serial.println("Channel berhasil diperbarui.");
  }
  else{
    Serial.println("Error : " + String(x));
  }
  

  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  
  delay(60000); 
}
