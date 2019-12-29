#ifndef Bluetooth_h
#define Bluetooth_h

#include "Arduino.h"
#include "Definitions.h"
#include "Color.h"
#include "Speedometer.h"
#include "Pattern.h"
#include "SoftwareSerial.h"
#include <math.h>

#if USE_NANOPB
#include "bluetooth.pb.h"
#include "pb.h"
#include "pb_common.h"
#include "pb_decode.h"
#include "pb_encode.h"
#endif

// Class that will wrap the Serial connection for ease of use
class btSerialWrapper
{
public:
    // Construct a stream wrapper object
    btSerialWrapper(Stream *stream);

    // Functions for receiving messages
    // Get the next byte(s) in the stream, if available
    bool initReceiveMessage();                                // Returns true on success
    bool readNextMessageByte(unsigned char &byteDestination); // Returns true on success
    bool readNextMessageBytes(unsigned char *byteDestinationArray, unsigned char numBytes);

    // Getters for information about the content of the message
    unsigned char getTotalMessages();
    unsigned char getContent();
    bool isRequest();

    // Getters for information about the communication
    int getCurMessageNum();
    void setCurMessageNum(int newCurMessageNum);
    unsigned char getCurByteNum();
    int available();

    // Functions for sending messages
    bool writeMetadata(const bool request, const unsigned char content);
    bool writeNextMessageByte(const unsigned char byteSource);
    bool writeNextMessageBytes(const unsigned char *byteSourceArray, const unsigned char numBytes);
    void sendCompletionSignal(); // Send a signal to Android that we are done transmitting

    // Reset the communication meta-data
    void resetCommunicationData();

    // Send a confirmation message back to Android
    bool sendConfirmation(); // A function for sending a confirmation to Android that we have received and processed a message

    Stream *stream; // Required by only constructor TODO: Make private again!!!

private:
    // Meta data from the message
    unsigned char totalMessages = 0;
    unsigned char content = 0;
    bool request = false;

    // Information for keeping of where we are in the entire communication
    int curMessageNum = 0; // The message that is being read now (or is being waited on being delivered)
    unsigned char nextByteNum = 0;   // The byte that will be read next
};

// The Bluetooth class will take care of the bluetooth connection to the Android component of the system.  It will be responsible for maintaining the Bluetooth connection, and communicating with the Android system by generating C++ objects that are sent over the line
class Bluetooth
{
public:
    Bluetooth(Pattern_Handler *pattern_handler, Speedometer *speedometer);
    ~Bluetooth();

    void mainLoop(); // The main loop that is called repeatedly for the Bluetooth class

#if USE_NANOPB
    // Decode methods (Generate local classes from a protocol buffer stream)
    void processBWA(BWA_BT &message);               // Take a BWA_BT Message, process it, and send it directly to the Pattern_Handler whose pointer is stored
    void processKalman(Kalman_BT &message);         // Take a Kalman_BT Message, process it, and send it directly to the Speedometer whose pointer is stored
    void processBrightness(float brightness_scale); // Take a Brightness_BT Message, process it, and send it directly to the Pattern_handler whose pointer is stored

    static Color_ *Color_FromPB(Color_BT &color_bt, Speedometer *speedometer); // Generate a Color_ from the Color_BT Message (can't be static, because creating a Color_dVel requires a Speedometer)
    static colorObj ColorObjFromPB(ColorObj_BT &colorObj_bt);                  // Generate a colorObj from the ColorObj_BT Message
    static BLEND_TYPE BlendTypeFromPB(BlendType_BT blendType_bt);              // Convert a BlendType_BT Message to a BLEND_TYPE variable

    // Encode methods (Generate a protocol buffer stream from local classes)
    BWA_BT PBFromPattern();                  // Generate a BWA_BT from the information held by pattern_handler
    Kalman_BT PBFromKalman();                // Generate a Kalman_BT object from a Kalman_BT Message
    static void memfree_BT(Message_BT *mIn); // Free any dynamically allocated memory used by a given Message_BT

    static Color_BT PBFromColor_(Color_ *color_);                             // Generate a Color_ from the Color_BT Message
    static ColorObj_BT PBFromColorObj(const colorObj &c);                     // Generate a colorObj from the ColorObj_BT Message
    static ImageType_BT PBFromImageType(Pattern_Handler *pattern_handler_in); // Convert the information in a pattern_handler to an ImageType_BT variable
    static BlendType_BT PBFromBlendType(BLEND_TYPE blendType);                // Convert a BLEND_TYPE Message to a BlendType_BT variable
    static ColorType_BT PBFromColorType(COLOR_TYPE colorType);                // Convert a COLOR_TYPE Message to a ColorType_BT variable
#endif

private:
#if BLUETOOTH_USE_HARDWARESERIAL
    btSerialWrapper * btSer = new btSerialWrapper(&Serial); // Initialize the Serial connection to the bluetooth device (HC06)
#else
    btSerialWrapper * btSer = new btSerialWrapper(new SoftwareSerial(BLUETOOTHPIN_RX, BLUETOOTHPIN_TX)); // Initialize the Serial connection to the bluetooth device (HC06)
#endif
    Pattern_Handler *pattern_handler = NULL; // A pointer to the pattern handler, so we can change it using information sent via Bluetooth
    Speedometer *speedometer = NULL;         // A pointer to the speedometer, so we can change it using information sent via Bluetooth

};

#endif
