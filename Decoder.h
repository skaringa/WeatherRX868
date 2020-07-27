#ifndef _DECODER_H
#define _DECODER_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#define boolean bool
#endif

/*
 * Decoder for data of wireless weather sensors like S 300, ASH 2200, and KS 300 manufactured by ELV (http://www.elv.de).
 *
 * Definition of decoder module.
 */

#define BUFLEN 100

/*
 * Internal decoder state.
 */
typedef enum {
  WAIT, SYNC, DATA
} EDecoderState;

/*
 * Value object to hold the decoded sensor values.
 */
typedef struct {
  int sensorType;
  const char* sensorTypeStr; 
  int address; 
  float temperature;
  float humidity;
  float wind;
  int rainSum;
  int rainDetect;
  int pressure;
  unsigned long timeStamp;
} DecoderOutput;

/*
 * The decoder.
 */
class Decoder {
  public:
    /*
     * Constructor.
     *
     * minLen, maxLen: min and max length of a valid pulse.
     * The unit is the same as for pulse, either microseconds or 
     * multiples of a fixed sample rate.
     */ 
    Decoder(unsigned long minLen = 500, unsigned long maxLen = 1400);
    /*
     * Feed a ON-OFF pulse into the decoder.
     *
     * len: The length of the whole pulse.
     * lo: The length of the OFF part.
     * return: true if a complete data packet is available.
     */
    bool pulse(unsigned long len, unsigned long lo);
    /*
     * Get the actual output of the decoder.
     *
     * Should be called when pulse returns true.
     */
    DecoderOutput getDecoderOutput() const;
  private:
    uint8_t bitval(unsigned long len, unsigned long lo);
    int popbits(int num);
    bool expectEon();
    bool decode();
    void pushData(uint8_t val);

    int syncCount;
    const unsigned long minLen;
    const unsigned long maxLen;
    uint8_t buffer[BUFLEN];
    int bufferEnd;
    int bufferFront;
    EDecoderState decoderState; 
    DecoderOutput decoderOutput;
};

#endif
