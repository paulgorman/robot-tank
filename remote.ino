#include <MicroView.h>      // include MicroView library

int Bbutton = A0;   //pin numbers
int Abutton = A1;
int Lbutton = A2;
int Dbutton = A3;
int Ubutton = 2;
int Rbutton = 3;

int Astatus = 0;
int Astate  = 0;
int Bstate  = 0;
int Bstatus = 0;
int Ustatus = 0;
int Dstatus = 0;
int Lstatus = 0;
int Rstatus = 0;

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

byte sendByte;  // 1st byte 	command (144 == note on, 128 == note off)
byte note;      // 2nd byte     note (36==middle_C)
byte velocity;	// 3rd byte     velocity (100==omg go on fast, 0==off / Note Off command does not have velocity)

long Atime = 0;         // the last time the output pin was toggled
long Btime = 0;
long debounce = 300;   // the debounce time, increase if the output flickers
long txInterval = 500;    // how often to transmit
long rxIntervalDisplay = 120;    // how often to indicate received data
long txIntervalDisplay = 120; // keep the tx indicator on the display
long stopDelay = 250;						// send stop moving command after this timeout
int stopCounterMax = 10;						// yes we get it the tank is stopped!
int stopCounter = 0;							// count the stop commands 
unsigned long previousMillis = 0;        // global loop timing
unsigned long previousMillisRx = 0;        // global loop timing
unsigned long previousMillisStop = 0;		// stop moving command timer

void setup() {
	Serial.begin(57600);
	
	pinMode(Bbutton, INPUT_PULLUP);
	pinMode(Abutton, INPUT_PULLUP);
	pinMode(Ubutton, INPUT_PULLUP);
	pinMode(Dbutton, INPUT_PULLUP);
	pinMode(Lbutton, INPUT_PULLUP);
	pinMode(Rbutton, INPUT_PULLUP);
					
    uView.begin();          // start MicroView
    uView.clear(PAGE);      // clear page
    uView.setCursor(0,0);
    //uView.print("Tank Remote");
    uView.display();
}

