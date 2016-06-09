#ifndef bluetooth_h
#define bluetooth_h
static const char *DEV_NAME = "ArduinoBT";
static const char *const DEV_PIN = "0000";

// Swap RX/TX connections on bluetooth chip
//   Pin 10 --> Bluetooth TX
//   Pin 11 --> Bluetooth RX
static const int DEFAULT_RX = 10;
static const int DEFAULT_TX = 11;
#include "Arduino.h"
#include <SoftwareSerial.h>

class Bluetooth{
private:
    uint8_t rxPin = DEFAULT_RX;
    uint8_t txPin = DEFAULT_TX;
    SoftwareSerial *btSerial;
    char msg[1024];
    char Name[256];

public:
    int getrxPin();
    void setrxPin(int rx);
    int gettxPin();
    void settxPin(int tx);
    void init();
    Bluetooth(const char name[]);
    Bluetooth(const char name[], int r, int t);
    String read();
    String readFully();
    void print(const char *c);
    void print(const String &s);
    char *getName();
    void setName(const char c[]);
};


#endif
