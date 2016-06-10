#include <Arduino.h>
#include <SoftwareSerial.h>
#include "bluetooth.h"
// Swap RX/TX connections on bluetooth chip
//   Pin 10 --> Bluetooth TX
//   Pin 11 --> Bluetooth RX
static const uint8_t DEFAULT_RX = 10;
static const uint8_t DEFAULT_TX = 11;

static const long BAUD4 = 9600;
static const long BAUD6 = 38400;
static const long BAUD7 = 57600;
static const long BAUD8 = 115200;
static const int SPEEDS_LEN = 4;
// Cycle over bauds to find current
// Try popular first
static const long SPEEDS[SPEEDS_LEN] = {BAUD4, BAUD7, BAUD8, BAUD6};
static const char *const EMPTY_MSG = "";
static const char *const OK_MSG = "OK";
static const String CONNECTING_ON = "Connecting on ";
static const String RESPONSE = "Response: ";
static const String NO_RESPONSE = "<NO RESPONSE>";
static const int DEFAULT_DELAY = 2000;
/*
 * Setups anc controls bluetooth controller
  The posible baudrates are:
    AT+BAUD1-------1200
    AT+BAUD2-------2400
    AT+BAUD3-------4800
    AT+BAUD4-------9600 - Default for hc-06
    AT+BAUD5------19200
    AT+BAUD6------38400
    AT+BAUD7------57600 - Johnny-five speed
    AT+BAUD8-----115200
    // This ones are possible not supported by arduino
    AT+BAUD9-----230400
    AT+BAUDA-----460800
    AT+BAUDB-----921600
    AT+BAUDC----1382400
*/

String readFullyInternal(Stream *serial);
String waitForResponse(SoftwareSerial *btSerial);
void autodetect(SoftwareSerial *btSerial);

int Bluetooth::getrxPin() {
  return rxPin;
}

void Bluetooth::setrxPin(uint8_t rx) {
  rxPin = rx;
}

int Bluetooth::gettxPin() {
  return txPin;
}

void Bluetooth::settxPin(uint8_t tx) {
  txPin = tx;
}

void Bluetooth::init() {
  btSerial = new SoftwareSerial(DEFAULT_RX, DEFAULT_TX);

  autodetect(btSerial);

  // Should respond with its version
  btSerial->print("AT+VERSION");
  waitForResponse(btSerial);

  // Set Name
  btSerial->print(Name);
  waitForResponse(btSerial);

  // Set PIN
  btSerial->print("AT+PIN");
  btSerial->print(DEV_PIN);
  waitForResponse(btSerial);

  // Set baudrate to 57600
  btSerial->print("AT+BAUD7");
  waitForResponse(btSerial);

  Serial.println("Bluetooth configured!");
}

Bluetooth::Bluetooth(const char name[]) {
  setName(name);
  setrxPin(DEFAULT_RX);
  settxPin(DEFAULT_TX);
}

Bluetooth::Bluetooth(const char name[], uint8_t r, uint8_t t) {
  setName(name);
  setrxPin(r);
  settxPin(t);
  init();
}

void Bluetooth::setName(const char c[]) {
  strcpy(Name, "AT+NAME");
  strcat(Name, c);
}
char *Bluetooth::getName() {
  return &Name[0];
}


String Bluetooth::read() {
  char c;
  String retorno = "";
  if (btSerial->available())
    while (1) {
      c = btSerial->read();
      retorno += c;
      if (c == '#')
        break;
    }
  return retorno;
}

void Bluetooth::print(const char *c) {
  btSerial->print(c);
}


void Bluetooth::print(const String &s) {
  btSerial->print(s);
}

String Bluetooth::readFully() {
  String content = "";
  char character;

  while (btSerial->available()) {
    character = btSerial->read();
    content.concat(character);
  }

  return content;
}


String readFullyInternal(Stream *serial) {
  String content = "";
  char character;

  while (serial->available()) {
    character = serial->read();
    content.concat(character);
  }

  return content;
}

String waitForResponse(SoftwareSerial *btSerial) {
  delay(DEFAULT_DELAY);
  String response = readFullyInternal(btSerial);
  if (response == EMPTY_MSG) {
    Serial.println(RESPONSE + NO_RESPONSE);
  } else {
    Serial.println(RESPONSE + response);
  }
  return response;
}

boolean connect(long speed, SoftwareSerial *btSerial) {
  Serial.println(CONNECTING_ON + speed);
  btSerial->begin(speed);
  delay(100);
  // Should respond with OK
  btSerial->print("AT");
  const String atResponse = waitForResponse(btSerial);
  return atResponse == OK_MSG;
}

//Autodetect speed rate
void autodetect(SoftwareSerial *btSerial) {
  //Speed detection!
  for (int i = 0; i < SPEEDS_LEN; ++i) {
    long speed = SPEEDS[i];
    if (connect(speed, btSerial)) {
      break;
    } else {
      btSerial->end();
    }
  }
}
