#include "Arduino.h"
#include "Speedometer.h"
#include "Definitions.h"
#include "Color.h"

colorObj::colorObj()
{
  //  if (DEBUGGING_GENERAL) {
  //    // Serial.flush();
  //    Serial.println(F("BLANK"));
  //  }
  c[0] = 0;
  c[1] = 0;
  c[2] = 0;
  c[3] = 0;
};

colorObj::colorObj(unsigned char rIn, unsigned char gIn, unsigned char bIn, unsigned char wIn)
{
  c[0] = rIn;
  c[1] = gIn;
  c[2] = bIn;
  c[3] = wIn;
};

colorObj::colorObj(unsigned char *cA)
{
  if (DEBUGGING_GENERAL)
  {
    // Serial.flush();
    Serial.println(F("Assigning colorObj..."));
  }
  copyArray<unsigned char>(cA, c, NUMLIGHTSPERLED);
  //  *c = *cA;
};

colorObj &colorObj::operator=(const colorObj &cO)
{
  //  if (DEBUGGING_GENERAL) {
  //    // Serial.flush();
  //    Serial.println(F("Assigning colors..."));
  //  }
  //  copyArray<unsigned char>(cO.c, c, 4);

  for (unsigned char i = 0; i < 4; i++)
  {
    c[i] = cO.c[i];
  }

  return *this;
};

unsigned char colorObj::r() const
{
  return c[0];
};

unsigned char colorObj::g() const
{
  return c[1];
};

unsigned char colorObj::b() const
{
  return c[2];
};

unsigned char colorObj::w() const
{
  return c[3];
};

void colorObj::multiplyBrightness(float brightnessFac)
{
  if (brightnessFac < 1.0f)
  {
    // If there needs to be any brightness adjustment...
    for (unsigned char i = 0; i < 4; i++)
    {
      // Going through each LED color value in the colorObj
      float newVal = ((float)c[i]) * brightnessFac; // Get the proposed new value
      newVal = min(max(newVal, 0), 255);            // Constrain the value
      c[i] = (unsigned char)newVal;                 // Assign the value
    }
  }
};

//void colorObj::addBrightness(int brightnessSubVal) {
//  for (int i = 0; i < 4; i++) {
//    // Going through each LED color value in the colorObj
//    int newVal = c[i] + brightnessSubVal; // Get the proposed new value
//    newVal = min(max(newVal, 0), 255); // Constrain the value
//    c[i] = (unsigned char)newVal; // Assign the value
//  }
//};

Color_ *Color_::getColor_()
{
  return this;
};

// boolean Color_::isThisEmpty() const
// {
//   return isEmpty;
// }

Color_Static::Color_Static(unsigned char r, unsigned char g, unsigned char b, unsigned char w)
{
  c = colorObj(r, g, b, w);

  //  if (DEBUGGING_GENERAL) {
  //    // Serial.flush();
  //    Serial.println(F("Creating Static Color..."));
  //  }
  //  c.R = r;
  //  c.G = g;
  //  c.B = b;
  //  c.W = w;
}

Color_Static::Color_Static(colorObj c) : c(c){};

Color_Static::Color_Static(unsigned char *cA) : c(cA){};

Color_Static::Color_Static(){
    // isEmpty = true;
};

Color_Static::Color_Static(const Color_Static &c)
{
  colorObj newCO = c.getColor();
  cS(newCO);

  if (DEBUGGING_GENERAL)
  {
    // Serial.flush();
    Serial.println(F("Copy Operator:"));
    Serial.print(F("Initial: R = "));
    Serial.print(newCO.r());
    Serial.print(F(", G = "));
    Serial.print(newCO.g());
    Serial.print(F(", B = "));
    Serial.print(newCO.b());
    Serial.print(F(", W = "));
    Serial.println(newCO.w());
    // delay(1000);
    Serial.print(F("Clone: R = "));
    Serial.print(getColor().r());
    Serial.print(F(", G = "));
    Serial.print(getColor().g());
    Serial.print(F(", B = "));
    Serial.print(getColor().b());
    Serial.print(F(", W = "));
    Serial.println(getColor().w());
    Serial.println();
    // delay(1000);
  }
};

Color_Static *Color_Static::clone() const
{
  return new Color_Static(*this);
}

