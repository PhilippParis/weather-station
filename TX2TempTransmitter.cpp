#include "TX2TempTransmitter.h"

TX2TempTransmitter::TX2TempTransmitter(uint8_t sensorID, uint8_t pin) {
  this->sensorID = sensorID;
  this->pin = pin;
}

void TX2TempTransmitter::transmit(float temp) {
  uint8_t nibble[11];
  // add 50 degree offset
  temp += 50;

  // preamble
  nibble[0] = TX2_PREAMBLE_1;
  nibble[1] = TX2_PREAMBLE_2;
  nibble[2] = TX2_TYPE_TEMP;
  // temperature
  nibble[5] = nibble[8] = int(temp) / 10;
  nibble[6] = nibble[9] = int(temp) % 10;
  nibble[7] = int(temp * 10.0) % 10;
  // sensor-id and parity
  nibble[3] = sensorID >> 4;
  nibble[4] = sensorID & 0xE | calcParity(nibble);
  //crc
  nibble[10] = calcCRC(nibble);
  
  // send the sequence twice
  for (uint8_t i = 0; i < 2; ++i) { 
    for (uint8_t j = 0; j < 11; j++) {
      sendNibble(nibble[j]);
    }
    delay(50);
  }
}

void TX2TempTransmitter::I() {
  digitalWrite(pin, HIGH); 
  delayMicroseconds(TX2_SHORT_PULSE); 
  digitalWrite(pin, LOW); 
  delayMicroseconds(TX2_DELAY);
}

void TX2TempTransmitter::O() {
  digitalWrite(pin, HIGH); 
  delayMicroseconds(TX2_LONG_PULSE); 
  digitalWrite(pin, LOW); 
  delayMicroseconds(TX2_DELAY); 
}

void TX2TempTransmitter::sendNibble(uint8_t i) {
  if (i&8) I(); else O();
  if (i&4) I(); else O();
  if (i&2) I(); else O();
  if (i&1) I(); else O();
}

uint8_t TX2TempTransmitter::calcCRC(uint8_t nibble[]) {
  int crc = 0;
  for (uint8_t i = 0; i < 10; i++) {
    crc += nibble[i];
  }
  return crc & 0xF;
}

uint8_t TX2TempTransmitter::calcParity(uint8_t nibble[]) {
  uint8_t parity = 0;
  for (uint8_t i = 5; i < 8; i++) {
    parity += nibble[i] & 1;
    parity += nibble[i] >> 1 & 1;
    parity += nibble[i] >> 2 & 1;
    parity += nibble[i] >> 3 & 1;
  }
  return parity & 1;
}