void loop() {
    unsigned long currentMillis = millis();
    
    if(currentMillis - previousMillis > txInterval/2) {  // let the TX illuminator glow for a moment.
        //uView.clear(PAGE);
		uView.circle(50,6,2,0,0);	// small circle means transmitting to tank
		uView.display();
    }
    if(currentMillis - previousMillisRx > rxIntervalDisplay) {	// received from tank
		uView.circle(50,6,4,0,0);	// big circle means received from tank
		uView.display();
        previousMillisRx = currentMillis;
    }

	if (digitalRead(Abutton) == LOW && millis() - Atime > debounce) {
		if (Astate == 0) {
			Astate = 1;
    	} else {
			Astate = 0;
    	}
		Atime = millis();    
	}
	if (digitalRead(Bbutton) == LOW && millis() - Btime > debounce) {
		if (Bstate == 0) {
			Bstate = 1;
    	} else {
			Bstate = 0;
    	}
		Btime = millis();    
	}

	if (digitalRead(Lbutton) == LOW) {
		Lstatus = 1;
	} else if (digitalRead(Lbutton) == HIGH) {
		Lstatus = 0;
	}
	if (digitalRead(Rbutton) == LOW) {
		Rstatus = 1;
	} else if (digitalRead(Rbutton) == HIGH) {
		Rstatus = 0;
	}
	if (digitalRead(Ubutton) == LOW) {
		Ustatus = 1;
	} else if (digitalRead(Ubutton) == HIGH) {
		Ustatus = 0;
	}
	if (digitalRead(Dbutton) == LOW) {
		Dstatus = 1;
	} else if (digitalRead(Dbutton) == HIGH) {
		Dstatus = 0;
	}
	
	uView.setCursor(0,0);

	if (Astate == 1 && Bstate == 1) {
		uView.print("A B");
	} else if (Astate == 1 && Bstate == 0) {
		uView.print("A  ");
	} else if (Astate == 0 && Bstate == 1) {
		uView.print("  B");
	} else {
		uView.print("   ");
	}

	// draw UI arrows

	if (Ustatus == 1) {
		uView.line(32,14,32,26); // vert line at 32 over, 14 thru 26 down
		uView.line(32,14,28,18);
		uView.line(32,14,37,19);
	} else {
		uView.line(32,14,32,26,0,0);
		uView.line(32,14,28,18,0,0);
		uView.line(32,14,37,19,0,0);
	}
	if (Dstatus == 1) {
		uView.line(32,34,32,46); // vert line down at 32 over, 34 thru 46 down
		uView.line(32,46,28,42);
		uView.line(32,46,37,41);
	} else {
		uView.line(32,34,32,46,0,0);
		uView.line(32,46,28,42,0,0);
		uView.line(32,46,37,41,0,0);
	}
	if (Lstatus == 1) {
		uView.line(12,30,30,30);	// horiz line 30 down, 12 thru 30 over
		uView.line(12,30,16,34);
		uView.line(12,30,16,26);
	} else {
		uView.line(12,30,30,30,0,0);
		uView.line(12,30,16,34,0,0);
		uView.line(12,30,16,26,0,0);
	}
	if (Rstatus == 1) {
		uView.line(35,30,51,30);	// horiz line 30 down, 35 thru 50 over
		uView.line(50,30,46,26);
		uView.line(50,30,46,34);
	} else {
		uView.line(35,30,51,30,0,0);
		uView.line(50,30,46,26,0,0);
		uView.line(50,30,46,34,0,0);
	}
	uView.display();

	// send drive or stop control code first
	if (Ustatus == 0 && Dstatus == 0 && Lstatus == 0 && Rstatus == 0) {
    	if(currentMillis - previousMillisStop > stopDelay) {	// no buttons are being held, stop the tank
        	previousMillisStop = currentMillis;					// don't flood stop commands
        	if (stopCounter < stopCounterMax) {					// don't send a bazillion stop commands.
        		stopCounter = stopCounter + 1;
        		uView.circle(50,6,1);
        		uView.display();
				Serial.write(STOP);
			}
    	}
		// heartbeat
    	if(currentMillis - previousMillis > txInterval) { // time to transmit heartbeat once every txInterval
    	    previousMillis = currentMillis;
			uView.circle(50,6,2);	// small circle means transmitting to tank
			uView.circle(50,6,1,0,0);  // erase command sent dot
			uView.display();
			Serial.write(HEARTBEAT);
		}
	} else {
		// send direction commands to tank
		uView.circle(50,6,1);
		uView.display();
		// Forward
		stopCounter = 0;	// okay to send new stop commands
		if (Ustatus == 1 && Dstatus == 0) {
			Serial.write(GO);
			Serial.write(FORWARD);
			if (Astate == 1) { Serial.write(FAST); } else { Serial.write(SLOW); }
		} 
		// Backwards
		if (Dstatus == 1 && Ustatus == 0) {
			Serial.write(GO);
			Serial.write(BACK);
			if (Astate == 1) { Serial.write(FAST); } else { Serial.write(SLOW); }
		}
		// Left
		if (Lstatus == 1 && Rstatus == 0) {
			Serial.write(GO);
			Serial.write(LEFT);
			if (Astate == 1) { Serial.write(FAST); } else { Serial.write(SLOW); }
		}
		// Right
		if (Rstatus == 1 && Lstatus == 0) {
			Serial.write(GO);
			Serial.write(RIGHT);
			if (Astate == 1) { Serial.write(FAST); } else { Serial.write(SLOW); }
		}
	}


	/*
	if (Bstate == 1) {
		Serial.println("B");
	} else {
		Serial.println("b");
	}
	*/

	// receive anything from tank
	if (Serial.available() > 0) {
		while (Serial.available()) {
        	previousMillisRx = currentMillis;
			byte incomingByte = Serial.read();
			if(incomingByte==CONTINUE) {
				uView.circle(50,6,4);	// bigger circle means received heartbeat back from tank
				uView.display();
			} else if (incomingByte == 60) {
				while (Serial.available()) {
					int voltage = Serial.read();
					uView.setCursor(0,11);
					uView.print("      ");
					uView.display();
					uView.setCursor(0,11);
					//String volts;
					//volts = convertFloatToString(voltage);
					uView.print(voltage);
					uView.display();
				}
			} else {
				/*
				// I don't care right now, voltage is more interesting
    			uView.setCursor(0,0);
				uView.print("   ");
				uView.display();
    			uView.setCursor(0,0);
				uView.print(incomingByte); // means we got something unexpected back from tank
				uView.display();
				*/
			}
		}
	}

}

String convertFloatToString(float longnumber) { // begin function
	char temp[10];
	String tempAsString;
  	// perform conversion
  	dtostrf(longnumber,3,3,temp);
  	// create string object
	tempAsString = String(temp);
	return tempAsString;
}