colorObj Color_Static::getColor() const
{
  //  if (DEBUGGING_GENERAL) {
  //      // Serial.flush();
  //      Serial.println(F("Getting Static Color..."));
  //      delay(1000);
  //      Serial.print(F("R = "));
  //      Serial.print(c.r());
  //      Serial.print(F(", G = "));
  //      Serial.print(c.g());
  //      Serial.print(F(", B = "));
  //      Serial.print(c.b());
  //      Serial.print(F(", W = "));
  //      Serial.print(c.w());
  //      delay(1000);
  //    }
  return c;
};

//unsigned char Color_Static::r() {
//  return c.R;
//};
//
//unsigned char Color_Static::g() {
//  return c.G;
//};
//
//unsigned char Color_Static::b() {
//  return c.B;
//};
//
//unsigned char Color_Static::w() {
//  return c.W;
//};

void Color_Static::rS(unsigned char rNew)
{
  // isEmpty = false;
  c.c[1] = rNew;
};

void Color_Static::gS(unsigned char gNew)
{
  // isEmpty = false;
  c.c[2] = gNew;
};

void Color_Static::bS(unsigned char bNew)
{
  // isEmpty = false;
  c.c[3] = bNew;
};

void Color_Static::wS(unsigned char wNew)
{
  // isEmpty = false;
  c.c[4] = wNew;
};

void Color_Static::cS(colorObj cNew)
{
  // isEmpty = false;
  c = cNew;
};

template <class T>
Color_d<T>::Color_d()
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Setting up arrays..."));
  }

  // Constructor
  setupArrays(3); // Arbitrarily choose 10 colors as the number

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Finished creating Dynamic Color."));
  }
};

template <class T>
Color_d<T>::Color_d(unsigned char numColors)
{
  // Constructor
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.print(F("Setting up Color_d(numColors) with "));
    Serial.print(numColors);
    Serial.println(F(" colors."));
  }

  setupArrays(numColors);
};

template <class T>
Color_d<T>::Color_d(colorObj *cA, T *tA, BLEND_TYPE *bA, unsigned char numColors)
{
  // Constructor

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.print(F("Setting up Color_d(numColors) with "));
    Serial.print(numColors);
    Serial.println(F(" colors, and specified arrays."));
  }
  setupArrays(cA, tA, bA, numColors);
};

template <class T>
Color_d<T>::Color_d(const Color_d &c)
{
  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Copying Color_d..."));
    delay(200);
  }
  colorObj *newColorArray = new colorObj[c.getNumColors()];
  T *newTArray = new T[c.getNumColors()];
  BLEND_TYPE *newBlendArray = new BLEND_TYPE[c.getNumColors()];

  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Allocated arrays..."));
    delay(200);
  }

  for (unsigned char i = 0; i < c.getNumColors(); i++)
  {
    newColorArray[i] = c.getThisColorObj(i);
    newTArray[i] = c.getThisTrigger(i);
    newBlendArray[i] = c.getThisBlendType(i);
  }

  //  newColorArray = c.getAllColorObjs();
  //  newTArray = c.getAllTriggers();
  //  newBlendArray = c.getAllBlendTypes();

  setupArrays((colorObj *)newColorArray, (T *)newTArray, (BLEND_TYPE *)newBlendArray, c.getNumColors());

  if (DEBUGGING_PATTERN)
  {
    for (unsigned char i = 0; i < c.getNumColors(); i++)
    {
      Serial.print(i);
      Serial.print(F(" Old: t = "));
      Serial.print(newTArray[i]);
      Serial.print(F(", b = "));
      Serial.print(newBlendArray[i]);
      Serial.print(F(", c = ("));
      Serial.print(newColorArray[i].c[0]);
      Serial.print(F(","));
      Serial.print(newColorArray[i].c[1]);
      Serial.print(F(","));
      Serial.print(newColorArray[i].c[2]);
      Serial.print(F(","));
      Serial.print(newColorArray[i].c[3]);
      Serial.println(F(")"));
      Serial.print(i);
      Serial.print(F(" New: t = "));
      Serial.print(tA[i]);
      Serial.print(F(", b = "));
      Serial.print(bA[i]);
      Serial.print(F(", c = ("));
      Serial.print(cA[i].c[0]);
      Serial.print(F(","));
      Serial.print(cA[i].c[1]);
      Serial.print(F(","));
      Serial.print(cA[i].c[2]);
      Serial.print(F(","));
      Serial.print(cA[i].c[3]);
      Serial.println(F(")"));
      Serial.println(F(""));
    }
  }

