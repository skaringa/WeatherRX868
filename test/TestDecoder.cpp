/*
 * Test the decoder offline with the log files from 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "../Decoder.h"

Decoder decoder;

char* nextField(char **p) {
  char *q = *p + 1;
  while (*q != 0 && *q != ',') {
    ++q;
  }
  *q = 0;
  char* value = *p;
  *p = q + 1;
  return value;
}

void readData(char * buffer, char ** input, DecoderOutput * ref) {
  // decoder input
  char* p = &buffer[0];
  *input = nextField(&p);
  // reference
  ref->sensorTypeStr = nextField(&p);
  ref->address = atoi(nextField(&p));
  ref->temperature = atof(nextField(&p));
  ref->humidity = atof(nextField(&p));
  ref->wind = atof(nextField(&p));
  ref->rainSum = atoi(nextField(&p));
  ref->rainDetect = atoi(nextField(&p));
}

bool compare(const DecoderOutput & ref, const DecoderOutput & is) {
  if (strcmp(ref.sensorTypeStr, is.sensorTypeStr)) {
    printf("sensorTypeStr: '%s' <> '%s'\n", ref.sensorTypeStr, is.sensorTypeStr);
    return false;
  }
  if (ref.address != is.address) {
    printf("address: %d <> %d\n", ref.address, is.address);
    return false;
  }
  if (fabs(ref.temperature - is.temperature) > 0.1) {
    printf("temperature: %f <> %f\n", ref.temperature, is.temperature);
    return false;
  }
  if (fabs(ref.humidity - is.humidity) > 0.1) {
    printf("humidity: %f <> %f\n", ref.humidity, is.humidity);
    return false;
  }
  if (is.sensorType == 7 /* Kombi */) {
    if (fabs(ref.wind - is.wind) > 0.1) {
      printf("wind: %f <> %f\n", ref.wind, is.wind);
      return false;
    }
    if (ref.rainSum != is.rainSum) {
      printf("rainSum: %d <> %d\n", ref.rainSum, is.rainSum);
      return false;
    }
    if (ref.rainDetect != is.rainDetect) {
      printf("rainDetect: %d <> %d\n", ref.rainDetect, is.rainDetect);
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s logfile\n", argv[0]);
    return 1;
  }
  FILE *logfile = fopen(argv[1], "r");
  if (NULL == logfile) {
    printf("Can't open file %s\n", argv[1]);
    return 2;
  }

  char buffer[200];
  int rc = 0;
  while (fgets(buffer, sizeof buffer, logfile) != NULL) {
    DecoderOutput ref;
    char* input;
    readData(buffer, &input, &ref);
    bool hasNewValue;
    // sync bits
    for (int i = 0; i < 10; ++i) {
      // 0 bit
      hasNewValue = decoder.pulse(1270, 366);
    }
    // start bit
    hasNewValue = decoder.pulse(1270, 854);
    for (int i = 0; i < strlen(input); ++i) {
      if (input[i] == '1') {
        // 1 bit
        hasNewValue = decoder.pulse(1270, 854);
      } else {
        // 0 bit
        hasNewValue = decoder.pulse(1270, 366);
      }
      if (hasNewValue) {
        fprintf(stderr, "*** Decoder finished too early ***\n");
        rc = 7;
        break;
      }
    }
    // stop bit
    hasNewValue = decoder.pulse(2000, 0);
    if (hasNewValue) {
      if (! compare(ref, decoder.getDecoderOutput())) {
        fprintf(stderr, "*** Compare failed ***\n");
        rc = 6;
      }
    } else {
      fprintf(stderr, "*** Decoder failed ***\n");
      rc = 5;
    }
  }
  return rc;
}
