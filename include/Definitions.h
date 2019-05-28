#ifndef Definitions_h
#define Definitions_h

/*

  Track all of the definitions in this project, useful for defining the sizes and configurations for hardware and the like.

  Samuel G. Brown
  July 16 2017

*/

// Unit testing
#define UNITTEST_SPEEDOMETER false // Should only the speedometer be created and tested?
#define LIBRARY_TEST false // Should a single script be run instead of the normal function (to test singular parts of the library)?

// Debugging
#define DEBUGGING_GENERAL true // Breakdown every step that the software takes
#define DEBUGGING_PATTERN true // Debug the pattern processing
#define DEBUGGING_MOVINGIMAGE false // Debug the moving image functions
#define DEBUGGING_DYNAMICCOLOR true // Debug the dynamic color main functions
#define DEBUGGING_SPEEDOMETER false // Debug the speedometer
#define DEBUGGING_KALMAN false // Debug the kalman filter
#define DEBUGGING_Q false // Experiment with different Q's
#define DEBUGGING_TIC true // See when the Arduino sees a tic
#define DEBUGGING_SPEED false // See what speed the arduino thinks the wheel is going
#define DEBUGGING_ANY (DEBUGGING_GENERAL || DEBUGGING_Q || DEBUGGING_TIC || DEBUGGING_SPEED || DEBUGGING_PATTERN || DEBUGGING_SPEEDOMETER || DEBUGGING_KALMAN || UNITTEST_SPEEDOMETER || LIBRARY_TEST)


// These are the only two interrupt pins that can be used for an external interrupt request
// Process can be sped up by using pin change interrupt and attaching interrupt by hand, using pins from different batches: https://arduino.stackexchange.com/questions/1784/how-many-interrupt-pins-can-an-uno-handle
#define TICKPIN 2
#define RTICKPIN 3
#define POWERPIN 5
#define NUMSWITCHES 3

#define NUMLEDS 120
#define NUMLIGHTSPERLED 4 // Total number of lights per LED (4 = RGBW, 3 = RGB)
#define REEDDETECTIONDIAMETER 1

#define MAXPULSELENGTH 50000
#define MAXTIMEBEWTEENTICS 1500

// For Kalman filtering
#define N_STA 3 // Tracking posiiton, velocity, and acceleration
#define N_OBS 2 // Measuring position and velocity (accerleration introduces too much error

// For bit banging LED control signal
#define T0H  300    // Width of a 0 bit in ns
#define T1H  800    // Width of a 1 bit in ns
#define T0L  700    // Width of a 0 bit in ns
#define T1L  500    // Width of a 1 bit in ns

#define RES 800000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays
#define NS_PER_SEC (1000000000L)          // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE ( NS_PER_SEC / CYCLES_PER_SEC )
#define NS_TO_CYCLES(n) ( (n) / NS_PER_CYCLE )

#define PIXEL_PORT  PORTD  // Port of the pin the pixels are connected to
#define PIXEL_DDR   DDRD   // Port of the pin the pixels are connected to
#define PIXEL_BIT   6      // Bit of the pin the pixels are connected to
// DENOTE PIN 6 ON THE ARDUINO NANO

// Define enums used for different animations
enum MAIN_ANIM {
  M_STATIONARY,
  M_STILL,
  M_MOVING,
  M_SPINNER,
  M_NOTHING
};

enum IDLE_ANIM {
  I_STATIONARY,
  I_STILL,
  I_MOVING,
  I_NOTHING
};

enum BLEND_TYPE {
  B_LINEAR,
  B_CONSTANT
};

enum COLOR_TYPE {
  COLOR_STATIC,
  COLOR_DTIME,
  COLOR_DVEL
};

// Common array functions
//template <typename T>
//void copyArray(const T *arraySrc, T *arrayDest, const int arraySize) {
//  for (int i = 0; i < arraySize; i++) {
//    arrayDest[i] = arraySrc[i];
//  }
//};

//template <typename T>
//void copyArray2D(T **arraySrc, T **arrayDest, int arraySize1, int arraySize2) {
//  for (int i = 0; i < arraySize1; i++) {
////    copyArray<T>(arraySrc[i], arrayDest[i], arraySize2);
//        for (int j = 0; j < arraySize2; j++) {
//          arrayDest[i][j] = arraySrc[i][j];
//        }
//  }
//};

template <typename T>
void copyArray(const T *arraySrc, T *arrayDest, const int arraySize) {
  memcpy(arrayDest, arraySrc, arraySize * sizeof(T));
};

template <typename T>
void copyArray2D(T **arraySrc, T **arrayDest, int arraySize1, int arraySize2) {
  for (int i = 0; i < arraySize1; i++) {
    memcpy(arrayDest[i], arraySrc[i], arraySize2 * sizeof(T));
  }
};

template <typename T>
T maxInArray(T *arrayIn, int arraySize) {
  //  if (DEBUGGING_GENERAL) {
  //    Serial.print(F("Finding Max Val of array of size "));
  //    Serial.println(arraySize);
  //  }
  T maxVal = 0;
  for (int i = 0; i < arraySize; i++) {
    maxVal = max(maxVal, arrayIn[i]);
    //    if (DEBUGGING_GENERAL) {
    //      Serial.println(arrayIn[i]);
    //    }
  }
  //    if (DEBUGGING_GENERAL) {
  //      Serial.print(F("Max Val is "));
  //      Serial.println(maxVal);
  //    }

  //  if (DEBUGGING_GENERAL) {
  //    Serial.println("Found Max Val...");
  //  }

  return maxVal;
};

int freeRam();
#endif
