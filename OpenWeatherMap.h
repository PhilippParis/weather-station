#ifndef OPEN_WEATHER_MAP_H
#define OPEN_WEATHER_MAP_H

#include <ESP8266HTTPClient.h>

#include "Measurement.h"

#define OPEN_WEATHER_MAP_URL ("http://api.openweathermap.org/data/3.0/measurements?APPID=")


class OpenWeatherMap {
  public:
    OpenWeatherMap(String apiKey, String stationId);
    bool post(Measurement measurements[], uint8_t n);
    
  private:
    String getMsgBody(Measurement measurements[], uint8_t n);
    String parse(Measurement measurement);
    bool isValid(Measurement measurement);

    String url;
    String stationId;
};

#endif // OPEN_WEATHER_MAP_H
