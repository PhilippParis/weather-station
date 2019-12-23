#ifndef DEEP_SLEEP_BUFFER_H
#define DEEP_SLEEP_BUFFER_H

#include <ESP8266WiFi.h>

#include "Measurement.h"

#define RTC_MEMORY_MAX_SIZE     (20)
#define RTC_MEMORY_MAGIC_NUMBER (126)

struct RTCData {
  uint8_t marker;
  uint8_t size;
  Measurement measurements[RTC_MEMORY_MAX_SIZE];
};

class RTCMemory {
  public:
    RTCMemory();
    void add(Measurement measurement);
    Measurement* read();
    uint8_t size();
    void clear();

  private:
    RTCData* readInternal();
    void writeInternal();
    bool isFirstUse();

    RTCData data;
};

#endif // DEEP_SLEEP_BUFFER_H
