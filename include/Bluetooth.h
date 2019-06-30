#ifndef Bluetooth_h
#define Bluetooth_h

#include "Arduino.h"
#include "Definitions.h"
#include "Color.h"
#include "Speedometer.h"
#include "Pattern.h"
#include "bluetooth.pb.h"
#include "pb.h"
#include "pb_common.h"
#include "pb_decode.h"
#include "pb_encode.h"
#include "SoftwareSerial.h"
#include <math.h>

// The Bluetooth class will take care of the bluetooth connection to the Android component of the system.  It will be responsible for maintaining the Bluetooth connection, and communicating with the Android system by generating C++ objects that are sent over the line
class Bluetooth
{
private:
    // TODO: Convert this to hardware serial!!!
    SoftwareSerial btSer = SoftwareSerial(BLUETOOTHPIN_RX, BLUETOOTHPIN_TX); // Initialize the Serial connection to the bluetooth device (HC06)
    Pattern_Handler * pattern_handler; // A pointer to the pattern handler, so we can change it using information sent via Bluetooth
public:
    Bluetooth();
    ~Bluetooth();

    void mainLoop(); // The main loop that is called repeatedly for the Bluetooth class

    // Top level decode/encode methods
    Pattern* PatternFromPB(BWA_BT *message); // Generate a Pattern from a protocol buffer stream
    BWA_BT* PBFromPattern(Pattern *pattern); // Generate a protocol buffer stream from a Pattern

    // Auxilliary methods
    // Decode methods (Generate local classes from a protocol buffer stream)
    bool color_callback(pb_istream_t *stream, uint8_t *buf, size_t count); // Callback function for pb_istream_t used when decoding Color_'s from the PB
    void processBWA(BWA_BT &message);
    Color_* Color_FromPB(Color_BT *color_bt); // Generate a Color_ from the Color_BT Message
    colorObj ColorObjFromPB(ColorObj_BT *colorObj_bt); // Generate a colorObj from the ColorObj_BT Message
    Kalman* KalmanFromPB(Kalman_BT *kalman_bt); // Generate a Kalman object from a Kalman_BT Message

    // Encode methods (Generate a protocol buffer stream from local classes)
    Color_BT* PBFromColor_(Color_ *color_); // Generate a Color_ from the Color_BT Message
    ColorObj_BT* PBFromColorObj(colorObj c); // Generate a colorObj from the ColorObj_BT Message
    Kalman_BT* PBFromKalman(Kalman *kalman); // Generate a Kalman object from a Kalman_BT Message
};

#endif