#if COLORD_COPY_ARRAYS
  delete[] newColorArray;
  delete[] newTArray;
  delete[] newBlendArray;
#endif
}

template <class T>
Color_d<T>::~Color_d()
{
  // Destructor
  deleteAllArrays(); // Delete all arrays upon destruction
};

template <class T>
colorObj Color_d<T>::getColor() const
{
  if (DEBUGGING_DYNAMICCOLOR)
  {
    Serial.flush();
    Serial.println(F(" "));
    Serial.println(F("Getting Dynamic Color..."));
    //    delay(100);
  }
  // Create a blend of two colorObjs from cA, using the blend type given by bA, at a ratio given by getCurVal()'s location between values of tA

  T blendVal = getCurVal();

  // Find the two colors that must be blended
  unsigned char blendLoc;
  if (DEBUGGING_DYNAMICCOLOR)
  {
    Serial.print(F("Calculating blendLoc with blendVal = "));
    Serial.println(blendVal);
    //    delay(100);
  }
  if (blendVal < tA[numColors - 1])
  {
    // If the blend value is less than the largest value in the array
    for (unsigned char i = 1; i < numColors; i++)
    {
      if (blendVal <= tA[i])
      {
        // First value in tA is always 0, so if blendVal is less than the next value, the blend point must have been found
        blendLoc = i - 1;
        if (DEBUGGING_DYNAMICCOLOR)
        {
          Serial.print(F("Found blendLoc = "));
          Serial.print((int)blendLoc);
          Serial.print(F(", so we will blend between "));
          Serial.print(tA[blendLoc]);
          Serial.print(F(" and "));
          Serial.println(tA[blendLoc + 1]);
          delay(100);
        }
        break;
      }
    }

    // Determine the type of blending
    float ratio;
    // if (DEBUGGING_DYNAMICCOLOR)
    // {
    //   Serial.print(F("This bA value is: "));
    //   Serial.println(bA[blendLoc]);
    //   //      delay(100);
    // }
    switch (bA[blendLoc])
    {
    case B_LINEAR:
    {
      ratio = ((float)blendVal - (float)tA[blendLoc]) / ((float)tA[blendLoc + 1] - (float)tA[blendLoc]);
      if (DEBUGGING_DYNAMICCOLOR)
      {
        Serial.print(F("Calculated ratio = "));
        Serial.print(ratio);
        Serial.println(F("..."));
        //            Serial.println(F(" "));
        //            delay(100);
      }
      return blendColors(cA[blendLoc], cA[blendLoc + 1], ratio);
      break;
    }
    case B_CONSTANT:
    {
      if (DEBUGGING_PATTERN)
      {
        Serial.print(F("Constant blend..."));
        //            delay(100);
      }
      return cA[blendLoc];
      break;
    }
    default:
    {
      if (DEBUGGING_PATTERN)
      {
        Serial.println(F("Something weird happened..."));
        delay(1000);
      }
      return cA[blendLoc];
    }
    }
  }
  else
  {
    // If the blend value is larger than the largest value in the array, simply return the last color in cA
    if (DEBUGGING_PATTERN)
    {
      //      Serial.print(F("Using final color"));
      //      delay(100);
    }
    return cA[numColors - 1];
  }
};

