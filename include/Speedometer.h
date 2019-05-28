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
#include <MatrixMath.h>
#include <math.h>

class Kalman
{
  public:
    Kalman(); // Constructor
    void mainLoop(); // Main function that the Kalman class
    void addMeasurement(boolean isReference, unsigned long timeAtThisMeasurement); // Add a measurement to the filter
    float checkNumLEDs() {
      return nLEDs;
    }; // REMOVE AFTER TESTING
    void setQ(float newQ); // Set a new value for Q

    // The current best guess at the LED position, velocity, and acceleration
    float xTrue;
    float velTrue;
    float accTrue;

    boolean isReset = true; // Is the Kalman filter currently in a "reset" state (the wheel is moving too slow)

  private:

    //  Create custom matrix function
    void ScalarAddF(float* A, float b, unsigned char numRows, unsigned char numCols, float* C); // Add scalar (float) b to matrix (float) A, and output to matrix C
    void Transpose(float* A, unsigned char numRows, unsigned char numCols, float* B); // Transpose matrix A, and output to matrix C
    void Subtract(float* A, float* B, unsigned char numRows, unsigned char numCols, float* C); // Subtract matrix B from matrix A, and output to matrix C = A-B

    // Declare variables needed for calculation (same naming as found on Wikipedia - Kalman Filter)
    float H[N_OBS][N_STA]; // H - matrix, size = observed x state (2x3)
    float Ht[N_STA][N_OBS]; //H(transpose) - matrix, size = state x observed (3 x 2)
    float R[N_OBS][N_OBS]; // R - matrix, size = observed x observed (2x2, [00001, 0;0 .1])
    float Q; // Q - scalar (10000)

    // If more space is needed, priori/posteriori can be combined
    float xPost[N_STA]; // x Posteriori - vector, size = state x 1 (3x1)
    float xPrior[N_STA]; // x Priori - vector, size = state x 1 (3x1)
    float PPost[N_STA][N_STA]; // P Posteriori - matrix, size = state x state (3x3)
    float PPrior[N_STA][N_STA]; // P Priori - matrix, size = state x state (3x3)

    unsigned long dt; // dt - scalar (in milliseconds)
    float F[N_STA][N_STA]; // F - matrix, size = state x state (3x3)
    float z[N_OBS]; // Observations - vector, size = observed x 1 (2x1)
    float y[N_OBS]; // Observation residual - vector, size = observed x 1 (2x1)
    float S[N_OBS][N_OBS]; // S - matrix, size = observed x observed (2x2)
    float K[N_STA][N_OBS]; // Kalman Gain, matrix, size = state x observed (3x2)
    float I[N_STA][N_STA]; // Identity matrix, size = state x state (3x3)

    // Control parameters
    boolean newMeasurement = false; // Is there a new measurement that was added?
    unsigned long timeAtLastMeasurement = -1 * MAXTIMEBEWTEENTICS; // At what time was the last measurement recorded? (Initialize such that the first tic does not add a measurement)
    unsigned long maxTimeBetweenMeasurements = MAXTIMEBEWTEENTICS; // The maximum time in milliseconds between tics that will be used to calculate high speeds
    float nTicLEDs; // The number of LEDs that pass between measurements/segments
    float nextMeasurePos; // The location of the next measurement
    float nLEDs = NUMLEDS; // The total number of LEDs on the wheel
    float xInitial; // The initial value of the position before the current update occurs
    unsigned long timeAtLastPredict = 0; // The time in milliseconds that the last prediction step was performed
    boolean justStarted = true; // Has the filter just started (is the current position ambiguous)?

    // Control functions
    void resetFilter(); // Reset the filter to its zero state, the next time two measurements come in quick succession, the filter will initialize with that position/velocity
    void initializeFilter(unsigned long dt, boolean isReference); // Initialize xPost with the position and velocity as most recently measured
};

class Speedometer
{
  public:
    //    Speedometer(void (*ticISR)(), void (*rticISR)());
    Speedometer();

    void mainLoop(); // Main function that the speedometer executes every loop

    void setQ(float newQ); // Set a new value for the Kalman filter's Q

    static void tic(); // Interrupt function called when ticPin is set to HIGH
    static void rTic(); // Interrupt function called when rticPin is set to HIGH

    // Get information from the underlying Kalman filter
    float getPos(); // Position
    float getVel(); // Velocity
    float getAcc(); // Acceleration
    boolean isSlow(); // Is the wheel moving too slow to get any reliable information?

  private:
    unsigned char numSwitches = NUMSWITCHES; // Number of reed switches that are on the bike wheel (including the reference switch)
    int ticPin = TICKPIN; // Pin that the reed switches sets HIGH, stored as a PHYSICAL PIN, not an interupt pin
    int rticPin = RTICKPIN; // Pin that the reference reed switch sets HIGH, stored as a PHYSICAL PIN, not an interupt pin

    // Hardware dimensions
    static const unsigned int reedDetectionDiameter = REEDDETECTIONDIAMETER; // The "diameter" around the reed switch (in µm) that the switch can detect the magnet (IF MORE THAN 65mm, THEN CHANGE TO LONG)

    // The Kalman filter object held by the speedometer
    Kalman kalman; // RETURN TO BEING PRIVATE AFTER TESTING

    //    // Tic detection
    //    static boolean newTic; // Is there a new reed switch signal?
    //    static boolean newReference; // Is there a new reference reed switch signal?

    // Tic pulse measurement
    boolean ticOn = false; // Is a reed switch currently activated?
    //    boolean referenceOn = false; // Is the reference reed switch currently activated?
    unsigned long timeAtTicOn = 0; // The time in microseconds that the last tic turned on (only updated while ticOn, used to estimate rotation at low speeds)
    unsigned int maxPulseLength = MAXPULSELENGTH; // The maximum length between on and off that will be accepted (waiting any longer than this between the reed switch's HIGH and LOW pulse will make the Arduino assume that the LOW pulse was simply missed)

    // Rotation and orientation estimation
    //    unsigned long timeAtLastTic = 0; // The time in milliseconds at the last tic (used to estimate rotation at high speeds)
    unsigned long timeAtThisTic = 0; // The time in milliseconds at the current tic (used to estimate rotation at high speeds, compared with timeAtLastTic)
    float currentLEDAdvance = 0; // The number of LEDs that should currently be advanced per second
    //    unsigned long timeAtLastReference = 0; // The time in milliseconds at the last reference tic (used to estimate orientation at high speeds)

    // Speed and LED location variables
    float currentReferenceLED = 0; // The index of the reference (top-most LED when upright) location for the LEDs
    float currentTopLED = 0; // The index of the top-most LED on the wheel (always shows the index of the LED at the top of the wheel over time)
    float currentWheelSpeed = 0; // The speed of the wheel, measured in the number of LEDS that traverse a point on the wheel per second
};

#endif
