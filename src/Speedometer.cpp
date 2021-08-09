#include "Arduino.h"
#include "Speedometer.h"
#include "Definitions.h"

boolean newTic = false;
boolean newReference = false;
static unsigned long debounceTime = 10; // Use a debouncing time (TODO: May need to get rid of?  Was 100ms (changed on 6/6/21, already uploaded), interferes with higher speeds)
unsigned long lastTicTime = 0;           // The time since the last interrupt was triggered (used in debugging)

#if !TIC_USE_LATCH
//void Speedometer::tic() {
void ticISR()
{
  unsigned long newIntTime = millis();
  if ((newIntTime - lastTicTime) > debounceTime)
  {
    newTic = true; // Set the newTic flag for the main loop
    if (DEBUGGING_TIC)
    {
      // Serial.flush();
      Serial.println(F("tic"));
    }
    lastTicTime = newIntTime;
  }
}

//void Speedometer::rTic() {
void rTicISR()
{
  unsigned long newIntTime = millis();
  if ((newIntTime - lastTicTime) > debounceTime)
  {
    newTic = true;       // Set the newTic flag for the main loop
    newReference = true; // Set the newReference flag for the main loop
    if (DEBUGGING_TIC)
    {
      // Serial.flush();
      Serial.println(F("rTic"));
      // delay(100);
    }
    lastTicTime = newIntTime;
  }
}
#endif

//Speedometer::Speedometer(void (*ticISR)(), void (*rticISR)()): ticPin(TICKPIN), rticPin(RTICKPIN) {
Speedometer::Speedometer()
{
  //  Serial.println(F("Setting up Speedometer..."));

#if TIC_USE_LATCH
  pinMode(RTICKPIN, INPUT);         // Set the reference pin as input
  pinMode(RESETTICKPIN, OUTPUT);    // Set up the output reset pin, so that we can reset the SR latch
  digitalWrite(RESETTICKPIN, HIGH); // Logic is active-low, so set the reset to be off (high)
#else
  // Set the reference pin as input (pullup, because the reed switch floats when not connected)
  pinMode(RTICKPIN, INPUT_PULLUP);

  // Set up the interrupt functions for rticPin
  attachInterrupt(digitalPinToInterrupt(RTICKPIN), rTicISR, FALLING);
#endif

  if (numSwitches > 1)
  {
// If there is more than 1 switch, then use the remaining switches to determine the speed
#if TIC_USE_LATCH
    pinMode(TICKPIN, INPUT); // Set the tic pin as input
#else
    pinMode(TICKPIN, INPUT_PULLUP);                                   // Set the tic pin as input (pullup, because the reed switch floats when not connected)
    attachInterrupt(digitalPinToInterrupt(TICKPIN), ticISR, FALLING); // Set up the interrupt functions for TICKPIN
#endif
  }

  // Create the Kalman object
  kalman = Kalman();

  // if (DEBUGGING_KALMAN)
  // {
  //   Serial.print(F("nLEDs is "));
  //   Serial.println(kalman.checkNumLEDs());
  //   delay(100);
  // }

  //  Serial.println(F("Speedometer set"));
}