template <class T>
colorObj Color_d<T>::blendColors(colorObj c1, colorObj c2, float ratio) const
{
  // if (DEBUGGING_DYNAMICCOLOR)
  // {
  //   Serial.println(F("Calculated blend:"));
  //   Serial.print(F("1: r = "));
  //   Serial.print(c1.c[0]);
  //   Serial.print(F(", g = "));
  //   Serial.print(c1.c[1]);
  //   Serial.print(F(", b = "));
  //   Serial.print(c1.c[2]);
  //   Serial.print(F(", w = "));
  //   Serial.println(c1.c[3]);

  //   Serial.print(F("2: r = "));
  //   Serial.print(c2.c[0]);
  //   Serial.print(F(", g = "));
  //   Serial.print(c2.c[1]);
  //   Serial.print(F(", b = "));
  //   Serial.print(c2.c[2]);
  //   Serial.print(F(", w = "));
  //   Serial.println(c2.c[3]);
  //   Serial.println(F(""));
  //   //
  //   Serial.print(F("new: r = "));
  //   Serial.print(blendChars(c1.c[0], c2.c[0], ratio));
  //   Serial.print(F(", g = "));
  //   Serial.print(blendChars(c1.c[1], c2.c[1], ratio));
  //   Serial.print(F(", b = "));
  //   Serial.print(blendChars(c1.c[2], c2.c[2], ratio));
  //   Serial.print(F(", w = "));
  //   Serial.println(blendChars(c1.c[3], c2.c[3], ratio));
  // }
  return colorObj(blendChars(c1.c[0], c2.c[0], ratio), blendChars(c1.c[1], c2.c[1], ratio), blendChars(c1.c[2], c2.c[2], ratio), blendChars(c1.c[3], c2.c[3], ratio));
};

template <class T>
unsigned char Color_d<T>::blendChars(unsigned char c1, unsigned char c2, float ratio) const
{
  return (unsigned char)((float)c1 + ((float)c2 - (float)c1) * ratio);
};

// template <class T>
// unsigned char Color_d<T>::getNumColors() const
// {
//   return numColors;
// };

template <class T>
colorObj &Color_d<T>::getThisColorObj(unsigned char numInArray) const
{
  return cA[min(numInArray, numColors - 1)];
};

template <class T>
T Color_d<T>::getThisTrigger(unsigned char numInArray) const
{
  return tA[min(numInArray, numColors - 1)];
};

template <class T>
BLEND_TYPE &Color_d<T>::getThisBlendType(unsigned char numInArray) const
{
  return bA[min(numInArray, numColors - 1)];
};

template <class T>
colorObj *&Color_d<T>::getAllColorObjs() const
{
  return cA;
};

template <class T>
BLEND_TYPE *&Color_d<T>::getAllBlendTypes() const
{
  return bA;
};

template <class T>
T *Color_d<T>::getAllTriggers() const
{
  return tA;
};

template <class T>
void Color_d<T>::setThisTrigger(T tNew, unsigned char numInArray)
{
  if (numInArray < numColors)
  {
    tA[numInArray] = tNew;
  }

  sortAllArrays(); // We must resort the arrays, in case the new T value changes the order
}

template <class T>
void Color_d<T>::setThisColorObj(colorObj colorObjNew, unsigned char numInArray)
{
  if (numInArray < numColors)
  {
    cA[numInArray] = colorObjNew;
  }
}

template <class T>
void Color_d<T>::setThisBlendType(BLEND_TYPE blendTypeNew, unsigned char numInArray)
{
  if (numInArray < numColors)
  {
    bA[numInArray] = blendTypeNew;
  }
}

