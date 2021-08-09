#ifndef Definitions_h
#define Definitions_h

/*

  Track all of the definitions in this project, useful for defining the sizes and configurations for hardware and the like.

  Samuel G. Brown
  July 16 2017

*/
//
//// Code differences based on hardware
//
// SHOULD BE TRUE
#define BLUETOOTH_USE_HARDWARESERIAL true // Should the HardwareSerial connection on the Arduino be used for communicating with the Bluetooth connection (should be used when not debugging, if possible)
#define TIC_USE_LATCH true                 // Should the "tics" (position switches) be attached via an SR latch (alternative: they are attached via interrupts)

// FALSE UNTIL FURTHER NOTICE
#define SRAM_ATTACHED false                // Is SRAM attached to this unit?  If so, then certain memory intensive computations, such as blurring, can be done
#define USE_NANOPB false                   // Should we use the nanoPB library (currently unusable due to severe FRAM limitations, ~10KB over the 32KB limit)
#define COLORD_COPY_ARRAYS false           // Should the Color_d<T>::setupArrays() function copy input arrays (takes more memory), or take ownership of new arrays that come in (arrays CANNOT be deleted by calling function)


// Optional code differences based on software
// SHOULD BE TRUE
#define USE_LOWERROR_PPOST true     // Should PPost be calculated in such a way as to reduce rounding errors (will use more RAM during the calculation, should be 4*(3*3 + 2*(2*3)) = 84 bytes)
#define USE_THREE_STATE_KALMAN true // Should the Kalman filter track three states (position + velocity + acceleration) instead of two (- acceleration)?
#define USE_VEL_MEASUREMENT true    // Should the Kalman filter accept velocities as measurements from the switches, or only positions?

// Schematic of the latch:
//
// Reed Switch (through high-pass)-- S        Q ---- Tic    |
//                                    \      /              |
//                                     FlipFlop             |
//                                    /      \              |
// Reset (output from Arduino)------ R        ~Q ---        |

// Unit testing
#define UNITTEST_SPEEDOMETER false // Should only the speedometer be created and tested?
#define LIBRARY_TEST false         // Should a single script be run instead of the normal function (to test singular parts of the library)?
#define NO_BLUETOOTH false         // Try compiling without any Bluetooth code...just cuz

// Debugging
#define DEBUGGING_GENERAL false            // Breakdown every step that the software takes
#define DEBUGGING_PATTERN false            // Debug the pattern processing
#define DEBUGGING_MOVINGIMAGE false        // Debug the moving image functions
#define DEBUGGING_DYNAMICCOLOR false       // Debug the dynamic color main functions
#define DEBUGGING_SPINNER false             // Debug the spinner pattern
#define DEBUGGING_SPEEDOMETER false        // Debug the speedometer
#define DEBUGGING_KALMAN false             // Debug the kalman filter
#define DEBUGGING_Q false                  // Experiment with different Q's
#define DEBUGGING_TIC false                 // See when the Arduino sees a tic
#define DEBUGGING_MEASUREMENT false         // See what measurements the arduino recieves
#define DEBUGGING_SPEED false              // See what speed the arduino thinks the wheel is going
#define DEBUGGING_BLUETOOTH false          // Debug the bluetooth connection
#define DEBUGGING_PATTERN_SIZE false       // Debug the memory allocation of the pattern object
#define DEBUGGING_BLUETOOTH_LOWLEVEL false // Debug the bluetooth connection
#define DEBUGGING_BTDEBUG false             // Debug the Bluetooth debugging interface
// #define DEBUGGING_ANY !BLUETOOTH_USE_HARDWARESERIAL && (DEBUGGING_GENERAL || DEBUGGING_Q || DEBUGGING_DYNAMICCOLOR || DEBUGGING_TIC || DEBUGGING_MEASUREMENT || DEBUGGING_SPINNER || DEBUGGING_SPEED || DEBUGGING_PATTERN || DEBUGGING_SPEEDOMETER || DEBUGGING_KALMAN || UNITTEST_SPEEDOMETER || DEBUGGING_BLUETOOTH || LIBRARY_TEST)
#define DEBUGGING_ANY (DEBUGGING_GENERAL || DEBUGGING_Q || DEBUGGING_DYNAMICCOLOR || DEBUGGING_TIC || DEBUGGING_MEASUREMENT || DEBUGGING_SPINNER || DEBUGGING_SPEED || DEBUGGING_PATTERN || DEBUGGING_SPEEDOMETER || DEBUGGING_KALMAN || UNITTEST_SPEEDOMETER || DEBUGGING_BLUETOOTH || DEBUGGING_BTDEBUG || LIBRARY_TEST)

// These are the only two interrupt pins that can be used for an external interrupt request
// Process can be sped up by using pin change interrupt and attaching interrupt by hand, using pins from different batches: https://arduino.stackexchange.com/questions/1784/how-many-interrupt-pins-can-an-uno-handle
#define TICKPIN 2
#define RTICKPIN 3
#define RESETTICKPIN 4 // Used with SR latch - common reset signal to both latches
#define POWERPIN 5
#define BLUETOOTHPIN_TX_TO_BT_RX 9
#define BLUETOOTHPIN_RX_TO_BT_TX 8
#define NUMSWITCHES 4

