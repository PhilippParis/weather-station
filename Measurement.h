#ifndef MEASUREMENT_H
#define MEASUREMENT_H

struct Measurement {
  unsigned long dt;
  int16_t temperature;
  uint8_t humidity;
  uint16_t pressure;
};

#endif // MEASUREMENT_H
