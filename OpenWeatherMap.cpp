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
          "\"temperature\":" + m.temperature + "," +
          "\"pressure\":" + m.pressure  + "," +
          "\"humidity\":" + m.humidity + "}";
}

String OpenWeatherMap::getMsgBody(Measurement measurements[], uint8_t n) {
  String body = "[" + parse(measurements[0]);
  for (int i = 1; i < n; i++) {
    body += "," + parse(measurements[i]);
  }
  return body + "]";
}
