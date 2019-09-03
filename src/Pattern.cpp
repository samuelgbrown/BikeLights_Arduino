#include "Arduino.h"
#include "Speedometer.h"
#include "Definitions.h"
#include "Color.h"
#include "Pattern.h"

Pattern::Pattern() : Pattern(new Static_Helper(), true)
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Starting Pattern()..."));
  }

  // numColors = 0;
  //  setImageZeros();
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.print(numColors);
    //    Serial.println(F(" colors currently in pattern..."));
  }

  // Set the default number of colors to 1 (some value is needed, and this simplifies things)
  //  setupColors(1);
};

Pattern::Pattern(Image_Helper * image_helper, bool groundRel): groundRel(groundRel) {
  setImageHelper(image_helper);
}

Pattern::~Pattern() {
  // Delete the Image_Helper that is owned by this Pattern
  delete image_helper;
}

void Pattern::anim(int xTrueRounded) {
  sendLEDsWithOffset((groundRel ? xTrueRounded : 0) - image_helper->getHelperOffset(xTrueRounded)); // Send the colors to the LED strip with an offset.  This offset will be modulus'd as needed.  The offset is defined as the negative of the helper's offset (so that positive offsets rotate the image forward), plus [xTrueRounded if the image position is defined relative to the ground, or 0 if the image position is defined relative to the wheel]
}

void Pattern::sendLEDsWithOffset(int offset) {
  // Go through each LED, and send specified color to the LED strip
  for (unsigned char LEDNum = 0; LEDNum < NUMLEDS; LEDNum++) {
    // Find the image index to use by adding the offset to the LED number, and using modulus to stay within NUMLEDS
    unsigned char imagePos = (unsigned char)((LEDNum + offset) % (int)NUMLEDS);

    // Send the color at the specified location to the LED strip
    controller::sendPixel(parent_handler->getPreCalculatedColorInPos(getImageValInPos(imagePos)));
  }
}

// Pattern::Pattern(Color_** colorsIn, unsigned char numColorsIn) {
//   setupColors(colorsIn, numColorsIn);

//   if (DEBUGGING_PATTERN) {
//     // Serial.flush();
//     //    Serial.println(F("Setting up Pattern..."));
//   }
// };

void Pattern_Handler::setupPalette(unsigned char numColorsIn)
{
  //  if (DEBUGGING_PATTERN) {
  //    Serial.println(F("Setting up colors..."));
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    Serial.println();
  //  }

  // TODO: MEMORY
  Color_ **newColorArray = new Color_ *[numColorsIn]; // Deleted at the end of this function
  unsigned char brightness = 50;

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Created color array..."));
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    Serial.println();
  //  }

  // Determine how many of the old colors should be preserved
  unsigned char numOldColorsToPreserve = min(numColorsIn, numColors);

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.print(numOldColorsToPreserve);
  //    Serial.println(F(" old colors being preserved."));
  //    Serial.print(numColorsIn);
  //    Serial.println(F(" colors in."));
  //    Serial.print(numColors);
  //    Serial.println(F(" colors exist now."));
  //  }

  for (unsigned char i = 0; i < numOldColorsToPreserve; i++)
  {
    if (DEBUGGING_PATTERN)
    {
      // Serial.flush();
      Serial.print(F("Cloning color "));
      Serial.println(i);
      // delay(1000);
      Serial.print(F("R = "));
      Serial.print(palette[i]->getColor().r());
      Serial.print(F(", G = "));
      Serial.print(palette[i]->getColor().g());
      Serial.print(F(", B = "));
      Serial.print(palette[i]->getColor().b());
      Serial.print(F(", W = "));
      Serial.println(palette[i]->getColor().w());
      // delay(1000);
    }
    newColorArray[i] = palette[i]->clone();
  }

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Cloned old color array..."));
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    Serial.println();
  //    //    delay(500);
  //    //      Serial.println(numOldColorsToPreserve);
  //    //      Serial.println(numColorsIn);
  //  }

  // Fill in the rest of the array with static default colors
  for (unsigned char i = numOldColorsToPreserve; i < numColorsIn; i++)
  {
    //    if (DEBUGGING_PATTERN) {
    //    // Serial.flush();
    //    Serial.print(F("Adding color number "));
    //    Serial.println(i);
    //    delay(500);
    //    //      Serial.println(numOldColorsToPreserve);
    //    //      Serial.println(numColorsIn);
    //  }
    if (i == 0)
    {
      newColorArray[i] = new Color_Static();
    }
    else if ((i % 4) == 1)
    {
      newColorArray[i] = new Color_Static(0, 0, 0, brightness);
    }
    else if ((i % 4) == 2)
    {
      newColorArray[i] = new Color_Static(brightness, 0, 0, 0);
    }
    else if ((i % 4) == 3)
    {
      newColorArray[i] = new Color_Static(0, brightness, 0, 0);
    }
    else if ((i % 4) == 0)
    {
      newColorArray[i] = new Color_Static(0, 0, brightness, 0);
    }
  }

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Initialized Color array..."));
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    Serial.println();
  //  }

  setupPalette(newColorArray, numColorsIn);

  // Delete newColorArray
  //  for (int i = 0;i<numColorsIn;i++) { // Do not delete the objects that these points point to, because they are in "colors"
  //    delete newColorArray[i];
  //  }

  // delete[] newColorArray;

  //  if (DEBUGGING_PATTERN) {
  //    Serial.println(F("Deleted temporary color array"));
  //    //    delay(500);
  //  }
};