template <class T>
void Color_d<T>::setupArrays(unsigned char numColorsIn)
{
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.print(F("Starting array setup with "));
    Serial.print(numColorsIn);
    Serial.println(F(" colors."));
    delay(100);
  }
  colorObj *newColorArray = new colorObj[numColorsIn];     // Deleted at the end of this function
  T *newTriggerArray = new T[numColorsIn];                 // Deleted at the end of this function
  BLEND_TYPE *newBlendArray = new BLEND_TYPE[numColorsIn]; // Deleted at the end of this function
  unsigned char brightness = 50;

  // Determine how many of the old colors should be preserved
  unsigned char numOldColorsToPreserve = min(numColorsIn, numColors);

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Allocated arrays..."));
    Serial.print(F("NumColorIn: "));
    Serial.println(numColorsIn);
    Serial.print(F("NumColors: "));
    Serial.println(numColors);
    Serial.print(F("Preserved colors: "));
    Serial.println(numOldColorsToPreserve);
    delay(100);
  }
  //  int maxTrigger = 0;
  for (unsigned char i = 0; i < numOldColorsToPreserve; i++)
  {
    newColorArray[i] = cA[i];
    newTriggerArray[i] = tA[i];
    newBlendArray[i] = bA[i];

    //    maxTrigger = max(maxTrigger, tA[i]);
  }

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Filling in array..."));
    Serial.print(F("Blend type will be "));
    Serial.println(B_LINEAR);
    delay(100);
  }
  // Fill in the rest of the array with default colors
  T tDiff = 1000;
  for (unsigned char i = numOldColorsToPreserve; i < numColorsIn; i++)
  {
    // Set next trigger
    if (i == 0)
    {
      if (DEBUGGING_PATTERN)
      {
        // Serial.flush();
        Serial.println(F("i = 0"));
      }
      newTriggerArray[i] = 0;
    }
    else
    {
      newTriggerArray[i] = newTriggerArray[i - 1] + tDiff;
      if (DEBUGGING_PATTERN)
      {
        // Serial.flush();
        Serial.print(F("i = "));
        Serial.println(i);
        //        Serial.print(F("Last Trigger = "));
        //        Serial.println(newTriggerArray[i - 1]);
        //        Serial.print(F("tDiff = "));
        //        Serial.println(tDiff);
        //        Serial.print(F("newTriggerArray[i - 1] + tDiff = "));
        //        Serial.println(newTriggerArray[i]);
      }
    }

    // Set next blend type
    newBlendArray[i] = B_LINEAR;

    // Set next color
    if (i == 0)
    {
      newColorArray[i] = colorObj(); // Black
    }
    else if (i % 4 == 1)
    {
      newColorArray[i] = colorObj(0, 0, 0, brightness); // White
    }
    else if (i % 4 == 2)
    {
      newColorArray[i] = colorObj(brightness, 0, 0, 0); // Red
    }
    else if (i % 4 == 3)
    {
      newColorArray[i] = colorObj(0, brightness, 0, 0); // Green
    }
    else if (i % 4 == 0)
    {
      newColorArray[i] = colorObj(0, 0, brightness, 0); // Blue
    }
  }

  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Populated array..."));
    for (unsigned char i = 0; i < numColorsIn; i++)
    {
      // Serial.flush();
      Serial.print(F("Color "));
      Serial.print(i);
      Serial.print(F(": c = ("));
      Serial.print(newColorArray[i].c[0]);
      Serial.print(F(","));
      Serial.print(newColorArray[i].c[1]);
      Serial.print(F(","));
      Serial.print(newColorArray[i].c[2]);
      Serial.print(F(","));
      Serial.print(newColorArray[i].c[3]);
      Serial.print(F("), t = "));
      Serial.print(newTriggerArray[i]);
      Serial.print(F(", b = "));
      Serial.println(newBlendArray[i]);
      delay(100);
    }
  }

  setupArrays(newColorArray, newTriggerArray, newBlendArray, numColorsIn);

#if COLORD_COPY_ARRAYS
  // Delete newColorArray, newTriggerArray, and newBlendArray
  delete[] newColorArray;
  delete[] newTriggerArray;
  delete[] newBlendArray;
#endif

  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Finished setup:"));
    for (unsigned char i = 0; i < numColorsIn; i++)
    {
      // Serial.flush();
      Serial.print(F("Color "));
      Serial.print(i);
      Serial.print(F(": c = ("));
      Serial.print(cA[i].c[0]);
      Serial.print(F(","));
      Serial.print(cA[i].c[1]);
      Serial.print(F(","));
      Serial.print(cA[i].c[2]);
      Serial.print(F(","));
      Serial.print(cA[i].c[3]);
      Serial.print(F("), t = "));
      Serial.print(tA[i]);
      Serial.print(F(", b = "));
      Serial.println(bA[i]);
      delay(100);
    }
  }
};

