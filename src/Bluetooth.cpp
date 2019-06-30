#include "Bluetooth.h"

Bluetooth::Bluetooth()
{
}

Bluetooth::~Bluetooth()
{
}

void Bluetooth::mainLoop()
{
    // Check if there is anything on the serial line
    int count = btSer.available();
    if (count > 0)
    {
        if (count > MAX_BT_BUFFER_SIZE)
        {
            // SOMETHING HAS GONE HORRIBLY WRONG!!!
        }
        else
        {
            // TODO: Do a RAM check?

            // Process using the protocol buffer
            uint8_t buffer[64]; // Buffer that will read from the Software Serial stream

            // Read into a local buffer (horribly inefficient, wonder if there's a way to fix...)
            for (int bufLoc = 0; bufLoc < count; bufLoc++)
            {
                buffer[bufLoc] = btSer.read();
            }

            // Create a protocol buffer stream
            // pb_istream_t stream;
            // stream = pb_istream_from_buffer(buffer, count);
            pb_istream_t serStream = {&bluetooth_decode_callback, &btSer, 64};
            Message_BT message = bluetooth_BluetoothMessage_init_default; // Initialize the message
            // START HERE!!!

            // Do some tests
            // Test 1: Automatic decoding
            // Let nanopb do the decoding for us...what could possibly go wrong?
            // Similar to http://zakovinko.com/57-how-to-speak-from-arduino-to-python-and-back.html
            // See if we can transfer the information from the Message to the respective Bike_Lights objects without using up too much extra memory
            bool status = pb_decode(&serStream, bluetooth_BluetoothMessage_fields, &message);

            if (message.request)
            {
                // The message is requesting information from the Arduino
                switch (message.which_contents)
                {
                case MessageType_BWA:

                    break;
                case MessageType_Kalman:

                    break;
                case MessageType_Brightness:

                    break;
                case MessageType_Storage:

                    break;
                case MessageType_Battery:

                    break;
                }
            }
            else
            {
                // The message is new information for the Arduino
                switch (message.which_contents)
                {
                case MessageType_BWA:

                    break;
                case MessageType_Kalman:

                    break;
                case MessageType_Brightness:

                    break;
                case MessageType_Storage:

                    break;
                case MessageType_Battery:

                    break;
                }
            }

            // Test 2: Manual decoding
            // Define all of the decoding callbacks manually...why trust nanobp?
            // Similar to https://github.com/nanopb/nanopb/blob/master/tests/callbacks/decode_callbacks.c
            // Method 1: Define all of the callbacks such that they assign the incoming value to a class (maybe a new helper class?) pointed to by arg.  May not be possible, because fields in the Message are not known until we know which content is in there
            // Method 2: Manually decode the entire message, byte by suffering byte...ugh.  Fuck being memory efficient.  Maybe find (or make?) function that will decode one field at a time...except oneof's and fields that contain other Messages?  Have I said "ugh" yet?
        }
    }
}

// Auxilliary decoding methods
bool bluetooth_decode_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    // A decode callback for a pb_istream_t built from a Serial stream
    SoftwareSerial *thisBTSer = (SoftwareSerial *)stream->state; // Get the Bluetooth stream
    int bytesToRead = min(thisBTSer->available(), count);        // Get the minimum between the size of the buffer (buf) and the number of bytes in the Serial buffer (thisBTSer->available())

    for (int byteNum = 0; byteNum < bytesToRead; byteNum++)
    {
        // For each byte in the input stream, add it to the buffer
        buf[byteNum] = thisBTSer->read();
    }

    // Record the number of bytes left in the Serial stream
    stream->bytes_left = thisBTSer->available();

    return bytesToRead == count; // If we read the correct number of bytes, return a positive
}

// Main function to manually decode a Message from an input stream
bool decode_message(pb_istream_t *stream, Message_BT *message)
{

    // First, determine if information is being sent or requested

    // Second, determine which kind of message is being sent (or requested)
}

// Function to determine which type of content (BikeWheelAnim, Kalman, etc) the bluetooth message has
int message_content_test(pb_istream_t *stream)
{
}

void Bluetooth::processBWA(BWA_BT &message)
{
    // Process the incoming BWA message, by changing the required parameters in the Pattern

    // First, check that the number of LED's is the same
    if (message.num_leds != NUMLEDS && DEBUGGING_BLUETOOTH)
    {
        // Something has gone horribly wrong
        Serial.println();
        Serial.print(F("WARNING: Mismatch between numLEDS: Arduino: "));
        Serial.print(NUMLEDS);
        Serial.print(F(", Bluetooth: "));
        Serial.println(message.num_leds);
    }

    // Next, check what kind of image is being sent
    switch (message.image_meta.type) {
        case ImageType_CONSTANT_BT:
        // See if it's a "still" image, or a moving one (can save a tiny bit of processing power if it's still)
        if (message.image_meta.parameter_set.p1 == 0) {
            // Convert the Pattern to a Still Image
            pattern_handler->setMainPattern(M_STILL);
            pattern_handler->setIdlePattern(I_STILL); // TODO: May need to be revised, once I update Android to have still/moving for idle/main individually
        } else {
            int rotationSpeed = message.image_meta.parameter_set.p1;
            pattern_handler->setMainPattern(M_MOVING);
            pattern_handler->setIdlePattern(I_STILL); // TODO: May need to be revised, once I update Android to have still/moving for idle/main individually

            // Now set the speed
        static_cast<Moving_Image_Main*>(pattern_handler->mainPattern)->setRotateSpeed(rotationSpeed);
        }
        break;
        case ImageType_SPINNER_BT:
        // TODO: Yet to be implemented!
        break;
    }

    // Save the images to the Pattern(s) from the message (and try to do it without setting a new local variable maybe?  Play with pointers, go ahead!)
}

bool color_decode_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
}

Color_ *Color_FromPB(Color_BT *color_bt)
{
    Color_ *outputColor_; // Declare the color that will be generated
    switch (color_bt->type)
    {
    case ColorType_STATIC_BT:
        outputColor_ = new Color_Static();
        // outputColor_.cS(ColorObjFromPB(color_bt->color_objs)); // START HERE!! How do I access the colorObj array?
        break;
    case ColorType_DTIME_BT:

        break;
    case ColorType_DVEL_BT:

        break;
    }
}

colorObj ColorObjFromPB(ColorObj_BT *colorObj_bt)
{
}

Kalman *Kalman_FromPB(Kalman_BT *kalman_bt)
{
}

// Auxilliary encoding methods
bool color_encode_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    // Start with https://blog.noser.com/arduino-iot-with-mqtt-and-nanopb/
}

Color_BT *PBFromColor_(Color_ *color_)
{
}

ColorObj_BT *PBFromColorObj(colorObj c)
{
}

Kalman_BT *PBFromKalman(Kalman *kalman)
{
}