void Pattern_Handler::setupPalette(Color_ **newColorArray, unsigned char numColorsIn)
{
  deleteColorArray(); // Always delete the old arrays when new ones are being created
  // TODO: Change this function so that it takes ownership of the incoming array, so that it does not need to be deleted

  //  if (DEBUGGING_PATTERN) {
  //    Serial.println(F("Checking input color array for first empty..."));
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    delay(500);
  //  }

  // if (!newColorArray[0]->isThisEmpty())
  // {
  //   // If the first color is not empty, then add a new blank color in the beginning
  //   numColorsIn++;                                        // Increase the number of colors
  //   Color_ **newerColorArray = new Color_ *[numColorsIn]; // Deleted at the end of this function
  //   newerColorArray[0] = new Color_Static();              // Create a new blank object
  //   for (unsigned char i = 1; i < numColorsIn; i--)
  //   {
  //     newerColorArray[i] = newColorArray[i - 1];
  //   }

  //   // Oh god please let this actually be real code
  //   delete[] newColorArray;
  //   newColorArray = newerColorArray;
  //   delete[] newerColorArray;
  // }

  //  if (DEBUGGING_PATTERN) {
  //    Serial.println(F("Copying input color array..."));
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    delay(500);
  //  }

  preCalculatedColors = new colorObj[numColorsIn]; // Allocate space for the preCalculatedColors array

  // palette = new Color_ *[numColorsIn];             // Allocate space for the color array
  // copyArray<Color_ *>(newColorArray, palette, numColorsIn); // Copy over the pointers to the incoming Color_* array
  palette = newColorArray; // TODO: Should we use this method, of just transfering the array?  Go through all code and see if this would be ok.
  numColors = numColorsIn;

  //  if (DEBUGGING_PATTERN) {
  //    Serial.println(F("Copied array..."));
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    for (unsigned char i = 0; i < numColorsIn; i++) {
  //      // Serial.flush();
  //      Serial.print(F("Color "));
  //      Serial.print(i);
  //      Serial.print(F(": R = "));
  //      Serial.print(colors[i]->getColor().r());
  //      Serial.print(F(", G = "));
  //      Serial.print(colors[i]->getColor().g());
  //      Serial.print(F(", B = "));
  //      Serial.print(colors[i]->getColor().b());
  //      Serial.print(F(", W = "));
  //      Serial.println(colors[i]->getColor().w());
  //      //      delay(500);
  //    }
  //  }
};

#if USE_NANOPB
void Pattern_Handler::setupPalette(Color_BT *colorMessagesIn, unsigned char numColorsIn)
{
  deleteColorArray(); // Always delete the old array when a new one is being created

  palette = new Color_ *[numColorsIn];             // Allocate space for the color array
  preCalculatedColors = new colorObj[numColorsIn]; // Allocate space for the preCalculatedColors array

  for (unsigned char i = 0; i < numColorsIn; i++)
  {
#if !NO_BLUETOOTH
    palette[i] = Bluetooth::Color_FromPB(colorMessagesIn[i], speedometer);
#endif
  }
  //  *colors = *newColorArray;
  numColors = numColorsIn;
}
#endif

void Pattern_Handler::deleteColorArray()
{
  for (unsigned char i = 0; i < numColors; i++)
  {
    delete palette[i]; // Delete each individual color object
  }
  delete[] palette;             // Delete the array of points that colors points to
  delete[] preCalculatedColors; // Delete the array of colorObj's
  numColors = 0;
};

unsigned char Pattern_Handler::getNumColors()
{
  return numColors;
};

colorObj Pattern_Handler::getPreCalculatedColorInPos(unsigned char colorNum)
{
  if (colorNum < numColors)
  {
    return preCalculatedColors[colorNum];
  }
  else
  {
    return preCalculatedColors[numColors];
  }
};

//Color_* Pattern::getPreCalculatedColorInPos(unsigned char colorNum) {
//  if (colorNum < numColors) {
//    return colors[colorNum];
//  } else {
//    return colors[numColors];
//  }
//};

unsigned char Pattern::getImageValInPos(unsigned char LEDNum)
{
  if (0 <= LEDNum && LEDNum < NUMLEDS)
  {
    unsigned char imageInd = LEDNum >> 1; // Right bit-shift the incoming LED number to get the integer division by 2 of LEDNum (very fast, because I'm very cool)

    switch (1 & LEDNum) // Figure out if LEDNum is even or odd, using exceedingly clever bit-wise arithmetic
    {
    case 0:
      // The number is even, we'll want the first nibble (remember, image[] starts indexing at 0)
      return valFromFirstNibble(image[imageInd]); // Get the value from the first nibble of the LEDNum/2 index of the image
      break;
    case 1:
      // The number is odd, we'll want the second nibble
      return valFromSecondNibble(image[imageInd]); // Get the value from the first nibble of the LEDNum/2 index of the image
      break;
    }
    // return image[LEDNum]; // Old method (where size(image) == NUMLEDS)
  }
  else
  {
    return valFromFirstNibble(image[0]);
  }
};

unsigned char Pattern::getImageRawByte(unsigned char byteNum)
{
  if (0 <= byteNum && byteNum < NUM_BYTES_PER_IMAGE)
  {
    return image[byteNum]; // Return the raw byte from image (raw because, if we wanted an actual meaningful number, we would have to split the byte in image into nibbles)
  }
  else
  {
    return image[0];
  }

  return 0;
}

const unsigned char *Pattern::getImage() const
{
  return image;
}

unsigned char Pattern::getImageType() const {
  // Annoyingly, the Android and Arduino systems think that different parts of the Image type are important, so the information sent to Android is a wierd amalgamation of the reference point information and the helper type
  
  // Find out which type of image is being used
  if (image_helper->getType() == SPINNER) {
    // If this is a spinner, return that information
    return 2U;
  } else {
    // Otherwise, return the reference point information
    if (groundRel) {
      return 1U;
    } else {
      return 0U;
    }
  }
}

bool Pattern::supportIdle() const {
  return image_helper->isIdleAllowed(); // Whether or not this Pattern supports an idle animation is dependant entirely on the type of helper function
}

signed char Pattern::getHelperParameter() const {
  return image_helper->getHelperParameter();
}

const Pattern * Pattern_Handler::getMainPattern() {
  return mainPattern;
}

const Pattern * Pattern_Handler::getIdlePattern() {
  return idlePattern;
}

void Pattern_Handler::setColor(Color_ *newColor, unsigned char colorNum)
{
  if (colorNum < numColors && colorNum != 0)
  {
    if (DEBUGGING_PATTERN)
    {
      // Serial.flush();
      Serial.print(F("newColor is of type "));
      Serial.println(newColor->getType());
    }
    // If colorNum is within the current size of colors (by being less than numColors), and not equal to 0 (which is reserved for a "blank" Static_Color)
    delete palette[colorNum];                   // Delete the current color object in the colors position
    Color_ *permenantColor = newColor->clone(); // Create a permenant copy of newColor (newColor will disappear after this function is completed)
    palette[colorNum] = permenantColor;         // Store the pointer to this color

    //    colors[colorNum] = newColor->clone(); // Store the pointer of a clone of the new color to the colors array

    //    if (DEBUGGING_PATTERN) {
    //      // Serial.flush();
    //      Serial.print(F("Setting a new color in slot "));
    //      Serial.println(colorNum);
    //      delay(500);
    //      Serial.print(F("R = "));
    //      Serial.print(colors[colorNum]->getColor().r());
    //      Serial.print(F(", G = "));
    //      Serial.print(colors[colorNum]->getColor().g());
    //      Serial.print(F(", B = "));
    //      Serial.print(colors[colorNum]->getColor().b());
    //      Serial.print(F(", W = "));
    //      Serial.println(colors[colorNum]->getColor().w());
    //      delay(500);
    //    }
  }

  // If colorNum refers to a location outside of the size of colors, then do nothing.
};

const Color_ *Pattern_Handler::getColor(unsigned char colorNum)
{
  // Ensure that the Color_ index to be returned is within bounds
  if (colorNum < 0)
  {
    colorNum = 0;
  }
  else if (colorNum >= numColors)
  {
    colorNum = numColors;
  }

  // Return the specified Color_
  return palette[colorNum];
}

//void Pattern::serialWriteAllColors() {
//  if (DEBUGGING_ANY) {
//    // Serial.flush();
//    Serial.print(F("Displaying all "));
//    Serial.print(numColors);
//    Serial.println(F(" colors"));
//    for (int i = 0; i < numColors; i++) {
//      Serial.print(F("Color "));
//      Serial.print(i);
//      Serial.print(F(": R = "));
//      Serial.print(colors[i]->getColor().r());
//      Serial.print(F(", G = "));
//      Serial.print(colors[i]->getColor().g());
//      Serial.print(F(", B = "));
//      Serial.print(colors[i]->getColor().b());
//      Serial.print(F(", W = "));
//      Serial.println(colors[i]->getColor().w());
//      //      delay(100);
//    }
//  }
//}

void Pattern_Handler::setBrightnessFactor(float newBrightnessFactor)
{
  if (newBrightnessFactor >= 0.0f && newBrightnessFactor <= 1.0f)
  {
    // If the new brightness factor is between 0 and 1 (inclusive), then save it to use during the display phase
    brightnessFactor = newBrightnessFactor;
  }
}

float Pattern_Handler::getBrightnessFactor()
{
  return brightnessFactor;
}

// void Pattern::setImageValInPos(unsigned char LEDNum) {
//   // Set a value in the image array
// }

void Pattern::setImage(unsigned char *imageIn)
{
  // Set a new image array, and ensure that enough colors to use it
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Setting image..."));
    //    Serial.print(F("Current memory: "));
    //    Serial.println(freeRam());
    //    Serial.println();
    //    Serial.println(F("Initial image:"));
    //    for (int i = 0; i < NUMLEDS; i++) {
    //      Serial.println(image[i]);
    //    }
    //    delay(50);
    //    // Serial.flush();
    //    Serial.println();
    //    Serial.println(F("Initial imageIn:"));
    //    for (int i = 0; i < NUMLEDS; i++) {
    //      Serial.println(imageIn[i]);
    //    }
    //    delay(50);
  }

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Copying image..."));
  //    // Serial.flush();
  //    Serial.print(F("Current memory: "));
  //    Serial.println(freeRam());
  //    Serial.print(F("NUMLEDS: "));
  //    Serial.println(NUMLEDS);
  //    Serial.print(F("NUMLEDS: "));
  //    Serial.println(NUMLEDS);
  //    Serial.println();
  //    // Serial.flush();
  //    Serial.println(F("Old Image:"));
  //    for (unsigned char i = 0; i < NUMLEDS; i++) {
  //      Serial.print(F(" "));
  //      Serial.print(image[i]);
  //    }
  //    delay(100);
  //    Serial.println();
  //    Serial.println(F("Image In:"));
  //    for (unsigned char i = 0; i < NUMLEDS; i++) {
  //      Serial.print(F(" "));
  //      Serial.print(imageIn[i]);
  //    }
  //    Serial.println();
  //    delay(100);
  //  }
  // Set the image to the new image
  for (unsigned char i = 0; i < NUM_BYTES_PER_IMAGE; i++)
  {
    image[i] = imageIn[i];
  }
  //  copyArray<unsigned int>(imageIn, image, NUMLEDS);

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Copied image..."));
  //    Serial.println();
  //    // Serial.flush();
  //    Serial.println(F("New Image:"));
  //    for (unsigned char i = 0; i < NUMLEDS; i++) {
  //      Serial.print(F(" "));
  //      Serial.print(image[i]);
  //    }
  //    Serial.println();
  //    delay(100);
  //  }

  // Create an array of
  unsigned char numColorsInArray = maxInArrayPerNibble<unsigned char>(image, NUM_BYTES_PER_IMAGE) + 1; // The number of colors is the maximum value plus one (i.e. if the maximum value is 2, then integers 0, 1, and 2 all correspond to different numbers, meaning there are 3 colors)

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.print(F("Making new colors array of size..."));
    //    Serial.println(numColorsInArray);
    //    Serial.println();
  }
  parent_handler->setupPalette(numColorsInArray);
  // setupColors(numColorsInArray);
};

