#include "Arduino.h"
#include "Speedometer.h"
#include "Definitions.h"
#include "Color.h"
#include "Pattern.h"

Pattern::Pattern()
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

// Pattern::Pattern(Color_** colorsIn, unsigned char numColorsIn) {
//   setupColors(colorsIn, numColorsIn);

//   if (DEBUGGING_PATTERN) {
//     // Serial.flush();
//     //    Serial.println(F("Setting up Pattern..."));
//   }
// };

Pattern::~Pattern()
{
  // deleteColorArray(); // Delete the color array when the Pattern is deleted
}

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

  delete[] newColorArray;

  //  if (DEBUGGING_PATTERN) {
  //    Serial.println(F("Deleted temporary color array"));
  //    //    delay(500);
  //  }
};

void Pattern_Handler::setupPalette(Color_ **newColorArray, unsigned char numColorsIn)
{
  deleteColorArray(); // Always delete the old array when a new one is being created

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

  palette = new Color_ *[numColorsIn];             // Allocate space for the color array
  preCalculatedColors = new colorObj[numColorsIn]; // Allocate space for the preCalculatedColors array

  copyArray<Color_ *>(newColorArray, palette, numColorsIn);
  //  *colors = *newColorArray;
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

void Pattern_Handler::setupPalette(Color_BT *colorMessagesIn, unsigned char numColorsIn)
{
  deleteColorArray(); // Always delete the old array when a new one is being created

  palette = new Color_ *[numColorsIn];             // Allocate space for the color array
  preCalculatedColors = new colorObj[numColorsIn]; // Allocate space for the preCalculatedColors array

  for (unsigned char i = 0; i < numColorsIn; i++)
  {
    palette[i] = Bluetooth::Color_FromPB(colorMessagesIn[i], speedometer);
  }
  //  *colors = *newColorArray;
  numColors = numColorsIn;
}

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
  if (LEDNum < nLEDs)
  {
    return image[LEDNum];
  }
  else
  {
    return image[nLEDs];
  }
};

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

Color_ *Pattern_Handler::getColor(unsigned char colorNum)
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
    //    for (int i = 0; i < nLEDs; i++) {
    //      Serial.println(image[i]);
    //    }
    //    delay(50);
    //    // Serial.flush();
    //    Serial.println();
    //    Serial.println(F("Initial imageIn:"));
    //    for (int i = 0; i < nLEDs; i++) {
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
  //    Serial.print(F("nLEDs: "));
  //    Serial.println(nLEDs);
  //    Serial.print(F("NUMLEDS: "));
  //    Serial.println(NUMLEDS);
  //    Serial.println();
  //    // Serial.flush();
  //    Serial.println(F("Old Image:"));
  //    for (unsigned char i = 0; i < nLEDs; i++) {
  //      Serial.print(F(" "));
  //      Serial.print(image[i]);
  //    }
  //    delay(100);
  //    Serial.println();
  //    Serial.println(F("Image In:"));
  //    for (unsigned char i = 0; i < nLEDs; i++) {
  //      Serial.print(F(" "));
  //      Serial.print(imageIn[i]);
  //    }
  //    Serial.println();
  //    delay(100);
  //  }
  // Set the image to the new image
  for (unsigned char i = 0; i < nLEDs; i++)
  {
    image[i] = imageIn[i];
  }
  //  copyArray<unsigned int>(imageIn, image, nLEDs);

  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.println(F("Copied image..."));
  //    Serial.println();
  //    // Serial.flush();
  //    Serial.println(F("New Image:"));
  //    for (unsigned char i = 0; i < nLEDs; i++) {
  //      Serial.print(F(" "));
  //      Serial.print(image[i]);
  //    }
  //    Serial.println();
  //    delay(100);
  //  }

  // Create an array of
  unsigned char numColorsInArray = maxInArray<unsigned char>(image, nLEDs) + 1; // The number of colors is the maximum value plus one (i.e. if the maximum value is 2, then integers 0, 1, and 2 all correspond to different numbers, meaning there are 3 colors)

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

