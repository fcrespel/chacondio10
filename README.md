# Chacon DIO 1.0 transmitter

This project provides a simple program to send commands to Chacon DIO 1.0 power outlet sockets. It has only been tested on a Raspberry Pi and ODROID-C4.

## Compilation

To compile this program, make sure you have built and installed the [WiringPi](https://projects.drogon.net/raspberry-pi/wiringpi/download-and-install/) library first.

Then execute `make` in the code directory and you should obtain the `send` program.

The `send.sh` wrapper script makes use of `flock` to prevent concurrent calls using a file lock.

## Usage

Connect a 433MHz transmitter to the GPIO pin of your choice on a Raspberry Pi (see the [pin numbering](https://projects.drogon.net/raspberry-pi/wiringpi/pins/) documentation).

Then execute:

`./send.sh <gpio pin> <sender code> <button number> <state> [repeat count]`

- `<gpio pin>` is the WiringPi pin number
- `<sender code>` is an arbitrary 26-bit number
- `<button number>` is a number between 0 and 15, or -1 for all (group function)
- `<state>` is 0 (OFF) or 1 (ON)
- `[repeat count]` is an optional number of times to repeat the message (default: 5)

For example, use:

- `./send.sh 0 12345678 1 1` for button 1 ON
- `./send.sh 0 12345678 1 0` for button 1 OFF
- `./send.sh 0 12345678 -1 1` for all buttons ON
- `./send.sh 0 12345678 -1 0` for all buttons OFF

## Protocol

The Chacon DIO 1.0 protocol was reverse engineered by Idleman, this project was directly inspired by it.

References:

- http://blog.idleman.fr/raspberry-pi-10-commander-le-raspberry-pi-par-radio/
- http://blog.idleman.fr/raspberry-pi-12-allumer-des-prises-distance/