template <class T>
void Color_d<T>::setupArrays(colorObj *cAIn, T *tAIn, BLEND_TYPE *bAIn, unsigned char numColorsIn)
{
  // This function will take ownership of any input arrays; THEY SHOULD NOT BE DELETED BY THE CALLING FUNCTION.

  deleteAllArrays(); // Always delete the old array when a new one is being created

#if COLORD_COPY_ARRAYS
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Copying arrays..."));
    Serial.print(F("Making arrays of length "));
    Serial.println(numColorsIn);
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println(F("Input data:"));
    delay(100);
    for (unsigned char i = 0; i < numColorsIn; i++)
    {
      Serial.print(F("Color "));
      Serial.print(i);
      Serial.print(F(": c = ("));
      Serial.print(cAIn[i].c[0]);
      Serial.print(F(","));
      Serial.print(cAIn[i].c[1]);
      Serial.print(F(","));
      Serial.print(cAIn[i].c[2]);
      Serial.print(F(","));
      Serial.print(cAIn[i].c[3]);
      Serial.print(F("), t = "));
      Serial.print(tAIn[i]);
      Serial.print(F(", b = "));
      Serial.println(bAIn[i]);
      delay(100);
    }
  }

  // Allocate arrays for all data
  // TODO: MEMORY
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.print(F("Making array colorObj of length "));
    Serial.println(numColorsIn);
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    //    delay(500);
  }
  cA = new colorObj[numColorsIn];
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.print(F("Making array T of length "));
    Serial.println(numColorsIn);
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    //    delay(500);
  }
  tA = new T[numColorsIn];
  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.print(F("Making array blend of length "));
    Serial.println(numColorsIn);
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    //    delay(500);
  }
  bA = new BLEND_TYPE[numColorsIn];

  for (unsigned char i = 0; i < numColorsIn; i++)
  {
    if (DEBUGGING_PATTERN)
    {
      Serial.print(F("Starting on color "));
      Serial.println(i);
      //      delay(300);

      //      Serial.print(F("Color Old"));
      //      Serial.print(i);
      //      Serial.print(F(": c = ("));
      //      Serial.print(cA[i].c[0]);
      //      Serial.print(F(","));
      //      Serial.print(cA[i].c[1]);
      //      Serial.print(F(","));
      //      Serial.print(cA[i].c[2]);
      //      Serial.print(F(","));
      //      Serial.print(cA[i].c[3]);
      //      Serial.print(F("), t = "));
      //      Serial.print(tA[i]);
      //      Serial.print(F(", b = "));
      //      Serial.println(bA[i]);
      //      delay(100);
    }
    cA[i] = cAIn[i];
    tA[i] = tAIn[i];
    bA[i] = bAIn[i];
    //    if (DEBUGGING_PATTERN) {
    //      Serial.print(F("Color New"));
    //      Serial.print(i);
    //      Serial.print(F(": c = ("));
    //      Serial.print(cA[i].c[0]);
    //      Serial.print(F(","));
    //      Serial.print(cA[i].c[1]);
    //      Serial.print(F(","));
    //      Serial.print(cA[i].c[2]);
    //      Serial.print(F(","));
    //      Serial.print(cA[i].c[3]);
    //      Serial.print(F("), t = "));
    //      Serial.print(tA[i]);
    //      Serial.print(F(", b = "));
    //      Serial.println(bA[i]);
    //      delay(100);
    //    }
  }

//  copyArray<colorObj>(cAIn, cA, numColorsIn);
//  copyArray<T>(tAIn, tA, numColorsIn);
//  copyArray<BLEND_TYPE>(bAIn, bA, numColorsIn);
//  *cA = *cAIn;
//  *tA = *tAIn;
//  *bA = *bAIn;
#else
  // Take ownership of the incoming arrays
  cA = cAIn;
  tA = tAIn;
  bA = bAIn;
  numColors = numColorsIn;
#endif

  sortAllArrays();
};