void Pattern::setImage(uint32_t *imageIn)
{
  // Set a new image array, and ensure that enough colors to use it

  // Set the image to the new image
  for (unsigned char i = 0; i < nLEDs; i++)
  {
    image[i] = (char)imageIn[i];
  }

  // Create an array of
  unsigned char numColorsInArray = maxInArray<unsigned char>(image, nLEDs) + 1; // The number of colors is the maximum value plus one (i.e. if the maximum value is 2, then integers 0, 1, and 2 all correspond to different numbers, meaning there are 3 colors)

  parent_handler->setupPalette(numColorsInArray);
  // setupColors(numColorsInArray);
};

void Pattern::setImageZeros()
{
  if (DEBUGGING_PATTERN)
  {
    Serial.flush();
    Serial.println(F("Setting zero image..."));
  }

  unsigned char newImage[NUMLEDS];

  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Starting loop..."));
  }
  for (unsigned char i = 0; i < nLEDs; i++)
  {
    //    for (int j = 0; j < nLightsPerLED; j++) {
    newImage[i] = 0; // Zero out the entire array
  }

  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("New image is: "));
    for (unsigned char i = 0; i < nLEDs; i++)
    {
      Serial.print(newImage[i]);
      Serial.print(F(" "));
    }
    Serial.println();
  }

  if (DEBUGGING_PATTERN)
  {
    //    Serial.println(F("Finished loop..."));
  }

  setImage(newImage);
};

void Pattern::setImageColorInd(unsigned char colorInd)
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Setting zero image..."));
  }

  colorInd = min(colorInd, parent_handler->getNumColors() - 1);

  unsigned char newImage[NUMLEDS];

  if (DEBUGGING_PATTERN)
  {
    //    Serial.println(F("Starting loop..."));
  }
  for (unsigned char i = 0; i < nLEDs; i++)
  {
    //    for (int j = 0; j < nLightsPerLED; j++) {
    newImage[i] = colorInd; // Zero out the entire array
    //    if (DEBUGGING_PATTERN) {
    //      Serial.println(newImage[i]);
    //    }
    //    }
  }

  if (DEBUGGING_PATTERN)
  {
    //    Serial.println(F("Finished loop..."));
  }

  setImage(newImage);
};

void Pattern::setImageNumSegs(unsigned char numSegs)
{
  //  if (DEBUGGING_PATTERN) {
  //    // Serial.flush();
  //    Serial.print(F("Making image with "));
  //    Serial.print(numSegs);
  //    Serial.println(F(" segments..."));
  //    //    delay(500);
  //  }
  unsigned char newImage[NUMLEDS];
  unsigned char distPerSeg = (unsigned char)roundf((float)nLEDs / (float)numSegs); // Get a *rounded* distance per segment to display (MAY NOT BE MATHEMATICALLY CORRECT, MAY NEED TO CALCULATE INSIDE OF LOOP)

  // Create the image
  for (unsigned char i = 0; i < nLEDs; i++)
  {
    newImage[i] = 0; // Zero out the entire array
  }

  for (unsigned char i = 0; i < nLEDs; i += distPerSeg)
  {
    newImage[i] = 1; // Place a 1 every distPerSed number of LEDs
  }

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Setting segmented image..."));
    //    delay(500);
  }
  setImage(newImage);
};

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
//  int distPerSeg = (int)roundf((float)numSegs / (float)nLEDs); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < nLEDs; i += distPerSeg) {
//    image[0][i] = brightness; //  out the entire array
//  }
//};
//
//void Pattern::setImageNumSegsG(int numSegs, unsigned char brightness) {
//  int distPerSeg = (int)roundf((float)numSegs / (float)nLEDs); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < nLEDs; i += distPerSeg) {
//    image[1][i] = brightness; //  out the entire array
//  }
//};
//
//void Pattern::setImageNumSegsB(int numSegs, unsigned char brightness) {
//  int distPerSeg = (int)roundf((float)numSegs / (float)nLEDs); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < nLEDs; i += distPerSeg) {
//    image[2][i] = brightness; //  out the entire array
//  }
//};
//
//void Pattern::setImageNumSegsW(int numSegs, unsigned char brightness) {
//  int distPerSeg = (int)roundf((float)nLEDs / (float)numSegs); // Get a *rounded* distance per segment to display
//
//  for (int i = 0; i < nLEDs; i += distPerSeg) {
//    image[3][i] = brightness; //  out the entire array
//  }
//};

Pattern_Main::Pattern_Main(Speedometer *speedometer) : speedometer(speedometer){};