void Speedometer::mainLoop()
{
  //
  // The loop the Speedometer executes every time loop() is called
  //

  if (DEBUGGING_SPEEDOMETER)
  {
    // Serial.flush();
    Serial.println(F("Starting Main Speedometer Loop..."));
    //    delay(500);
  }

  //
  // Evaluate any new tics that just came in
  //

#if TIC_USE_LATCH
  //  If we are using an SR latch, we must first detect if a tic has occurred

  // Record the raw inputs from the latch (still subject to debouncing)
  int tic = digitalRead(TICKPIN);
  int rTic = digitalRead(RTICKPIN);

  if (debug_flag_block_extra_ref && !debug_BER_readyForRef) {
    // If we are blocking extra reference tics for debugging:
    // If we are not ready for another reference tic, then make sure no reference tics come through
    rTic = LOW;
  }

  if (debug_flag_block_extra_tic && (debug_BET_numConsecTics >= (NUMSWITCHES - 1))) {
    // If we are blocking extra standard tics for debugging:
    // If the number of consecutive tics that we have already registered is greater than
    // or equal to NUMSWITCHES - 1 (or 3, for the standard configuration), then make sure no 
    // standard tics come through
    tic = LOW;
    kalman.debug_resetFromRef = true;
  }

  // TODO START HERE: Another idea for a false measurement mitigation:
  //    If 3 tics have been read since the last rtic, do not accept any more.  If another tic occurs, then 1) ignore it, and 2) set a flag.
  //    If the flag is set when an rtic is read, then re-initialize the Kalman filter with the location/speed as measured from the last valid rtic

  // if (DEBUGGING_TIC) {
  //   Serial.print(F("Pin is: "));
  //   Serial.println(digitalRead(RTICKPIN));
  //   Serial.print(F("rTic is: "));
  //   Serial.println(rTic == HIGH);
  //   Serial.print(F("tic is: "));
  //   Serial.println(tic == HIGH);
  //   Serial.print(F("both is: "));
  //   Serial.println((tic == HIGH) || (rTic == HIGH));
  // }

  if ((tic == HIGH) || (rTic == HIGH))
  {
    // TODO: Debounce the input (probably not needed...)
    // if (DEBUGGING_TIC) {
    //   Serial.println(F("Got Tic"));
    // }

    timeAtThisTic = millis();

    // Reset the SR latch, so we can catch the next tic (using active-low).  This is done even if the tic is still debouncing, because if we don't clear it now, this SAME tic will still be read again once the debounce timer is done (by nature of the SR latch)
    digitalWrite(RESETTICKPIN, HIGH);
    digitalWrite(RESETTICKPIN, LOW);
    digitalWrite(RESETTICKPIN, HIGH);

    if ((timeAtThisTic - lastTicTime) > debounceTime)
    {
      // if (DEBUGGING_TIC) {
      //   Serial.println(F("Tic Debounced"));
      // }
      // If there has been any tic, then record it and its time
      newTic = true;       // Set the newTic flag for the main loop
      newReference = (rTic == HIGH); // Set the newReference flag for the main loop
      lastTicTime = timeAtThisTic;

      if (DEBUGGING_TIC)
      {
        // Serial.flush();
        if (tic == HIGH)
        {
          Serial.println(F("tic"));
        }

        if (rTic == HIGH)
        {
          Serial.println(F("rTic"));
        }
      }

      if (debug_tic_info && bt) {
        // If we need to print the tic's to Bluetooth, do so
        if (tic == HIGH) {
          const static char ticStr[] PROGMEM = "tic ";
          char ticNum[4];
          sprintf(ticNum, "%d\n", debug_BET_numConsecTics);
          bt->sendStrPROGMEM(ticStr);
          bt->sendStr(ticNum);
        }
        else if (rTic == HIGH)
        {
          const static char refTicStr[] PROGMEM = "REF tic\n";
          bt->sendStrPROGMEM(refTicStr);
        }
      }

      // For debugging: We are ready for a new reference if the last tic was NOT a reference.  Otherwise, block any new reference tics until the next non-reference
      debug_BER_readyForRef = !newReference;

      if (tic == HIGH)
      {
        debug_BET_numConsecTics++; // Increment the number of consecutive tics we have received
      }
      else // if (rTic == HIGH)
      { 
        debug_BET_numConsecTics = 0; // If we just got a reference, reset our counter
      }
    }
  }

#else
  if (newTic)
  {
    // ticOn = true; // Indicate that a new tic pulse has just started
    //    if (newReference) {
    //      referenceOn = true; // Indicate that a new reference pulse has just started
    //
    //      //    timeAtLastReference = millis(); // Record the current time in milliseconds, for timing between pulses
    //    }

    // timeAtTicOn = micros();   // Record the current time in microseconds, for timing the pulse
    timeAtThisTic = millis(); // Record the current time in milliseconds, for timing between pulses
  }
#endif

  //
  // See if the tic pins are still active
  //

  //  if (ticOn) {
  //    // If ticOn has not been set off yet
  //    unsigned long pulseDuration = micros() - timeAtTicOn; // Get the duration of the pulse
  //
  //    // Check if the pin is still LOW (connected)
  //    if (digitalRead(ticPin) == HIGH) {
  //      // The pin has just shut off
  //      ticOn = false; // Stop measuring the duration of the pulse, as it has ended
  //
  //      // Calculate the (low) speed from the pulse duration
  //      // GET INTER-PULSE WORKING FIRST, THEN ADD INTRA-PULSE
  //    }
  //    if (pulseDuration > maxPulseLength) {
  //      // Assume that the return to HIGH signal was missed, because enough time has passed
  //      ticOn = false;
  //    }
  //  }

  //
  // Evaluate the speed/orientation of the wheel using tic information
  //

  if (newTic)
  {
    // Add this measurement to the Kalman filter
    kalman.addMeasurement(newReference, timeAtThisTic);

    newTic = false;       // Reset the newTic flag
    newReference = false; // Reset the newReference flag
  }
  //  Serial.println(F("Speed loop"));

  //
  // Run the Kalman filter's main loop (after any measurements have been added, if any)
  //

  if (DEBUGGING_SPEEDOMETER)
  {
    // Serial.flush();
    Serial.println(F("Evaluated tics.  Starting Kalman Loop..."));
    //    delay(500);
  }
  kalman.mainLoop();

  if (DEBUGGING_SPEEDOMETER)
  {
    // Serial.flush();
    Serial.println(F("Finished Kalman Loop..."));
    //    delay(500);
  }
}

Kalman *Speedometer::getKalman()
{
  return &kalman;
}

void Speedometer::setBluetooth(Bluetooth * newBTPointer) {
  bt = newBTPointer;
}

bool Speedometer::isDebugging(unsigned char debugCode)
{
  switch (debugCode)
  {
  case DEBUG_TIC_INFO_1:
    return debug_tic_info;
  case DEBUG_BLOCK_EXTRA_REF_2:
    return debug_flag_block_extra_ref;
  case DEBUG_BLOCK_EXTRA_TIC_4:
    return debug_flag_block_extra_tic;
  default:
    return false;
  }
}

bool Speedometer::toggleDebugging(unsigned char debugCode)
{
  switch (debugCode)
  {
  case DEBUG_TIC_INFO_1:
    debug_tic_info ^= 1;
    break;
  case DEBUG_BLOCK_EXTRA_REF_2:
    debug_flag_block_extra_ref ^= 1;
    break;
  case DEBUG_BLOCK_EXTRA_TIC_4:
    debug_flag_block_extra_tic ^= 1;
    break;
  default:
    break;
  }

  return isDebugging(debugCode);
}

float Speedometer::getPos()
{
  return kalman.getPos();
}

float Speedometer::getVel()
{
  return kalman.getVel();
}

float Speedometer::getAcc()
{
  return kalman.getAcc();
}

boolean Speedometer::isSlow()
{
  return kalman.isReset;
}

void Speedometer::resetFilter()
{
  kalman.resetFilter();
}

void Speedometer::setNumLEDs(unsigned char numLEDs) {
  kalman.setNumLEDs(numLEDs);
}

unsigned char Speedometer::getNumLEDs() {
  return kalman.getNumLEDs();
}

//// Define static members
//boolean Speedometer::newTic = false;
//boolean Speedometer::newReference = false;

