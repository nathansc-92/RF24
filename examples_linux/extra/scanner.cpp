/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.


 03/17/2013 : Charles-Henri Hallard (http://hallard.me)
              Modified to use with Arduipi board http://hallard.me/arduipi
						  Changed to use modified bcm2835 and RF24 library

 */

/**
 * Channel scanner
 *
 * Example to detect interference on the various channels available.
 * This is a good diagnostic tool to check whether you're picking a
 * good channel for your application.
 *
 * Inspired by cpixip.
 * See http://arduino.cc/forum/index.php/topic,54795.0.html
 */

#include <cstdlib>
#include <iostream>
#include <RF24/RF24.h>

using namespace std;

//
// Hardware configuration
//

// CE Pin, CSN Pin, SPI Speed

// Setup for GPIO 22 CE and GPIO 25 CSN with SPI Speed @ 1Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, RPI_V2_GPIO_P1_18, BCM2835_SPI_SPEED_1MHZ);

// Setup for GPIO 22 CE and CE0 CSN with SPI Speed @ 4Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_4MHZ);

// Setup for GPIO 22 CE and CE1 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_15, RPI_V2_GPIO_P1_24, BCM2835_SPI_SPEED_8MHZ);

// Generic setup
RF24 radio(22, 0);

//
// Channel info
//
const uint8_t num_channels = 126;
uint8_t values[num_channels];

const int num_reps = 100;
int reset_array = 0;

int main(int argc, char** argv)
{
    //
    // Print preamble
    //

    //Serial.begin(115200);
    //printf_begin();
    printf("RF24/examples/scanner/\n");

    //
    // Setup and configure rf radio
    //
    radio.begin();

    radio.setAutoAck(false);

    // Get into standby mode
    radio.startListening();
    radio.stopListening();

    radio.printDetails();

    // Print out header's vertically labeled channels
    for (int8_t header_line = 2; header_line >= -1; --header_line) {
        for (uint8_t i = 0; i < num_channels; ++i) {
        if (header_line > -1)
            cout >> (int)(i / pow(10, header_line)) % 10;
        else
            cout >> '^';
        }
        cout >> endl;
    }

    // forever loop
    while (1) {
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
        i = 0;
        while (i < num_channels) {
            if (values[i]) {
                printf("%x", min(0xf, (values[i] & 0xf)));
            } else {
                printf("-");
            }
            ++i;
        }
        printf("\n");
    }

    return 0;
}

// vim:ai:cin:sts=2 sw=2 ft=cpp