Moving_Image::Moving_Image()
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Creating Moving_Image..."));
    //    Serial.print(F("Current memory: "));
    //    Serial.println(freeRam());
    //    delay(500);
  }

  // Initialize the colorMemory array with colorObj's whose color data will be rewritten each animMain()
  // Used with the memory-expensive color blur method
  //  for (unsigned char colorInd = 0; colorInd < NUMLEDS; colorInd++) {
  //    if (DEBUGGING_PATTERN) {
  //      // Serial.flush();
  //      //      Serial.print(F("Making color object "));
  //      //      Serial.print(colorInd);
  //      //      Serial.println(F("..."));
  //      //    delay(500);
  //    }
  //    colorMemory[colorInd] = colorObj();
  //  }

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Finished making Moving_Image..."));
    //    Serial.print(F("Current memory: "));
    //    Serial.println(freeRam());
    //    delay(500);
  }
}

void Moving_Image::anim()
{
  //  if (DEBUGGING_MOVINGIMAGE) {
  //    Serial.println(F("Starting Moving Image upper level animation..."));
  //  }
  //  colorBlur(); // Calculate the blurring of the color memory (Occurs before animMain() if using the memory-heavy method)
  animMain();      // Perform the custom animation defined by the derived classes
                   //  colorBlur(); // Calculate the blurring of the color memory
                   //  sendColors(); // Send the colors in colorMemory to the LEDs
  advanceLEDPos(); // Advance the LED position

  if (DEBUGGING_MOVINGIMAGE)
  {
    Serial.println();
  }
};

void Moving_Image::setImageBleed(unsigned char imageBleedIn)
{
  if (0 <= imageBleedIn && imageBleedIn < 255)
  {
    imageBleed = imageBleedIn;
  }
};

void Moving_Image::setRotateSpeed(int rotateSpeedIn)
{
  rotateSpeed = rotateSpeedIn;
};

int Moving_Image::getRotateSpeed()
{
  return rotateSpeed;
}

float Moving_Image::getLEDPos()
{
  return currentLEDPos;
};

//void Moving_Image::colorBlur() {
//  Method for doing color blur that requires a large amount of memory (NUMLIGHTSPERLED * NUMLEDS bytes) per pattern, in the form of the colorMemory array.
//  May be useful if using external RAM.
//  unsigned long dt = micros() - lastLEDAdvanceTime; // How much time as passed since the LED position was last updated?
//  float tune = 700; // Used to remap the imageBleed variable to make it easier to choose bleed times
//  float bleedFac = (float)dt / 1000000.0; // Convert milliseconds to seconds
//  if (DEBUGGING_MOVINGIMAGE) {
//    Serial.println(dt);
//    Serial.println(bleedFac, 10);
//  }
//  for (unsigned char colorInd = 0; colorInd < NUMLEDS; colorInd++) {
//    if (DEBUGGING_MOVINGIMAGE && colorInd == 0) {
//      Serial.println(1.0 - tune * bleedFac * (1.0 - pow((float)imageBleed / 255.0, .2)), 10);
//    }
//    //    colorMemory[colorInd].multiplyBrightness((bleedFac * (float)imageBleed) / 255.0);
//    colorMemory[colorInd].multiplyBrightness(1.0 - tune * bleedFac * (1.0 - pow((float)imageBleed / 255.0, .2)));
//  }
//};

void Moving_Image::addImagePosition(colorObj colorObjIn, unsigned char imagePosition)
{
  unsigned char thisInd = (unsigned char)fmod(round((float)imagePosition + currentLEDPos), NUMLEDS); // Add currentLED pos to imagePosition, then wrap it within NUMLEDS to reference colorMemory (This may be slow?)
  //  if (DEBUGGING_MOVINGIMAGE) {
  //    Serial.println(thisInd);
  //  }
  //  colorMemory[thisInd] = colorObjIn;
};

//void Moving_Image::sendColors() {
// Send colorMemory object (used with memory-expensive blur method)
//  for (unsigned char i = 0; i < NUMLEDS; i++) {
//    controller::sendPixel(colorMemory[i]); // Send the color in colorMemory
//  }
//}