Kalman::Kalman()
{
  // Initialize all variables that must be initialized
  resetFilter();
  for (unsigned char s = 0; s < N_STA; s++)
  {
    // Go through all state variable slots
    for (unsigned char o = 0; o < N_OBS; o++)
    {
      // Go through all observation slots
      H[o][s] = 0; // Initialize to 0, then fill later
    }
    for (unsigned char s2 = 0; s2 < N_STA; s2++)
    {
      // Go through all state variable slots (again)
      F[s][s2] = 0; // Initialize to 0, then fill later

      if (s == s2)
      {
        I[s][s2] = 1; // The diagonal in I is equal to 1...
      }
      else
      {
        I[s][s2] = 0; // ...and everywhere else is 0
      }
    }
  }

  // for (unsigned char o = 0; o < N_OBS; o++)
  // {
  //   // Go through all observation slots
  //   for (unsigned char o2 = 0; o2 < N_OBS; o2++)
  //   {
  //     // Go through all observation slots (again)
  //     R[o][o2] = 0; // Initialize to 0, then fill later
  //   }
  // }

  // H
  H[0][0] = 1;
#if USE_VEL_MEASUREMENT
  H[1][1] = 1;
#endif
// TODO: (UN-DID REMOVAL OF Ht) Eliminate Ht if H is always an only diagonal matrix?
Transpose((float *)H, N_OBS, N_STA, (float *)Ht); // Transpose H to get Ht

// R
#if USE_VEL_MEASUREMENT
  // float r[N_OBS * N_OBS] = {.00001, 0, 0, .01};
  // float r[N_OBS * N_OBS] = {.10, 0, 0, .01};
  float r[N_OBS * N_OBS] = {10, 0, 0, 5};
#else
  float r[N_OBS] = {1};
#endif
  setR(r);
  // R[0][0] = .00001; // .00001
  // R[1][1] = .01;    // .1

  // Q
  // setPhi(.1);
  setPhi(.01);
  // Q = 1; // 10000;

  // F (partial, F is fully built when needed, as it depends on dt).  Make the diagonal ones, because each component is added to itself plus a linear sum of the others
  F[0][0] = 1;
  F[1][1] = 1;
#if USE_THREE_STATE_KALMAN
  F[2][2] = 1;
#endif

  // Set the number of LEDs and the number of LEDs between each tic
  setNumLEDs(NUMLEDS);

  // Set the location of the first measurement to be the length of a segment
  nextMeasurePos = 0;
}

void Kalman::resetFilter()
{
  // Reset the matrices in the filter to their initial conditions
  isReset = true;
  nextMeasurePos = 0;

  if (DEBUGGING_SPEED || DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Resetting Filter..."));
  }

// Set PPrior
#if USE_THREE_STATE_KALMAN
  float p0[N_STA * N_STA] = {.01, 0, 0, 0, 5, 0, 0, 0, 5}; // Set an array for the initial PPrior
#else
  float p0[N_STA * N_STA] = {.1, 0, 0, .1}; // Set an array for the initial PPrior
#endif
  setP0(p0);

  // Set the velocity and acceleration to 0, but keep the position where it is (mostly just for Spinner patterns.)
  xPost[1] = 0;
  xPost[2] = 0;

  // for (unsigned char s = 0; s < N_STA; s++)
  // {
  //   // Go through all state variable slots
  //   xPost[s] = 0; // xPost - System starts at 0

  //   // for (unsigned char s2 = 0; s2 < N_STA; s2++)
  //   // {
  //   //   // Go through all state variable slots (again)
  //   //   if (s == s2)
  //   //   {
  //   //     if (s == 0)
  //   //     {
  //   //       PPost[s][s2] = 0; // PPost - Completely certain about position
  //   //     }
  //   //     else if (s == 1)
  //   //     {
  //   //       PPost[s][s2] = 1; // PPost - Somewhat certain about velocity
  //   //     }
  //   //     else if (s == 2)
  //   //     {
  //   //       PPost[s][s2] = 10; // PPost - Somewhat uncertain about acceleration
  //   //     }
  //   //   }
  //   //   else
  //   //   {
  //   //     PPost[s][s2] = 0; // PPost - Completely certain in initial parameters
  //   //   }
  //   // }
  // }

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Filter reset:"));
    //    Serial.print(F("nLEDs is "));
    //    Serial.println(nLEDs);
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("P["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(PPost[i][j]);
    //      }
    //    }
  }
}

void Kalman::addMeasurement(boolean isReference, unsigned long timeAtThisMeasurement)
{
  // Update the value of the next measurement
  if (isReference)
  {
    nextMeasurePos += (nLEDs - fmod(nextMeasurePos, nLEDs)); // A reference signal was just received, so snap the nextMeasurePos to the next highest multiple of nLEDs (nextMeasurePos should be the same before and after this line, but this is just a failsafe)
  }
  else
  {
    nextMeasurePos += nTicLEDs; // A normal tic was recieved, add the tic number of LEDs to the measurement
  }

  if (!(isReset || debug_resetFromRef))
  {
    // Add a measurement to the Kalman filter
    z[0] = nextMeasurePos; // Position, in LEDs
#if USE_VEL_MEASUREMENT
    z[1] = nTicLEDs / (timeAtThisMeasurement - timeAtLastMeasurement); // Velocity, in LEDs per ms
#endif
    //  z[2] = (sq(newVell) - sq(xPost[1])) / (2 * nTicLEDs); // LEDs per ms^2 (a = ((vf)^2  - (vi)^2)/(2*d) ) [ACCELERATION NOT MEASURED, TOO UNSTABLE]

    // Set the newMeasurement flag
    newMeasurement = true;

    if (DEBUGGING_MEASUREMENT)
    {
      // Serial.flush();
      Serial.println(F("New measurement:"));
      Serial.print(F("x = "));
      Serial.println(z[0]);
#if USE_VEL_MEASUREMENT
      Serial.print(F("v = "));
      Serial.println(z[1]);
#endif
      Serial.println();
    }
  }
  else
  {
    // If the filter is reset, attempt to initialize the filter (will succeed if !justStarted and it has been less than maxTimeBetweenMeasurements since the last measurement)
    initializeFilter(timeAtThisMeasurement, isReference);
  }

  // Do these every time, even if the filter is currently reset and more than maxTimeBetweenMeasurements has passed since the last measurement
  timeAtLastMeasurement = timeAtThisMeasurement; // Update the timeAtLastMeasurement

  // TODO: Make sure that this gets set before debug_resetFromRef can be set! Otherwise, things might go...strange!
  if (isReference) {
    timeAtLastRef = timeAtThisMeasurement; // Update the last reference time
  }
}

