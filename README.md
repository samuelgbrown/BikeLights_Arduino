# BikeLights_Arduino
BikeLights project portion for Arduino

"BikeLights" is a custom-made lighting setup to generate speed-sensitive light patterns on the edge of a bike's wheel.

Made using [PlatformIO](https://platformio.org/)!

TODO:  Link to video

[See Android portion here](https://github.com/samuelgbrown/BikeLights_Android)

## Instructions to use
TODO: Write up useful instructions to use this version, and to adapt to another bike (in general)

## Theory of Operation
To use BikeLights, the user will design a custom lighting pattern on their Android phone using the Bike Lights app.  The user can then connect to the bluetooth adapter that is mounted on their bike wheel to send the lighting pattern to the LED's on the wheel.  The Arduino board on the wheel-mounted device will then display the designed pattern on the water-proof LED strip mounted along the rim of the wheel, using the information collected by the reed switches.

#### 1. Bluetooth Communication protocol
The Android app and Arduino board must be able to communicate via bluetooth, despite the fact that their software is written in different languages.  Additionally, the memory constraints on the Arduino board necessitated writing a custom bluetooth protocol that would minimize the size of the message that must be stored on the Arduino during processing, while still allowing arbitrarily large messages to be received.  TODO: Add link to bluetooth protocol.

## Electronics / Hardware
[Hardware Bill of Materials](https://htmlpreview.github.io/?https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_bom.html)

Additionally:
  * ~2m per wheel WS2812B (or equivalent) RGBW LED strip
  * 1x 12V Lithium ion battery pack (I used 6800 mAh)


[Schematic](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_schem.png) (please forgive some of the upside-down values...)


[Bread Board configuration](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_bb.png)
  * Blue, black, green, and red are wires
  * Pink are solder blobs)


[Models for mounting hardware](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models)


[Reference pictures of installation](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/pics)
