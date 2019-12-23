#include "TX2TempTransmitter.h"

TX2TempTransmitter::TX2TempTransmitter(uint8_t sensorID, uint8_t pin) {
  this->sensorID = sensorID;
  this->pin = pin;
}

void TX2TempTransmitter::transmit(float temp) {
  int tens = 10 - (int(temp) / 10);
  int ones = 15 - (int(temp) % 10);
  int tenths = 15 - (int(temp * 10.0) % 10);

  int checksum = (tens & 1) + (tens>>1 & 1) + (tens>>2 & 1) + (tens>>3 & 1);
  checksum += (ones & 1) + (ones>>1 & 1) + (ones>>2 & 1) + (ones>>3 & 1);
  checksum += (tenths & 1) + (tenths>>1 & 1) + (tenths>>2 & 1) + (tenths>>3 & 1);
  checksum &= 1;  // clear everything except the last bit (even or odd count);
  checksum ^= 1;  // flip the bit: checksum = 1 if an even number of bits

  int crc = (sensorID>>4)+(sensorID&0xF)+checksum+tens+ones+tenths+tens+ones-4;
  crc &= 0xF;  // throw away everything except the lower four bits
  
  // send data
  for (int i = 0; i < 2; ++i) { // send the sequence twice
    sendModelID();
    sendSensorID();
    if (checksum) I(); else O();
    sendNibble(tens); sendNibble(ones); sendNibble(tenths);
    sendNibble(tens); sendNibble(ones); sendNibble(crc);
    delay(20);
  }
}

void TX2TempTransmitter::I() {
  digitalWrite(pin, HIGH); 
  delayMicroseconds(TX2_LONG_PULSE); 
  digitalWrite(pin, LOW); 
  delayMicroseconds(TX2_DELAY); 
}

void TX2TempTransmitter::O() {
  digitalWrite(pin, HIGH); 
  delayMicroseconds(TX2_SHORT_PULSE); 
  digitalWrite(pin, LOW); 
  delayMicroseconds(TX2_DELAY); 
}

void TX2TempTransmitter::sendNibble(int i) {
  if (i&8) I(); else O();
  if (i&4) I(); else O();
  if (i&2) I(); else O();
  if (i&1) I(); else O();
}

void TX2TempTransmitter::sendModelID() { 
  I();I();I();I();O();I();O();I();I();I();I();I();
}

void TX2TempTransmitter::sendSensorID() {
  // because it is easier to look at the sensor ID as it appears in the sequence
  // with a checksum at the end, we use a value that has an extra (unused)
  // bit at the end, where the checksum would go, e.g., ssss sssc
  if (sensorID & 0x80) I(); else O();
  if (sensorID & 0x40) I(); else O();
  if (sensorID & 0x20) I(); else O();
  if (sensorID & 0x10) I(); else O();
  if (sensorID & 0x08) I(); else O();
  if (sensorID & 0x04) I(); else O();
  if (sensorID & 0x02) I(); else O();
}