void Kalman::initializeFilter(unsigned long timeAtThisMeasurement, boolean isReference)
{
  if (DEBUGGING_SPEED)
  {
    // Serial.flush();
    Serial.println(F("Initializing Filter..."));
    //      delay(1000);
  }

  // Test if the filter has "just started" (has not received a reference signal yet, and therefore does not have the orientation of the wheel)
  if (justStarted)
  {
    if (!isReference)
    {
      return; // If the filter just started and most recent tic was not a reference, ignore it
    }
    else
    {
      if (DEBUGGING_SPEED || DEBUGGING_KALMAN)
      {
        // Serial.flush();
        Serial.println(F("First filter initialization!!!"));
      }
      justStarted = false; // The filter just started, but we now have a reference, so turn off the just started signal (permanently)
    }
  }

  if (!debug_resetFromRef)
  {
    if ((timeAtThisMeasurement - timeAtLastMeasurement) > maxTimeBetweenMeasurements)
    {
      // If this measurement is outside of the allowable window, then do not initialize the filter
      return;
    }

    // If the filter has not just it has been less than maxTimeBetweenMeasurements
    xPost[0] = nextMeasurePos; // Position, in LEDs
    xPost[1] = nTicLEDs / (timeAtThisMeasurement - timeAtLastMeasurement);  // Velocity, in LEDs per ms
#if USE_THREE_STATE_KALMAN
    xPost[2] = 0; // Acceleration in LEDs per ms^2
#endif
  }
  else
  {
    // Reset the filter, and then initialize using only the last reference tic (this only occurs during a current reference tic)
    resetFilter();

    xPost[0] = nextMeasurePos; // Position, in LEDs
    xPost[1] = nLEDs / (timeAtThisMeasurement - timeAtLastRef);  // Velocity (from the last reference tic), in LEDs per ms
#if USE_THREE_STATE_KALMAN
    xPost[2] = 0; // Acceleration in LEDs per ms^2
#endif
    // Reset the flag
    debug_resetFromRef = false;
  }
  isReset = false; // Turn off the reset settings

  timeAtLastPredict = millis(); // Initialize the timeAtLastPredict to the current time to prepare for the prediction step

  if (DEBUGGING_SPEED)
  {
    // Serial.flush();
    Serial.println(F("Initialized Filter."));
    //      delay(1000);
  }
}

