/*

  The Speedometer keeps track of how quickly the wheel on the bicycle is moving by detecting signals from reed switches.
  It requires two interruptable pins on the Arduino, one for the reference reed switch to trigger, and one for all of the remaining reed switches to share.

  Samuel G. Brown
  July 13 2017

*/

#ifndef Speedometer_h
#define Speedometer_h

#include "Arduino.h"
#include "Definitions.h"
#include "Bluetooth.h"
#include <MatrixMath.h>
#include <math.h>

class Bluetooth;
class Kalman
{
public:
  Kalman();                                                                      // Constructor
  void mainLoop();                                                               // Main function that the Kalman class
  void addMeasurement(boolean isReference, unsigned long timeAtThisMeasurement); // Add a measurement to the filter

  // Getter functions for position, velocity, and acceleration
  float getPos(); // LED
  float getVel(); // LED/s
  float getAcc(); // LED/(s^2)

  void setNumLEDs(unsigned char numLEDs);
  unsigned char getNumLEDs();
  unsigned char getNumTicLEDs();

  void setPhi(float newPhi);    // Set a new value for Q
  void setP0(float *newP0); // Set a new P0 matrix (will not take a size value for the matrix, because we're just going to assume that it's using the size defined by num_states)
  void setR(float *newR);   // Set a new R matrix (will not take a size value for the matrix, because we're just going to assume that it's using the size defined by num_observed)

  void setP0Elem(unsigned char row, unsigned char col, float newElem); // Set a new value for an element in P0
  void setRElem(unsigned char row, unsigned char col, float newElem); // Set a new value for an element in R

  float getPhi();          // Get the value of Q
  const float **getP0(); // Get a pointer to the p0 matrix
  const float **getR();  // Get a pointer to the r matrix

  void resetFilter();     // Reset the filter to its zero state, the next time two measurements come in quick succession, the filter will initialize with that position/velocity
  bool getIsReset();         // Is the Kalman filter currently in a "reset" state (the wheel is moving too slow)
  bool debug_resetFromRef = false; // For the block extra standard tic function: should we reset the Kalman filter using only this and the previous reference tics?

private:

  // The current best guess at the LED position, velocity, and acceleration
  float xTrue = 0.0f;
  float velTrue = 0.0f;
  float accTrue = 0.0f;

  // Is the Kalman filter currently in a "reset" state (the wheel is moving too slow)
  boolean isReset = true;

  //  Create custom matrix function
  void ScalarAddF(float *A, float b, unsigned char numRows, unsigned char numCols, float *C); // Add scalar (float) b to matrix (float) A, and output to matrix C
  void Transpose(float *A, unsigned char numRows, unsigned char numCols, float *B);           // Transpose matrix A, and output to matrix C
  void Subtract(float *A, float *B, unsigned char numRows, unsigned char numCols, float *C);  // Subtract matrix B from matrix A, and output to matrix C = A-B

  // Declare variables needed for calculation (same naming as found on Wikipedia - Kalman Filter)
  float H[N_OBS][N_STA];  // H - matrix, size = observed x state (2x3)
  float Ht[N_STA][N_OBS]; //H(transpose) - matrix, size = state x observed (3 x 2)
  float R[N_OBS][N_OBS];  // R - matrix, size = observed x observed (2x2, [00001, 0;0 .1])
  float Q[N_STA][N_STA];                // Q - scalar (10000)
  float phi;              // Process white noise spectral density used to calculated Q)

  // If more space is needed, priori/posteriori can be combined
  float xPrior[N_STA];        // x Priori - vector, size = state x 1 (3x1)
  float xPost[N_STA];         // x Posteriori - vector, size = state x 1 (3x1)
  float PPrior[N_STA][N_STA]; // P Priori - matrix, size = state x state (3x3)
  float PPost[N_STA][N_STA];  // P Posteriori - matrix, size = state x state (3x3)

  unsigned long dt;      // dt - scalar (in milliseconds)
  float F[N_STA][N_STA]; // F - matrix, size = state x state (3x3)
  float z[N_OBS];        // Observations - vector, size = observed x 1 (2x1)
  float y[N_OBS];        // Observation residual - vector, size = observed x 1 (2x1)
  float S[N_OBS][N_OBS]; // S - matrix, size = observed x observed (2x2)
  float K[N_STA][N_OBS]; // Kalman Gain, matrix, size = state x observed (3x2)
  float I[N_STA][N_STA]; // Identity matrix, size = state x state (3x3)

  // Control parameters
  boolean newMeasurement = false;                                // Is there a new measurement that was added?
  unsigned long timeAtLastMeasurement = -1 * MAXTIMEBEWTEENTICS; // At what time was the last measurement recorded? (Initialize such that the first tic does not add a measurement)
  unsigned long timeAtLastRef = -1 * MAXTIMEBEWTEENTICS;       // At what time was the last reference recorded? (Initialize such that the first tic does not add a measurement)
  unsigned long maxTimeBetweenMeasurements = MAXTIMEBEWTEENTICS; // The maximum time in milliseconds between tics that will be used to calculate high speeds
  float nTicLEDs;                                                // The number of LEDs that pass between measurements/segments
  float nextMeasurePos;                                          // The location of the next measurement
  float nLEDs = NUMLEDS;                                         // The total number of LEDs on the wheel
  float xInitial;                                                // The initial value of the position before the current update occurs
  unsigned long timeAtLastPredict = 0;                           // The time in milliseconds that the last prediction step was performed
  boolean justStarted = true;                                    // Has the filter just started (is the current position ambiguous)?