void Moving_Image::advanceLEDPos()
{
  unsigned long thisLEDAdvanceTime = micros();
  unsigned long dt = thisLEDAdvanceTime - lastLEDAdvanceTime; // How much time as passed since the LED position was last updated?

  currentLEDPos = fmodf(currentLEDPos + ((float)dt / 1000000.0) * (float)rotateSpeed, NUMLEDS); // Update currentLEDPos, and keep it between 0<=currentLEDPos<nLEDs
  lastLEDAdvanceTime = thisLEDAdvanceTime;                                                      // Update lastLEDAdvanceTime
};

Still_Image_Main::Still_Image_Main(Speedometer *speedometer) : Pattern_Main(speedometer)
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Making still main pattern..."));
    //    delay(500);
  }
  //  setImageZeros();
  setImageNumSegs(10);
  //  Serial.println(F("Pattern set."));
}

Still_Image_Main::Still_Image_Main(Speedometer *speedometer, unsigned char *imageIn) : Pattern_Main(speedometer)
{
  setImage(imageIn);
};

void Still_Image_Main::anim()
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Main Start..."));
    //    delay(500);
  }

  // Get the wheel reference location
  int xTrueRounded = (int)roundf(speedometer->getPos());
  //  Serial.print(F("xTrueRounded is: "));
  //  Serial.println(xTrueRounded);
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Got position..."));
    //    delay(500);
  }

  // Pre-calculate all Color_'s
  parent_handler->preCalculateAllColor_();
  // preCalculateAllColor_();

  // Load the image onto the wheel, as is
  for (unsigned char LEDNum = 0; LEDNum < nLEDs; LEDNum++)
  {
    //    if (DEBUGGING_PATTERN) {
    //      // Serial.flush();
    //      Serial.print(F("Loop "));
    //      Serial.print(LEDNum);
    //      Serial.print(F(", xTrueRounded is "));
    //      Serial.print(xTrueRounded);
    //      Serial.print(F(", nLEDs is "));
    //      Serial.println(nLEDs);
    ////      delay(500);
    //    }
    unsigned char imagePos = (unsigned char)((LEDNum + xTrueRounded) % (int)nLEDs); // Adjust the position in the image
    //    if (DEBUGGING_PATTERN) {
    //      Serial.print(F("imagePos is "));
    //      Serial.println(imagePos);
    //      Serial.print(F("image[imagePos] is "));
    //      Serial.println(image[imagePos]);
    //      Serial.print(F("Total number of colors is "));
    //      Serial.println(getNumColors());
    //      delay(500);
    //      Serial.print(F("R = "));
    //      Serial.print(colors[image[imagePos]]->getColor().r());
    //      Serial.print(F(", G = "));
    //      Serial.print(colors[image[imagePos]]->getColor().g());
    //      Serial.print(F(", B = "));
    //      Serial.print(colors[image[imagePos]]->getColor().b());
    //      Serial.print(F(", W = "));
    //      Serial.println(colors[image[imagePos]]->getColor().w());
    //      Serial.println(F("Sending pixel..."));
    //      delay(500);
    //    }
    //    controller::sendPixel(image[0][imagePos], image[1][imagePos], image[2][imagePos], image[3][imagePos]);
    controller::sendPixel(parent_handler->getPreCalculatedColorInPos(getImageValInPos(imagePos)));
  }

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Finished Main..."));
    delay(500);
  }
};

Still_Image_Idle::Still_Image_Idle() : Pattern_Idle()
{
  if (DEBUGGING_PATTERN)
  {
    //    Serial.println(F("Making Still_Image_Idle"));
    //    // Serial.flush();
    //    Serial.print(getNumColors());
    //    Serial.println(F(" colors currently in still idle pattern..."));
  }

  // Set up black and red idle animation
  //  setImageZeros();
  setImageNumSegs(10);

  if (DEBUGGING_PATTERN)
  {
    //    Serial.println(F("Done with Still_Image_Idle..."));
    //    delay(200);
  }

  // Blink class leftovers
  //  setupColors(2); // Need two colors for this animation
  //  setColor(Color_Static(0, 0, 0, 0).getColor_(), 0);
  //  setColor(Color_Static(150, 0, 0, 0).getColor_(), 1);
};

Still_Image_Idle::Still_Image_Idle(unsigned char *imageIn) : Pattern_Idle()
{
  setImage(imageIn);
};