void Kalman::mainLoop()
{
  // Main loop for the Kalman Filter
  // Get first value of x
  //  xInitial = xPost[0];

  if (isReset)
  {
    // If the filter is currently in a reset state, then do not calculate the filter
    if (DEBUGGING_SPEEDOMETER)
    {
      // Serial.flush();
      Serial.println(F("Is reset, returning."));
      //    delay(1000);
    }
    return;
  }

  if (DEBUGGING_SPEEDOMETER)
  {
    // Serial.flush();
    Serial.println(F("Is not reset..."));
    Serial.println(F("Getting timing..."));
  }

  // Calculate timing of this step
  unsigned long curTime = millis(); // Get the current time as the time that the prediction step started
  dt = curTime - timeAtLastPredict;
  unsigned long timeSinceLastMeasurement = curTime - timeAtLastMeasurement;
  timeAtLastPredict = curTime;

  if (timeSinceLastMeasurement > maxTimeBetweenMeasurements)
  {
    // If enough time has passed since the last measurement, put the filter into a reset state, and do not calculate the filter
    if (DEBUGGING_SPEEDOMETER)
    {
      // Serial.flush();
      Serial.println(F("Resetting now..."));
      delay(100);
    }
    resetFilter();

    if (DEBUGGING_SPEEDOMETER)
    {
      // Serial.flush();
      Serial.println(F("Just reset filter."));
      //      delay(1000);
    }
    return;
  }

  //
  // Perform a prediction step
  //

  // if (DEBUGGING_KALMAN && newMeasurement)
  // {
  //   // Serial.flush();
  //   Serial.println(F("Building F..."));
  //   //    delay(500);
  // }
  // Build F (only modify the parts that depend on dt)
  F[0][1] = float(dt); // Position depends on v*dt

#if USE_THREE_STATE_KALMAN
  F[0][2] = .5 * float(sq(dt)); // Position depends on 1/2*a*(dt^2) component
  F[1][2] = float(dt);          // Velocity depends on a*dt
#endif

  // if (DEBUGGING_KALMAN && newMeasurement)
  // {
  //   // Serial.flush();
  //      for (int i = 0; i < N_STA; i++) {
  //        for (int j = 0; j < N_STA; j++) {
  //          Serial.print(F("F["));
  //          Serial.print(i);
  //          Serial.print(F("]["));
  //          Serial.print(j);
  //          Serial.print(F("] = "));
  //          Serial.println(F[i][j]);
  //        }
  //      }
  //   // Serial.println(F("Transposing F..."));
  //   // delay(100);
  // }

  // if (DEBUGGING_KALMAN && newMeasurement)
  // {
  //   Serial.flush();
  //     //  for (int i = 0; i < N_STA; i++) {
  //     //    for (int j = 0; j < N_STA; j++) {
  //     //      Serial.print(F("Ft["));
  //     //      Serial.print(i);
  //     //      Serial.print(F("]["));
  //     //      Serial.print(j);
  //     //      Serial.print(F("] = "));
  //     //      Serial.println(Ft[i][j]);
  //     //    }
  //     //  }
  //   Serial.println(F("Calculating X A Priori..."));
  //   delay(100);
  // }

  // Calculate x a priori based on the previous xPost
  Matrix.Multiply((float *)F, (float *)xPost, N_STA, N_STA, 1, (float *)xPrior);

  if (DEBUGGING_KALMAN && newMeasurement && (xPrior[1] < 0))
  {
    Serial.print(F("XPost[0] = "));
    Serial.println(xPost[0]);
    Serial.print(F("XPost[1] = "));
    Serial.println(xPost[1]);
    Serial.print(F("XPost[2] = "));
    Serial.println(xPost[2]);

    Serial.print(F("XPrior[0] = "));
    Serial.println(xPrior[0]);
    Serial.print(F("XPrior[1] = "));
    Serial.println(xPrior[1]);
    Serial.print(F("XPrior[2] = "));
    Serial.println(xPrior[2]);

    // Serial.flush();
    Serial.println(F("Calculating P A Priori..."));
    // Serial.print(F("nLEDs is "));
    // Serial.println(nLEDs);
    // Serial.print(F("Current memory: "));
    // Serial.println(freeRam());
    // Serial.flush();
    for (int i = 0; i < N_STA; i++)
    {
      for (int j = 0; j < N_STA; j++)
      {
        Serial.print(F("(Old) PPost["));
        Serial.print(i);
        Serial.print(F("]["));
        Serial.print(j);
        Serial.print(F("] = "));
        Serial.println(PPost[i][j]);
      }
    }

    for (int i = 0; i < N_STA; i++)
    {
      for (int j = 0; j < N_STA; j++)
      {
        Serial.print(F("F["));
        Serial.print(i);
        Serial.print(F("]["));
        Serial.print(j);
        Serial.print(F("] = "));
        Serial.println(F[i][j]);
      }
    }
    Serial.println();
  }
  // Calculate P a priori
  {
    // Generate the first part of the PPrior calculation
    float Ft[N_STA][N_STA];
    float PTemp[N_STA][N_STA];
    Transpose((float *)F, N_STA, N_STA, (float *)Ft);                                   // Calculate F transpose
    Matrix.Multiply((float *)F, (float *)PPost, N_STA, N_STA, N_STA, (float *)PTemp);   // PTemp = F*PPost
    Matrix.Multiply((float *)PTemp, (float *)Ft, N_STA, N_STA, N_STA, (float *)PPrior); // PPrior = F*PPost*F'
  }
  {
// Generate the Q matrix
// Continuous White Noise Model (https://github.com/rlabbe/Kalman-and-Bayesian-Filters-in-Python/blob/master/07-Kalman-Filter-Math.ipynb)
#if USE_THREE_STATE_KALMAN
    Q[0][0] = (pow(dt, 5) / 20) * phi;

    Q[0][1] = (pow(dt, 4) / 8) * phi;
    Q[1][0] = (pow(dt, 4) / 8) * phi;

    Q[0][2] = (pow(dt, 3) / 6) * phi;
    Q[2][0] = (pow(dt, 3) / 6) * phi;

    Q[1][1] = (pow(dt, 3) / 3) * phi;

    Q[1][2] = (sq(dt) / 2) * phi;
    Q[2][1] = (sq(dt) / 2) * phi;

    Q[2][2] = dt * phi;
#else
    Q[0][0] = (pow(dt, 3) / 3) * phi;

    Q[1][0] = (sq(dt) / 2) * phi;
    Q[0][1] = (sq(dt) / 2) * phi;

    Q[1][1] = dt * phi;
#endif
  }

  Matrix.Add((float *)PPrior, (float *)Q, N_STA, N_STA, (float *)PPrior); // PPrior = F*PPost*F' + Q = N_STA*N_STA

  if (DEBUGGING_KALMAN && newMeasurement && (xPrior[1] < 0))
  {
    // Serial.flush();
    Serial.println(F("Finished P A Priori..."));
    // Serial.print(F("Current memory: "));
    // Serial.println(freeRam());
    //  for (int i = 0; i < N_STA; i++) {
    //    for (int j = 0; j < N_STA; j++) {
    //      Serial.print(F("PPost["));
    //      Serial.print(i);
    //      Serial.print(F("]["));
    //      Serial.print(j);
    //      Serial.print(F("] = "));
    //      Serial.println(PPost[i][j]);
    //    }
    //  }
    for (int i = 0; i < N_STA; i++)
    {
      for (int j = 0; j < N_STA; j++)
      {
        Serial.print(F("(New) PPrior["));
        Serial.print(i);
        Serial.print(F("]["));
        Serial.print(j);
        Serial.print(F("] = "));
        Serial.println(PPrior[i][j]);
      }
    }
    //    Serial.println();
    // delay(100);
  }

  if (newMeasurement)
  {
    //
    // If there is a new measurement, perform an update step
    //

    // if (DEBUGGING_KALMAN)
    // {
    //   // Serial.flush();
    //   Serial.println(F("Performing update step..."));
    //   delay(100);
    // }

    // Calculate the residual y
    {
      float yTemp[N_OBS];
      Matrix.Multiply((float *)H, (float *)xPrior, N_OBS, N_STA, 1, (float *)yTemp); // y = H * xPriori
      Subtract((float *)z, (float *)yTemp, N_OBS, 1, (float *)y);                    // y = z - H*xPriori

      if (DEBUGGING_KALMAN && (xPrior[1] < 0))
      {
        // Serial.flush();
        Serial.print(F("z[0] = "));
        Serial.println(z[0]);
#if USE_VEL_MEASUREMENT
        Serial.print(F("z[1] = "));
        Serial.println(z[1]);
#endif

        Serial.print(F("procErr[0] = "));
        Serial.println(yTemp[0]);
#if USE_VEL_MEASUREMENT
        Serial.print(F("procErr[1] = "));
        Serial.println(yTemp[1]);
#endif

        Serial.print(F("y[0] = "));
        Serial.println(y[0]);
#if USE_VEL_MEASUREMENT
        Serial.print(F("y[1] = "));
        Serial.println(y[1]);
#endif
        delay(100);
      }
    }

    // Calculate S
    {
      float STemp[N_OBS][N_STA];
      Matrix.Multiply((float *)H, (float *)PPrior, N_OBS, N_STA, N_STA, (float *)STemp); // STemp = H*PPrior
      // Matrix.Multiply((float *)STemp, (float *)H, N_OBS, N_STA, N_OBS, (float *)S);      // S = H*PPrior*H'
      Matrix.Multiply((float *)STemp, (float *)Ht, N_OBS, N_STA, N_OBS, (float *)S);     // S = H*PPrior*H'
      Matrix.Add((float *)S, (float *)R, N_OBS, N_OBS, (float *)S); //S = H*PPrior*H' + R
    }

    if (DEBUGGING_KALMAN && (xPrior[1] < 0))
    {
      // Serial.flush();
      for (int r = 0; r < N_OBS; r++)
      {
        for (int c = 0; c < N_OBS; c++)
        {
          Serial.print(F("S["));
          Serial.print(r);
          Serial.print(F("]["));
          Serial.print(c);
          Serial.print(F("] = "));
          Serial.println(S[r][c]);
        }
      }
    }

    // Calculate K
    {
      float KTemp[N_STA][N_OBS];
      int success = Matrix.Invert((float *)S, N_OBS); // Invert S (done in place, variable S is now equal to S^-1) TIME CONSUMING most likely...
      if (success == 0)
      {
        // If the matrix inversion failed, then seed the calculation with SOMETHING that it can use to continue
        S[0][0] = 2e15;
#if USE_VEL_MEASUREMENT
        S[1][0] = 6.77e11;
        S[0][1] = 6.77e11;
        S[1][1] = 2.25e8;
#endif
      }
      // Matrix.Multiply((float *)PPrior, (float *)H, N_STA, N_STA, N_OBS, (float *)KTemp); // KTemp = PPrior*H'
      Matrix.Multiply((float *)PPrior, (float *)Ht, N_STA, N_STA, N_OBS, (float *)KTemp); // KTemp = PPrior*H'
      Matrix.Multiply((float *)KTemp, (float *)S, N_STA, N_OBS, N_OBS, (float *)K); // K = (PPrior*H')/S
    }

    if (DEBUGGING_KALMAN && (xPrior[1] < 0))
    {
      // Serial.flush();
      for (int r = 0; r < N_STA; r++)
      {
        for (int c = 0; c < N_OBS; c++)
        {
          Serial.print(F("K["));
          Serial.print(r);
          Serial.print(F("]["));
          Serial.print(c);
          Serial.print(F("] = "));
          Serial.println(K[r][c]);
        }
      }
    }

    // Calculate xPost
    {
      float xPostTemp[N_STA];
      Matrix.Multiply((float *)K, (float *)y, N_STA, N_OBS, 1, (float *)xPostTemp); // xPostTemp = K*y
      Matrix.Add((float *)xPrior, (float *)xPostTemp, N_STA, 1, (float *)xPost);    // xPost = xPrior + K*y

      if (DEBUGGING_KALMAN && ((xPrior[1] < 0) || (xPost[1] < 0)))
      {
        Serial.print(F("xUpdate[0] = "));
        Serial.println(xPostTemp[0]);
        Serial.print(F("xUpdate[1] = "));
        Serial.println(xPostTemp[1]);
#if USE_THREE_STATE_KALMAN
        Serial.print(F("xUpdate[2] = "));
        Serial.println(xPostTemp[2]);
#endif

        Serial.print(F("xPost[0] = "));
        Serial.println(xPost[0]);
        Serial.print(F("xPost[1] = "));
        Serial.println(xPost[1]);
#if USE_THREE_STATE_KALMAN
        Serial.print(F("xPost[2] = "));
        Serial.println(xPost[2]);
#endif
      }
    }

    // Calculate PPost
    {
      float PPostTemp[N_STA][N_STA];
      Matrix.Multiply((float *)K, (float *)H, N_STA, N_OBS, N_STA, (float *)PPostTemp);          // PPostTemp = K*H = N_STA*N_STA
      Subtract((float *)I, (float *)PPostTemp, N_STA, N_STA, (float *)PPostTemp);                // PPostTemp = I - K*H = N_STA*N_STA
      Matrix.Multiply((float *)PPostTemp, (float *)PPrior, N_STA, N_STA, N_STA, (float *)PPost); // PPost = (I - K*H)*PPrior = N_STA*N_STA

// For original PPost calculation, stop here.  For rounding error compensation method, use below lines
#if USE_LOWERROR_PPOST
      float PPostTemp2[N_STA][N_STA];
      Transpose((float *)PPostTemp, N_STA, N_STA, (float *)PPostTemp2);                              // PPostTemp2 = PPostTemp = (I - K*H)' = N_STA*N_STA
      Matrix.Multiply((float *)PPost, (float *)PPostTemp2, N_STA, N_STA, N_STA, (float *)PPostTemp); // PPostTemp = PPost*PPostTemp2 = (I - K*H)*PPrior*(I - K*H)' = N_STA*N_STA

      float KR[N_STA][N_OBS];
      float KT[N_OBS][N_STA];
      Matrix.Multiply((float *)K, (float *)R, N_STA, N_OBS, N_OBS, (float *)KR);           // KR = K*R = N_STA*N_OBS
      Transpose((float *)K, N_STA, N_OBS, (float *)KT);                                    // KT = K' = N_OBS*N_STA
      Matrix.Multiply((float *)KR, (float *)KT, N_STA, N_OBS, N_STA, (float *)PPostTemp2); // PPostTemp2 = K*R*K' = N_STA*N_STA
      Matrix.Add((float *)PPostTemp, (float *)PPostTemp2, N_STA, N_STA, (float *)PPost);   // PPost = PPostTemp + PPostTemp2 = (I - K*H)*PPrior*(I - K*H)' + K*R*K' = N_STA*N_STA
#endif
    }

    if (DEBUGGING_KALMAN && ((xPrior[1] < 0) || (xPost[1] < 0)))
    {
      for (int i = 0; i < N_STA; i++)
      {
        for (int j = 0; j < N_STA; j++)
        {
          Serial.print(F("PPost["));
          Serial.print(i);
          Serial.print(F("]["));
          Serial.print(j);
          Serial.print(F("] = "));
          Serial.println(PPost[i][j]);
        }
      }
      Serial.println();
      //    delay(500);
    }

    // Reset the newMeasurement flag
    newMeasurement = false;

    // if (DEBUGGING_KALMAN && ((xPrior[1] < 0) || (xPost[1] < 0)))
    // {
    //   // Serial.flush();
    //   //      Serial.println(F("xPost is: "));
    //   Serial.print(F("Pos: "));
    //   Serial.println(xPost[0]);
    //   Serial.print(F("Vel: "));
    //   Serial.println(xPost[1]);
    //   Serial.print(F("Acc: "));
    //   Serial.println(xPost[2]);
    //   Serial.println();
    // }
  }
  else
  {
    //
    // If there is no new measurement, set the a priori values as the a posteriori (skip the update step)
    //

    if (DEBUGGING_KALMAN)
    {
      // Serial.flush();
      // Serial.println(F("No update step, copying x prior to post..."));
      // Serial.print(F("Current memory: "));
      // Serial.println(freeRam());
      // for (unsigned char i = 0; i < N_STA; i++)
      // {
      //   Serial.print(F("xPrior["));
      //   Serial.print(i);
      //   Serial.print(F("] = "));
      //   Serial.println(xPrior[i]);
      // }
      // for (unsigned char i = 0; i < N_STA; i++)
      // {
      //   Serial.print(F("xPost["));
      //   Serial.print(i);
      //   Serial.print(F("] = "));
      //   Serial.println(xPost[i]);
      // }
      // delay(100);
    }
    // copyArray<float>((float *)xPrior, (float *)xPost, N_STA);
    for (int i = 0; i < N_STA; i++)
    {
      xPost[i] = xPrior[i];
    }

    if (DEBUGGING_KALMAN)
    {
      // // Serial.flush();
      // Serial.println(F("After copying..."));
      // for (unsigned char i = 0; i < N_STA; i++)
      // {
      //   Serial.print(F("xPrior["));
      //   Serial.print(i);
      //   Serial.print(F("] = "));
      //   Serial.println(xPrior[i]);
      // }
      // for (unsigned char i = 0; i < N_STA; i++)
      // {
      //   Serial.print(F("xPost["));
      //   Serial.print(i);
      //   Serial.print(F("] = "));
      //   Serial.println(xPost[i]);
      // }

      // Serial.println();
      // Serial.println(F("Copying P prior to post..."));
      // for (unsigned char i = 0; i < N_STA; i++)
      // {
      //   for (unsigned char j = 0; j < N_STA; j++)
      //   {
      //     Serial.print(F("PPrior["));
      //     Serial.print(i);
      //     Serial.print(F("]["));
      //     Serial.print(j);
      //     Serial.print(F("] = "));
      //     Serial.println(PPrior[i][j]);
      //   }
      // }
    }
    //        copyArray2D<float>((float**)PPrior, (float**)PPost, N_STA, N_STA);
    //        copyArray2D((float**)PPrior, (float**)PPost, N_STA, N_STA);

    // Fucking work, dammit
    PPost[0][0] = PPrior[0][0];
    PPost[0][1] = PPrior[0][1];
    PPost[1][0] = PPrior[1][0];
    PPost[1][1] = PPrior[1][1];
#if USE_THREE_STATE_KALMAN
    PPost[0][2] = PPrior[0][2];
    PPost[1][2] = PPrior[1][2];
    PPost[2][0] = PPrior[2][0];
    PPost[2][1] = PPrior[2][1];
    PPost[2][2] = PPrior[2][2];
#endif

    //    *xPost = *xPrior;
    //    **PPost = **PPrior;

    if (DEBUGGING_KALMAN)
    {
      // //      Serial.flush();
      // //      delay(500);
      // Serial.println(F("After copying..."));
      // for (unsigned char i = 0; i < N_STA; i++)
      // {
      //   for (unsigned char j = 0; j < N_STA; j++)
      //   {
      //     Serial.print(F("PPrior["));
      //     Serial.print(i);
      //     Serial.print(F("]["));
      //     Serial.print(j);
      //     Serial.print(F("] = "));
      //     Serial.println(PPrior[i][j]);
      //   }
      // }
      // for (unsigned char i = 0; i < N_STA; i++)
      // {
      //   for (unsigned char j = 0; j < N_STA; j++)
      //   {
      //     Serial.print(F("PPost["));
      //     Serial.print(i);
      //     Serial.print(F("]["));
      //     Serial.print(j);
      //     Serial.print(F("] = "));
      //     Serial.println(PPost[i][j]);
      //   }
      // }
    }

    // if (DEBUGGING_KALMAN)
    // {
    //   // Serial.flush();
    //   //      Serial.println(F("xPost is: "));
    //   Serial.print(F("Pos: "));
    //   Serial.println(xPost[0]);
    //   Serial.print(F("Vel: "));
    //   Serial.println(xPost[1]);
    //   Serial.print(F("Acc: "));
    //   Serial.println(xPost[2]);
    //   Serial.println();
    // }
  }

  if (DEBUGGING_SPEED)
  {
    Serial.print(F("Speed = "));
    Serial.println(getVel());
#if USE_THREE_STATE_KALMAN
    Serial.print(F("Acc = "));
    Serial.println(getAcc());
#endif
    Serial.println();
    Serial.println();
  }

  // if (DEBUGGING_KALMAN)
  // {
  //   // Serial.flush();
  //   Serial.println(F("Finishing Kalman..."));
  //   delay(500);
  // }
}