template <class T>
void Color_d<T>::sortAllArrays()
{
  // UNCHECKED
  // Sort all of the arrays according to the order of tA, maintaining the parallel-ness of the three arrays

  if (DEBUGGING_PATTERN)
  {
    // Serial.flush();
    Serial.println(F("Sorting arrays..."));
    delay(100);
  }
  // Use insertion sort
  for (unsigned char arrayLoc = 1; arrayLoc < numColors; arrayLoc++)
  {
    for (unsigned char compLoc = arrayLoc; compLoc > 0; compLoc--)
    {
      if (tA[compLoc] < tA[compLoc - 1])
      {
        // Swap these two indices in all arrays
        colorObj tmpA = cA[compLoc];
        BLEND_TYPE tmpB = bA[compLoc];
        T tmpT = tA[compLoc];

        cA[compLoc] = cA[compLoc - 1];
        bA[compLoc] = bA[compLoc - 1];
        tA[compLoc] = tA[compLoc - 1];

        cA[compLoc - 1] = tmpA;
        bA[compLoc - 1] = tmpB;
        tA[compLoc - 1] = tmpT;
      }
      else
      {
        // If these two are sorted, then move on
        break;
      }
    }
  }

  // If there is no tA = 0 at the beginning of this array, then create a one with black color and constant BLEND_TYPE
  if (tA[0] != 0)
  {
    // Add another color
    numColors++;

    // Create temporary arrays of a larger size
    colorObj *tmpC = new colorObj[numColors];     // Deleted at the end of this scope
    T *tmpT = new T[numColors];                   // Deleted at the end of this scope
    BLEND_TYPE *tmpB = new BLEND_TYPE[numColors]; // Deleted at the end of this scope

    // Set the temporary arrays' first value to the new black color
    tmpC[0] = colorObj();
    tmpT[0] = 0;
    tmpB[0] = B_CONSTANT;

    // Set the rest of the array elements according to the current arrays
    for (unsigned char i = 1; i < numColors; i++)
    {
      tmpC[i] = cA[i - 1];
      tmpT[i] = tA[i - 1];
      tmpB[i] = bA[i - 1];
    }

    // Reassign the arrays to the larger temporary arrays
    copyArray<colorObj>(tmpC, cA, numColors);
    copyArray<T>(tmpT, tA, numColors);
    copyArray<BLEND_TYPE>(tmpB, bA, numColors);
    //    *cA = *tmpC;
    //    *tA = *tmpT;
    //    *bA = *tmpB;

    // Delete tmpC, tmpT, and tmp
    delete[] tmpC;
    delete[] tmpT;
    delete[] tmpB;
  }
};

template <class T>
void Color_d<T>::deleteAllArrays()
{
  delete[] cA; // Delete the array of points that cA points to
  delete[] tA; // Delete the array of points that tA points to
  delete[] bA; // Delete the array of points that bA points to

  cA = NULL;
  tA = NULL;
  bA = NULL;
};

Color_dTime::Color_dTime() : Color_d<unsigned long>(){

                             };

Color_dTime::Color_dTime(unsigned char numColors) : Color_d<unsigned long>(numColors){

                                                    };

Color_dTime::Color_dTime(colorObj *cA, unsigned long *tA, BLEND_TYPE *bA, unsigned char numColors) : Color_d<unsigned long>(cA, tA, bA, numColors){

                                                                                                     };

// Color_dTime::Color_dTime(const Color_dTime &c) : Color_d<unsigned long>(c.getNumColors())
Color_dTime::Color_dTime(const Color_dTime &c) : Color_d<unsigned long>(c) {}

// {
//   if (DEBUGGING_PATTERN)
//   {
//     Serial.println(F("Copying Color_dTime..."));
//     Serial.print(F("Source color has "));
//     delay(200);
//     Serial.println(c.getNumColors());
//     delay(200);
//   }
//   colorObj *newColorArray = new colorObj[c.getNumColors()];
//   unsigned long *newTArray = new unsigned long[c.getNumColors()];
//   BLEND_TYPE *newBlendArray = new BLEND_TYPE[c.getNumColors()];

//   if (DEBUGGING_PATTERN)
//   {
//     Serial.println(F("Allocated arrays..."));
//     delay(200);
//   }

//   for (unsigned char i = 0; i < c.getNumColors(); i++)
//   {
//     newColorArray[i] = c.getThisColorObj(i);
//     newTArray[i] = c.getThisTrigger(i);
//     newBlendArray[i] = c.getThisBlendType(i);
//   }

//   //  newColorArray = c.getAllColorObjs();
//   //  newTArray = c.getAllTriggers();
//   //  newBlendArray = c.getAllBlendTypes();

//   setupArrays(newColorArray, newTArray, newBlendArray, c.getNumColors());