void Pattern::setImageFromBluetooth(btSerialWrapper btSer) {
  // Read data in directly from the Bluetooth device, and set it as the image.
  // NOTE: This function assumes that the Bluetooth Serial line is queued up such that the next NUM_BYTES_PER_IMAGE bytes are meant to define an image for a Pattern.  No error-checking can or will be performed.
  btSer.readNextMessageBytes(image, NUM_BYTES_PER_IMAGE);
}

void Pattern::setImageHelper(Image_Helper * image_helper_in) {
  delete image_helper; // Delete the old image helper, if it exists
  image_helper = image_helper_in;
}

// void Pattern::setImageZeros()
// {
//   // Using old image format (one char per LED, instead of one nible per LED)
//   if (DEBUGGING_PATTERN)
//   {
//     Serial.flush();
//     Serial.println(F("Setting zero image..."));
//   }

// unsigned char newImage[NUMLEDS];

//   if (DEBUGGING_PATTERN)
//   {
//     Serial.println(F("Starting loop..."));
//   }
//   for (unsigned char i = 0; i < NUMLEDS; i++)
//   {
//     //    for (int j = 0; j < nLightsPerLED; j++) {
//     newImage[i] = 0; // Zero out the entire array
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     Serial.println(F("New image is: "));
//     for (unsigned char i = 0; i < NUMLEDS; i++)
//     {
//       Serial.print(newImage[i]);
//       Serial.print(F(" "));
//     }
//     Serial.println();
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.println(F("Finished loop..."));
//   }