float Kalman::getPos()
{
  // Constrain xTrue to be within [0 nLEDs)
  return fmodf(xPost[0], nLEDs);
  ; // No unit conversion necessary
}

float Kalman::getVel()
{
  return xPost[1] * 1000; // Convert to LED/s from LED/ms
}

float Kalman::getAcc()
{
  return xPost[2] * 1000 * 1000; // Convert to LED/(s^2) from LED/(ms^2)
}

void Kalman::setPhi(float newPhi)
{
  phi = newPhi; // TODO: This should be a N_STA x N_STA matrix [(T^3)/3 (T^2)/2;(T^2)/2 T] where T is the sampling time (I know...might have to imagine the average sampling time at some speed?)
}

void Kalman::setP0(float *newP0)
{
  // Set a new value for PPrior (there's definitely a "more C" way of doing this, but I can't for the life of me figure out how to do it consistently)
  for (unsigned char col = 0; col < N_STA; col++)
  {
    for (unsigned char row = 0; row < N_STA; row++)
    {
      PPrior[row][col] = newP0[row + N_STA * col];
      PPost[row][col] = newP0[row + N_STA * col];
    }
  }
}

void Kalman::setR(float *newR)
{
  // Set a new value for R
  for (unsigned char col = 0; col < N_OBS; col++)
  {
    for (unsigned char row = 0; row < N_OBS; row++)
    {
      R[row][col] = newR[row + N_OBS * col];
    }
  }
}

