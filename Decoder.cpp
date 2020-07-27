/*
 * Receiver for data of wireless weather sensors with RX868 and Arduino.
 *
 * Implementation of decoder module.
 */
#include "Decoder.h"

//#define DEBUG

#ifndef ARDUINO
#ifdef DEBUG
class Serial_t {
  public:
    void print(const char* str) {
      printf("%s", str);
    };
    void print(int i) {
      printf("%d", i);
    };
    void print(char c) {
      printf("%c", c);
    }
    void println(const char* str) {
      printf("%s\n", str);
    };
    void println(int i) {
      printf("%d\n", i);
    };
    void println() {
      printf("\n");
    };
};
Serial_t Serial;

#endif // DEBUG

unsigned long millis() {
  return clock() / (CLOCKS_PER_SEC / 1000);
}
#endif // ARDUINO



const char* SENSOR_TYPES[] = {"Thermo", "Thermo/Hygro", "Rain(?)", "Wind(?)", "Thermo/Hygro/Baro", "Luminance(?)", "Pyrano(?)", "Kombi"};

const int SENSOR_DATA_COUNT[] = {5, 8, 5, 8, 12, 6, 6, 14};

Decoder::Decoder(unsigned long minLen, unsigned long maxLen) 
  : minLen(minLen), maxLen(maxLen), decoderState(WAIT), syncCount(0), bufferEnd(0)
  {}

bool Decoder::pulse(unsigned long len, unsigned long lo) {
  bool hasNewValue = false;
  uint8_t val = bitval(len, lo);
  if (255 == val) {
    if (DATA == decoderState) {
      // end of frame?
      pushData(1);
      hasNewValue = decode();
      bufferEnd = 0;
    }
    decoderState = WAIT;
  } else if (WAIT == decoderState) {
    if (0 == val) {
      // first sync pulse
      syncCount = 1;
      decoderState = SYNC;
    }
  } else if (SYNC == decoderState) {
    if (0 == val) {
      // another sync pulse
      syncCount++;
    } else if (1 == val && syncCount > 6) {
      // got the start bit
      syncCount = 0;
      decoderState = DATA;
    }
  } else if (DATA == decoderState) {
    pushData(val);
  }
  
  return hasNewValue;
}

uint8_t Decoder::bitval(unsigned long len, unsigned long lo) {
  uint8_t val = 255;
  if (len >= minLen && len <= maxLen) {
    if (lo < len/2) {
      val = 0;
    } else {
      val = 1;
    }
  }
  return val;
}

void Decoder::pushData(uint8_t val) {
  buffer[bufferEnd++] = val;
  if (bufferEnd == BUFLEN) {
    bufferEnd = 0;
  }
}

int Decoder::popbits(int num) {
  int val = 0;
  if (bufferEnd - bufferFront < num) {
#ifdef DEBUG
    Serial.println("data exhausted");
#endif
    return 0;
  }
  for (int i = 0; i < num; ++i) {
    val += buffer[bufferFront] << i;
    bufferFront++;
  }
  return val;
}

bool Decoder::expectEon() {
  // check end of nibble (1)
  if (popbits(1) != 1) {
#ifdef DEBUG
    Serial.println("end of nibble is not 1");
#endif
    return false;
  }
  return true;
}

bool Decoder::decode() {
#ifdef DEBUG
  Serial.println("decode: ");
  for (int i = 0; i < bufferEnd; ++i) {
    Serial.print(char(buffer[i] + '0'));
  }
  Serial.println();
#endif
  bufferFront = 0;
  int check = 0;
  int sum = 0;
  int sensorType = popbits(4) & 7;
  if (!expectEon()) {
    return false;
  }
  check ^= sensorType;
  sum += sensorType;

  // read data as nibbles
  int nibbleCount = SENSOR_DATA_COUNT[sensorType];
  int *dec = new int[nibbleCount];
  for (int i = 0; i < nibbleCount; ++i) {
    int nibble = popbits(4);
    if (!expectEon()) {
      delete[] dec;
      return false;
    }
    dec[i] = nibble;
    check ^= nibble;
    sum += nibble;
  }

  // check
  if (check != 0) {
#ifdef DEBUG
    Serial.print("Check is not 0 but ");
    Serial.println(check);
#endif
    delete[] dec;
    return false;
  }

  // sum
  int sumRead = popbits(4);
  sum += 5;
  sum &= 0xF;
  if (sumRead != sum) {
#ifdef DEBUG
    Serial.print("Sum read is ");
    Serial.print(sumRead);
    Serial.print(" but computed is ");
    Serial.println(sum);
#endif
    delete[] dec;
    return false;
  }

  // compute values
  decoderOutput.sensorType = sensorType;
  decoderOutput.sensorTypeStr = SENSOR_TYPES[sensorType];
  decoderOutput.address = dec[0] & 7;
  decoderOutput.temperature = (dec[3]*10.f + dec[2] + dec[1]/10.f) * (dec[0]&8?-1.f:1.f);
  decoderOutput.humidity = 0.f;
  decoderOutput.wind = 0.f;
  decoderOutput.rainSum = 0;
  decoderOutput.rainDetect = 0;
  decoderOutput.pressure = 0;
  decoderOutput.timeStamp = millis();
  
  if (7 == sensorType) {
    // Kombisensor
    decoderOutput.humidity = dec[5]*10.f + dec[4];
    decoderOutput.wind = dec[8]*10.f + dec[7] + dec[6]/10.f;
    decoderOutput.rainSum = dec[11]*16*16 + dec[10]*16 + dec[9];
    decoderOutput.rainDetect = dec[0]&2 == 1;
  }
  if (1 == sensorType || 4 == sensorType) {
    // Thermo/Hygro
    decoderOutput.humidity = dec[6]*10.f + dec[5] + dec[4]/10.f;
  }
  if (4 == sensorType) {
    // Thermo/Hygro/Baro
    decoderOutput.pressure = 200 + dec[9]*100 + dec[8]*10 + dec[7];
  }

  delete[] dec;
  return true;
}

DecoderOutput Decoder::getDecoderOutput() const {
  return decoderOutput;
}
