
// Set up Zumo motor shield pins
int PWM_L = 10;
int PWM_R = 9;
int DIR_L = 8;
int DIR_R = 7;

int ledPin = 13;					// LED connected to digital pin 13
int txIntervalDisplay = 200;		// keep the tx indicator on the display
unsigned long previousMillis = 0;	// global loop timing
int batteryUpdateInterval = 1000;	// how often do we message back battery status
unsigned long previousBatteryMillis = 0;

byte FORWARD = 36;	// 3rd octave middle C
byte RIGHT = 38;	// 3rd oct D
byte LEFT = 40;		// 3rd oct E
byte BACK = 41;		// 3rd oct F
byte FAST = 127;	// velocity fast
byte SLOW = 1;		// velocity slow

byte GO = 144;		// MIDI for "Note On"
byte STOP = 128;		// MIDI for "Note Off"

byte HEARTBEAT=248; // MIDI Clock that I'm using as a XBee debug heartbeat
byte CONTINUE=251;	// MIDI "Continue" as response to heartbeat clock

int fastSpeed = 400;	// How fast do I want fast to be, scale of 0 ~ 400
int slowSpeed = 160;		// How slow should slow be, scale of 0 ~ 400

byte incomingByte;  // first byte, command (144 == note on, 128 == note off)
byte note;      // 2nd byte     (36==middle_C)
byte velocity; // 3rd byte     (100==omg go on fast, 0==off / Note Off command does not have velocity)

byte previousMove;		// I want to monitor if the previous move was a forward/back command to smooth turning

void setup() {
	Serial.begin(57600);
	pinMode(ledPin, OUTPUT);
	pinMode(PWM_L, OUTPUT);
	pinMode(PWM_R, OUTPUT);
	pinMode(DIR_L, OUTPUT);
	pinMode(DIR_R, OUTPUT);
}

void loop() {
    unsigned long currentMillis = millis();
	if (Serial.available() > 0) {
		while (Serial.available()) {
			byte incomingByte = Serial.read();
			// heartbeat?
			if (incomingByte == HEARTBEAT) {	// 248 is MIDI clock / debug heartbeat
				digitalWrite(ledPin, HIGH); // XXX DEBUG TURN ME BACK ON
				byte note=251;	// MIDI "Continue"
				Serial.write(note); // message back our acknowledgement
			}
			// Move command?
			if (incomingByte == STOP) {
				previousMove = STOP;
				digitalWrite(ledPin, HIGH); // XXX DEBUG TURN ME BACK ON
				leftMotor(0);
				rightMotor(0);
			}
			if (incomingByte == GO) {
//Serial.write(incomingByte); // debug echo back what we received
				digitalWrite(ledPin, HIGH); // XXX DEBUG TURN ME BACK ON
				if (Serial.available() > 0) {
					byte direction = Serial.read();
					byte velocity = Serial.read();
					if (direction == FORWARD) {
						previousMove = FORWARD;
						if (velocity == FAST) {
							leftMotor(fastSpeed);
							rightMotor(fastSpeed);
						} else if (velocity == SLOW) {
							leftMotor(slowSpeed);
							rightMotor(slowSpeed);
						}
					}
					if (direction == BACK) {
						previousMove = BACK;
						if (velocity == FAST) {
							leftMotor(fastSpeed,1);
							rightMotor(fastSpeed,1);
						} else if (velocity == SLOW) {
							leftMotor(slowSpeed,1);
							rightMotor(slowSpeed,1);
						}
					}
					if (direction == LEFT) {
						if (velocity == FAST){
							if (previousMove == FORWARD) {	// make pushing two buttons smoother
								leftMotor(slowSpeed); // ahead to left
								rightMotor(fastSpeed);
							} else if (previousMove == BACK) {
								leftMotor(fastSpeed,1); // butt to right
								rightMotor(slowSpeed,1);
							} else {
								leftMotor(fastSpeed,1); // twirl
								rightMotor(fastSpeed);
							}
						} else if (velocity == SLOW) {
							if (previousMove == FORWARD) {
								rightMotor(slowSpeed);	// moving slow forward left, just don't run left motor
							} else if (previousMove == BACK) {
								leftMotor(slowSpeed,1);
							} else {
								leftMotor(slowSpeed,1);
								rightMotor(slowSpeed);
							}
						}
					}
					if (direction == RIGHT) {
						if (velocity == FAST) {
							if (previousMove == FORWARD) {
								leftMotor(fastSpeed);
								rightMotor(slowSpeed);
							} else if (previousMove == BACK) {
								leftMotor(slowSpeed,1);
								rightMotor(fastSpeed,1);
							} else {
								leftMotor(fastSpeed);
								rightMotor(fastSpeed,1);
							}
						} else if (velocity == SLOW) {
							if (previousMove == FORWARD) {
								leftMotor(slowSpeed);
							} else if (previousMove == BACK) {
								rightMotor(slowSpeed,1);
							} else {
								leftMotor(slowSpeed);
								rightMotor(slowSpeed,1);
							}
						}
					}
					delay(10);
				} else {
					//Serial.write(222);
					delay(10);
				}
			}
		}
	} else {
    	if(currentMillis - previousMillis > txIntervalDisplay) { // cycle off the heartbeat LED
        	previousMillis = currentMillis;
			digitalWrite(ledPin, LOW);
		}
	}
	if (currentMillis - previousBatteryMillis > batteryUpdateInterval) {
		// update the controlelr with battery status
		previousBatteryMillis = currentMillis;
		// read the input on analog pin 1:
		int sensorValue = analogRead(A1);
		// Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
		float voltage = sensorValue * (5.0 / 1023.0);
		Serial.write(60);
		Serial.print(sensorValue);
	}
}

void leftMotor(int speed, int reverse=0) {
	if (speed > 400) {  // Max 
		speed = 400;
	}
	analogWrite(PWM_L, speed * 51 / 80); // default to using analogWrite, mapping 400 to 255
	if (reverse) {
		digitalWrite(DIR_L, HIGH);
	} else {
		digitalWrite(DIR_L, LOW);
	}
}

void rightMotor(int speed, int reverse=0) {
	if (speed > 400) { //max value is 400
		speed = 400;
	}
	analogWrite(PWM_R, speed * 51 / 80);
	if (reverse) {
		digitalWrite(DIR_R, HIGH);
	} else {
		digitalWrite(DIR_R, LOW);
	}
}