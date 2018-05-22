# BLE controlled LED Arduino based device

LED lights controlled with bluetooth, LED, arduino

## Installation
`pio lib install`

## Dependencies
`SofwareSerial`
`Adafruit_Neopixel`

## Build and compilation
`pio run` -- build everything
`pio run --target upload` -- build and upload to arduino

## Additional info
Everything works by COM protocol, tested on `nanoatmega328`