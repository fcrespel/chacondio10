#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#define TIME_HIGH_LOCK 275
#define TIME_LOW_LOCK1 9900
#define TIME_LOW_LOCK2 2675

#define TIME_HIGH_DATA 275        // 310 or 275 or 220
#define TIME_LOW_DATA_LONG 1225   // 1340 or 1225 or 1400
#define TIME_LOW_DATA_SHORT 275   // 310 or 275 or 350

void sendBit(int pin, bool b){
	if (b) {
		digitalWrite(pin, HIGH);
		delayMicroseconds(TIME_HIGH_DATA);
		digitalWrite(pin, LOW);
		delayMicroseconds(TIME_LOW_DATA_LONG);
	} else {
		digitalWrite(pin, HIGH);
		delayMicroseconds(TIME_HIGH_DATA);
		digitalWrite(pin, LOW);
		delayMicroseconds(TIME_LOW_DATA_SHORT);
	}
}

void sendPair(int pin, bool b) {
	sendBit(pin, b);
	sendBit(pin, !b);
}

void sendWord(int pin, unsigned word, unsigned bits) {
	for (int i = bits-1; i >= 0; i--) {
		if (word & (1 << i)) {
			sendPair(pin, true);
		} else {
			sendPair(pin, false);
		}
	}
}

void transmit(int pin, unsigned sender, unsigned button, bool onoff) {
	// Start lock
	digitalWrite(pin, HIGH);
	delayMicroseconds(TIME_HIGH_LOCK);
	digitalWrite(pin, LOW);
	delayMicroseconds(TIME_LOW_LOCK1);
	digitalWrite(pin, HIGH);
	delayMicroseconds(TIME_HIGH_LOCK);
	digitalWrite(pin, LOW);
	delayMicroseconds(TIME_LOW_LOCK2);
	digitalWrite(pin, HIGH);

	// Sender code (26 bits)
	sendWord(pin, sender, 26);

	// Group bit
	sendPair(pin, false);

	// On/off bit
	sendPair(pin, onoff);

	// Button number (4 bits)
	sendWord(pin, button, 4);

	// End lock
	digitalWrite(pin, HIGH);
	delayMicroseconds(TIME_HIGH_LOCK);
	digitalWrite(pin, LOW);
}

int main(int argc, char **argv) {
	// If no command line argument is given, print the help text
	if (argc < 5) {
		printf("Usage:  %s <gpio pin> <sender code> <button number> <0|1> [repeat count]\n", argv[0]);
		printf("Sample: %s 0 12345678 1 1 5\n", argv[0]);
		return -1;
	}

	// Parse arguments
	int pin = atoi(argv[1]);    // see https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	int sender = atoi(argv[2]); // 26-bit sender code
	int button = atoi(argv[3]); // 4-bit button number
	int onoff = atoi(argv[4]);  // 1-bit on/off
	int repeat = 5;             // number of times to repeat the message
	if (argc >= 6) {
		repeat = atoi(argv[5]);
	}

	// Configure WiringPi
	if (wiringPiSetup() == -1) return 1;
	pinMode(pin, OUTPUT);

	// Send message
	for (int i = 0; i < repeat; i++) {
		transmit(pin, sender, button, onoff);
		delay(10);
	}

	return 0;
}
