/*

  The Pattern object is responsible for generating the LED pattern displayed to the wheel.  Its input is the position, velocity, and acceleration of the "reference point" on the LED strip.
  The "reference point" is a value from [0 NUMLEDS) that represents which LED is at the top of the wheel at any given point in time.

  The Pattern_Handler object is responsible for loading Patterns, allowing the user to swap out different patterns at will.
  It also calls main and idle animations independently, allowing the user to chose main and idle animations individually.

  If the speed of the wheel falls under some threshold, it will be assumed that program no longer has a good guess of where the reference point is, so an "idling animation" will begin,
  an animation which plays when the wheel is Still or moving slowly that does not rely on the location of the reference point.

  Samuel G. Brown
  July 28 2017

*/

#ifndef Pattern_h
#define Pattern_h

#include "Arduino.h"
#include "Definitions.h"
#include "Speedometer.h"
#include "Bluetooth.h"
#include "Color.h"
#include <math.h>

// Each Pattern holds on to an array of Color_'s (abstract color objects which may be static or dynamic without input needed from the Pattern), and an "image" array that maps each LED on the wheel to a Color_
class Pattern
{
public:
  Pattern();                                             // Constructor
  Pattern(Color_ **colorsIn, unsigned char numColorsIn); // Constructor

  ~Pattern(); // Destructor

  virtual void anim() = 0; // Animation function

  // // Functions to manage the "palette" array of Color_**'s, or the Color_'s that this pattern may show
  // void setupPalette(unsigned char numColors);
  // void setupPalette(Color_ **colorsIn, unsigned char numColorsIn);
  // void setColor(Color_ *newColor, unsigned char colorNum); // Set a color in the specified location

  // unsigned char getNumColors(); // Get the number of palette currently assigned to this pattern

  // Functions to manage the "image", or the LED map which shows which LEDs are showing which Color_
  void setImage(unsigned char *image); // Set an image to the pattern
  // void setImageValInPos(unsigned char LEDNum); // Set an image value in a specific LED location
  // void setImage(uint32_t *image);      // Set an image to the pattern
  // void setImageColorInd(unsigned char colorInd); // Set the image as all references to a single color
  // void setImageNumSegs(unsigned char numSegs);   // Set some number of equally spaced dots on the image

  unsigned char getImageValInPos(unsigned char LEDNum); // Get the value of image at the specified location
  unsigned char getImageRawByte(unsigned char byteNum); // Get the raw byte value in image at the specified location
  const unsigned char *getImage();                      // Get a pointer to the image

  // Function for encoding information to Android
  virtual ImageMeta_BT getImageType(); // Get the type of image represented by this Pattern, and its related information (default is a constant with 0 rotational speed)

protected:
  Pattern_Handler *parent_handler; // A pointer to the parent pattern_handler, to ask for Color_
  // unsigned char numColors;                       // The number of palette is defaulted to 0 (Max of 255 Color_'s)

  // void preCalculateAllColor_();                                // Pre-calculate all colorObj's from each Color_ being used this loop
  // colorObj getPreCalculatedColorInPos(unsigned char colorNum); // Get a specific colorObj according to its position in palette (If using this function, MUST have run preCalculateAllColor_() earlier in the loop)

  //    void serialWriteAllColors(); // Write all palette to the output

private:
  //
  // //
  // // // TODO: Change the behavior of image!!!  Make each HALF byte (every 4 bits) encode the index of a color!  Save a ton of memory!!!
  // //
  //

  unsigned char image[NUM_BYTES_PER_IMAGE] = {0}; // An array of integers, each of which represents a "color index", or the index in the palette array that represents the color desired
  // TODO: Reconfigure so that Pattern_Handler is in control of the palette
  // void deleteColorArray(); // Delete the array palette and its contents

  // Color_** palette = NULL; // Array of Color_ objects
  // colorObj * preCalculatedColors = NULL; // Array (of size numColors) of colorObj's that represents the colorObj for each Color_ represented in image that is being used this loop (each Color_ is calculated only once, so computation time will be saved on dynamic palette).
};

// Abstract class to describe main animations, or animations that have a speed dependence
class Pattern_Main : public Pattern
{
public:
  Pattern_Main(Speedometer *speedometer); // Constructor
  boolean doesAllowIdle();                // Getter for allowIdle

