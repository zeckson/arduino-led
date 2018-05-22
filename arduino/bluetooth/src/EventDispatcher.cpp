#include <SoftwareSerial.h>
#include <Arduino.h>
#include "StripController.h"
#include "bluetooth.h"

static const char *const EMPTY_MSG = "";

const int INIT = 0;
const int READ = 1;
const int LED = 2;

const int ARDUINO_LED_PIN = 13;

int state = INIT;
int counter = 0;
String message;
int current_show = 0;

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

String str(const char *chars) {
  return String(chars);
}

const int DEFAULT_DELAY = 1000;
void blink() {
  digitalWrite(ARDUINO_LED_PIN, HIGH);       // sets the digital pin 13 on
  delay(DEFAULT_DELAY);                  // waits for a second
  digitalWrite(ARDUINO_LED_PIN, LOW);        // sets the digital pin 13 off
  delay(DEFAULT_DELAY);
}

void setup() {
  pinMode(ARDUINO_LED_PIN, OUTPUT);
  blink();
  Serial.begin(9600);

  while (!Serial) { ; // wait for serial port to connect. Needed for Leonardo only
  }
  blue->init();

  configureStrip();

  blink();
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
        Serial.print("Send command to BT: ");
        Serial.println(message);
        blue->print(message);
        delay(1500);
        Serial.println(blue->readFully());
        state = INIT;
        break;
      }
      message = blue->read();
      if (message != EMPTY_MSG) {
        message = message.substring(0, message.length() - 1);
        Serial.print("Message from BT:");
        Serial.println(message);
        current_show = findShow(message.c_str());
        if (current_show >= 0) {
          state = LED;
        }
      }
      if (message == "rainbow#") {
        Serial.println("Begin rainbow!");
        state = LED;
      }
      break;
    case LED:
      Serial.print("Rainbow - ");
      Serial.print(message);
      Serial.println(counter++);
      startShow(current_show);
      if (counter >= 1) {
        counter = 0;
        state = INIT;
      }
      break;
    default:
      Serial.println("Unknown state:" + state);
  }

  delay(500);
}