//   setImage(newImage);
// };

// void Pattern::setImageColorInd(unsigned char colorInd)
// {
//   if (DEBUGGING_PATTERN)
//   {
//     // Serial.flush();
//     //    Serial.println(F("Setting zero image..."));
//   }

//   colorInd = min(colorInd, parent_handler->getNumColors() - 1);

//   unsigned char newImage[NUMLEDS];

//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.println(F("Starting loop..."));
//   }
//   for (unsigned char i = 0; i < NUMLEDS; i++)
//   {
//     //    for (int j = 0; j < nLightsPerLED; j++) {
//     newImage[i] = colorInd; // Zero out the entire array
//     //    if (DEBUGGING_PATTERN) {
//     //      Serial.println(newImage[i]);
//     //    }
//     //    }
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.println(F("Finished loop..."));
//   }

//   setImage(newImage);
// };

// void Pattern::setImageNumSegs(unsigned char numSegs)
// {
//   // Using old image format (one char per LED, instead of one nible per LED)
//   //  if (DEBUGGING_PATTERN) {
//   //    // Serial.flush();
//   //    Serial.print(F("Making image with "));
//   //    Serial.print(numSegs);
//   //    Serial.println(F(" segments..."));
//   //    //    delay(500);
//   //  }
//   unsigned char newImage[NUMLEDS];
//   unsigned char distPerSeg = (unsigned char)roundf((float)NUMLEDS / (float)numSegs); // Get a *rounded* distance per segment to display (MAY NOT BE MATHEMATICALLY CORRECT, MAY NEED TO CALCULATE INSIDE OF LOOP)

//   // Create the image
//   for (unsigned char i = 0; i < NUMLEDS; i++)
//   {
//     newImage[i] = 0; // Zero out the entire array
//   }

//   for (unsigned char i = 0; i < NUMLEDS; i += distPerSeg)
//   {
//     newImage[i] = 1; // Place a 1 every distPerSed number of LEDs
//   }

//   for (unsigned char i = 0; i < nLEDs; i += distPerSeg)
//   {
//     newImage[i] = 1; // Place a 1 every distPerSed number of LEDs
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     // Serial.flush();
//     //    Serial.println(F("Setting segmented image..."));
//     //    delay(500);
//   }
//   setImage(newImage);
// };

void Pattern_Handler::preCalculateAllColor_()
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Pre-calculating all colorObj's..."));
  }
  // For each Color_, get its current colorObj
  for (unsigned char colorInd = 0; colorInd < numColors; colorInd++)
  {
    //    if (DEBUGGING_PATTERN) {
    //      // Serial.flush();
    //      Serial.print(F("Getting color "));
    //      Serial.print(colorInd + 1);
    //      Serial.print(F(" of "));
    //      Serial.print(numColors);
    //      Serial.println(F(":"));
    //    }

    // Precalculate the color, and apply the blanket brightness multiplier
    preCalculatedColors[colorInd] = palette[colorInd]->getColor();
    preCalculatedColors[colorInd].multiplyBrightness(brightnessFactor);
    //    if (DEBUGGING_PATTERN) {
    //      delay(500);
    //    }
  }
}

//void Pattern::setImageNumSegsR(int numSegs, unsigned char brightness) {
//  int distPerSeg = (int)roundf((float)numSegs / (float)NUMLEDS); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < NUMLEDS; i += distPerSeg) {
//    image[0][i] = brightness; //  out the entire array
//  }
//};
//
//void Pattern::setImageNumSegsG(int numSegs, unsigned char brightness) {
//  int distPerSeg = (int)roundf((float)numSegs / (float)NUMLEDS); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < NUMLEDS; i += distPerSeg) {
//    image[1][i] = brightness; //  out the entire array
//  }
//};
//
//void Pattern::setImageNumSegsB(int numSegs, unsigned char brightness) {
//  int distPerSeg = (int)roundf((float)numSegs / (float)NUMLEDS); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < NUMLEDS; i += distPerSeg) {
//    image[2][i] = brightness; //  out the entire array
//  }
//};
//
//void Pattern::setImageNumSegsW(int numSegs, unsigned char brightness) {
//  int distPerSeg = (int)roundf((float)NUMLEDS / (float)numSegs); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < NUMLEDS; i += distPerSeg) {
//    image[3][i] = brightness; //  out the entire array
//  }
//};

// Define the Image_Helper class and its derived forms
Image_Helper::Image_Helper(bool idleAllowed): idleAllowed(idleAllowed) {};

bool Image_Helper::isIdleAllowed() {
  return idleAllowed;
}

int Static_Helper::getHelperOffset(int xTrueRounded) {
  return 0; // Always return 0, so that the image does not move
}

signed char Static_Helper::getHelperParameter() {
  // Always return 0
  return 0;
}

Moving_Helper::Moving_Helper(int rotateSpeed): rotateSpeed(rotateSpeed) {}

void Moving_Helper::setRotateSpeed(int rotateSpeedIn)
{
  rotateSpeed = rotateSpeedIn;
};

int Moving_Helper::getRotateSpeed()
{
  return rotateSpeed;
}

int Moving_Helper::getHelperOffset(int xTrueRounded) {
  advanceLEDPos(); // Advance the LED position, based on the current time
  return (unsigned char) round(imageMovementPos); // Return the newly calculated image offset, converted from float to unsigned char.  TODO: Check that this maps the float value properly
}

void Moving_Helper::advanceLEDPos()
{
  unsigned long thisLEDAdvanceTime = micros();
  unsigned long dt = thisLEDAdvanceTime - lastLEDAdvanceTime; // How much time as passed since the LED position was last updated?

  imageMovementPos = fmodf(imageMovementPos + ((float)dt / 1000000.0) * (float)rotateSpeed, NUMLEDS); // Update imageMovementPos, and keep it between 0<=imageMovementPos<NUMLEDS
  lastLEDAdvanceTime = thisLEDAdvanceTime;                                                            // Update lastLEDAdvanceTime
};