  // virtual MAIN_ANIM getMainPatternType() = 0; // TODO: FILL IN; Alternatively, make a virtual function in Pattern that either a) returns the Image_Meta parameter (hardcoded 0 in Pattern, unless overwritten in some child class), or b) returns a full fleged ImageMetaParam_BT object (hard coded "default" in Pattern, then fill in for child functions)
protected:
  // Speedometer pointer (derived Pattern classes only get access to it during main animations)
  Speedometer *speedometer = NULL; // Speedometer (used to retrieve the pos/vel/acc of the wheel), DO NOT ATTEMPT TO DELETE

  boolean allowIdle = true; // Does this class allow an idle animation (true for most, but some main animations will need to override the idle animation and play all the time)
};

// Abstract class to describe idle animations, or animations that have no speed dependence
class Pattern_Idle : public Pattern
{
public:
  Pattern_Idle() : Pattern(){}; // Constructor

  // virtual IDLE_ANIM getMainPatternType() = 0; // TODO: FILL IN
};

// An abstract class to describe Patterns that use a moving image
class Moving_Image
{
public:
  Moving_Image();              // Constructor
  void anim();                 // The animation function
  virtual void animMain() = 0; // Each derived Pattern makes an animation that is run, similar to the anim() function of each other Pattern.  However, the currentLEDPos must be advanced each animation cycle, which will be done in the real Moving_Image::anim()

  void setImageBleed(unsigned char imageBleedIn); // Change imageBleed

  void setRotateSpeed(int rotateSpeedIn);                                  // Change rotateSpeed
  int getRotateSpeed();                                                    // Get the rotation speed
  float getLEDPos();                                                       // Get the current value of the protected currentLEDPos variable
  void addImagePosition(colorObj colorObjIn, unsigned char imagePosition); // (Bit of a weird one...) For use during the animMain() loop in derived classes.  Replaces adding a colorObj to the controller::sendPixel() function, instead sending the pixel to this object's colorMemory buffer for further processing (offsetting by currentLEDPos and "blurring")

protected:
  //    colorObj colorMemory[NUMLEDS]; // An array of colorObj's that keeps track of the history of movements

private:
  unsigned char imageBleed = 150;       // Amount of "bleed" from the moving image (0 <= bleedIn <= 255, higher values mean more after-image)
  int rotateSpeed = 10;                 // Speed of image rotation (in LEDs/second)
  float currentLEDPos = 0;              // Current image reference position around the wheel
  unsigned long lastLEDAdvanceTime = 0; // The time at which currentLEDPos was last updated

  //    void colorBlur(); // Calculate the blurring of the color memory
  void advanceLEDPos(); // Advance the current location of the image reference position around the wheel
  //    void sendColors(); // Send the palette in colorMemory to the LEDs
};

// Object that holds onto and manages each pattern currently instantiated (one main, and one idle)
class Pattern_Handler
{
public:
  Pattern_Handler(Speedometer *speedometer);                                               // Constructor
  Pattern_Handler(Speedometer *speedometer, Color_ **colorsIn, unsigned char numColorsIn); // Constructor
  ~Pattern_Handler();                                                                      // Destructor

  void mainLoop();                             // Main function that executes every loop
  void setMainPattern(MAIN_ANIM newAnimation); // Set the main pattern
  void setIdlePattern(IDLE_ANIM newAnimation); // Set the idle pattern

  Pattern_Main *mainPattern = NULL; // The main pattern
  Pattern_Idle *idlePattern = NULL; // The idle pattern

  // Functions to manage the "palette" array of Color_**'s, or the Color_'s that this pattern may show
  unsigned char numColors;      // The number of palette is defaulted to 0 (Max of 255 Color_'s)
  unsigned char getNumColors(); // Get the number of palette currently assigned to this pattern

  void setupPalette(unsigned char numColors);
  void setupPalette(Color_ **colorsIn, unsigned char numColorsIn);
  void setupPalette(Color_BT *colorMessagesIn, unsigned char numColorsIn);

  void setColor(Color_ *newColor, unsigned char colorNum); // Set a Color_ in the specified location
  Color_ *getColor(unsigned char colorNum);                // Get a pointer to the Color_ at the specified location

