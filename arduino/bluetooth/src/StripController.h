//
// Created by Evgenii Shchepotev on 09/06/16.
//

#include <HID.h>
#include <Adafruit_NeoPixel.h>
#ifndef BTSETUP_RGBWSTRANDTEST_H
#define BTSETUP_RGBWSTRANDTEST_H

#endif //BTSETUP_RGBWSTRANDTEST_H

void configureStrip();
void rainbowCycle(uint8_t wait);
void startShow(int i);
int findShow(const char *name);