signed char Moving_Helper::getHelperParameter() {
  return rotateSpeed;
}

#if SRAM_ATTACHED
Moving_Helper(int rotateSpeed, imageBleed): rotateSpeed(rotateSpeed), imageBleed(imageBleed) {};

void Moving_Helper::setImageBleed(unsigned char imageBleedIn)
{
  if (0 <= imageBleedIn && imageBleedIn < 255)
  {
    imageBleed = imageBleedIn;
  }
};

void Moving_Helper::colorBlur() {
 Method for doing color blur that requires a large amount of memory (NUMLIGHTSPERLED * NUMLEDS bytes) per pattern, in the form of the colorMemory array.
 May be useful if using external RAM.
 unsigned long dt = micros() - lastLEDAdvanceTime; // How much time as passed since the LED position was last updated?
 float tune = 700; // Used to remap the imageBleed variable to make it easier to choose bleed times
 float bleedFac = (float)dt / 1000000.0; // Convert milliseconds to seconds
 if (DEBUGGING_MOVINGIMAGE) {
   Serial.println(dt);
   Serial.println(bleedFac, 10);
 }
 for (unsigned char colorInd = 0; colorInd < NUMLEDS; colorInd++) {
   if (DEBUGGING_MOVINGIMAGE && colorInd == 0) {
     Serial.println(1.0 - tune * bleedFac * (1.0 - pow((float)imageBleed / 255.0, .2)), 10);
   }
   //    colorMemory[colorInd].multiplyBrightness((bleedFac * (float)imageBleed) / 255.0);
   colorMemory[colorInd].multiplyBrightness(1.0 - tune * bleedFac * (1.0 - pow((float)imageBleed / 255.0, .2)));
 }
};

void Moving_Helper::sendColors() {
// Send colorMemory object (used with memory-expensive blur method)
 for (unsigned char i = 0; i < NUMLEDS; i++) {
   controller::sendPixel(colorMemory[i]); // Send the color in colorMemory
 }
}

void Moving_Helper::addImagePosition(colorObj colorObjIn, unsigned char imagePosition)
{
  unsigned char thisInd = (unsigned char)fmod(round((float)imagePosition + imageMovementPos), NUMLEDS); // Add currentLED pos to imagePosition, then wrap it within NUMLEDS to reference colorMemory (This may be slow?)
  if (DEBUGGING_MOVINGIMAGE)
  {
    Serial.println(thisInd);
  }
  colorMemory[thisInd] = colorObjIn;
};
#endif

// For the Spinner_Helper class, note that it does NOT allow an idle, so construct the Image_Helper accordingly
Spinner_Helper::Spinner_Helper(): Image_Helper(false) {};
Spinner_Helper::Spinner_Helper(signed char intertia): Image_Helper(false), inertia(inertia) {}

void Spinner_Helper::setInertia(signed char inertiaIn) {
  inertia = inertiaIn;
}

unsigned char Spinner_Helper::getInertia() {
  return inertia;
}

int Spinner_Helper::getHelperOffset(int xTrueRounded) {
  advanceLEDPos(); // Advance the LED position, based on the current time
  return (unsigned char) round(imageMovementPos); // Return the newly calculated image offset, converted from float to unsigned char.  TODO: Check that this maps the float value properly
}

void Spinner_Helper::advanceLEDPos() {
  // TODO: Write the Spinner image type!!!  
  // Calculate the next position based on the physical model
  imageMovementPos++;
};

signed char Spinner_Helper::getHelperParameter() {
  return inertia;
}

// Still_Image_Main::Still_Image_Main(Speedometer *speedometer) : Pattern_Main(speedometer)
// {
//   if (DEBUGGING_PATTERN)
//   {
//     // Serial.flush();
//     Serial.println(F("Making still main pattern..."));
//     //    delay(500);
//   }
//   //  setImageZeros();
//   // setImageNumSegs(10);
//   //  Serial.println(F("Pattern set."));
// }

// Still_Image_Main::Still_Image_Main(Speedometer *speedometer, unsigned char *imageIn) : Pattern_Main(speedometer)
// {
//   setImage(imageIn);
// };

// void Still_Image_Main::anim()
// {
//   if (DEBUGGING_PATTERN)
//   {
//     // Serial.flush();
//     Serial.println(F("Main Start..."));
//     //    delay(500);
//   }

//   // Get the wheel reference location
//   int xTrueRounded = (int)roundf(speedometer->getPos());
//   //  Serial.print(F("xTrueRounded is: "));
//   //  Serial.println(xTrueRounded);
//   if (DEBUGGING_PATTERN)
//   {
//     // Serial.flush();
//     Serial.println(F("Got position..."));
//     //    delay(500);
//   }

//   // Pre-calculate all Color_'s
//   parent_handler->preCalculateAllColor_();
//   // preCalculateAllColor_();

//   // Load the image onto the wheel, as is
//   for (unsigned char LEDNum = 0; LEDNum < NUMLEDS; LEDNum++)
//   {
//     //    if (DEBUGGING_PATTERN) {
//     //      // Serial.flush();
//     //      Serial.print(F("Loop "));
//     //      Serial.print(LEDNum);
//     //      Serial.print(F(", xTrueRounded is "));
//     //      Serial.print(xTrueRounded);
//     //      Serial.print(F(", NUMLEDS is "));
//     //      Serial.println(NUMLEDS);
//     ////      delay(500);
//     //    }
//     unsigned char imagePos = (unsigned char)((LEDNum + xTrueRounded) % (int)NUMLEDS); // Adjust the position in the image
//     //    if (DEBUGGING_PATTERN) {
//     //      Serial.print(F("imagePos is "));
//     //      Serial.println(imagePos);
//     //      Serial.print(F("image[imagePos] is "));
//     //      Serial.println(image[imagePos]);
//     //      Serial.print(F("Total number of colors is "));
//     //      Serial.println(getNumColors());
//     //      delay(500);
//     //      Serial.print(F("R = "));
//     //      Serial.print(colors[image[imagePos]]->getColor().r());
//     //      Serial.print(F(", G = "));
//     //      Serial.print(colors[image[imagePos]]->getColor().g());
//     //      Serial.print(F(", B = "));
//     //      Serial.print(colors[image[imagePos]]->getColor().b());
//     //      Serial.print(F(", W = "));
//     //      Serial.println(colors[image[imagePos]]->getColor().w());
//     //      Serial.println(F("Sending pixel..."));
//     //      delay(500);
//     //    }
//     //    controller::sendPixel(image[0][imagePos], image[1][imagePos], image[2][imagePos], image[3][imagePos]);
//     controller::sendPixel(parent_handler->getPreCalculatedColorInPos(getImageValInPos(imagePos)));
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     // Serial.flush();
//     Serial.println(F("Finished Main..."));
//     delay(500);
//   }
// };

