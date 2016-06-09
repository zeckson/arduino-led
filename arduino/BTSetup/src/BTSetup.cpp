#include <SoftwareSerial.h>
#include <Arduino.h>

static const int DEFAULT_DELAY = 2000;
static const char *const DEV_NAME = "ArduinoBT";
static const char *const DEV_PIN = "0000";

const long BAUD4 = 9600;
const long BAUD6 = 38400;
const long BAUD7 = 57600;
const long BAUD8 = 115200;

const int SPEEDS_LEN = 4;
// Cycle over bauds to find current
const long SPEEDS[SPEEDS_LEN] = {BAUD4, BAUD6, BAUD7, BAUD8};


// Swap RX/TX connections on bluetooth chip
//   Pin 10 --> Bluetooth TX
//   Pin 11 --> Bluetooth RX
SoftwareSerial btSerial(10, 11); // RX, TX

static const String EMPTY_MSG = "";
static const char *const OK_MSG = "OK";
const String CONNECTING_ON = "Connecting on ";

const int INIT = 0;
const int READ = 1;
int state = INIT;
String message;

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

String waitForResponse() {
  delay(DEFAULT_DELAY);
  String response = readStream(btSerial);
  Serial.println(response);
  return response;
}

boolean connect(long speed) {
  Serial.println(CONNECTING_ON + speed);
  btSerial.begin(speed);
  delay(100);
  // Should respond with OK
  btSerial.print("AT");
  const String atResponse = waitForResponse();
  return atResponse == OK_MSG;
}

void connect() {
  //Speed detection!
  for (int i = 0; i < SPEEDS_LEN; ++i) {
    long speed = SPEEDS[i];
    if (connect(speed)) {
      break;
    } else {
      btSerial.end();
    }
  }
}

void setup() {
  Serial.begin(9600);

  while (!Serial) { ; // wait for serial port to connect. Needed for Leonardo only
  }
  connect();

  // Should respond with its version
  btSerial.print("AT+VERSION");
  waitForResponse();

  btSerial.print("AT+PIN");
  btSerial.print(DEV_PIN);
  waitForResponse();

  btSerial.print("AT+NAME");
  btSerial.print(DEV_NAME);
  waitForResponse();

  // Set baudrate to 57600
  btSerial.print("AT+BAUD7");
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
      message = readStream(Serial);
      if (message != EMPTY_MSG) {
        Serial.println("Send command to BT: " + message);
        btSerial.print(message);
        waitForResponse();
        state = INIT;
        break;
      }
      message = readStream(btSerial);
      if (message != EMPTY_MSG) {
        Serial.print("Message from BT:" + message);
      }
      break;
    default:
      Serial.println("Unknown state:" + state);
  }

  delay(500);
}
