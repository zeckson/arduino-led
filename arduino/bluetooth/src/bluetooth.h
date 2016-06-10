#ifndef bluetooth_h
#define bluetooth_h
static const char *DEV_NAME = "ArduinoBT";
static const char *const DEV_PIN = "0000";

#include "Arduino.h"
#include <SoftwareSerial.h>

class Bluetooth{
private:
    uint8_t rxPin = 10;
    uint8_t txPin = 11;
    SoftwareSerial *btSerial;
    char msg[1024];
    char Name[256];

public:
    int getrxPin();
    void setrxPin(uint8_t rx);
    int gettxPin();
    void settxPin(uint8_t tx);
    void init();
    Bluetooth(const char name[]);
    Bluetooth(const char name[], uint8_t r, uint8_t t);
    String read();
    String readFully();
    void print(const char *c);
    void print(const String &s);
    char *getName();
    void setName(const char c[]);
};


#endif