// Still_Image_Idle::Still_Image_Idle() : Pattern_Idle()
// {
//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.println(F("Making Still_Image_Idle"));
//     //    // Serial.flush();
//     //    Serial.print(getNumColors());
//     //    Serial.println(F(" colors currently in still idle pattern..."));
//   }

//   // Set up black and red idle animation
//   //  setImageZeros();
//   // setImageNumSegs(10);

//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.println(F("Done with Still_Image_Idle..."));
//     //    delay(200);
//   }

//   // Blink class leftovers
//   //  setupColors(2); // Need two colors for this animation
//   //  setColor(Color_Static(0, 0, 0, 0).getColor_(), 0);
//   //  setColor(Color_Static(150, 0, 0, 0).getColor_(), 1);
// };

// Still_Image_Idle::Still_Image_Idle(unsigned char *imageIn) : Pattern_Idle()
// {
//   setImage(imageIn);
// };

// void Still_Image_Idle::anim()
// {
//   if (DEBUGGING_PATTERN)
//   {
//     // Serial.flush();
//     //    Serial.println(F("Idle animation starting..."));
//   }
//   //  unsigned long curTime = millis();
//   //  Serial.print(F("Idle: "));
//   //  Serial.println((curTime - timeAtToggle));
//   //  if (!isIdle) {
//   //    // Idling just started
//   //    //    Serial.println(F("Start"));
//   //    isIdle = true;
//   //    timeAtToggle = curTime;
//   //    idleToggleSig = true;
//   //  }

//   // Blink class leftovers
//   //  if (idleToggleSig) {
//   //    if ((curTime - timeAtToggle) > timeOn) {
//   //      idleToggleSig = false;
//   //      //      idleBrightness = 0;
//   //      timeAtToggle = curTime;
//   //      //      Serial.println(F("Off"));
//   //    }
//   //  } else {
//   //    if ((curTime - timeAtToggle) > timeOff) {
//   //      idleToggleSig = true;
//   //      //      idleBrightness = 150;
//   //      timeAtToggle = curTime;
//   //      //      Serial.println(F("On"));
//   //    }
//   //  }
//   //  if (DEBUGGING_PATTERN) {
//   // Serial.flush();
//   //    Serial.println(F("Setting color..."));
//   //    delay(500);
//   //  }

//   //  Color_ *thisC;
//   //  if (idleToggleSig) {
//   //    thisC = colors[0];

//   //    if (DEBUGGING_PATTERN) {
//   //      // Serial.flush();
//   //      Serial.print(F("Using first color of "));
//   //      Serial.print(getNumColors());
//   //      //      delay(500);
//   //      Serial.print(F(" total colors..."));
//   //      Serial.print(F("R = "));
//   //      Serial.print(colors[0]->getColor().r());
//   //      Serial.print(F(", G = "));
//   //      Serial.print(colors[0]->getColor().g());
//   //      Serial.print(F(", B = "));
//   //      Serial.print(colors[0]->getColor().b());
//   //      Serial.print(F(", W = "));
//   //      Serial.println(colors[0]->getColor().w());
//   //      //      delay(500);
//   //    }
//   //  } else {
//   //    if (DEBUGGING_PATTERN) {
//   // Serial.flush();
//   //      Serial.println(F("Using second color..."));
//   // delay(1000);
//   //    }
//   //    thisC = colors[1];
//   //  }

//   // Pre-calculate all Color_'s
//   parent_handler->preCalculateAllColor_();
//   // preCalculateAllColor_();

//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.flush();
//     //    Serial.println(F("Sending all LEDs..."));
//     //    delay(1000);
//   }
//   // Load the image onto the wheel, as is
//   for (unsigned char LEDNum = 0; LEDNum < NUMLEDS; LEDNum++)
//   {
//     controller::sendPixel(parent_handler->getPreCalculatedColorInPos(getImageValInPos(LEDNum)));
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.flush();
//     //    Serial.println(F("Sent all LEDs..."));
//     //    delay(1000);
//   }

//   //  Serial.println();
// };

// // Moving Image Main functions
// Moving_Helper_Main::Moving_Helper_Main(Speedometer *speedometer, unsigned char *image) : Still_Image_Main(speedometer, image){};
// Moving_Helper_Main::Moving_Helper_Main(Speedometer *speedometer) : Still_Image_Main(speedometer){};

// void Moving_Helper_Main::anim_preImagePosUpdate(int xTrueRounded)
// {
//   if (DEBUGGING_PATTERN)
//   {
//     Serial.flush();
//     Serial.println(F("Main animation starting..."));
//   }

//   // int xTrueRounded = (int)roundf(speedometer->getPos());

//   // Pre-calculate all Color_'s
//   parent_handler->preCalculateAllColor_();
//   // preCalculateAllColor_();

//   // Load the image into color memory, as is
//   for (unsigned char thisWheelPos = 0; thisWheelPos < NUMLEDS; thisWheelPos++)
//   {
// // Add a new color to the LED strip
// #if SRAM_ATTACHED
//     // Add the pixel color specified to RAM, to be sent after processing
//     addImagePosition(parent_handler->getPreCalculatedColorInPos(getImageValInPos(((thisWheelPos + xTrueRounded) % (int)NUMLEDS))), imagePos);
// #else
//     // Send the pixels directly to the LED strip
//     unsigned char thisInd = (unsigned char)fmod(round(((thisWheelPos + xTrueRounded) % (int)NUMLEDS) - getImageMovementPos()), NUMLEDS); // Add currentLED pos to imagePosition, then wrap it within NUMLEDS to reference colorMemory (This may be slow?)
//     controller::sendPixel(parent_handler->getPreCalculatedColorInPos(getImageValInPos(thisInd)));
// #endif
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     Serial.flush();
//     Serial.println(F("Wrote all LEDs to colorMemory..."));
//     //    delay(1000);
//   }

