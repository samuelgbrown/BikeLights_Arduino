# BikeLights_Arduino

"BikeLights" is a system to generate speed-sensitive light patterns on the edge of a bike's wheel.  It uses an Android device to design a pattern, and an Arduino system to display it on the wheel.

TODO:  Link to video

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
            1. **Prepare** 2 female 2-pin JST connectors with wires, to be used as connectors for the reed switches
        4. **Prepare** the 1-3 sensor connector adapter
            1. **Print** the [top half](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/1-to-3_Sensor_Connector_Adapter_Top_Final.stl) and [bottom half](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/1-to-3_Sensor_Connector_Adapter_Bottom_Final.stl) of the adapter case.
            2. **Crimp** a single male 2-pin JST connector to 3 female 2-pin JST connectors (in parallel).
            3. **Glue** this circuit of JST connectors in the adapter case (it will be a tight fit, but it will work).
            4. **Test** the connections of the connector adapter (using a test LED, a continuity tester, etc).
        5. **Prepare** the reed switch modules
            1. **Solder** 4 reed switches to wires in an ["inline" configuration](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Magnet.jpg) (wires should run along side the reed switch).  The 2 of the modules should have wire lengths of about 8" and 2 should have about 14".  Heat-shrink the exposed wires/solder.
            2. **Crimp** a male 2-pin JST connectors onto each module.
        7. **Prepare** the LED strip
            1. **Measure** the length of LED strip (and, most importantly, the number of LEDs) you will need to surround the circumference of your bike wheel.  Note that the BikeLights looks best when the LEDs are pushed as far towards the circumference of the wheel as possible, and are facing ["outward" from the wheel](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Reed%20Switch%20Positioning.jpg).
            2. **Cut** the LED strip to length.
            3. (Optional) **Repair** the waterproofing of the LED strip (using the LED strip waterproofing accessories, listed below), as needed.
            4. **Solder** any wires to the strip, as needed.
            5. (Optional) **Solder** an extension cable for the LED strip wires (a female 3-pin JST connector to a male 3-pin JST connector), that should be about 6" long.
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
            1. Place the soldered breadboard in its 3D printed case ([top](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Case_Top_New_Final.stl) and [bottom](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Case_Bottom_New_Final.stl)).  Attach any connectors to the breadboard, and glue these connectors to the top half of the case (the bottom should still be removable after gluing).
            2. Place the battery in its 3D printed case ([top](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Battery_Case_Top_Final.stl) and [bottom](https://github.com/samuelgbrown/BikeLights_Arduino/tree/master/models/Bike_Lights_Battery_Case_Bottom_Final.stl)).
2. **Mount** The Hardware
    1. **Mount** the annular rings
        1. (Optional) **Tape** the insides of the rings with some double-sided tape, to aid adhesion to the central hub.
        2. **Orient** the halves of the ring around the hub how you would like them.  Note that the "[back ring](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-Annular_Ring_Back_Final.stl)" connects to the battery case, and the "[front ring](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-Annular_Ring_Front_Final.stl)" connects to the breadboard case.  It is recommended that you orient the rings such that the "front ring" is [facing a landmark on the wheel, such as the valve](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Main%20Case%202.jpg).
    2. **Mount** the LED strip
        1. **Position** the LED strip on the wheel.
        2. **Attach** the LED strip to the wheel using the LED clips.  Connect the clip to individual spokes by [mounting them to the desired spoke, and feeding a zip-tie through the clip](https://github.com/samuelgbrown/BikeLights_Arduino/blob/readme/pics/Magnet.jpg).  When finally positioned, tighten the zip-tie and cut off any extra.
        3. (Optional) [**Mark** the LED strip where the LED clips are touching it](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Suggested%20Guide%20Marks.jpg), for easier re-mounting later.
    3. **Attach** the battery and breadboard cases to the annular ring.  Note that the breadboard case has a specific orientation relative to the annular ring.
        1. **Secure** any unused wires on the battery (try [securely wrapping them around the spokes near the hub](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Main%20Case%202.jpg)).
        2. **Attach** the 1-3 connector adapter to the "standard switch" port on the breadboard case.
    4.  **Mount** the reed switch modules to the spokes of the wheel.
        1.  **Position** the modules such that they are [equally spaced around the wheel](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Full%20System.jpg).  The module that will become the "reference" should be oriented such that it is going over the portion of the fork that will hold the magnet when the wheel is oriented "upwards".  Also keep in mind the amount of wire required to connect the module back to the breadboard case.
        2.  **Tape** the modules to the wheel in these positions.  Note that they should all be about the same radius away from the hub.
        3.  **Mount** the magnets to the bike fork, [such that they will be close to the reed switches as the wheel rotates](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Magnet.jpg).  Note that a bit of trial and error will be required here (see Debug The System), as you may find that the reed switch may be most sensitive to the magnet at the edges of the module rather than dead center (see [picture reference](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/pics/Magnet.jpg)).
    5.  **Connect** all components
        1. **Plug** in the three standard reed switch modules to the 1-3 sensor connector adapter.
        2. **Plug** in the reference reed switch module to the reference sensor port on the breadboard case.
        3. **Plug** in the LED strip (optionally using the ~6" extension cable that you may have made) to the LED strip port on the breadboard case.
        4. **Plug** in the battery to the DC power port on the breadboard case.
    6. **Secure** the system
        1. **Test** how well connected the various parts of the system are, and add duct tape where necessary to reduce rattle and keep everything in place.
4. **Debug** The System
    1. **Rejoice** that you should now have a fully functional BikeLights system!  However, if something has gone wrong, consider the following debug steps.
    2. **Connect** to the Arduino's debug screen
        1. **Download** a bluetooth terminal for your Android device (I use [S2 Terminal for Bluetooth](https://m.apkpure.com/s2-terminal-for-bluetooth-free/jp.side2.apps.btterm), but consider any Bluetooth terminal application).
        2. **Connect** to the Arduino using the bluetooth terminal.
        3. **Send** the character "d" for the debug screen.
            1. "Tic debugging" - This will send a message to your bluetooth terminal every time a reed switch module is activated.  Also differentiates between "standard tics" and "reference tics".
            2. "Block extra reference tics" - This will prevent extra reference signals being sent, by requiring that at least one "standard tic" is detected between registering "reference tics".
            3. "Block extra standard tics" - This will block any extra "standard tics" being sent between "reference tics".
            4. "Use adaptive debouncing" (obsolete - incorporated) - This flag used to enable the adaptive debouncing feature that has since been merged.  It now enables some information about the adaptive debouncing.
        4. **Send** the character corresponding to the desired debug mode (such as "d1" for "Tic debugging").
    3. **Test** the reed switch positioning and functionality.  Try rigging an LED to light when the reed switch is closed.  A spare magnet can be used to check that they work, and the bike-mounted magnet can be used to confirm that they are being triggered correctly.
    4. **Design** a "location test" pattern on the Arduino device to send.  A good test of the actual position is setting only a single pixel at a time to be active.
        

# Adapting for your own bike
BikeLights was custom made for my bike wheel (a Specialized 28" wheel [700x32c]), but it should be possible to adapt to your own needs.  To adapt to your own bike, consider changing the following:
* The mounting system
    * This will likely be the most difficult part.
    * You should adapt the annular ring (both the [front](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-Annular_Ring_Front_Final.stl) and [back](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights-Annular_Ring_Back_Final.stl) models).  The inner curvature of these models will likely need to be changed.  If any of the geometry of the connection system to the battery case or breadboard cases is changed, make sure to make complimentary changes to the [top](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights_Battery_Case_Top_Final.stl) and [bottom](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights_Battery_Case_Bottom_Final.stl) halves of the battery cases and/or the [top](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights_Case_Top_New_Final.stl) and [bottom](https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/models/Bike_Lights_Case_Bottom_New_Final.stl) halves of the breadboard case.
    * Make sure that you will still be able to get the battery/breadboard cases between the spokes, and that there will be room for them inside of the wheel.
* The defined number of LEDs, and the number of reed switches (as described in "Prepare Arduino Software" -> "Customize", above).

# Electronics / Hardware
[Hardware Bill of Materials](https://htmlpreview.github.io/?https://github.com/samuelgbrown/BikeLights_Arduino/blob/master/Bike_Lights_refined_bom.html)

(Note 1: This BOM (and the following) is to make ONE system for ONE bike wheel.  Consider doubling where necessary to make a pair.)

(Note 2: for reed switches, they must be NO switches (normaly open).  Also, I recommend getting some that have a plastic case around them, for longevity.)

Additionally:
  * ~2m per wheel WS2812B (or equivalent) RGBW LED strip, preferably with IP67 waterproofing
  * (Optionally) Various LED strip waterproofing accessories
      * [Waterproof Sealant](https://www.superbrightleds.com/moreinfo/adhesives/wfls-gl1oz-1-oz-waterproof-sealant-tube/861/2135/)
      * [Extra silicone tubing](https://www.superbrightleds.com/moreinfo/waterproofing/st10-10mm-silicone-tubing/869/2142/) (probably 10mm)
      * [Extra silicone end-caps](https://www.superbrightleds.com/moreinfo/waterproofing/10mm-silicone-end-cap/864/2138/)
      * [Extra silicone end-caps with wire holes](https://www.superbrightleds.com/moreinfo/waterproofing/10mm-silicone-end-cap-4-holes/866/2140/)
  * [2-pin "JST" connectors](https://www.aliexpress.com/item/32872148939.html?spm=a2g0o.productlist.0.0.6c813bc6g6SV67&algo_pvid=44d72e61-6fd6-4175-92c8-ef2d347c9b56&algo_exp_id=44d72e61-6fd6-4175-92c8-ef2d347c9b56-16&pdp_ext_f=%7B%22sku_id%22%3A%2265482419468%22%7D)
  * (Optionally) 3-pin "JST SM" connectors for WS2812B WS2811 WS2812 SK6812 LED strips
  * Small lot (~5) of 5.5x2.1mm DC power plug (male and female)
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