void Kalman::setP0Elem(unsigned char row, unsigned char col, float newElem)
{
  // Set a new value for the given element of P0, assuming the row and col are within bounds
  if (row < N_STA && col < N_STA)
  {
    PPrior[row][col] = newElem;
  }
}

void Kalman::setRElem(unsigned char row, unsigned char col, float newElem)
{
  // Set a new value for the given element of R, assuming the row and col are within bounds
  if (row < N_OBS && col < N_OBS)
  {
    R[row][col] = newElem;
  }
}

void Kalman::setNumLEDs(unsigned char numLEDs) {
  nLEDs = numLEDs;

  // Calculate the number of LEDs per segment
  nTicLEDs = (float)numLEDs / (float)NUMSWITCHES;
}

unsigned char Kalman::getNumLEDs() {
  return nLEDs;
}

float Kalman::getPhi()
{
  return phi;
}

const float **Kalman::getP0()
{
  return (const float **)PPrior;
}

const float **Kalman::getR()
{
  return (const float **)R;
}

void Kalman::ScalarAddF(float *A, float b, unsigned char numRows, unsigned char numCols, float *C)
{
  // CAN BE DONE IN PLACE
  // A = input matrix (numRows x numCols)
  // b = input scalar
  // numRows = number of rows in A = number of rows in B
  // numCols = number of columns in A = number of columns in B
  // C = output matrix = A+b (numRows x numCols)

  unsigned char i, j;
  for (i = 0; i < numRows; i++)
    for (j = 0; j < numCols; j++)
      C[numCols * i + j] = A[numCols * i + j] + b;
}

void Kalman::Subtract(float *A, float *B, unsigned char numRows, unsigned char numCols, float *C)
{
  // CAN BE DONE IN PLACE
  // A = input matrix (numRows x numCols)
  // B = input matrix (numRows x numCols)
  // numRows = number of rows in A = number of rows in B
  // numCols = number of columns in A = number of columns in B
  // C = output matrix = A-B (numRows x numCols)

  unsigned char i, j;
  for (i = 0; i < numRows; i++)
    for (j = 0; j < numCols; j++)
      C[numCols * i + j] = A[numCols * i + j] - B[numCols * i + j];
}

void Kalman::Transpose(float *A, unsigned char numRows, unsigned char numCols, float *B)
{
  // CANNOT BE DONE IN PLACE
  // A = input matrix (numRows x numCols)
  // numRows = number of rows in A = number of columns in B
  // numCols = number of columns in A = number of rows in B
  // B = output matrix = At (numCols x numRows)

  unsigned char i, j;
  for (i = 0; i < numRows; i++)
    for (j = 0; j < numCols; j++)
      B[numRows * j + i] = A[numCols * i + j];
}
