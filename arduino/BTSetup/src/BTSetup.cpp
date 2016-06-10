#include <SoftwareSerial.h>
#include <Arduino.h>
#include "StripController.h"
#include "bluetooth.h"

static const char *const EMPTY_MSG = "";

const int INIT = 0;
const int READ = 1;
const int LED = 2;
int state = INIT;
int counter = 0;
String message;

Bluetooth *blue = new Bluetooth(DEV_NAME);

String readStream(Stream &serial) {
  String content = "";
  char character;

  while (serial.available()) {
    character = serial.read();
    content.concat(character);
  }

  if (content != EMPTY_MSG) {
    serial.println(content);
  }
  return content;
}

String str(const char* chars) {
  return String(chars);
}

void setup() {
  Serial.begin(9600);

  while (!Serial) { ; // wait for serial port to connect. Needed for Leonardo only
  }
  blue->init();

  configureStrip();
}


void loop() {
  switch (state) {
    case INIT:
      Serial.println("Ready to read AT command");
      state = READ;
      break;
    case READ:
      message = readStream(Serial);
      if (message != EMPTY_MSG) {
        Serial.println("Send command to BT: " + message);
        blue->print(message);
        delay(1500);
        Serial.println(blue->readFully());
        state = INIT;
        break;
      }
      message = blue->read();
      if (message != EMPTY_MSG) {
        Serial.print("Message from BT:" + message);
      }
      if (message == "rainbow#") {
        Serial.println("Begin rainbow!");
        state = LED;
        rainbowCycle(10);
      }
      break;
    case LED:
      Serial.println(str("Rainbow - ") + (counter++));
      rainbowCycle(10);
      if (counter > 20) {
        counter = 0;
        state = INIT;
      }
      break;
    default:
      Serial.println("Unknown state:" + state);
  }

  delay(500);
}
