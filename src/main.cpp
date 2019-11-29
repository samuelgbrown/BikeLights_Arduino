#include "Speedometer.h"
#include "Pattern.h"
#include "Definitions.h"
#if !NO_BLUETOOTH
#include "Bluetooth.h"
#endif

//void tickISR() {
//  // Global scope wrapper function for the Speedometer's member function (Just a member function to keep everything nicely grouped)
//  Speedometer::tic();
//}
//
//void rTickISR() {
//  // Global scope wrapper function for the Speedometer's member function (Just a member function to keep everything nicely grouped)
//  Speedometer::rTic();
//}

//Speedometer speedometer = Speedometer(tickISR, rTickISR);
int outOfScopeRam = freeRam();
Speedometer *speedometer = NULL;
Pattern_Handler *pattern_handler = NULL; // Make a generic pattern object
#if !NO_BLUETOOTH
Bluetooth *bluetooth = NULL;
#endif

// TODO: DEBUGGING
Pattern *idlePat = NULL; // Used for debugging

void setup()
{
  // Set up power
  pinMode(POWERPIN, OUTPUT);
  digitalWrite(POWERPIN, HIGH); // Enable the VRAE-10E1A0 connected to POWERPIN

  // Set up debugging
  if (DEBUGGING_ANY)
  {
    Serial.begin(9600);
    Serial.flush();
    delay(100);
  }

  if (DEBUGGING_ANY)
  {
    //    Serial.flush();
    Serial.println();
    Serial.println();
    Serial.println(F("***"));
    Serial.println(F("*********"));
    Serial.println(F("Starting setup..."));
    Serial.println(F("*********"));
    Serial.println(F("***"));
    delay(100);
    Serial.print(F("Initial memory: "));
    Serial.println(outOfScopeRam);
    //    randomSeed(analogRead(0));
    //    Serial.println(random(1000));
  }

  if (!LIBRARY_TEST)
  {

    // float newQ;

    if (DEBUGGING_GENERAL)
    {
      //    Serial.flush();
      Serial.println();
      Serial.println(F("Making Speedometer..."));
      Serial.println(freeRam());
    }

    // Set up Speedometer
    speedometer = new Speedometer();
    // if (DEBUGGING_Q)
    // {
    //   speedometer->getKalman()->setQ(newQ);
    // }

    if (!UNITTEST_SPEEDOMETER)
    {
      if (DEBUGGING_GENERAL)
      {
        //    Serial.flush();
        Serial.println(F("Making Pattern Handler..."));
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
      }

      // Set up a Pattern Handler
      pattern_handler = new Pattern_Handler(speedometer); // Make a generic pattern object

      if (DEBUGGING_GENERAL)
      {
        //    Serial.flush();
        Serial.println(F("Made Pattern Handler..."));
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
      }

#if !NO_BLUETOOTH
      if (DEBUGGING_GENERAL)
      {
        //    Serial.flush();
        Serial.println(F("Making Bluetooth..."));
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
      }

      // Set up a Bluetooth object
      bluetooth = new Bluetooth(pattern_handler, speedometer);

      if (DEBUGGING_GENERAL)
      {
        //    Serial.flush();
        Serial.println(F("Made Bluetooth..."));
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
      }
#endif
    }
  }
  else
  {
    //
    //// Library test script
    //

    for (int i = 0; i < 1000; i++)
    {
      if (DEBUGGING_ANY)
      {
        Serial.flush();
        Serial.print(F("Making idle pattern "));
        Serial.println(i);
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
      }

      // Create a still idle image
      // idlePat = new Still_Image_Idle();

      if (DEBUGGING_ANY)
      {
        Serial.flush();
        Serial.print(F("Made idle pattern "));
        Serial.println(i);
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
        Serial.println(F("Deleting idle pattern..."));
        Serial.println();
      }

      delete idlePat;

      if (DEBUGGING_ANY)
      {
        Serial.flush();
        Serial.print(F("Deleted idle pattern "));
        Serial.println(i);
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
      }
    }
  }

  if (DEBUGGING_ANY)
  {
    //    Serial.flush();
    Serial.println(F("Set finished."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    Serial.println();
    Serial.println();
    delay(100);
    //    delay(1000);
  }
};

void loop()
{
  if (!LIBRARY_TEST)
  {
    if (DEBUGGING_GENERAL)
    {
      Serial.println(F("Speedometer loop..."));
      //      Serial.print(F("nLEDs is "));
      //      Serial.println(speedometer->kalman.checkNumLEDs());
      //    delay(500);
    }
    //  while (1) {};
    // Execute the Speedometer's main loop (determine the current orientation of the wheel)
    speedometer->mainLoop();

    if (!UNITTEST_SPEEDOMETER)
    {
      if (DEBUGGING_GENERAL)
      {
        Serial.println(F("Pattern loop..."));
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
        //    delay(500);
      }
      // Execute the Pattern' main loop
      pattern_handler->mainLoop();

#if !NO_BLUETOOTH
      if (DEBUGGING_GENERAL)
      {
        Serial.println(F("Bluetooth Loop..."));
        Serial.print(F("Current memory: "));
        Serial.println(freeRam());
        Serial.println();
        //    delay(500);
      }
      // Execute the Bluetooth main loop
      bluetooth->mainLoop();
#endif
    }

    if (DEBUGGING_GENERAL)
    {
      Serial.println();
    }
  }
  else
  {
    //
    //// Library test loop
    //

    //    idlePat->anim();
    //    controller::show_LEDs();
  }
};

// float getNewQ()
// {
//   //  Serial.flush();
//   Serial.println(F("What is Q?"));
//   String inString = "";
//   float finalVal = 0;
//   while (finalVal <= 0)
//   {
//     //    while (Serial.available() == 0) {}
//     while (Serial.available() > 0)
//     {
//       int inChar = Serial.read();
//       if (inChar != '\n')
//       {
//         inString += (char)inChar;
//         //        Serial.println(inString);
//       }
//       else
//       {
//         finalVal = inString.toFloat();
//         inString = "";
//         if (finalVal <= 0)
//         {
//           //          Serial.flush();
//           Serial.print(F("Please enter valid positive value for Q (Q = "));
//           //          Serial.flush();
//           Serial.print(finalVal);
//           //          Serial.flush();
//           Serial.println(").");
//         }
//         break;
//       }
//     }
//   }

//   //  Serial.flush();
//   Serial.print(F("Q = "));
//   //  Serial.flush();
//   Serial.println(finalVal);
//   return finalVal;
// };

int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
};

// Define nibble operations (to deal with the image field, which has two image locations stored in each byte)

unsigned char valFromFirstNibble(unsigned char fullByte)
{
  // Get an int representation of the first nibble

  // Mask out to get only the first nibble (lowest significance half), and return the resulting value as an unsigned char
  return (unsigned char)(FIRST_NIBBLE_MASK & fullByte);
}

unsigned char valFromSecondNibble(unsigned char fullByte)
{
  // Get an int representation of the second nibble

  // Bit-shift the second nibble (highest significance half) into the lowest significance half, leaving 0's in the high significance place
  return (unsigned char)fullByte >> 4;
}

void valToFirstNibble(unsigned char valToSet, unsigned char &fullByte)
{
  // Set a value to the first nibble in fullByte
  // (Assuming that valToSet <= 16, without checks)

  // Mask out to get only the second nibble, then set the first nibble to valToSet
  fullByte = (unsigned char)((SECOND_NIBBLE_MASK & fullByte) | valToSet);
}

void valToSecondNibble(unsigned char valToSet, unsigned char &fullByte)
{
  // Set a value to the second nibble in fullByte
  // (Assuming that valToSet <= 16, without checks)

  // Mask out to get only the first nibble, then set the second nibble to valToSet (by bit-wise or-ing with a bit-shifted valToSet)
  fullByte = (unsigned char)((FIRST_NIBBLE_MASK & fullByte) | valToSet << 4);
}

void serialPrintColor(unsigned char color[NUMLIGHTSPERLED])
{
    Serial.print(F("[ "));
    for (int i = 0; i < NUMLIGHTSPERLED; i++)
    {
        Serial.print(color[i]);
        Serial.print(F(" "));
    }
    Serial.print(F("]"));
}

void printByte(unsigned char byteToPrint)
{
  Serial.print(F("[ "));

  for (int i = 7; i >= 0; i--)
  {
    Serial.print((byteToPrint >> i) & 1);
    Serial.print(" ");
  }

  Serial.println(F("]"));
}