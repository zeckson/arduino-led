#include <SoftwareSerial.h>
#include "bluetooth.h"

//R - 10 pin (connected to T BT)
//T - 11 pin (connected to R BT)
Bluetooth *blue = new Bluetooth("ArduinoBT");


void setup(){
	Serial.begin(9600);
	blue->setrxPin(10);
	blue->settxPin(11);
	blue->setupBluetooth();
}


void loop(){
	String msg = blue->Read();
	if(msg.length() > 1){
		Serial.print("Received: ");
		Serial.println(msg);
	}
	if(Serial.available()){

		blue->Send("Example message#");
	}
}