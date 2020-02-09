#ifndef TX2_TEMP_TRANSMITTER_H
#define TX2_TEMP_TRANSMITTER_H

#include <ESP8266WiFi.h>

#define TX2_LONG_PULSE (1300)
#define TX2_SHORT_PULSE (500)
#define TX2_DELAY (1450)

#define TX2_PREAMBLE_1 (0)
#define TX2_PREAMBLE_2 (10)
#define TX2_TYPE_TEMP (0)

/*
 * example transmission:
 *  [00001010 | 0000 |  1100110   |    1   |0101|0010| 0110 |0101|0010|0111]
 *  [preamble | type | sensor id  | parity |tens|ones|tenths|tens|ones| crc]
 * 
 * type: some transmitters can transmit temperature (0000) and humidity (1110)
 * sensor-id: id of the sensor. is reset randomly if battery swapped
 * parity: even parity of 'tens', 'ones' and 'tenths'
 * crc: 4 lowest bits of arithmetic sum of all nibbles
 * 
 * temperature with offset: 52.6°C
 * temperature: 2.6°C
 */
class TX2TempTransmitter {
  public:
    TX2TempTransmitter(uint8_t sensorID, uint8_t pin);
    void transmit(float temperature);

  private:
    void I();   // sends one
    void O();   // sends zero
    void sendNibble(uint8_t i);
    uint8_t calcCRC(uint8_t nibble[]);
    uint8_t calcParity(uint8_t nibble[]);
    uint8_t pin = 0;
    uint8_t sensorID = 0;
};

#endif //  TX2_TEMP_TRANSMITTER_H