  // Control functions
  void initializeFilter(unsigned long timeAtThisMeasurement, boolean isReference); // Initialize xPost with the position and velocity as most recently measured
};

class Speedometer
{
public:
  //    Speedometer(void (*ticISR)(), void (*rticISR)());
  Speedometer();

  void mainLoop(); // Main function that the speedometer executes every loop

  // static void tic();  // Interrupt function called when ticPin is set to HIGH
  // static void rTic(); // Interrupt function called when rticPin is set to HIGH

  // Get information from the underlying Kalman filter
  float getPos();     // Position
  float getVel();     // Velocity
  float getAcc();     // Acceleration
  boolean isSlow();   // Is the wheel moving too slow to get any reliable information?
  void resetFilter(); // Expose the Kalman filter's resetFilter() function publically

  void setNumLEDs(unsigned char numLEDs); // Set the number of LEDs for the Kalman filter

  unsigned char getNumLEDs(); // Get the number of LEDs from the Kalman filter

  Kalman *getKalman(); // Get a read-only pointer to the kalman object

  // For debugging:
  void setBluetooth(Bluetooth * newBTPointer);
  bool toggleDebugging(unsigned char debugCode); // For toggling a debug flag
  bool isDebugging(unsigned char debugCode); // For checking if a certain debug flag is set

private:
  unsigned char numSwitches = NUMSWITCHES; // Number of reed switches that are on the bike wheel (including the reference switch)
  // int ticPin = TICKPIN;                    // Pin that the reed switches sets HIGH, stored as a PHYSICAL PIN, not an interupt pin
  // int rticPin = RTICKPIN;                  // Pin that the reference reed switch sets HIGH, stored as a PHYSICAL PIN, not an interupt pin

  // Hardware dimensions
  static const unsigned int reedDetectionDiameter = REEDDETECTIONDIAMETER; // The "diameter" around the reed switch (in µm) that the switch can detect the magnet (IF MORE THAN 65mm, THEN CHANGE TO LONG)

  // The Kalman filter object held by the speedometer
  Kalman kalman; // Initialized by constructor

  //    // Tic detection
  //    static boolean newTic; // Is there a new reed switch signal?
  //    static boolean newReference; // Is there a new reference reed switch signal?

  // Tic pulse measurement
  boolean ticOn = false; // Is a reed switch currently activated?
  //    boolean referenceOn = false; // Is the reference reed switch currently activated?
  // unsigned long timeAtTicOn = 0;                // The time in microseconds that the last tic turned on (only updated while ticOn, used to estimate rotation at low speeds)
  unsigned int maxPulseLength = MAXPULSELENGTH; // The maximum length between on and off that will be accepted (waiting any longer than this between the reed switch's HIGH and LOW pulse will make the Arduino assume that the LOW pulse was simply missed)

  // Rotation and orientation estimation
  //    unsigned long timeAtLastTic = 0; // The time in milliseconds at the last tic (used to estimate rotation at high speeds)
  unsigned long timeAtThisTic = 0; // The time in milliseconds at the current tic (used to estimate rotation at high speeds, compared with timeAtLastTic)
  float currentLEDAdvance = 0;     // The number of LEDs that should currently be advanced per second
  //    unsigned long timeAtLastReference = 0; // The time in milliseconds at the last reference tic (used to estimate orientation at high speeds)

  // Speed and LED location variables
  float currentReferenceLED = 0; // The index of the reference (top-most LED when upright) location for the LEDs
  float currentTopLED = 0;       // The index of the top-most LED on the wheel (always shows the index of the LED at the top of the wheel over time)
  float currentWheelSpeed = 0;   // The speed of the wheel, measured in the number of LEDS that traverse a point on the wheel per second

  // For debugging
  // Debug flags
  Bluetooth *bt = NULL; // A pointer to the Bluetooth object, for debugging
  bool debug_tic_info = false; // Should the tic info be sent over Bluetooth?
  bool debug_flag_block_extra_ref = false;    // Should the extra reference tics be blocked?
  bool debug_flag_block_extra_tic = false;    // Should extra standard tics be blocked?
  bool debug_flag_adaptive_debounce = false;  // Should adaptive debouncing be used (allows aggressive debouncing at low speeds, but a higher maximum speed)?

  // Debug parameters
  bool debug_BER_readyForRef = true; // For the block extra reference tic function: are we ready for the next reference tic?
  unsigned char debug_BET_numConsecTics = 0; // For the block extra standard tic function: how many consecutive standard tics have we seen?
};

#endif
