#include <SoftwareSerial.h>
#include <Arduino.h>

const String DEV_NAME = "ArduinoBT";

const long BAUD4 = 9600;
const long BAUD7 = 57600;
const long BAUD8 = 115200;

const int SPEEDS_LEN = 3;
// If you haven't configured your device before use BAUD4
const long SPEEDS[SPEEDS_LEN] = {BAUD4, BAUD7, BAUD8};


// Swap RX/TX connections on bluetooth chip
//   Pin 10 --> Bluetooth TX
//   Pin 11 --> Bluetooth RX
SoftwareSerial mySerial(10, 11); // RX, TX

const String EMPTY_STR = "";

const int INIT = 0;
const int READ = 1;
const int ERR = 2;
int state = INIT;
String command;

/*
  The posible baudrates are:
    AT+BAUD1-------1200
    AT+BAUD2-------2400
    AT+BAUD3-------4800
    AT+BAUD4-------9600 - Default for hc-06
    AT+BAUD5------19200
    AT+BAUD6------38400
    AT+BAUD7------57600 - Johnny-five speed
    AT+BAUD8-----115200
    AT+BAUD9-----230400
    AT+BAUDA-----460800
    AT+BAUDB-----921600
    AT+BAUDC----1382400
*/
String readString() {
  String content = "";
  char character;

  while (Serial.available()) {
    character = Serial.read();
    content.concat(character);
  }

  if (content != EMPTY_STR) {
    Serial.println(content);
  }
  return content;
}

String waitForResponse() {
  delay(2500);
  String response = readString();
  Serial.println(response);
  return response;
}


void setup() {
  Serial.begin(9600);

  //Speed detection!
  for (int i = 0; i < SPEEDS_LEN; ++i) {
    long speed = SPEEDS[i];
    Serial.println("Connecting at " + speed);
    mySerial.begin(speed);
    // Should respond with OK
    mySerial.print("AT");
    const String &atResponse = waitForResponse();
    if (atResponse == "OK") {
      break;
    } else {
      mySerial.end();
    }
  }

  // Should respond with its version
  mySerial.print("AT+VERSION");
  waitForResponse();

  // Set pin to 0000
  mySerial.print("AT+PIN0000");
  waitForResponse();

  // Set the name to ROBOT_NAME
  mySerial.print("AT+NAME");
  mySerial.print(DEV_NAME);
  waitForResponse();

  // Set baudrate to 57600
  mySerial.print("AT+BAUD7");
  waitForResponse();

  Serial.println("Done!");
}


void loop() {
  switch (state) {
    case INIT:
      Serial.println("Ready to read AT command");
      state = READ;
      break;
    case READ:
      command = readString();
      if (command != EMPTY_STR) {
        mySerial.print(command);
        waitForResponse();
        state = INIT;
      }
      break;
    default:
      Serial.println("Unknown state:" + state);
      break;
  }

  delay(100);
}
