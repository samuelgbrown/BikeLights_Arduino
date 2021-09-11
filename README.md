# BikeLights_Arduino

TODO: Finish the prepare electronics section!  Make the 2-pin reed switch adapter and the reed switch modules!
TODO: Upload the model for the 2-pin adapter for the reed switches!
TODO:  Link to video

"BikeLights" is a custom-made lighting setup to generate speed-sensitive light patterns on the edge of a bike's wheel.

[See Android portion here](https://github.com/samuelgbrown/BikeLights_Android)

Made using [PlatformIO](https://platformio.org/)!



# Theory of Operation
To use BikeLights, the user will design a custom lighting pattern on their Android phone using the Bike Lights app.  The user can then connect to the bluetooth adapter that is mounted on their bike wheel to send the lighting pattern to the LED's on the wheel.  The Arduino board on the wheel-mounted device will then display the designed pattern on the water-proof LED strip mounted along the rim of the wheel, using the information collected by the reed switches.

### 1. Bluetooth Communication protocol
The Android app and Arduino board must be able to communicate via bluetooth, despite the fact that their software is written in different languages.  Additionally, the memory constraints on the Arduino board necessitated writing a custom bluetooth protocol that would minimize the size of the message that must be stored on the Arduino during processing, while still allowing arbitrarily large messages to be received.  [The protocol design can be found here](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/bluetooth_protocol.txt).

### 2. Speed Detection
The position of the bike wheel is detected using a magnet mounted on the fork of the bike frame and some number N of reed switches mounted in an equiangular fashion around the wheel (see "[Sensors (standard)](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_schem.png)").  The signals from all of the reed switches is treated in the same way, except for one (the "reference").  Each time a reed switch is closed, the signal is filtered through an RC circuit, and one of the hardware flip-flops is closed (one each for standard and reference, see "[RS Flip-Flop](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_schem.png)").  A flip-flop is used so that interrupts are not required.

Once the Speedometer receives a signal from the reed switch (a "tic" or "reference tic"/"rTic"), the new measurement of position and velocity is fed to a [Kalman filter](https://en.wikipedia.org/wiki/Kalman_filter) calculating the wheel's [dead-reckoning](https://en.wikipedia.org/wiki/Dead_reckoning).  The Kalman filter's state variables are the wheel's position, speed, and acceleration in (LEDs, LEDs/s, LEDs/s^2), respectively.  For standard "tic"s, the new position is 1/N fraction of the distance around the wheel.  For "rTic"s, the new position is equal to the next "reference position" (i.e. the location of the wheel when the reference reed switch is aligned with the magnet).  Only one reference reed switch is used to save cost on hardware and space on the PCB.  Once the new measurement is added, the Kalman filter is propogated, and the new estimated position is used to calculate the new lighting position.

### 3. Pattern/Color_ Definition
The lighting on the wheel is defined by two constructs, the Color_ and the Pattern.

The Color_ represents the "paint", and is composed of individual colorObj objects.  Each colorObj represents an actual color (i.e. a hue represented by some combination of red/green/blue light).  A colorObj on its own is represented by a Color_Static object.  A color can then made to change with time (Color_dTime), or with wheel speed (Color_dVel).

The Pattern represents how the Color_ objects are arranged on the wheel.  Each LED can be painted with a Color_.  This pattern can be defined relative to the wheel (wheel speed independent) or relative to the ground (Pattern location changes dependent on wheel speed, to make it appear like it's not moving).  It each case, it can also have some constant angular velocity.  Finally, different patterns can be display in "idle" mode (when the wheel is moving too slowly to be measured by the speedometer) or "main" mode (when the wheel is moving at speed).

# Instructions for use
Note that BikeLights only works on Android devices.

To set up BikeLights:
1. **Prepare** all materials
    1. **Prepare** Electronics
        1. **Purchase** the items on the Bill of Materials and any listed additional items (see below)
        2. **Prepare** a soldering workstation (including solder, wires, etc)
        3. **Solder** the breadboard according to the [bread board configuration](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_bb.png) (see below for wire color legend), using the [schematic](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_schem.png) for reference.
        4. **Prepare** the LED strip
            1. **Measure** the length of LED strip (and, most importantly, the number of LEDs) you will need to surround the circumference of your bike wheel.  Note that the BikeLights looks best when the LEDs are pushed as far towards the circumference of the wheel as possible, and are facing ["outward" from the wheel](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Reed%20Switch%20Positioning.jpg).
            2. **Cut** the LED strip to length
            3. (Optional) **Repair** the waterproofing of the LED strip (using the LED strip waterproofing accessories, listed below), as needed
            4. **Solder** any wires to the strip, as needed
    2. **Prepare** Arduino Software
        1. **Clone** this repo, and open in [PlatformIO](https://platformio.org/)
        2. **Customize** the [definitions in the software](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/include/Definitions.h#L65).  Specifically, set the number of reed switches you will use around the wheel (NUMSWITCHES), the number of LEDs you measured in the previous step (NUMLEDS, must be even), and the number of lights per LED (NUMLIGHTSPERLED, 3 for RGB lights, 4 for RGBW lights [note RGB lights may need some adjustment to work fully]).  You may also later want to customize the timing of the bit-banging for the LED control signal (T0H, T1H, T0L, T1L), using the system written by [bigjosh2 on wp.josh.com](https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/).
        3. **Flash** the software onto the Arduino Nano
    3. **Prepare** Arduino Device
        1. **Side-load** the [BikeLights_Android](https://github.com/samuelgbrown/BikeLights_Android) app onto your device.
        2. **Test** the system, by designing a light Pattern and sending it to the Arduino.  Use a button across the reed switch line or magnets to close the reed switchs themselves to send fake speed data to the system (note that, upon startup, the Arduino must see at least one reference tic [to locate the wheel's angular position] before starting the Main pattern).
    4. **Prepare** Bike wheel
        1. **Print** the [models for the mounting hardware](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models).
            1. The number of [LED clips](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-LED_Strip_Clamp_Friction.stl) that you will need may vary.  For my mount, I used 12, but you want more or less.
            2. Use your own judgement for orientation when printing.  Keep in mind, however, that the cantilever clips operate best when printed in the correct orientation (i.e. such that bending does not separate the layers).  [See this website for more information](https://markforged.com/resources/blog/joinery-onyx) (go to header "Snap Fits").  For some parts, it will not be possible to print all of the clips in the optimal orientation.  ...Sorry.
        3. **Assemble** the parts.
            1. Place the soldered breadboard in its 3D printed case ([top](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Case_Top_New_Final.stl) and [bottom](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Case_Bottom_New_Final.stl)).
            2. Place the battery in its 3D printed case ([top](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Battery_Case_Top_Final.stl) and [bottom](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Battery_Case_Bottom_Final.stl)).
2. **Mount** The Hardware
    1. **Mount** the annular rings
        1. (Optional) **Tape** the insides of the rings with some double-sided tape, to aid adhesion to the central hub.
        2. **Orient** the halves of the ring around the hub how you would like them.  Note that the "[back ring](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-Annular_Ring_Back_Final.stl)" connects to the battery case, and the "[front ring](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-Annular_Ring_Front_Final.stl)" connects to the breadboard case.  It is recommended that you orient the rings such that the "front ring" is [facing a landmark on the wheel, such as the valve](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Main%20Case%202.jpg).
    2. **Mount** the LED strip
        1. **Position** the LED strip on the wheel.
        2. **Attach** the LED strip to the wheel using the LED clips.  Connect the clip to individual spokes by [mounting them to the desired spoke, and feeding a zip-tie through the clip](https://github.com/samuelgbrown/BikeLights_Arduino/blob/readme/pics/Magnet.jpg).  When finally positioned, tighten the zip-tie and cut off any extra.
    3. 
4. **Debug** The System
        

# Adapting for your own bike
BikeLights was custom made for my bike wheel (a Specialized 28" wheel [700x32c]), but it should be possible to adapt to your own needs. 

TODO: Write up useful instructions to use this version, and to adapt to another bike (in general)

# Electronics / Hardware
[Hardware Bill of Materials](https://htmlpreview.github.io/?https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_bom.html)

Additionally:
  * ~2m per wheel WS2812B (or equivalent) RGBW LED strip, preferably with IP67 waterproofing
  * (Optionally) Various LED strip waterproofing accessories
      * [Waterproof Sealant](https://www.superbrightleds.com/moreinfo/adhesives/wfls-gl1oz-1-oz-waterproof-sealant-tube/861/2135/)
      * [Extra silicone tubing](https://www.superbrightleds.com/moreinfo/waterproofing/st10-10mm-silicone-tubing/869/2142/) (probably 10mm)
      * [Extra silicone end-caps](https://www.superbrightleds.com/moreinfo/waterproofing/10mm-silicone-end-cap/864/2138/)
      * [Extra silicone end-caps with wire holes](https://www.superbrightleds.com/moreinfo/waterproofing/10mm-silicone-end-cap-4-holes/866/2140/)
  * 1x 12V Lithium ion battery pack (I used 6800 mAh)
  * Required wire (potentially two types, see Bread Board Configuration below)
  * 1x (4"x6") Prototype PCB / solderable bread-board
  * Neodymium Magnet(s)
  * Bag of small zip-ties (2mm wide will fit in the [LED clips](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-LED_Strip_Clamp_Friction.stl))
  * A healthy dose of duct tape


[Schematic](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_schem.png) (please forgive some of the upside-down values...)


[Bread Board Configuration](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_bb.png)
  * Blue, black, and red are standard-gauge (20-22 AWG) wires
  * Green are fine gauge (36 AWG) wires
    * Note these are a bit difficult to work with
  * Pink are solder blobs


[Models for mounting hardware](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models)


[Reference pictures of installation](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/pics)