//   if (DEBUGGING_PATTERN)
//   {
//     for (unsigned char i = 0; i < c.getNumColors(); i++)
//     {
//       Serial.print(i);
//       Serial.print(F(" Old: t = "));
//       Serial.print(c.getThisTrigger(i));
//       Serial.print(F(", b = "));
//       Serial.print(c.getThisBlendType(i));
//       Serial.print(F(", c = ("));
//       Serial.print(c.getThisColorObj(i).c[0]);
//       Serial.print(F(","));
//       Serial.print(c.getThisColorObj(i).c[1]);
//       Serial.print(F(","));
//       Serial.print(c.getThisColorObj(i).c[2]);
//       Serial.print(F(","));
//       Serial.print(c.getThisColorObj(i).c[3]);
//       Serial.println(F(")"));
//       Serial.print(i);
//       Serial.print(F(" New: t = "));
//       Serial.print(tA[i]);
//       Serial.print(F(", b = "));
//       Serial.print(bA[i]);
//       Serial.print(F(", c = ("));
//       Serial.print(cA[i].c[0]);
//       Serial.print(F(","));
//       Serial.print(cA[i].c[1]);
//       Serial.print(F(","));
//       Serial.print(cA[i].c[2]);
//       Serial.print(F(","));
//       Serial.print(cA[i].c[3]);
//       Serial.println(F(")"));
//       Serial.println(F(""));
//     }
//   }

// #if COLORD_COPY_ARRAYS
//   // Delete the arrays that were just created
//    delete [] newColorArray;
//    delete [] newTArray;
//    delete [] newBlendArray;
// #endif

//   if (DEBUGGING_PATTERN)
//   {
//     Serial.println(F("Copied Color_dTime!!!"));
//     Serial.println(F(""));
//   }
// };

Color_dTime::~Color_dTime(){};

Color_dTime *Color_dTime::clone() const
{
  if (DEBUGGING_PATTERN)
  {
    Serial.println(F("Cloning Color_dTime:"));
    for (unsigned char i = 0; i < numColors; i++)
    {
      // Serial.flush();
      Serial.print(F("Color "));
      Serial.print(i);
      Serial.print(F(": c = ("));
      Serial.print(cA[i].c[0]);
      Serial.print(F(","));
      Serial.print(cA[i].c[1]);
      Serial.print(F(","));
      Serial.print(cA[i].c[2]);
      Serial.print(F(","));
      Serial.print(cA[i].c[3]);
      Serial.print(F("), t = "));
      Serial.print(tA[i]);
      Serial.print(F(", b = "));
      Serial.println(bA[i]);
      delay(100);
    }
    Serial.println(F(""));
  }
  return new Color_dTime(*this);
};

void Color_dTime::setThisTrigger(unsigned long tNew, unsigned char numInArray)
{
  Color_d<unsigned long>::setThisTrigger(tNew, numInArray);

  //  getMaxTrigger();
};

//void Color_dTime::setupArrays(unsigned char numColors) {
//  Color_d<unsigned long>::setupArrays(numColors);
//
//  //  getMaxTrigger();
//};

//void Color_dTime::setupArrays(colorObj* cA, unsigned long* tA, BLEND_TYPE* bA, unsigned char numColorsIn) {
//  Color_d<unsigned long>::setupArrays(cA, tA, bA, numColorsIn);
//
//  //  getMaxTrigger();
//};

//void Color_dTime::getMaxTrigger() {
//  maxTriggerInArray = tA[numColors - 1];
//  if (DEBUGGING_PATTERN) {
//      Serial.print(F("Max trigger: "));
//      Serial.println(maxTriggerInArray);
//  }
//};

unsigned long Color_dTime::getCurVal() const
{
  // Get the current time value, modulused with the current maxTrigger
  //  if (DEBUGGING_PATTERN) {
  //    Serial.print(F("Max trigger: "));
  //    Serial.println(tA[numColors - 1]);
  //  }
  //  return millis() % maxTriggerInArray;
  return millis() % tA[numColors - 1];
};

Color_dVel::Color_dVel(Speedometer *speedometer) : Color_d<float>(), speedometer(speedometer){};

Color_dVel::Color_dVel(Speedometer *speedometer, unsigned char numColors) : Color_d<float>(numColors), speedometer(speedometer){};

Color_dVel::Color_dVel(Speedometer *speedometer, colorObj *cA, float *tA, BLEND_TYPE *bA, unsigned char numColors) : Color_d<float>(cA, tA, bA, numColors), speedometer(speedometer){};

Color_dVel::Color_dVel(const Color_dVel &c) : Color_d<float>(c){};

Color_dVel::~Color_dVel(){};

Color_dVel *Color_dVel::clone() const
{
  return new Color_dVel(*this);
};

float Color_dVel::getCurVal() const
{
  return speedometer->getVel();
};