#define NUMLEDS 96                     // Must be an even number!!!
#define NUM_BYTES_PER_IMAGE NUMLEDS / 2 // The number of chars needed to store one image (each led will be stored in one nibble, i.e. half of a byte)
#define NUMLIGHTSPERLED 4               // Total number of lights per LED (4 = RGBW, 3 = RGB)
#define REEDDETECTIONDIAMETER 1
#define MAX_BT_BUFFER_SIZE 32               // The number of bytes available to read from the Serial buffer
#define BLUETOOTH_COMPLETION_SIGNAL_SIZE 10 // The size in bytes of the completion signal to be sent to Android to let it know that we're done transmitting
#define BLUETOOTH_TIMEOUT_MILLI 5000        // The timeout period in which we will wait for a bluetooth message that we requested
#define BLUETOOTH_BAUD 9600                 // The baud rate for communicating to the bluetooth module

#define MAXPULSELENGTH 50000
#define MAXTIMEBEWTEENTICS 1500

#define TOTAL_MEMORY 2048

// For debugging
// TO USE BLUETOOTH DEBUGGING: Over a bluetooth terminal interface, send "dN", where N is the number of the debug code you would like to use (below)
#define DEBUG_BUF_SIZE 24
#define DEBUG_MAX_DIGITS 3 // Max of 999 debug codes (sue me)
#define ASCII_SPACE_32 32 // The ASCI value of " "
#define ASCII_ZERO_48 48 // The ASCI value of "0"
#define DEBUG_NONE 255 // To display the help screen
#define DEBUG_HELP_0 0 // To display the help screen
#define DEBUG_TIC_INFO_1 1 // To get notified when a tic occurs (and of what type)
#define DEBUG_BLOCK_EXTRA_REF_2 2 // To test the block extra reference tics method!
#define DEBUG_RENAME_BLUETOOTH_3 3 // To rename the Bluetooth device (Note: NOT POSSIBLE WITH CURRENT CIRCUIT (key pin is not connected))
#define DEBUG_BLOCK_EXTRA_TIC_4 4 // To test the block extra reference tics method!
#define DEBUG_DEBUG_9 9 // To debug the debugging (BWAAAAAAA)

// For Kalman filtering
#if USE_THREE_STATE_KALMAN
#define N_STA 3 // Tracking posiiton, velocity, and acceleration
#else
#define N_STA 2 // Tracking posiiton and velocity
#endif
#if USE_VEL_MEASUREMENT
#define N_OBS 2 // Measuring position and velocity (acceleration introduces too much error)
#else
#define N_OBS 1 // Measuring position
#endif

// For bit banging LED control signal
#define T0H 300 // Width of a 0 bit in ns
#define T1H 800 // Width of a 1 bit in ns
#define T0L 700 // Width of a 0 bit in ns
#define T1L 500 // Width of a 1 bit in ns

#define RES 800000 // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays
#define NS_PER_SEC (1000000000L) // Note that this has to be SIGNED since we want to be able to check for negative values of derivatives
#define CYCLES_PER_SEC (F_CPU)
#define NS_PER_CYCLE (NS_PER_SEC / CYCLES_PER_SEC)
#define NS_TO_CYCLES(n) ((n) / NS_PER_CYCLE)

#define PIXEL_PORT PORTD // Port of the pin the pixels are connected to
#define PIXEL_DDR DDRD   // Port of the pin the pixels are connected to
#define PIXEL_BIT 6      // Bit of the pin the pixels are connected to
// DENOTE PIN 6 ON THE ARDUINO NANO

// Set up some class declarations that are needed before they're defined
class Color_;
class Pattern_Handler;
class colorObj;
class Bluetooth;
class Speedometer;

// Define enums used for different animations
// enum MAIN_ANIM
// {
//   M_WREL_STILL, // No rotation relative to the wheel
//   M_WREL_MOVING, // Constant relative to the wheel
//   M_GREL_STILL,      // No rotation relative to the ground
//   M_GREL_MOVING,     // Constant rotation relative to the ground
//   M_SPINNER,    // "Spinner" behavior
//   M_NOTHING     // No animation (remove?)
// };

// enum IDLE_ANIM
// {
//   I_STATIONARY, // No rotation relative to the wheel
//   I_STILL,      // No rotation relative to the ground
//   I_MOVING,     // Constant rotation relative to the ground
//   I_NOTHING     // No animation (remove?)
// };

enum IMAGE_REFPT
{
  WREL,   // Rotation relative to the wheel
  GREL,   // Rotation relative to the ground
  NOTHING // No animation (remove?)
};

enum IMAGE_HELPER_TYPE
{
  STATIC, // The image does not move relative to its reference point
  MOVING, // The image does move relative to its reference point
  SPINNER // The image behaves like a "Spinner"
};

enum BLEND_TYPE
{
  B_LINEAR,
  B_CONSTANT
};

enum COLOR_TYPE
{
  COLOR_STATIC,
  COLOR_DTIME,
  COLOR_DVEL
};