//   //  Serial.println();
// };

// // Moving Image Idle functions
// Moving_Helper_Idle::Moving_Helper_Idle(unsigned char *image) : Still_Image_Idle(image){};
// Moving_Helper_Idle::Moving_Helper_Idle() : Still_Image_Idle()
// {
//   if (DEBUGGING_PATTERN)
//   {
//     //    Serial.flush();
//     //    Serial.println(F("Finished making Moving_Helper_Idle"));
//     //    delay(500);
//   }
// };

// void Moving_Helper_Idle::anim_preImagePosUpdate()
// {
//   if (DEBUGGING_PATTERN)
//   {
//     Serial.flush();
//     Serial.println(F("Idle animation starting..."));
//   }

//   // Pre-calculate all Color_'s
//   parent_handler->preCalculateAllColor_();
//   // preCalculateAllColor_();

//   // Load the image onto the wheel, as is
//   for (unsigned char thisWheelPos = 0; thisWheelPos < NUMLEDS; thisWheelPos++)
//   {
// #if SRAM_ATTACHED
//     // Add the pixel color specified to RAM, to be sent after processing
//     addImagePosition(parent_handler->getPreCalculatedColorInPos(getImageValInPos(thisWheelPos)), thisWheelPos); // Use overriden copy assignment operator, which copies the color data to the colorMemory array
// #else
//     // Add a new color to the LED strip
//     unsigned char thisInd = (unsigned char)fmod(round(((thisWheelPos) % (int)NUMLEDS) - getImageMovementPos()), NUMLEDS); // Add currentLED pos to imagePosition, then wrap it within NUMLEDS to reference colorMemory (This may be slow?)
//     controller::sendPixel(parent_handler->getPreCalculatedColorInPos(getImageValInPos(thisInd)));
// #endif
//   }

//   if (DEBUGGING_PATTERN)
//   {
//     Serial.flush();
//     Serial.println(F("Wrote all LEDs to colorMemory..."));
//     //    delay(1000);
//   }

//   //  Serial.println();
// };

// // Actually send a bit to the string. We must to drop to asm to enusre that the complier does
// // not reorder things and make it so the delay happens in the wrong place.

void controller::sendBit(bool bitVal)
{
  // Credit: wp.josh.com

  if (bitVal)
  { // 0 bit

    asm volatile(
        "sbi %[port], %[bit] \n\t" // Set the output bit
        ".rept %[onCycles] \n\t"   // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t"
        "cbi %[port], %[bit] \n\t" // Clear the output bit
        ".rept %[offCycles] \n\t"  // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t" ::
            [port] "I"(_SFR_IO_ADDR(PIXEL_PORT)),
        [bit] "I"(PIXEL_BIT),
        [onCycles] "I"(NS_TO_CYCLES(T1H) - 2), // 1-bit width less overhead  for the actual bit setting, note that this delay could be longer and everything would still work
        [offCycles] "I"(NS_TO_CYCLES(T1L) - 2) // Minimum interbit delay. Note that we probably don't need this at all since the loop overhead will be enough, but here for correctness

    );
  }
  else
  { // 1 bit

    // **************************************************************************
    // This line is really the only tight goldilocks timing in the whole program!
    // **************************************************************************

    asm volatile(
        "sbi %[port], %[bit] \n\t" // Set the output bit
        ".rept %[onCycles] \n\t"   // Now timing actually matters. The 0-bit must be long enough to be detected but not too long or it will be a 1-bit
        "nop \n\t"                 // Execute NOPs to delay exactly the specified number of cycles
        ".endr \n\t"
        "cbi %[port], %[bit] \n\t" // Clear the output bit
        ".rept %[offCycles] \n\t"  // Execute NOPs to delay exactly the specified number of cycles
        "nop \n\t"
        ".endr \n\t" ::
            [port] "I"(_SFR_IO_ADDR(PIXEL_PORT)),
        [bit] "I"(PIXEL_BIT),
        [onCycles] "I"(NS_TO_CYCLES(T0H) - 2),
        [offCycles] "I"(NS_TO_CYCLES(T0L) - 2)

    );
  }

  // Note that the inter-bit gap can be as long as you want as long as it doesn't exceed the 5us reset timeout (which is A long time)
  // Here I have been generous and not tried to squeeze the gap tight but instead erred on the side of lots of extra time.
  // This has the nice side effect of avoid glitches on very long strings becuase
};

void controller::sendByte(unsigned char byte)
{
  // Credit: wp.josh.com
  for (unsigned char bit = 0; bit < 8; bit++)
  {

    sendBit(bitRead(byte, 7)); // Neopixel wants bit in highest-to-lowest order
    // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
    byte <<= 1; // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
  }
};

/*

  The following two functions are the public API:
  sendPixel( r g , b , w) - send a single pixel to the string. Call this once for each pixel in a frame.
  show() - show the recently sent pixel on the LEDs . Call once per frame.

*/

void controller::sendPixel(unsigned char r, unsigned char g, unsigned char b, unsigned char w)
{
  // Credit: wp.josh.com
  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Sending pixel colors..."));
  //    // delay(1000);
  //  }
  cli();       // THIS MAY BE A PROBLEM (if tics keep getting dropped, consider removing this, or finding another solutions)
  sendByte(g); // Neopixel wants colors in green then red then blue then white order
  sendByte(r);
  sendByte(b);
  sendByte(w);
  sei();
}

void controller::sendPixel(colorObj colorObjIn)
{
  sendPixel(colorObjIn.r(), colorObjIn.g(), colorObjIn.b(), colorObjIn.w());
}

//void controller::sendPixel(Color_* colorIn) {
//  sendPixel(colorIn->getColor());
//}

// Just wait long enough without sending any bits to cause the pixels to latch and display the last sent frame

void controller::show_LEDs()
{
  // Credit: wp.josh.com
  _delay_us((RES / 1000UL) + 1); // Round up since the delay must be _at_least_ this long (too short might not work, too long not a problem)
}

void controller::ledsetup()
{
  // Credit: wp.josh.com
  bitSet(PIXEL_DDR, PIXEL_BIT);
}

// Pattern_Handler::Pattern_Handler()
// {
//   setupPalette(1);

//   speedometer = NULL;
// }