void Still_Image_Idle::anim()
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    //    Serial.println(F("Idle animation starting..."));
  }
  //  unsigned long curTime = millis();
  //  Serial.print(F("Idle: "));
  //  Serial.println((curTime - timeAtToggle));
  //  if (!isIdle) {
  //    // Idling just started
  //    //    Serial.println(F("Start"));
  //    isIdle = true;
  //    timeAtToggle = curTime;
  //    idleToggleSig = true;
  //  }

  // Blink class leftovers
  //  if (idleToggleSig) {
  //    if ((curTime - timeAtToggle) > timeOn) {
  //      idleToggleSig = false;
  //      //      idleBrightness = 0;
  //      timeAtToggle = curTime;
  //      //      Serial.println(F("Off"));
  //    }
  //  } else {
  //    if ((curTime - timeAtToggle) > timeOff) {
  //      idleToggleSig = true;
  //      //      idleBrightness = 150;
  //      timeAtToggle = curTime;
  //      //      Serial.println(F("On"));
  //    }
  //  }
  //  if (DEBUGGING_PATTERN) {
  // Serial.flush();
  //    Serial.println(F("Setting color..."));
  //    delay(500);
  //  }

  //  Color_ *thisC;
  //  if (idleToggleSig) {
  //    thisC = colors[0];

  //    if (DEBUGGING_PATTERN) {
  //      // Serial.flush();
  //      Serial.print(F("Using first color of "));
  //      Serial.print(getNumColors());
  //      //      delay(500);
  //      Serial.print(F(" total colors..."));
  //      Serial.print(F("R = "));
  //      Serial.print(colors[0]->getColor().r());
  //      Serial.print(F(", G = "));
  //      Serial.print(colors[0]->getColor().g());
  //      Serial.print(F(", B = "));
  //      Serial.print(colors[0]->getColor().b());
  //      Serial.print(F(", W = "));
  //      Serial.println(colors[0]->getColor().w());
  //      //      delay(500);
  //    }
  //  } else {
  //    if (DEBUGGING_PATTERN) {
  // Serial.flush();
  //      Serial.println(F("Using second color..."));
  // delay(1000);
  //    }
  //    thisC = colors[1];
  //  }

  // Pre-calculate all Color_'s
  parent_handler->preCalculateAllColor_();
  // preCalculateAllColor_();

  if (DEBUGGING_PATTERN)
  {
    //    Serial.flush();
    //    Serial.println(F("Sending all LEDs..."));
    //    delay(1000);
  }
  // Load the image onto the wheel, as is
  for (unsigned char LEDNum = 0; LEDNum < nLEDs; LEDNum++)
  {
    controller::sendPixel(parent_handler->getPreCalculatedColorInPos(getImageValInPos(LEDNum)));
  }

  if (DEBUGGING_PATTERN)
  {
    //    Serial.flush();
    //    Serial.println(F("Sent all LEDs..."));
    //    delay(1000);
  }

  //  Serial.println();
};

// Moving Image Main functions
Moving_Image_Main::Moving_Image_Main(Speedometer *speedometer, unsigned char *image) : Still_Image_Main(speedometer, image){};
Moving_Image_Main::Moving_Image_Main(Speedometer *speedometer) : Still_Image_Main(speedometer){};

void Moving_Image_Main::animMain()
{
  if (DEBUGGING_PATTERN)
  {
    Serial.flush();
    Serial.println(F("Main animation starting..."));
  }

  int xTrueRounded = (int)roundf(speedometer->getPos());

  // Pre-calculate all Color_'s
  parent_handler->preCalculateAllColor_();
  // preCalculateAllColor_();

  // Load the image into color memory, as is
  for (unsigned char LEDNum = 0; LEDNum < nLEDs; LEDNum++)
  {
    unsigned char imagePos = (unsigned char)((LEDNum + xTrueRounded) % (int)nLEDs); // Adjust the position in the image
    if (getImageValInPos(imagePos) != 0)
    {
      // If the image at this location is not 0 (referencing a blank Color_), then add a new color to the LED strip
      addImagePosition(parent_handler->getPreCalculatedColorInPos(getImageValInPos(imagePos)), imagePos);
    }
  }

  if (DEBUGGING_PATTERN)
  {
    Serial.flush();
    Serial.println(F("Wrote all LEDs to colorMemory..."));
    //    delay(1000);
  }

  //  Serial.println();
};

