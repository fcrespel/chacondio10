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

void transmit(int pin, unsigned sender, bool group, unsigned button, bool onoff) {
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
	sendPair(pin, group);

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
		printf("Usage:  %s <gpio pin> <sender code> <button number> <state> [repeat count]\n", argv[0]);
		printf("Sample: %s 0 12345678 1 1 (button 1 ON)\n", argv[0]);
		printf("        %s 0 12345678 1 0 (button 1 OFF)\n\n", argv[0]);
		printf("Arguments:\n");
		printf("  <gpio pin> is the WiringPi pin number\n");
		printf("  <sender code> is an arbitrary 26-bit number\n");
		printf("  <button number> is a number between 0 and 15, or -1 for all (group function)\n");
		printf("  <state> is 0 (OFF) or 1 (ON)\n");
		printf("  [repeat count] is an optional number of times to repeat the message (default: 5)\n");
		return -1;
	}

	// Parse arguments
	int pin = atoi(argv[1]);    // see https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	int sender = atoi(argv[2]); // 26-bit sender code
	bool group = false;
	int button = atoi(argv[3]); // 4-bit button number
	if (button < 0) {
		button = 0;
		group = true;
	}
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
		transmit(pin, sender, group, button, onoff);
		delay(10);
	}

	return 0;
}