Pattern_Handler::Pattern_Handler(Speedometer *speedometer) : speedometer(speedometer)
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("***"));
    Serial.println(F("Initializing Pattern Handler..."));
    Serial.println(F("***"));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println();
  }
  // Perform 1-time led setup
  controller::ledsetup();

  // Set up the palette
  setupPalette(1);

  //   Set initial main and idle animations
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Setting up initial main pattern..."));
    //    Serial.print(F("Current memory: "));
    //    Serial.println(freeRam());
    Serial.println();
  }
  setMainPattern(new Pattern());

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("***"));
    //    Serial.println(F("Setting up initial idle pattern..."));
    //    Serial.println(F("***"));
    //    Serial.print(F("Current memory: "));
    //    Serial.println(freeRam());
    Serial.println();
    delay(1000);
  }
  //  setIdlePattern(I_MOVING);
  setIdlePattern(new Pattern());

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("***"));
    Serial.println(F("Finished Pattern Handler"));
    Serial.println(F("***"));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println();
  }
};

Pattern_Handler::Pattern_Handler(Speedometer *speedometer, Color_ **colorsIn, unsigned char numColorsIn) : Pattern_Handler(speedometer)
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("***"));
    Serial.println(F("Initializing Pattern Handler..."));
    Serial.println(F("***"));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println();
  }
  // Perform 1-time led setup
  controller::ledsetup();

  // Set up the palette
  setupPalette(colorsIn, numColorsIn);

  //   Set initial main and idle animations
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Setting up initial main pattern..."));
    //    Serial.print(F("Current memory: "));
    //    Serial.println(freeRam());
    Serial.println();
  }
  setMainPattern(new Pattern());

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("***"));
    //    Serial.println(F("Setting up initial idle pattern..."));
    //    Serial.println(F("***"));
    //    Serial.print(F("Current memory: "));
    //    Serial.println(freeRam());
    Serial.println();
    delay(1000);
  }
  //  setIdlePattern(I_MOVING);
  setIdlePattern(new Pattern());

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("***"));
    Serial.println(F("Finished Pattern Handler"));
    Serial.println(F("***"));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println();
  }
};

// boolean Pattern_Main::doesAllowIdle()
// {
//   return allowIdle;
// }

void Pattern_Handler::mainLoop()
{
  if (speedometer->isSlow() && mainPattern->supportIdle())
  {
    // Wheel is moving slowly (and the main Pattern supports an idle function), do idle animation

    if (DEBUGGING_PATTERN)
    {
      // Serial.flush();
      Serial.println(F("Idle animation..."));
    }

    idlePattern->anim();
  }
  else
  {
    // Wheel is moving at pace, do main animation

    if (DEBUGGING_PATTERN)
    {
      //      // Serial.flush();
      Serial.println(F("Main animation..."));
      //      Serial.print(F("allowIdle set to "));
      //      Serial.println(mainPattern->allowIdle);
      delay(500);
    }
    mainPattern->anim((int)roundf(speedometer->getPos()));
  }

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Finished Pattern Handler loop..."));
  }

  // Display the image
  controller::show_LEDs();
};

void Pattern_Handler::setMainPattern(Pattern * newMainPattern)
{
  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Deleting old Main Pattern..."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }

  delete mainPattern; // Delete old main pattern if it exists

  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Creating Main Pattern..."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }
  // switch (newAnimationEnum)
  // {
  // case M_GREL_STILL:
  //   mainPattern = new Pattern(new Static_Helper(), true);
  //   break;
  // case M_GREL_MOVING:
  //   mainPattern = new Moving_Helper_Main(speedometer);
  //   break;
  // default:
  //   mainPattern = new Still_Image_Main(speedometer);
  //   break;
  // }

  mainPattern = newMainPattern; // Assign the new Pattern to the main Pattern in this Handler

  if (mainPattern->supportIdle()) {
    // If the Pattern supports an idle animation, create a new placeholder, in case a new one is not added
    idlePattern = new Pattern();
  } else {
    // If the pattern does not support an idle animation, delete the idlePattern
    delete idlePattern;
    idlePattern = NULL;
  }


  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Set main pattern..."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }
};

void Pattern_Handler::setIdlePattern(Pattern * newIdlePattern)
{
  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Deleting old Idle Pattern..."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }

  delete idlePattern; // Delete old idle pattern if it exists

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Checked and potentially deleted idle pointer..."));
  //  }

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Creating Idle Pattern..."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }

    if (!newIdlePattern->supportIdle()) {
    // If the incoming Pattern does not support an idle, then it cannot be used as an idle.
    // Create a simple Pattern as a place-holder
    idlePattern = new Pattern();
    return;
  }

  idlePattern = newIdlePattern; // Assign the new Pattern to the idle Pattern in this Handler

  // switch (newAnimationEnum)
  // {
  // case I_STILL:
  //   newPattern = new Still_Image_Idle();
  //   break;
  // case I_MOVING:
  //   newPattern = new Moving_Helper_Idle();
  //   break;
  // default:
  //   newPattern = new Still_Image_Idle();
  //   break;
  // }
  // idlePattern = newPattern; // Assign the new pattern

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Finished Idle Pattern"));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }
};

Pattern_Handler::~Pattern_Handler()
{
  deleteColorArray(); // Delete the color array when the Pattern is deleted

  delete mainPattern;
  delete idlePattern;
}

// // Encoding Image type functions
// ImageMeta_BT Pattern::getImageType()
// {
//   ImageMeta_BT im = ImageMeta_BT_default;
//   im.type = ImageType_CONSTANT_BT;
//   im.parameter_set = ImageMetaParam_BT_default;
//   im.parameter_set.p1 = 0;
//   return im;
// }

// ImageMeta_BT Moving_Helper_Main::getImageType()
// {
//   ImageMeta_BT im = ImageMeta_BT_default;
//   im.type = ImageType_CONSTANT_BT;
//   im.parameter_set = ImageMetaParam_BT_default;
//   im.parameter_set.p1 = getRotateSpeed();
//   return im;
// }

// ImageMeta_BT Moving_Helper_Idle::getImageType()
// {
//   ImageMeta_BT im = ImageMeta_BT_default;
//   im.type = ImageType_CONSTANT_BT;
//   im.parameter_set = ImageMetaParam_BT_default;
//   im.parameter_set.p1 = getRotateSpeed();
//   return im;
// }