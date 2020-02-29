#include "OpenWeatherMap.h"

OpenWeatherMap::OpenWeatherMap(String apiKey, String stationId) {
  this->url = OPEN_WEATHER_MAP_URL + apiKey;
  this->stationId = stationId;
}

bool OpenWeatherMap::post(Measurement measurements[], uint8_t n) {
  HTTPClient http;
  if (!http.begin(url)) {
    return false;
  }
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(getMsgBody(measurements, n));  
  http.end();
  return httpCode == HTTP_CODE_NO_CONTENT;
}

String OpenWeatherMap::parse(Measurement m) {
  return "{\"station_id\":\"" + stationId + "\"," +
          "\"dt\":" + m.dt + "," +
          "\"temperature\":" + ((float) m.temperature) / 100.0 + "," +
          "\"pressure\":" + m.pressure  + "," +
          "\"humidity\":" + m.humidity + "}";
}

String OpenWeatherMap::getMsgBody(Measurement measurements[], uint8_t n) {
  String body = "[";
  for (int i = 0; i < n; i++) {
    if (body.length() > 1) {
      body += ",";
    }
    if (isValid(measurements[i])) {
      body += parse(measurements[i]);
    }
  }
  return body + "]";
}

bool OpenWeatherMap::isValid(Measurement m) {
  return m.temperature > -4000 && m.temperature < 5000 && m.humidity >= 0 &&
    m.humidity <= 100 && m.pressure > 0 && m.pressure < 1200;
}

