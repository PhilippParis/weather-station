#ifndef TX2_TEMP_TRANSMITTER_H
#define TX2_TEMP_TRANSMITTER_H

#include <ESP8266WiFi.h>

#define TX2_LONG_PULSE (975)
#define TX2_SHORT_PULSE (250)
#define TX2_DELAY (1450)

class TX2TempTransmitter {
  public:
    TX2TempTransmitter(uint8_t sensorID, uint8_t pin);
    void transmit(float temperature);

  private:
    void I();   // sends one
    void O();   // sends zero
    void sendModelID();
    void sendSensorID();
    void sendNibble(int i);
    uint8_t pin = 0;
    uint8_t sensorID = 0;
};

#endif //  TX2_TEMP_TRANSMITTER_H
