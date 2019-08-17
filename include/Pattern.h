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
#include "Color.h"
#include <math.h>

#if !NO_BLUETOOTH
#include "Bluetooth.h"
#endif

// Each Pattern holds on to an array of Color_'s (abstract color objects which may be static or dynamic without input needed from the Pattern), and an "image" array that maps each LED on the wheel to a Color_
class Pattern
{
public:
  Pattern();                                             // Constructor
  Pattern(Color_ **colorsIn, unsigned char numColorsIn); // Constructor

  virtual ~Pattern() = default; // Destructor

  virtual void anim(int xTrueRounded) = 0; // Animation function
  void anim() {anim(0);}; // Define a parameter-less call to anim to be the same as claiming that the x position is always 0

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

// TODO: START HERE: Pattern redesign:
// 1. Patterns will not hold onto a Speedometer pointer, only Pattern_Handler will.  It will pass the "xTrueRounded" int to each Pattern through its anim() function (soon to be anim(int xTrueRounded))
// 2. Idle and Main patterns will be identical, due to 1. An anim() will still exist, which will simply call anim(0).  anim() will be used by Pattern_Handler to force a Pattern to be an "idle" Pattern (not dependant on wheel speed).
// 3. The Pattern_Handler will hold onto the allowIdle bool, updated each time the main pattern is reassigned (idle pattern will be deleted when main pattern does not support idle)
// 4. Derived classes have getLEDOffset(int xTrueRounded), which defines this image's behavior
// Need basic function sendLEDsWithOffset(int offset) that applies image to the LED strip, but offsets the image by some number which is dependant on the class.  Number will be offset = xTrueRounded + this_pattern_offset, where this_pattern_offset is defined by the class (Moving, Spinner, Still, wRel, gRel, ...)
// gRel will use xTrueRounded, wRel will not.  Moving and Spinner both have internal calculations to perform.
// Moving_Helper and Spinner_Helper (or any other similar class) will derive the Image_Helper class, which simply has a pure virtual function unsigned char getHelperOffset(int xTrueRounded).  Define anim(xTrueRounded) as: sendLEDsWithOffset(getLEDOffset(xTrueRounded)) , and give Pattern a new virtual function int getLEDOffset(int xTrueRounded).  Example implementations:
// wRel_Still: return 0; (getHelperOffset returns a 0;)
// gRel_Still: return xTrueRounded;
// wRel_Moving (derives Moving_Helper): return getHelperOffset(xTrueRounded);
// gRel_Moving (derives Moving_Helper): return xTrueRounded + getHelperOffset(xTrueRounded);
// Spinner (derives Spinner_Helper): return getHelperOffset(xTrueRounded);
// The getHelperOffset function will be called once per

// An abstract helper class that defines the single function that all helpers must return.  It allows the derived classes to change how the image moves around the wheel (either relative to the ground, or relative to the wheel)
class Image_Helper
{
  virtual unsigned char getHelperOffset(int xTrueRounded) = 0; // Returns the image offset defined by this image helper.  Called once per loop, it must perform all necessary calculations to updating and returning the offset value
};

// A helper class to describe Patterns whose images do not move over time
class Static_Helper: public Image_Helper
{
  unsigned char getHelperOffset(int xTrueRounded); // Always returns a 0, because the image should not move
};


// A helper class to describe Patterns that use a moving image
// TODO: Improve the documentation and naming of the variables in here; blurring may become viable if I use SRAM, and I'm never going to get it to work if I have no idea what any of this stuff does.
class Moving_Helper
{
public:
// Class specific functions
  Moving_Helper();
  Moving_Helper(int rotateSpeed);

  void setRotateSpeed(int rotateSpeedIn);                                  // Change rotateSpeed

// Image_Helper function
  unsigned char getHelperOffset(int xTrueRounded); // Return the current image position

// TODO: Functions below here need to be stripped for parts so that they fit into the functions above here.  They should probably also become private, if they need to be individual functions at all
  void anim(int xTrueRounded);                               // The internal animation function; calls anim_preImagePosUpdate(), which is defined by derived class
  virtual void anim_preImagePosUpdate(int xTrueRounded) = 0; // Each derived Pattern makes an animation that is run, similar to the anim() function of each other Pattern.  However, the imageMovementPos must be advanced each animation cycle, which will be done in the real Moving_Helper::anim()

  int getRotateSpeed();                                                    // Get the rotation speed
  float getImageMovementPos();                                             // Get the current value of the protected imageMovementPos variable
  void addImagePosition(colorObj colorObjIn, unsigned char imagePosition); // (Bit of a weird one...) For use during the anim_preImagePosUpdate() loop in derived classes.  Replaces adding a colorObj to the controller::sendPixel() function, instead sending the pixel to this object's colorMemory buffer for further processing (offsetting by imageMovementPos and "blurring")

#if SRAM_ATTACHED
  void setImageBleed(unsigned char imageBleedIn); // Change imageBleed
#endif

protected:
#if SRAM_ATTACHED
  colorObj colorMemory[NUMLEDS]; // An array of colorObj's that keeps track of the history of movements
#endif

private:
  int rotateSpeed = 10;                 // Speed of image rotation (in LEDs/second)
  float imageMovementPos = 0;           // Current image reference position around the wheel
  unsigned long lastLEDAdvanceTime = 0; // The time at which imageMovementPos was last updated

// TODO: Should probably incorporate this into getHelperOffset
  void advanceLEDPos(); // Advance the current location of the image reference position around the wheel

#if SRAM_ATTACHED
  unsigned char imageBleed = 150; // Amount of "bleed" from the moving image (0 <= bleedIn <= 255, higher values mean more after-image)

  void colorBlur();  // Calculate the blurring of the color memory
  void sendColors(); // Send the palette in colorMemory to the LEDs

#endif
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

  Pattern *mainPattern = NULL; // The main pattern
  Pattern *idlePattern = NULL; // The idle pattern

  // Functions to manage the "palette" array of Color_**'s, or the Color_'s that this pattern may show
  unsigned char numColors;      // The number of palette is defaulted to 0 (Max of 255 Color_'s)
  unsigned char getNumColors(); // Get the number of palette currently assigned to this pattern

  void setupPalette(unsigned char numColors);
  void setupPalette(Color_ **colorsIn, unsigned char numColorsIn);

#if USE_NANOPB
  void setupPalette(Color_BT *colorMessagesIn, unsigned char numColorsIn);
#endif

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

// A main animation that features an image that appears to rotates at a set speed relative to the ground
class Moving_Helper_Main : public Still_Image_Main, public Moving_Helper
{
public:
  Moving_Helper_Main(Speedometer *speedometer, unsigned char *image); // Constructor
  Moving_Helper_Main(Speedometer *speedometer);                       // Constructor

  ImageMeta_BT getImageType();

  void anim_preImagePosUpdate(int xTrueRounded); // Main animation function
  void anim() { Moving_Helper::anim(); };         // Redefine anim() to be the Moving_Helper anim()
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
class Moving_Helper_Idle : public Still_Image_Idle, public Moving_Helper
{
public:
  Moving_Helper_Idle(unsigned char *image); // Constructor
  Moving_Helper_Idle();                     // Constructor

  ImageMeta_BT getImageType();

  void anim_preImagePosUpdate(int xTrueRounded); // Idle animation function
  void anim() { Moving_Helper::anim(); };         // Redefine anim() to be the Moving_Helper anim()
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
