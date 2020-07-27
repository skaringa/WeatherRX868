#include "Decoder.h"

/*
 * Arduino sketch to replace the USB-WDE1 weather data receiver from ELV.
 * It receives data from weather sensors like the S 300, ASH 2200, and KS 300 using the RX868 rf receiver module
 * from ELV (https://www.elv.de).
 * The data is decoded by the TempHygroRX868 library and written to the serial output in the same format like the USB-WDE1.
 * For details of the protocol and the wiring see README.md
 *
 */

// Pin at the which is RX868 receiver is connected.
// This has to be a digital pin that is capable to trigger interrupts.
#define DATA_PIN 3

volatile unsigned long hilo = 0L;
volatile unsigned long lohi = 0L;
volatile bool hasNewValue = false;
Decoder decoder(1100, 1300);

#define NUM_SENSORS 9
DecoderOutput sensors[NUM_SENSORS];

void setup() {
  // initialize receiver
  pinMode(DATA_PIN, INPUT);

  // LED
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize interrupt
  attachInterrupt(digitalPinToInterrupt(DATA_PIN), recv, CHANGE);

  // initialize serial communications at 57600 bps:
  Serial.begin(57600);

  // init buffer to hold the data
  memset(&sensors[0], 128, NUM_SENSORS * sizeof(DecoderOutput));
}

void loop() {
  if (hasNewValue) {
    digitalWrite(LED_BUILTIN, 1);
    DecoderOutput data = decoder.getDecoderOutput();
    //printDecoderOutput(data);
    handleLogViewOutput(data);
    digitalWrite(LED_BUILTIN, 0);
    hasNewValue = false;
  }
}

/*
 * Interrupt routine,
 * Called on each slope of the signal from the receiver.
 */
void recv() {
  int x = digitalRead(DATA_PIN);
  if (0 == x) {
    // slope hi->lo
    hilo = micros();
  } else {
    // slope lo->hi
    unsigned long now = micros();
    hasNewValue |= decoder.pulse(now - lohi, now - hilo);
    lohi = now;
  }
}

/* 
 * Implementation to print DecoderOutput to Serial
 * in a human readable format.
 */
void printDecoderOutput(DecoderOutput val) {
  // print sensor values depending on type of sensor
  Serial.print("sensor type: ");
  Serial.println(val.sensorTypeStr);
  Serial.print("address: ");
  Serial.println(val.address);

  Serial.print("temperature: ");
  Serial.println(val.temperature, 1);

  if (7 == val.sensorType) {
    // Kombisensor
    Serial.print("humidity: ");
    Serial.println(val.humidity, 0);
    Serial.print("wind: ");
    Serial.println(val.wind, 1);
    Serial.print("rain sum: ");
    Serial.println(val.rainSum);
    Serial.print("rain detector: ");
    Serial.println(val.rainDetect);
  }
  if (1 == val.sensorType || 4 == val.sensorType) {
    // Thermo/Hygro
    Serial.print("humidity: ");
    Serial.println(val.humidity, 0);
  }
  if (4 == val.sensorType) {
    // Thermo/Hygro/Baro
    Serial.print("pressure: ");
    Serial.println(val.pressure);
  }
  Serial.println();
}

/*
 * Print DecoderOutput to Serial in LogView format like the USB-WDE1.
 */
void handleLogViewOutput(DecoderOutput data) {
  // store actual data
  if (data.sensorType == 7 /* Kombi */) {
    sensors[8] = data;
  } else {
    sensors[data.address & 7] = data;
  }

  // print accumulated data
  Serial.print("$1;1;;");
  for (int i = 0; i < NUM_SENSORS - 1; ++i) {
    if (sensors[i].address == i && millis() - sensors[i].timeStamp < 600000) {
      Serial.print(sensors[i].temperature, 1);
    }
    Serial.print(";");
  }
  for (int i = 0; i < NUM_SENSORS - 1; ++i) {
    if (sensors[i].address == i && millis() - sensors[i].timeStamp < 600000) {
      Serial.print(sensors[i].humidity, 1);
    }
    Serial.print(";");
  }
  if (sensors[8].sensorType == 7 && millis() - sensors[8].timeStamp < 600000) {
    Serial.print(sensors[8].temperature, 1);
    Serial.print(";");
    Serial.print(sensors[8].humidity, 1);
    Serial.print(";");
    Serial.print(sensors[8].wind, 1);
    Serial.print(";");
    Serial.print(sensors[8].rainSum);
    Serial.print(";");
    Serial.print(sensors[8].rainDetect);
  } else {
    Serial.print(";;;;");
  }
  Serial.println(";0");
}
