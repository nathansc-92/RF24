/*
 * Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 * Updated 2020 TMRh20
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/**
 * Channel scanner and Continuous Carrier Wave Output
 *
 * Example to detect interference on the various channels available.
 * This is a good diagnostic tool to check whether you're picking a
 * good channel for your application.
 *
 * Run this sketch on two devices. On one device, start CCW output by sending a 'g'
 * character over Serial. The other device scanning should detect the output of the sending
 * device on the given channel. Adjust channel and output power of CCW below.
 *
 * Inspired by cpixip.
 * See http://arduino.cc/forum/index.php/topic,54795.0.html
 */

#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8

RF24 radio(7, 8);

//
// Channel info
//

const uint8_t num_channels = 126;
uint8_t values[num_channels];

//
// Setup
//

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(115200);
  printf_begin();
  Serial.println(F("\n\rRF24/examples/scanner/"));

  //
  // Setup and configure rf radio
  //

  radio.begin();
  radio.setAutoAck(false);

  // Get into standby mode
  radio.startListening();
  radio.stopListening();
  radio.printDetails();

  //delay(1000);
  // Print out header's vertically labeled channels
  for (int8_t header_line = 2; header_line >= -1; --header_line) {
    for (uint8_t i = 0; i < num_channels; ++i) {
      if (header_line > -1)
        Serial.print((int)(i / pow(10, header_line)) % 10);
      else
        Serial.print(F("^"));
    }
    Serial.println();
  }
  //delay(1000);
}

//
// Loop
//

const int num_reps = 100;
bool constCarrierMode = 0;

void loop(void)
{
  /****************************************/
  // Send g over Serial to begin CCW output
  // Configure the channel and power level below
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'g') {
      constCarrierMode = 1;
      radio.stopListening();
      delay(2);
      Serial.println("Starting Carrier Out");
      radio.startConstCarrier(RF24_PA_LOW, 40);
    } else if (c == 'e') {
      constCarrierMode = 0;
      radio.stopConstCarrier();
      Serial.println("Stopping Carrier Out");
    }
  }
  /****************************************/

  if (constCarrierMode == 0) {
    // Clear measurement values
    memset(values, 0, sizeof(values));

    // Scan all channels num_reps times
    int rep_counter = num_reps;
    while (rep_counter--) {
      int i = num_channels;
      while (i--) {
        radio.setChannel(i);       // Select this channel
        radio.startListening();    // start an RX session
        delayMicroseconds(128);    // Listen for a little
        if (radio.testCarrier()) { // Did we get a carrier?
            ++values[i];           // note the detected signal
        }
        radio.stopListening();     // reset the RPD flag
      }
    }


    // Print out channel measurements, clamped to a single hex digit
    int i = 0;
    while ( i < num_channels )
    {
      Serial.print(min(0xf, values[i]), HEX);
      ++i;
    }
    Serial.println();

  }//If constCarrierMode == 0
}