// Moving Image Idle functions
Moving_Image_Idle::Moving_Image_Idle(unsigned char *image) : Still_Image_Idle(image){};
Moving_Image_Idle::Moving_Image_Idle() : Still_Image_Idle()
{
  if (DEBUGGING_PATTERN)
  {
    //    Serial.flush();
    //    Serial.println(F("Finished making Moving_Image_Idle"));
    //    delay(500);
  }
};

void Moving_Image_Idle::animMain()
{
  if (DEBUGGING_PATTERN)
  {
    Serial.flush();
    Serial.println(F("Idle animation starting..."));
  }

  // Pre-calculate all Color_'s
  parent_handler->preCalculateAllColor_();
  // preCalculateAllColor_();

  // Load the image onto the wheel, as is
  for (unsigned char LEDNum = 0; LEDNum < nLEDs; LEDNum++)
  {
    if (getImageValInPos(LEDNum) != 0)
    {
      // If the image at this location is not 0 (referencing a blank Color_), then add a new color to the LED strip
      addImagePosition(parent_handler->getPreCalculatedColorInPos(getImageValInPos(LEDNum)), LEDNum); // Use overriden copy assignment operator, which copies the color data to the colorMemory array
    }
  }

  if (DEBUGGING_PATTERN)
  {
    Serial.flush();
    Serial.println(F("Wrote all LEDs to colorMemory..."));
    //    delay(1000);
  }

  //  Serial.println();
};

// Actually send a bit to the string. We must to drop to asm to enusre that the complier does
// not reorder things and make it so the delay happens in the wrong place.

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
  setMainPattern(M_STILL);

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
  setIdlePattern(I_STILL);

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

Pattern_Handler::Pattern_Handler(Speedometer *speedometer, Color_ **colorsIn, unsigned char numColorsIn) : speedometer(speedometer)
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
  setMainPattern(M_STILL);

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
  setIdlePattern(I_STILL);

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

boolean Pattern_Main::doesAllowIdle()
{
  return allowIdle;
}

void Pattern_Handler::mainLoop()
{
  if (speedometer->isSlow() && mainPattern->doesAllowIdle())
  {
    // Wheel is moving slowly, do idle animation

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
    mainPattern->anim();
  }

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Finished Pattern Handler loop..."));
  }

  // Display the image
  controller::show_LEDs();
};

void Pattern_Handler::setMainPattern(MAIN_ANIM newAnimationEnum)
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
  switch (newAnimationEnum)
  {
  case M_STILL:
    mainPattern = new Still_Image_Main(speedometer);
    break;
  case M_MOVING:
    mainPattern = new Moving_Image_Main(speedometer);
    break;
  default:
    mainPattern = new Still_Image_Main(speedometer);
    break;
  }

  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Set main pattern..."));
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

void Pattern_Handler::setIdlePattern(IDLE_ANIM newAnimationEnum)
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

  Pattern_Idle *newPattern; // Create a pointer for a new pattern

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Creating Idle Pattern..."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }

  switch (newAnimationEnum)
  {
  case I_STILL:
    newPattern = new Still_Image_Idle();
    break;
  case I_MOVING:
    newPattern = new Moving_Image_Idle();
    break;
  default:
    newPattern = new Still_Image_Idle();
    break;
  }

  idlePattern = newPattern; // Assign the new pattern

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Finished Idle Pattern"));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F(""));
  }
};

// Encoding Image type functions
ImageMeta_BT Pattern::getImageType()
{
  ImageMeta_BT im = ImageMeta_BT_default;
  im.type = ImageType_CONSTANT_BT; // TODO: Perhaps the default should be stationary? (or whatever I end up calling not-moving-relative-to-the-wheel)
  im.parameter_set = ImageMetaParam_BT_default;
  im.parameter_set.p1 = 0;
  return im;
}

ImageMeta_BT Moving_Image_Main::getImageType()
{
  ImageMeta_BT im = ImageMeta_BT_default;
  im.type = ImageType_CONSTANT_BT;
  im.parameter_set = ImageMetaParam_BT_default;
  im.parameter_set.p1 = getRotateSpeed();
  return im;
}

ImageMeta_BT Moving_Image_Idle::getImageType()
{
  ImageMeta_BT im = ImageMeta_BT_default;
  im.type = ImageType_CONSTANT_BT;
  im.parameter_set = ImageMetaParam_BT_default;
  im.parameter_set.p1 = getRotateSpeed();
  return im;
}