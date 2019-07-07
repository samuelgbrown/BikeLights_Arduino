#include "Arduino.h"
#include "Speedometer.h"
#include "Definitions.h"

boolean newTic = false;
boolean newReference = false;
unsigned long lastIntTime = 0;           // The time since the last interrupt was triggered (used in debugging)
static unsigned long debounceTime = 200; // Use a 200 ms debouncing time

//void Speedometer::tic() {
void ticISR()
{
  unsigned long newIntTime = millis();
  if ((newIntTime - lastIntTime) > debounceTime)
  {
    newTic = true; // Set the newTic flag for the main loop
    if (DEBUGGING_TIC)
    {
      // Serial.flush();
      Serial.println(F("tic"));
    }
    lastIntTime = newIntTime;
  }
}

//void Speedometer::rTic() {
void rTicISR()
{
  unsigned long newIntTime = millis();
  if ((newIntTime - lastIntTime) > debounceTime)
  {
    newTic = true;       // Set the newTic flag for the main loop
    newReference = true; // Set the newReference flag for the main loop
    if (DEBUGGING_TIC)
    {
      // Serial.flush();
      Serial.println(F("rTic"));
      delay(100);
    }
    lastIntTime = newIntTime;
  }
}

//Speedometer::Speedometer(void (*ticISR)(), void (*rticISR)()): ticPin(TICKPIN), rticPin(RTICKPIN) {
Speedometer::Speedometer()
{
  // Set local variables
  ticPin = TICKPIN;
  rticPin = RTICKPIN;

  //  Serial.println(F("Setting up Speedometer..."));
  // Set the reference pin as input
  pinMode(rticPin, INPUT_PULLUP);

  // Set up the interrupt functions for rticPin
  attachInterrupt(digitalPinToInterrupt(rticPin), rTicISR, FALLING);

  if (numSwitches > 1)
  {
    // If there is more than 1 switch, then use the remaining switches to determine the speed
    pinMode(ticPin, INPUT_PULLUP);                                   // Set the tic pin as input
    attachInterrupt(digitalPinToInterrupt(ticPin), ticISR, FALLING); // Set up the interrupt functions for rticPin
  }

  // Create the Kalman object
  kalman = Kalman();

  if (DEBUGGING_KALMAN)
  {
    Serial.print(F("nLEDs is "));
    Serial.println(kalman.checkNumLEDs());
    delay(500);
  }

  //  Serial.println(F("Speedometer set"));
}

