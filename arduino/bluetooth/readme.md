# BLE controlled LED Arduino based device

LED lights controlled with bluetooth, LED, arduino

## Installation
`brew install platformio` -- install platformio

## Dependencies
`SofwareSerial`
`Adafruit_Neopixel`

## Build and compilation
`pio run` -- build everything
`pio run --target upload` -- build and upload to arduino

## Additional info
Everything works by COM protocol, tested on `nanoatmega328`
Project is based on [platfromio](http://docs.platformio.org/en/latest/)
