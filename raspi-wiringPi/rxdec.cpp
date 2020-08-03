/*
 * Receiver for data of wireless weather sensors with RX868 and Raspberry Pi.
 *
 * Main program.
 */
#include <wiringPi.h>
#include <stdio.h>
#include <signal.h>
#include "Decoder.h"

static volatile int keepRunning = 1;
static DecoderOutput out;
static volatile int hasOut = 0;

/*
 * Signal handler to catch Ctrl-C to terminate the program safely.
 */
void sigIntHandler(int dummy) {
  keepRunning = 0;
}

/*
 * Thread to read the output of the receiver module
 * and to decode it using Decoder.
 */
PI_THREAD (decoderThread) {
  piHiPri(50);

  // init decoder for a sample rate of 1/200µs
  Decoder decoder(5,14);
  int x;
  int len = 0;
  int lo = 0;
  int px = 0;

  while (keepRunning) {
    x = digitalRead(2);

    len++;
    if (0 == x) {
      lo++;
    }
    if (x != px && 0 != x) {
      // slope low->high
      if (decoder.pulse(len, lo)) {
        piLock(1);
        out = decoder.getDecoderOutput();
        hasOut = 1;
        piUnlock(1);
      }
      len = 1;
      lo = 0;
    }
    px = x;

    delayMicroseconds(200);
  }
}


void printDecoderOutput(DecoderOutput val) {
  // print sensor values depending on type of sensor
  printf("sensor type: %s\n", val.sensorTypeStr);
  printf("address: %d\n", val.address);

  printf("temperature: %.1f\n", val.temperature);

  if (7 == val.sensorType) {
    // Kombisensor
    printf("humidity: %.0f\n", val.humidity);
    printf("wind: %.1f\n", val.wind);
    printf("rain sum: %d\n", val.rainSum);
    printf("rain detector: %d\n", val.rainDetect);
  }
  if (1 == val.sensorType || 4 == val.sensorType) {
    // Thermo/Hygro
    printf("humidity: %.1f\n", val.humidity);
  }
  if (4 == val.sensorType) {
    // Thermo/Hygro/Baro
    printf("pressure: %d\n", val.pressure);
  }
  printf("\n");
}

/*
 * MAIN
 *
 * Initialize the hardware and print the output of the Decoder.
 */
int main() {
  wiringPiSetup();
/*
  BCM GPIO 27: DATA (IN) == WiPin 2
*/
  pinMode(2, INPUT);
  pullUpDnControl(2, PUD_DOWN);

  signal(SIGINT, sigIntHandler);

  piThreadCreate(decoderThread);

  while (keepRunning) {
    piLock(1);
    if (hasOut) {
      hasOut = 0;
      piUnlock(1);
      printDecoderOutput(out);
    }
    piUnlock(1);
    delay(100);
  }

  printf("clean up and exit\n");
  digitalWrite(3, 0); // disable rx
}