void Speedometer::mainLoop()
{
  //
  // The loop the Speedometer executes every time loop() is called
  //

  //
  // Evaluate any new tics that just came in
  //

  if (DEBUGGING_SPEEDOMETER)
  {
    // Serial.flush();
    Serial.println(F("Starting Main Speedometer Loop..."));
    //    delay(500);
  }

  if (newTic)
  {
    ticOn = true; // Indicate that a new tic pulse has just started
    //    if (newReference) {
    //      referenceOn = true; // Indicate that a new reference pulse has just started
    //
    //      //    timeAtLastReference = millis(); // Record the current time in milliseconds, for timing between pulses
    //    }

    timeAtTicOn = micros();   // Record the current time in microseconds, for timing the pulse
    timeAtThisTic = millis(); // Record the current time in milliseconds, for timing between pulses
  }

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
    // Using a second "if" statement so that the pulse calculation can occur as quickly as possible after an interrupt

    // Add this measurement to the Kalman filter
    kalman.addMeasurement(newReference, timeAtThisTic);

    //    // Save the tic time to be used next time there is a tic
    //    timeAtLastTic = timeAtThisTic;

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

void Speedometer::setQ(float newQ)
{
  kalman.setQ(newQ);
}

void Speedometer::setP0(float *newP0)
{
  kalman.setP0(newP0);
}

void Speedometer::setR(float *newR)
{
  kalman.setR(newR);
}

float Speedometer::getPos()
{
  return kalman.xTrue;
}

float Speedometer::getVel()
{
  return kalman.velTrue;
}

float Speedometer::getAcc()
{
  return kalman.accTrue;
}

boolean Speedometer::isSlow()
{
  return kalman.isReset;
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

  for (unsigned char o = 0; o < N_OBS; o++)
  {
    // Go through all observation slots
    for (unsigned char o2 = 0; o2 < N_OBS; o2++)
    {
      // Go through all observation slots (again)
      R[o][o2] = 0; // Initialize to 0, then fill later
    }
  }

  // H
  H[0][0] = 1;
  H[1][1] = 1;
  Transpose((float *)H, N_OBS, N_STA, (float *)Ht); // Transpose H to get Ht

  // R
  R[0][0] = .00001; // .00001
  R[1][1] = .01;    // .1

  // Q
  Q = 1; // 10000;

  // F (partial, F is fully built when needed, as it depends on dt)
  F[0][0] = 1;
  F[1][1] = 1;
  F[2][2] = 1;

  // Calculate the number of LEDs per segment
  nTicLEDs = (float)NUMLEDS / (float)NUMSWITCHES;

  // Set the location of the first measurement to be the length of a segment
  nextMeasurePos = nTicLEDs;

  // Set the current best guess for X as 0
  xTrue = 0;
}

void Kalman::resetFilter()
{
  // Reset the matrices in the filter to their initial conditions
  isReset = true;
  nextMeasurePos = nTicLEDs;

  if (DEBUGGING_SPEED || DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Resetting Filter..."));
  }
  for (unsigned char s = 0; s < N_STA; s++)
  {
    // Go through all state variable slots
    xPost[s] = 0; // xPost - System starts at 0

    for (unsigned char s2 = 0; s2 < N_STA; s2++)
    {
      // Go through all state variable slots (again)
      if (s == s2)
      {
        if (s == 0)
        {
          // TODO: REFACTOR THIS TO USE setP0, AND SET UP setP0 properly!
          PPost[s][s2] = 0; // PPost - Completely certain about position
        }
        else if (s == 1)
        {
          PPost[s][s2] = 1; // PPost - Somewhat certain about velocity
        }
        else if (s == 2)
        {
          PPost[s][s2] = 10; // PPost - Somewhat uncertain about acceleration
        }
      }
      else
      {
        PPost[s][s2] = 0; // PPost - Completely certain in initial parameters
      }
    }
  }

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
  dt = timeAtThisMeasurement - timeAtLastMeasurement;
  if (isReference)
  {
    nextMeasurePos += fmod(nextMeasurePos, nLEDs); // A reference signal was just received, so snap the nextMeasurePos to the next highest multiple of nLEDs (nextMeasurePos should be the same before and after this line, but this is just a failsafe)
  }

  if (!isReset)
  {
    if (DEBUGGING_SPEED)
    {
      // Serial.flush();
      Serial.print(F("Added measurement at "));
      Serial.println(timeAtThisMeasurement);
      //      delay(1000);
    }
    // Add a measurement to the Kalman filter
    z[0] = nextMeasurePos; // Position, in LEDs
    z[1] = nTicLEDs / dt;  // Velocity, in LEDs per ms
    //  z[2] = (sq(newVell) - sq(xPost[1])) / (2 * nTicLEDs); // LEDs per ms^2 (a = ((vf)^2  - (vi)^2)/(2*d) ) [ACCELERATION NOT MEASURED, TOO UNSTABLE]

    // Set the newMeasurement flag
    newMeasurement = true;
  }
  else
  {
    // If the filter is reset, attempt to initialize the filter (will succeed if !justStarted and it has been less than maxTimeBetweenMeasurements since the last measurement)
    initializeFilter(dt, isReference);
  }

  // Do these every time, even if the filter is currently reset and more than maxTimeBetweenMeasurements has passed since the last measurement
  timeAtLastMeasurement = timeAtThisMeasurement; // Update the timeAtLastMeasurement
  nextMeasurePos += nTicLEDs;                    // Update the nextMeasurePos, for the next measurement
}

void Kalman::initializeFilter(unsigned long dt, boolean isReference)
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

  if (dt > maxTimeBetweenMeasurements)
  {
    // If this measurement is outside of the allowable window, then do not initialize the filter
    return;
  }

  // If the filter has not just it has been less than maxTimeBetweenMeasurements
  xPost[0] = nextMeasurePos; // Position, in LEDs
  xPost[1] = nTicLEDs / dt;  // Velocity, in LEDs per ms
  isReset = false;           // Turn off the reset settings

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
      Serial.println(F("Checking reset..."));
      //    delay(1000);
    }

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
    //    delay(1000);
  }

  if (DEBUGGING_SPEEDOMETER)
  {
    // Serial.flush();
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
      delay(500);
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

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Building F..."));
    //    delay(500);
  }
  // Build F (only modify the parts that depend on dt)
  F[0][1] = float(dt);
  F[0][2] = float(sq(dt));
  F[1][2] = float(dt);

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("F["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(F[i][j]);
    //      }
    //    }
    Serial.println(F("Transposing F..."));
    delay(500);
  }
  // Calculate F transpose
  float Ft[N_STA][N_STA];
  Transpose((float *)F, N_STA, N_STA, (float *)Ft);

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("Ft["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(Ft[i][j]);
    //      }
    //    }
    Serial.println(F("Calculating X A Priori..."));
    delay(500);
  }

  // Calculate x a priori
  Matrix.Multiply((float *)F, (float *)xPost, N_STA, N_STA, 1, (float *)xPrior);

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Calculating P A Priori..."));
    Serial.print(F("nLEDs is "));
    Serial.println(nLEDs);
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    // Serial.flush();
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("PPost["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(PPost[i][j]);
    //      }
    //    }
    Serial.println();
    delay(500);
  }
  // Calculate P a priori
  float PTemp[N_STA][N_STA];
  Matrix.Multiply((float *)F, (float *)PPost, N_STA, N_STA, N_STA, (float *)PTemp); // PTemp = F*PPost
  if (DEBUGGING_KALMAN)
  {
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("PTemp["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(PTemp[i][j]);
    //      }
    //    }
    //    Serial.println();
    delay(500);
  }
  Matrix.Multiply((float *)PTemp, (float *)Ft, N_STA, N_STA, N_STA, (float *)PPrior); // PPrior = F*PPost*F'
  if (DEBUGGING_KALMAN)
  {
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("PPrior["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(PPrior[i][j]);
    //      }
    //    }
    //    Serial.println();
    //    delay(500);
  }
  ScalarAddF((float *)PPrior, Q, N_STA, N_STA, (float *)PPrior); // PPrior = F*PPost*F' + Q

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Finished P A Priori..."));
    Serial.print(F("Current memory: "));
    Serial.println(freeRam());
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("PPost["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(PPost[i][j]);
    //      }
    //    }
    //    for (int i = 0; i < N_STA; i++) {
    //      for (int j = 0; j < N_STA; j++) {
    //        Serial.print(F("PPrior["));
    //        Serial.print(i);
    //        Serial.print(F("]["));
    //        Serial.print(j);
    //        Serial.print(F("] = "));
    //        Serial.println(PPrior[i][j]);
    //      }
    //    }
    //    Serial.println();
    delay(500);
  }

  if (newMeasurement)
  {
    //
    // If there is a new measurement, perform an update step
    //

    if (DEBUGGING_KALMAN)
    {
      // Serial.flush();
      Serial.println(F("Performing update step..."));
      delay(500);
    }

    // Calculate the residual y
    Matrix.Multiply((float *)H, (float *)xPrior, N_OBS, N_STA, 1, (float *)y); // y = H * xPriori
    Subtract((float *)z, (float *)y, N_OBS, 1, (float *)y);                    // y = z - H*xPriori

    // Calculate S
    float STemp[N_OBS][N_STA];
    Matrix.Multiply((float *)H, (float *)PPrior, N_OBS, N_STA, N_STA, (float *)STemp); // STemp = H*PPrior
    Matrix.Multiply((float *)STemp, (float *)Ht, N_OBS, N_STA, N_OBS, (float *)S);     // S = H*PPrior*H'
    Matrix.Add((float *)S, (float *)R, N_OBS, N_OBS, (float *)S);                      //S = H*PPrior*H' + R

    // Calculate K
    float KTemp[N_STA][N_OBS];
    Matrix.Invert((float *)S, N_OBS);                                                   // Invert S (done in place, variable S is now equal to S^-1) TIME CONSUMING most likely...
    Matrix.Multiply((float *)PPrior, (float *)Ht, N_STA, N_STA, N_OBS, (float *)KTemp); // KTemp = PPrior*H'
    Matrix.Multiply((float *)KTemp, (float *)S, N_STA, N_OBS, N_OBS, (float *)K);       // K = (PPrior*H')/S

    // Calculate xPost
    float xPostTemp[N_STA];
    Matrix.Multiply((float *)K, (float *)y, N_STA, N_OBS, 1, (float *)xPostTemp); // xPostTemp = K*y
    Matrix.Add((float *)xPrior, (float *)xPostTemp, N_STA, 1, (float *)xPost);    // xPost = xPrior + K*y

    // Calculate PPost
    float PPostTemp[N_STA][N_STA];
    Matrix.Multiply((float *)K, (float *)H, N_STA, N_OBS, N_STA, (float *)PPostTemp);          // PPostTemp = K*H
    Subtract((float *)I, (float *)PPostTemp, N_STA, N_STA, (float *)PPostTemp);                // PPostTemp = I - K*H
    Matrix.Multiply((float *)PPostTemp, (float *)PPrior, N_STA, N_STA, N_STA, (float *)PPost); // PPost = (I - K*H)*PPrior

    // Reset the newMeasurement flag
    newMeasurement = false;

    if (DEBUGGING_KALMAN)
    {
      // Serial.flush();
      //      Serial.println(F("xPost is: "));
      Serial.print(F("Pos: "));
      Serial.println(xPost[0]);
    }
    //        Serial.print(F("Vel: "));
    //        Serial.println(xPost[1]);
    //    Serial.println();
    //    Serial.print(F("Acc: "));
    //    Serial.println(xPost[2]);
    //    Serial.println();
  }
  else
  {
    //
    // If there is no new measurement, set the a priori values as the a posteriori (skip the update step)
    //

    if (DEBUGGING_KALMAN)
    {
      // Serial.flush();
      Serial.println(F("No update step, copying x prior to post..."));
      Serial.print(F("Current memory: "));
      Serial.println(freeRam());
      for (unsigned char i = 0; i < N_STA; i++)
      {
        Serial.print(F("xPrior["));
        Serial.print(i);
        Serial.print(F("] = "));
        Serial.println(xPrior[i]);
      }
      for (unsigned char i = 0; i < N_STA; i++)
      {
        Serial.print(F("xPost["));
        Serial.print(i);
        Serial.print(F("] = "));
        Serial.println(xPost[i]);
      }
      delay(500);
    }
    copyArray<float>((float *)xPrior, (float *)xPost, N_STA);

    if (DEBUGGING_KALMAN)
    {
      // Serial.flush();
      Serial.println(F("After copying..."));
      for (unsigned char i = 0; i < N_STA; i++)
      {
        Serial.print(F("xPrior["));
        Serial.print(i);
        Serial.print(F("] = "));
        Serial.println(xPrior[i]);
      }
      for (unsigned char i = 0; i < N_STA; i++)
      {
        Serial.print(F("xPost["));
        Serial.print(i);
        Serial.print(F("] = "));
        Serial.println(xPost[i]);
      }

      Serial.println();
      Serial.println(F("Copying P prior to post..."));
      for (unsigned char i = 0; i < N_STA; i++)
      {
        for (unsigned char j = 0; j < N_STA; j++)
        {
          Serial.print(F("PPrior["));
          Serial.print(i);
          Serial.print(F("]["));
          Serial.print(j);
          Serial.print(F("] = "));
          Serial.println(PPrior[i][j]);
        }
      }
    }
    //        copyArray2D<float>((float**)PPrior, (float**)PPost, N_STA, N_STA);
    //        copyArray2D((float**)PPrior, (float**)PPost, N_STA, N_STA);

    // Fucking work, dammit
    PPost[0][0] = PPrior[0][0];
    PPost[0][1] = PPrior[0][1];
    PPost[0][2] = PPrior[0][2];
    PPost[1][0] = PPrior[1][0];
    PPost[1][1] = PPrior[1][1];
    PPost[1][2] = PPrior[1][2];
    PPost[2][0] = PPrior[2][0];
    PPost[2][1] = PPrior[2][1];
    PPost[2][2] = PPrior[2][2];

    //    *xPost = *xPrior;
    //    **PPost = **PPrior;

    if (DEBUGGING_KALMAN)
    {
      //      Serial.flush();
      //      delay(500);
      Serial.println(F("After copying..."));
      for (unsigned char i = 0; i < N_STA; i++)
      {
        for (unsigned char j = 0; j < N_STA; j++)
        {
          Serial.print(F("PPrior["));
          Serial.print(i);
          Serial.print(F("]["));
          Serial.print(j);
          Serial.print(F("] = "));
          Serial.println(PPrior[i][j]);
        }
      }
      for (unsigned char i = 0; i < N_STA; i++)
      {
        for (unsigned char j = 0; j < N_STA; j++)
        {
          Serial.print(F("PPost["));
          Serial.print(i);
          Serial.print(F("]["));
          Serial.print(j);
          Serial.print(F("] = "));
          Serial.println(PPost[i][j]);
        }
      }
    }
  }

  //
  // Constrain xTrue to be within [0 nLEDs)
  //

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Constraining xTrue..."));
    delay(500);
    Serial.print(F("xPost[0] is "));
    Serial.println(xPost[0]);
    delay(500);
    Serial.print(F("nLEDs is "));
    Serial.println(nLEDs);
    delay(500);
    Serial.print(F("fmodf(xPost[0], nLEDs) is "));
    Serial.println(fmodf(xPost[0], nLEDs));
    delay(500);
  }
  // Set the true x to the difference added to the posteriori position during this loop
  //  xTrue += xPost[0] - xInitial;
  xTrue = fmodf(xPost[0], nLEDs);

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Getting vel and acc..."));
    delay(500);
  }
  velTrue = xPost[1];
  accTrue = xPost[2];

  //  Serial.println(xPost[1]);

  // If xTrue goes above nLEDS, subtract nLEDS (make it a topological circle)
  //  if (xTrue >= nLEDs) {
  //    // May want to test timing of using a mod operator on xPost[0] instead?
  //    xTrue -= nLEDs;
  //  }

  if (DEBUGGING_SPEED)
  {
    Serial.print(F("Speed = "));
    Serial.println(velTrue);
  }

  if (DEBUGGING_KALMAN)
  {
    // Serial.flush();
    Serial.println(F("Finishing Kalman..."));
    delay(500);
  }
}

void Kalman::setQ(float newQ)
{
  Q = newQ;
}

void Kalman::setP0(float *newP0) {

}

void Kalman::setR(float *newR) {

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
