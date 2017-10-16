# Robot Tank

My take on combining Pololu's Zumo Robot for Arduino, Sparkfun's Microview, XBee radio, and MIDI Protocol to build a very expensive toy car.

## Usage

Control the tank's movement by depressing directional buttons, speed determined by "A" option toggle button

### Modes:

1. Default slow mode:  forward, reverse, twirl clockwise, twirl counter-clockwise
2. "A" Full-speed mode:  forward, reverse, twirl, and slightly turn left or right while moving forward or backwards
3. "B" Future use (probably "auto pilot")

## Software Design

Uses MIDI protocol to send Note-On / Note-Off commands from remote to tank.  MIDI note velocity determins speed of vehicle.  MIDI clock & continue commands convey "connected" heartbeat.  Remote controller's Microview display indicates heartbeat radio 2-way communication, vehicle's battery level, and remote's button status

## Hardware

1. Pololu #2510 Zumo Robot for Arduino - https://www.pololu.com/product/2510
2. SparkFun #13975 RedBoard Arduino Clone - https://www.sparkfun.com/products/13975
3. SparkFun #12923 MicroView - https://www.sparkfun.com/products/12923
4. SparkFun #12924 MicroView USB Programmer - https://www.sparkfun.com/products/12924
5. Adafruit #128 XBee Series 1 Radio Module - https://www.adafruit.com/product/128
6. Adafruit #126 XBee Adapter Kit - https://www.adafruit.com/product/126  --  Since discontinued ?!?! NOOO DUDE I LOVE THESE THINGS!!!!
7. Adafruit #1609 Perma-Proto Breadboard Half-Size - https://www.adafruit.com/product/1609
8. Adafruit #367 Tactile Buttons - https://www.adafruit.com/product/367
9. Adafruit #1862 JST 2-Pin Breakout - https://www.adafruit.com/product/1862
10. Adafruit #1304 LiPo Battery Charger - https://www.adafruit.com/product/1304
11. Adafruit Lipo Battery of any size, it's just for the remote right so whatev.
12. Generic AA batteries for Zumo tank

## Installation

I wired a 6 buttons from the positive rail to the MicroView's GPIO pins, sinking them to ground with internal pullup resistors.

Arduino slapped ontop of the Zumo kit.

XBee radio soldered to serial IO pins on the Zumo shield.

## Credits

* SparkFun's MicroView is awesome
* Pololu's Zumo stuff is awesome
* XBee radios are awesome
* Adafruit's parts & guides are awesome

## License

Have a party!