  void setBrightnessFactor(float newBrightnessFactor); // Set the brightnessFactor
  float getBrightnessFactor();                         // Get the brightness factor

  void preCalculateAllColor_();                                // Pre-calculate all colorObj's from each Color_ being used this loop
  colorObj getPreCalculatedColorInPos(unsigned char colorNum); // Get a specific colorObj according to its position in palette (If using this function, MUST have run preCalculateAllColor_() earlier in the loop)

private:
  // Speedometer pointer (kept to pass on to new Patterns)
  Speedometer *speedometer = NULL; // Speedometer (used to retrieve the pos/vel/acc of the wheel), DO NOT ATTEMPT TO DELETE

  // Color_ functions and information
  void deleteColorArray();              // Delete the array palette and its contents
  Color_ **palette = NULL;              // Array of Color_ objects
  colorObj *preCalculatedColors = NULL; // Array (of size numColors) of colorObj's that represents the colorObj for each Color_ represented in image that is being used this loop (each Color_ is calculated only once, so computation time will be saved on dynamic palette).
  float brightnessFactor = 1.0f;        // A scale factor to adjust the brightness each time the colors are precalculated (can be used to homogenously dim the LEDs)
};

// A main animation that features an image that appears to not rotate relative to the ground
class Still_Image_Main : public Pattern_Main
{
public:
  Still_Image_Main(Speedometer *speedometer, unsigned char *image); // Constructor
  Still_Image_Main(Speedometer *speedometer);                       // Constructor

  void anim(); // Main animation function
};

// A main animation that features an image that appears to rotate independently relative to the ground
class Moving_Image_Main : public Still_Image_Main, public Moving_Image
{
public:
  Moving_Image_Main(Speedometer *speedometer, unsigned char *image); // Constructor
  Moving_Image_Main(Speedometer *speedometer);                       // Constructor

  ImageMeta_BT getImageType();

  void animMain();                       // Main animation function
  void anim() { Moving_Image::anim(); }; // Redefine anim() to be the Moving_Image anim()
};

// An idle animation that features an image that does not rotate relative to the wheel
class Still_Image_Idle : public Pattern_Idle
{
public:
  Still_Image_Idle(unsigned char *image); // Constructor
  Still_Image_Idle();                     // Constructor
  void anim();                            // Idle animation function

private:
  // Blink class leftovers
  //    unsigned long timeAtToggle = 0;
  //    unsigned long timeOff = 200;
  //    unsigned long timeOn = 800;
  //
  //    //    unsigned char idleBrightness = 0;
  //    boolean idleToggleSig = false;
};

// An idle animation that features an image that rotates relative to the wheel
class Moving_Image_Idle : public Still_Image_Idle, public Moving_Image
{
public:
  Moving_Image_Idle(unsigned char *image); // Constructor
  Moving_Image_Idle();                     // Constructor

  ImageMeta_BT getImageType();

  void animMain();                       // Idle animation function
  void anim() { Moving_Image::anim(); }; // Redefine anim() to be the Moving_Image anim()
};

// A class in charge of communicating with the LED strip
class controller
{
public:
  // Public functions that are needed by other objects
  static void sendPixel(unsigned char r, unsigned char g, unsigned char b, unsigned char w); // Load a single pixel onto the LED strip (first LED in strip receives first color loaded)
  static void sendPixel(Color_ *colorIn);                                                    // Load a single pixel onto the LED string (same as above, using a Color_ object instead of unsigned char values)
  static void sendPixel(colorObj colorObjIn);                                                // Load a single pixel onto the LED string (same as above, using a colorObj object instead of unsigned char values or a Color_ object)
  static void show_LEDs();                                                                   // Display the pattern that was just loaded onto the NUMLEDS number of pixels using sendPixel()
  static void ledsetup();                                                                    // Set up the Arduino for low-level communication with the LED strip

private:
  // Low-level communication with the LED strip
  static void sendBit(bool bitVal);         // Send a bit to the LED string
  static void sendByte(unsigned char byte); // Send a byte to the LED string, represents brightness of a single-colored LED
};

// TODO: Add Spinner_Pattern (remember to include instance of getImageType(), so that info can be sent back to Android)
#endif
