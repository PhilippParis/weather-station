#include "RTCMemory.h"

RTCMemory::RTCMemory() {
  ESP.rtcUserMemoryRead(0, (uint32_t*) &data, sizeof(RTCData));
}

void RTCMemory::add(Measurement measurement) {
  if (isFirstUse() || data.size >= RTC_MEMORY_MAX_SIZE) {
    data.size = 0;
    data.marker = RTC_MEMORY_MAGIC_NUMBER;
  }
  data.measurements[data.size++] = measurement;
  writeInternal();
}

Measurement* RTCMemory::read() {
  return data.measurements;
}

uint8_t RTCMemory::size() {
  return data.size;
}

void RTCMemory::clear() {
  data.size = 0;
  data.marker = RTC_MEMORY_MAGIC_NUMBER;
  writeInternal();
}

void RTCMemory::writeInternal() {
  ESP.rtcUserMemoryWrite(0, (uint32_t*) &data, sizeof(RTCData));
}

bool RTCMemory::isFirstUse() {
  return data.marker != RTC_MEMORY_MAGIC_NUMBER;
}