// Define nibble operations (to deal with the image field, which has two image locations stored in each byte)
#define FIRST_NIBBLE_MASK 15   // Mask to get only the first nibble (lowest significance bits, 15 == 0b00001111)
#define SECOND_NIBBLE_MASK 240 // Mask to get only the second nibble (highest significance bits, 240 == 0b11110000)

unsigned char valFromFirstNibble(unsigned char fullByte);

unsigned char valFromSecondNibble(unsigned char fullByte);

unsigned char valToFirstNibble(unsigned char valToSet, unsigned char fullByte);

unsigned char valToSecondNibble(unsigned char valToSet, unsigned char fullByte);

void serialPrintColor(unsigned char color[NUMLIGHTSPERLED]);

void printByte(unsigned char byteToPrint);

void printColorBytes(unsigned char *colorBytes);

void printColorObj(colorObj color);

int posMod(int k, int n);

int freeRam();

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
void copyArray(const T *arraySrc, T *arrayDest, const int arraySize)
{
  memcpy(arrayDest, arraySrc, arraySize * sizeof(T));
};

template <typename T>
void copyArray2D(T **arraySrc, T **arrayDest, int arraySize1, int arraySize2)
{
  for (int i = 0; i < arraySize1; i++)
  {
    memcpy(arrayDest[i], arraySrc[i], arraySize2 * sizeof(T));
  }
};

template <typename T>
T maxInArray(T *arrayIn, int arraySize)
{
  //  if (DEBUGGING_GENERAL) {
  //    Serial.print(F("Finding Max Val of array of size "));
  //    Serial.println(arraySize);
  //  }
  T maxVal = 0;
  for (int i = 0; i < arraySize; i++)
  {
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

template <typename T>
T maxInArrayPerNibble(T *arrayIn, int arraySize)
{
  T maxVal = 0;
  for (unsigned char i = 0; i < arraySize; i++)
  {
    maxVal = max(maxVal, valFromFirstNibble(arrayIn[i]));
    maxVal = max(maxVal, valFromSecondNibble(arrayIn[i]));
  }

  return maxVal;
}

#if USE_NANO_PB
#include "bluetooth.pb.h"

// Define some values to make using the protocol buffer much less verbose
#define Message_BT bluetooth_BluetoothMessage
#define BWA_BT bluetooth_BluetoothMessage_BikeWheelAnim
#define ImageMeta_BT bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta
#define MessageLength_BT bluetooth_BluetoothLength
#define Color_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color_
#define ColorObj_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj
#define Kalman_BT bluetooth_BluetoothMessage_Kalman
#define Storage_BT bluetooth_BluetoothMessage_Storage

#define Message_BT_Fields bluetooth_BluetoothMessage_fields

#define Message_BT_default bluetooth_BluetoothMessage_init_default
#define BWA_BT_default bluetooth_BluetoothMessage_BikeWheelAnim_init_default
#define ImageMeta_BT_default bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_init_default
#define ImageMetaParam_BT_default bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageMetaParameter_init_default
#define MessageLength_BT_default bluetooth_BluetoothLength_init_default
#define Color_BT_default bluetooth_BluetoothMessage_BikeWheelAnim_Color__init_default
#define ColorObj_BT_default bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_init_default
#define Kalman_BT_default bluetooth_BluetoothMessage_Kalman_init_default
#define Storage_BT_default bluetooth_BluetoothMessage_Storage_init_default

#define MessageType_BT bluetooth_BluetoothMessage_MessageType
#define ColorType_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorType
#define BlendType_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_BlendType
#define ImageType_BT bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageType
#define ImageMetaParam_BT bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageMetaParameter

#define MessageType_BWA bluetooth_BluetoothMessage_MessageType_BIKE_WHEEL_ANIM
#define MessageType_Kalman bluetooth_BluetoothMessage_MessageType_KALMAN
#define MessageType_Brightness bluetooth_BluetoothMessage_MessageType_BRIGHTNESS
#define MessageType_Storage bluetooth_BluetoothMessage_MessageType_STORAGE
#define MessageType_Battery bluetooth_BluetoothMessage_MessageType_BATTERY

#define ColorType_STATIC_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorType_STATIC
#define ColorType_DTIME_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorType_D_TIME
#define ColorType_DVEL_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorType_D_VEL

#define BlendType_CONSTANT_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_BlendType_CONSTANT
#define BlendType_LINEAR_BT bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_BlendType_LINEAR

#define ImageType_CONSTANT_BT bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageType_CONST_ROT
#define ImageType_SPINNER_BT bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageType_SPINNER

#define BWA_BT_Tag bluetooth_BluetoothMessage_bike_wheel_anim_tag
#define Kalman_BT_Tag bluetooth_BluetoothMessage_kalman_tag
#define Brightness_BT_Tag bluetooth_BluetoothMessage_brightness_scale_tag
#define Storage_BT_Tag bluetooth_BluetoothMessage_storage_tag
#define Battery_BT_Tag bluetooth_BluetoothMessage_battery_tag
#endif

#endif
