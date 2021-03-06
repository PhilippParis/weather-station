#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "Adafruit_HTU21DF.h"
#include "Adafruit_BMP085_U.h"

#include "Measurement.h"
#include "TX2TempTransmitter.h"
#include "OpenWeatherMap.h"
#include "RTCMemory.h"
#include "secrets.h"

// #define debug

// TX2 TEMPERATURE TRANSMITTER
const uint8_t TX2_SENSOR_ID = 0x20;
const uint8_t TX2_PIN = 13;

// OPEN WEATHER MAP
const uint8_t OWM_BATCH_SIZE = 20;
const char* OWM_API_KEY = SECRET_OWM_API_KEY;
const char* OWM_STATION_ID = SECRET_OWM_STATION_ID;

// WIFI
const char* WIFI_SSID = SECRET_WIFI_SSID;
const char* WIFI_PASSWD = SECRET_WIFI_PASSWD;
const long WIFI_TIMEOUT = 5000;                  // 5 seconds
const IPAddress STATIC_IP(192,168,178,201);
const IPAddress GATEWAY(192,168,178,1);
const IPAddress SUBNET(255,255,255,0);
const IPAddress DNS_1(8,8,8,8);
const IPAddress DNS_2(8,8,4,4);

// OTHER
const long DEEP_SLEEP = 1000000 * 60 * 3;        // 3 minutes

void setup() {  
#ifdef debug
  Serial.begin(19200);
  Serial.setTimeout(2000);
#endif

  println("");
  println("-------------------------------------");
  println("Running Weatherstation Firmware");
  println("-------------------------------------");
  
  pinMode(TX2_PIN, OUTPUT);
  RTCMemory buffer = RTCMemory();
  
  struct Measurement measurement;
  bool success = readFromHTU21DF(measurement.temperature, measurement.humidity);
  success = success && readFromBMP180(measurement.pressure);

  if (success) {
    buffer.add(measurement);
    transmitVia433Mhz(measurement);
    if (buffer.size() >= OWM_BATCH_SIZE) {
      publishToOpenWeatherMap(buffer);
    }
  }
  println("DONE");
  ESP.deepSleep(DEEP_SLEEP, buffer.size() >= (OWM_BATCH_SIZE - 1) ? WAKE_RFCAL : WAKE_RF_DISABLED);
}

void loop() {}

void publishToOpenWeatherMap(RTCMemory &buffer) {
  println("publish to OWM");
  if (connectToWifi(WIFI_SSID, WIFI_PASSWD)) {
    long dt = getEpochTime();
    
    if (dt > 0) {
      for (int i = buffer.size() - 1; i >= 0; i--) {
        buffer.read()[i].dt = dt;
        dt -= DEEP_SLEEP / 1000000;
      }
    
      OpenWeatherMap owm = OpenWeatherMap(OWM_API_KEY, OWM_STATION_ID);
      if (owm.post(buffer.read(), buffer.size())) {
        buffer.clear();
      }
    }
  }
  WiFi.disconnect(true);
}

void transmitVia433Mhz(Measurement measurement) {
  println("transmit via 433Mhz");
  TX2TempTransmitter tx2 = TX2TempTransmitter(TX2_SENSOR_ID, TX2_PIN);
  tx2.transmit(((float) measurement.temperature) / 100.0);
}

long getEpochTime() {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);
  timeClient.begin();
  if (!timeClient.update()) {
    return -1;
  }
  return timeClient.getEpochTime();
}

bool readFromBMP180(uint16_t &pressure) {
  Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
  if(!bmp.begin()) {
    println("BMP180 not detected!");
    return false;
  }
  sensors_event_t event;
  bmp.getEvent(&event);
  pressure = (uint16_t) event.pressure;
  return true;
}

bool readFromHTU21DF(int16_t &temperature, uint8_t& humidity) {
  Adafruit_HTU21DF htu = Adafruit_HTU21DF();
  if (!htu.begin()) {
    println("HTU21DF not detected!");
    return false;
  }

  temperature = (int16_t) (htu.readTemperature() * 100.0);
  humidity = (uint8_t) htu.readHumidity();  
  return true;
}

bool connectToWifi(const char* ssid, const char* password) {
  println("Connecting to wifi...");
  WiFi.disconnect(true);
  WiFi.config(STATIC_IP, GATEWAY, SUBNET, DNS_1, DNS_2);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  unsigned long wifiConnectStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (WiFi.status() == WL_CONNECT_FAILED) {
      println("Failed to connect to WiFi");
      return false;
    }

    delay(50);
    if (millis() - wifiConnectStart > WIFI_TIMEOUT) {
      println("Failed to connect to WiFi: timeout");
      return false;
    }
  }
  println("WiFi connected");
  return true;
}

void println(String line) {
#ifdef debug
  Serial.println(line);
#endif
}
