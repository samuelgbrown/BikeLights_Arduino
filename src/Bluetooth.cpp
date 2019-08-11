#include "Bluetooth.h"


Bluetooth::Bluetooth(Pattern_Handler *pattern_handler, Speedometer *speedometer) : pattern_handler(pattern_handler), speedometer(speedometer)
{
}

Bluetooth::~Bluetooth()
{
}

// TODO: START HERE:  Finish writing the decoding function
void Bluetooth::mainLoop() {
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
            // Before any of this stuff, store the current amount of freeRam, in case the user asks for it (don't want to taint the answer will all of the memory we'll need to do the message processing)
            int freeRamNow = freeRam();

            // TODO: START HERE: Rewrite this section, using the btSerialWrapper object held by Bluetooth

            // Read the meta-data from the information that is now in the Bluetooth buffer, to initialize btSer member variables.
            bool readSuccess = btSer.prepMessage();

            // while (messageNum < totalNumMessages) {
                if (btSer.isRequest()) {
                // If this message contains content sent by the Android program, then read the message and update the Arduino
                switch (btSer.getContent()) {
                    case 0:
                    // Bike wheel animation
                    // First, get the invariate information from the head of the message

                    unsigned char incomingNumLEDs;
                    unsigned char totalNumColor_s;
                     
                    btSer.nextMessageByte(incomingNumLEDs); // Get the number of LEDs that the Android thinks we have
                    btSer.nextMessageByte(totalNumColor_s); // Get the number of Color_'s that are in the palette

                    // TODO: Start process of saving the information from the message to an actual object (set the size of the palette)

                    // Start saving the palette information
                    for (unsigned char thisColor_Num = 0;thisColor_Num < totalNumColor_s;thisColor_Num++) {
                        // First, get this Color_ type
                        unsigned char thisColor_Type;
                        btSer.nextMessageByte(thisColor_Type); // Get the color type from the message
                        thisColor_Type = getNUIntFromByte(thisColor_Type, 4, 7); // Save the first nibble to the color type

                        if (thisColor_Type == 0) {
                            // Static Color_
                            unsigned char incomingColors [NUMLIGHTSPERLED];
                            btSer.nextMessageBytes(incomingColors, NUMLIGHTSPERLED);
                            // TODO: Create a new Static Color_ object from this array and save it to the palette

                        } else {
                            // This is a dynamic Color_, so the loading process will be quite similar
                            unsigned char numColorObj_metas;
                            btSer.nextMessageByte(numColorObj_metas); // Get the number of colorObj_metas that describe this Color_d
                            
                            // TODO: Set up the number of colorObj's in this Color_ (using setupArrays(unsigned char numColors) ), to be modified later
                            for (unsigned char colorObjNum = 0; colorObjNum < numColorObj_metas; colorObjNum++) {
                                // For every colorObjNum, get all of the information (LED brightnesses, T, and blend types)
                                colorObj thisColorObj;
                                unsigned char thisBlendTypeByte;

                                {
                                    // Get the colorObj for this index in the Color_d
                                    unsigned char thisColorObjArray [NUMLIGHTSPERLED];
                                    btSer.nextMessageBytes(thisColorObjArray, NUMLIGHTSPERLED);
                                    thisColorObj = colorObj(thisColorObjArray);

                                }
                                btSer.nextMessageByte(thisBlendTypeByte);

                                // TODO: Set up the colorObj and blend value for this colorObj in the Color_d

                                unsigned char thisTValArray[4];
                                btSer.nextMessageBytes(thisTValArray, 4);

                                if (thisColor_Type == 1) {
                                    // Time-based Color_
                                    unsigned long thisTVal = getLongFromByteArray(thisTValArray, 0);
                                    // TODO: Assign the colorObj, BLEND_TYPE, and t value for this Color_dTime

                                } else if (thisColor_Type == 2) {
                                    // Velocity-based Color_
                                    float thisTVal = getFloatFromByteArray(thisTValArray, 0);
                                    // TODO: Assign the colorObj, BLEND_TYPE, and t value for this Color_dVel

                                }
                            }

                            // TODO: Try to make sure that the creation of the dynamic Color_ has little memory overhead
                            switch (thisColor_Type) {
                                case 1:
                                // Time-based Color_

                                break;
                                case 2:
                                // Velocity-based Color_

                                break;
                            }
                        }
                    }

                    // Next, start saving the Pattern information
                    // First, get the palette-specific meta-data
                    bool supportsIdle;
                    unsigned char mainImageType;
                    signed char mainImageParam; // The parameter for the image type (signed to allow for negative rotation rates, etc.)

                    {
                        // Get the supports idle bool, as well as the image type
                        unsigned char supportsIdleImageTypeByte;
                        btSer.nextMessageByte(supportsIdleImageTypeByte);
                        supportsIdle = getBoolFromByte(supportsIdleImageTypeByte, 4);
                        mainImageType = getNUIntFromByte(supportsIdleImageTypeByte, 4, 0);
                    }

                    {
                        // Get the parameter for this image type
                        unsigned char imageParamByte;
                        btSer.nextMessageByte(imageParamByte);
                        mainImageParam = getNSIntFromByte(imageParamByte, 8, 0);
                    }

                    // TODO: Set the main image type on the Pattern, and apply the parameter(s), as needed

                    // Read in the main Image
                    for (unsigned char curImageByte = 0; curImageByte < NUM_BYTES_PER_IMAGE; curImageByte++) {
                        // Go through each expected byte in the image, and extract it to the color
                        
                        // Read the next byte from the message
                        unsigned char thisImageByte;
                        btSer.nextMessageByte(thisImageByte);
                        
                        // TODO: Set the newly read byte into the Pattern
                    }

                    // If this pattern supports an idle image, read the data from the message about the idle image
                    if (supportsIdle) {
                        // First, get the image type for the idle image
                        unsigned char idleImageType;
                        signed char idleImageParam;

                        // Get the idle image type
                        btSer.nextMessageByte(idleImageType);

                        {
                            // Get the idle image parameter
                            unsigned char idleImageParamByte;
                            btSer.nextMessageByte(idleImageParamByte);
                            idleImageParam = getNSIntFromByte(idleImageParamByte, 8, 0);
                        }

                        // TODO: Set the idle image type in the Pattern, and apply the parameter(s) as needed
                        
                        // Read in the idle Image
                        for (unsigned char curImageByte = 0; curImageByte < NUM_BYTES_PER_IMAGE; curImageByte++) {
                            // Go through each expected byte in the image, and extract it to the color
                        
                            // Read the next byte from the message
                            unsigned char thisImageByte;
                            btSer.nextMessageByte(thisImageByte);
                        
                            // TODO: Set the newly read byte into the Pattern

                        }

                    }

                    break;
                    case 1:
                    // TODO: Kalman Info
                    
                    break;
                    case 2:
                    // TODO: Brightness scale

                    break;
                }

            } else {
                // This message is a request for information from the Arduino
                switch (btSer.getContent()) {
                    case 0:
                    // Bike wheel animation

                    break;
                    case 1:
                    // Kalman Info

                    break;
                    case 2:
                    // Brightness scale

                    break;
                    case 3:
                    // Storage

                    break;
                    case 4:
                    // Battery

                    break;
                    // TODO: Complete all request cases!!!
                }
            }
            // }
        }
    }
}

// Bit-wise reading functions
bool getBoolFromByte(unsigned char byte, unsigned char bitPos) {
    // Extract a single bit from a byte, and interpret it as a boolean.
    // bitPos is the number of the bit (starting with the right-most bit as 0)
    return (byte >> bitPos) & 1; // Shift the byte bitPos bits to the right, so the bit of interest is in the least-significant position.  Then, mask it with 1 (0b00000001)
}

unsigned char getNUIntFromByte(unsigned char byte, unsigned char intSize, unsigned char firstBitPos) {
    // Get some n-sized unsigned integer from a byte.
    // NOTE: intsize MUST be less than 8!!!  No checks will be performed!!!
    // For example, an unsigned int is defined by the 3 bytes indicated by X in 0boooXXXoo, this function should be called as getNIntFromByte(0boooXXXoo, 3, 2)
    unsigned char bitMask = (1 << intSize) - 1; // Bit-mask generation idea from John Gietzan, on Stack Exchange (https://stackoverflow.com/a/1392065)
    return (byte >> firstBitPos) & bitMask;
}

signed char getNSIntFromByte(unsigned char byte, unsigned char intSize, unsigned char firstBitPos) {
    // Get some n-sized signed integer from a byte.
    // NOTE: intsize MUST be less than 8!!!  No checks will be performed!!!
    // For example, an unsigned int is defined by the 3 bytes indicated by X in 0boooXXXoo, this function should be called as getUIntFromByte(0boooXXXoo, 3, 2)
    unsigned char bitMask = (1 << intSize) - 1; // Bit-mask generation idea from John Gietzan, on Stack Exchange (https://stackoverflow.com/a/1392065)
    return (byte >> firstBitPos) & bitMask;
}

// Byte-wise reading functions
unsigned long getLongFromByteArray(unsigned char * byteArray, unsigned char firstBytePos) {
    // TODO: CHECK THIS CODE!!!
    // Convert a series of 4 bytes, located at some point firstBytePos in the array pointed to by byteArray, to an unsigned long
    
    // Create an array to be reinterpretted
    unsigned char longByteArray[4];
    // Copy the 4 bytes from byteArray, starting at firstBytePos, to longByteArray    
    for (int i = 0; i < 4; i++) {
    // I do not fucking trust the memory copying functions, probably due to my own incompetence, but still I'll do it the stupid way and let the goddamn compiler deal with it.
        longByteArray[i] = byteArray[i + firstBytePos];
    }

    // Convert the byteArray to an unsigned long pointer, then dereference the pointer to get an unsigned long
    return *((unsigned long *) longByteArray);  // Type conversion idea from imreal, on Stack Exchange (https://stackoverflow.com/a/22029162)
}

unsigned long getFloatFromByteArray(unsigned char * byteArray, unsigned char firstBytePos) {
    // TODO: CHECK THIS CODE!!!
    // Convert a series of 4 bytes, located at some point firstBytePos in the array pointed to by byteArray, to a float
    
    // Create an array to be reinterpretted
    unsigned char floatByteArray[4];
    // Copy the 4 bytes from byteArray, starting at firstBytePos, to longByteArray    
    for (int i = 0; i < 4; i++) {
    // I do not fucking trust the memory copying functions, probably due to my own incompetence, but still I'll do it the stupid way and let the goddamn compiler deal with it.
        floatByteArray[i] = byteArray[i + firstBytePos];
    }

    // Convert the byteArray to a float pointer, then dereference the pointer to get a float
    return *((float *) floatByteArray);  // Type conversion idea from imreal, on Stack Exchange (https://stackoverflow.com/a/22029162)
}

btSerialWrapper::btSerialWrapper(SoftwareSerial stream): stream(stream) {}

bool btSerialWrapper::prepMessage() {
    // This function reads the meta-data from the next message, if it exists, and returns if the meta-data was successfully read

    if (nextByteNum == 0) {
        // We have not yet read the meta-data.  Extract this first

            // Read the first two bytes into a temporary buffer
            unsigned char tmpStreamBuf[2];
            stream.readBytes(tmpStreamBuf, 2);

            // Read the data from this temporary buffer
            request = getBoolFromByte(tmpStreamBuf[0], 7); // Determine if this is a request for information, or a message containing infromation
            content = getNUIntFromByte(tmpStreamBuf[0], 3, 4); // Get the type of content that is being requested or sent
            totalMessages = getNUIntFromByte(tmpStreamBuf[1], 4, 4); // Get the total number of messages
            unsigned char readMessageNum = getNUIntFromByte(tmpStreamBuf[1], 4, 0); // Get the message number, according to the message
            
            // Check the message number
            if (readMessageNum != curMessageNum) {
                // Something has gone horribly wrong...
            }

            // Set the byte number so that we are after the meta data
            nextByteNum = 2;
    } else {
        return 0;  // If nextByteNum is not 0, then we are not EXPECTING to be at the beginning of a new message, so skip this function and return an error
    }
}

boolean btSerialWrapper::nextMessageByte(unsigned char & byteDestination) {
    // Check if there is any available data
    if (available() > 0) {
        // We have data avilable
        // Check if we've already read the meta-data from this message
        if (nextByteNum == 0) {
            prepMessage(); // If this message has not been read yet, get the meta-data from this one
        }

        // Now that we definitely have read the meta-data, read the next byte from the stream into byteDestination
        unsigned char tmpStreamBuf[1]; // A small temporary buffer to read bytes into before being moved into byteDestination
        stream.readBytes(tmpStreamBuf, 1); // (Always reading a single byte at a time.  Perhaps this is inefficient...)
        byteDestination = tmpStreamBuf[0]; // Get the first (only) byte from thisStreamBuf, and put it into the output bytesDestination

        // Iterate the byte number (is this actually used...?  Perhaps for debugging...)
        nextByteNum++;

        // Finally, return success
        return true;

    } else {
        // If no data is available

        // Check if we're expecting more content for this communication
        if (curMessageNum < totalMessages) {
            // We are expecting more content from this communication, ask for the next bit from Android
            // TODO: Send a message to Android requesting the next message in this communication

            // TODO: Wait for the new communication (If nothing comes for a bit of time, return a 0 due to timeout, and abandon the entire reading operation)

            // Reset the byte number and iterate the curMessageNum, so we know to extract the metadata next round
            nextByteNum = 0;
            curMessageNum++;

            // Finally, call this function once more since we just got a new message
            return nextMessageByte(byteDestination);

        } else {
            // We are not expecting any more information from this communication

            // Reset the communication, and return nothing
            resetCommunicationData(); // Should have been reset by this point already, but may as well just confirm

            byteDestination = 0;
            return false; // Error, because there was nothing to read
        }
    }
}

boolean btSerialWrapper::nextMessageBytes(unsigned char * byteDestination, unsigned char numBytes) {
    
}

unsigned char btSerialWrapper::getTotalMessages() {
    return totalMessages;
}

unsigned char btSerialWrapper::getContent() {
    return content;
}

bool btSerialWrapper::isRequest() {
    return isRequest;
}

unsigned char btSerialWrapper::getCurMessageNum() {
    return curMessageNum;
}

unsigned char btSerialWrapper::getnextByteNum() {
    return nextByteNum;
}

int btSerialWrapper::available() {
    return stream.available();
}

void btSerialWrapper::resetCommunicationData() {
    totalMessages = 0;
    content= 0;
    request = false;
    curMessageNum = 0;
    nextByteNum = 0;
}

bool bluetooth_decode_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    // A decode callback for a pb_istream_t built from a Serial stream
    SoftwareSerial *thisBTSer = (SoftwareSerial *)stream->state; // Get the Bluetooth stream
    size_t bytesToRead = min((size_t) thisBTSer->available(), count);        // Get the minimum between the size of the buffer (buf) and the number of bytes in the Serial buffer (thisBTSer->available())

    for (unsigned int byteNum = 0; byteNum < bytesToRead; byteNum++)
    {
        // For each byte in the input stream, add it to the buffer
        buf[byteNum] = thisBTSer->read();
    }

    // Record the number of bytes left in the Serial stream
    stream->bytes_left = thisBTSer->available();

    return bytesToRead == count; // If we read the correct number of bytes, return a positive
}

void oldMainLoop()
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

            // // Process using the protocol buffer
            // uint8_t buffer[64]; // Buffer that will read from the Software Serial stream

            // // Read into a local buffer (horribly inefficient, wonder if there's a way to fix...)
            // for (int bufLoc = 0; bufLoc < count; bufLoc++)
            // {
            //     buffer[bufLoc] = btSer.read();
            // }

            // Create a protocol buffer stream
            // pb_istream_t stream;
            // stream = pb_istream_from_buffer(buffer, count);

            // Before any of this stuff, store the current amount of freeRam, in case the user asks for it (don't want to taint the answer will all of the memory we'll need to do the message processing)
            int freeRamNow = freeRam();

            Message_BT message;
            {
                // Read the message from a serial stream (do so inside of a block, so the large serStream object is dismissed immediately after)
                pb_istream_t serStream = {&bluetooth_decode_callback, &btSer, 256}; // TODO: Memory: Is this enough?  Do I need more/less?
                message = bluetooth_BluetoothMessage_init_default;                  // Initialize the message
                bool status = pb_decode(&serStream, Message_BT_Fields, &message);
            }

            // Do some tests
            // Test 1: Automatic decoding
            // Let nanopb do the decoding for us...what could possibly go wrong?
            // Similar to http://zakovinko.com/57-how-to-speak-from-arduino-to-python-and-back.html
            // See if we can transfer the information from the Message to the respective Bike_Lights objects without using up too much extra memory

            // TODO: START HERE:
            // First, try setting no_unions:true!!!
            // Next, try setting type:FT_POINTER in .options and #define PB_ENABLE_MALLOC=1 in pb.h, see how that changes things, should now be able to use dynamic arrays?
            // Try using https://github.com/nanopb/nanopb/blob/master/examples/using_union_messages/decode.c to figure out which content is there (try setting no_unions:true ?).  Perhaps try this first as a standard C++ program to see what happens if we just decode the thing without setting arg/funcs...may require writing encoding code first
            // message.contents.bike_wheel_anim.image_main.funcs = MAKE A FUNCTION THAT WILL READ FROM THE SERIAL INPUT AND PLACE THE VALUES DIRECTLY INTO THE ARG VARIABLE, WHICH IS A POINTER TO THE PATTERN_HANDLER, TO GET THE IMAGE_MAIN ARRAY (and possibly the image_idle?)

            // TODO: Put this into a callback: depending on what we're expecting, we may need to decode differently (i.e. getting a BWA or a Kalman, or getting/not getting an image_idle)
            if (message.request)
            {
                // The message is requesting information from the Arduino

                // Store the message type, but otherwise dismiss the information in the message.
                int message_type_which = message.which_contents;
                pb_release(Message_BT_Fields, &message);

                // Make room for a new Message_BT, to send.
                Message_BT message_out;

                //  TODO: Populate message_out
                switch (message.which_contents)
                {
                case BWA_BT_Tag:
                    message_out.contents.bike_wheel_anim = PBFromPattern();
                    message_out.type = MessageType_BWA;
                    break;
                case Kalman_BT_Tag:
                    message_out.contents.kalman = PBFromKalman();
                    message_out.type = MessageType_Kalman;
                    break;
                case Brightness_BT_Tag:
                    message_out.contents.brightness_scale = pattern_handler->getBrightnessFactor();
                    message_out.type = MessageType_Brightness;
                    break;
                case Storage_BT_Tag:
                    message_out.contents.storage = Storage_BT_default;

                    // Populate the storage_bt object
                    message_out.contents.storage.total = TOTAL_MEMORY;
                    message_out.contents.storage.remaining = freeRamNow;

                    message_out.type = MessageType_Storage;
                    break;
                case Battery_BT_Tag:
                    // TODO: Uhhhhh...
                    message_out.type = MessageType_Battery;
                    break;
                }

                // Add in the required meta-data for the new message
                message_out.which_contents = message_type_which;
                message_out.request = false;

                // TODO: Send the message

                // TODO: Release the memory held by the message we just sent
                pb_release(Message_BT_Fields, &message_out);
            }
            else
            {
                // The message is new information for the Arduino
                bool needRestart = false; // Does the new information mean that we need to restart the Kalman filter?
                switch (message.which_contents)
                {
                case MessageType_BWA:
                    processBWA(message.contents.bike_wheel_anim); // Process the Bike_Wheel_Animation information from the message

                    needRestart = true; // We got a new BWA, so we should probably just start fresh
                    break;
                case MessageType_Kalman:
                    processKalman(message.contents.kalman); // Process the Kalman information from the message

                    needRestart = true; // We got some new Kalman parameters, so we should probably just start fresh
                    break;
                case MessageType_Brightness:
                {
                    processBrightness(message.contents.brightness_scale); // Process the Brightness information from the message
                    break;
                }
                }

                if (needRestart)
                {
                    // If we have changed something fundamental to the display, reset Kalman filter, and start over again
                    speedometer->resetFilter();
                }

                // Now that we are done reading the data, release the information from the Message
                pb_release(Message_BT_Fields, &message);
            }
        }
    }
}

// Decoding methods
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
    switch (message.image_meta.type)
    {
    case ImageType_CONSTANT_BT:
        // See if it's a "still" image, or a moving one (can save a tiny bit of processing power if it's still)
        if (message.image_meta.parameter_set.p1 == 0)
        {
            // Convert the Pattern to a Still Image
            pattern_handler->setMainPattern(M_STILL);
            pattern_handler->setIdlePattern(I_STILL); // TODO: May need to be revised, once I update Android to have still/moving for idle/main individually
        }
        else
        {
            int rotationSpeed = message.image_meta.parameter_set.p1;
            pattern_handler->setMainPattern(M_MOVING);
            pattern_handler->setIdlePattern(I_STILL); // TODO: May need to be revised, once I update Android to have still/moving for idle/main individually

            // Now set the speed
            static_cast<Moving_Image_Main *>(pattern_handler->mainPattern)->setRotateSpeed(rotationSpeed);
        }
        break;

        // Set the image for the idle pattern (the main pattern's image will be set in just a moment, because Spinners don't have idle images)
        pattern_handler->idlePattern->setImage(message.image_idle->bytes);
        // bluetooth_BluetoothMessage m = bluetooth_BluetoothMessage_init_zero;

    case ImageType_SPINNER_BT:
        // TODO: Yet to be implemented!
        break;
    }

    // Save the images for the main pattern
    pattern_handler->mainPattern->setImage(message.image_main->bytes);

    // Set the palette for the Pattern_Handler
    pattern_handler->setupPalette(message.palette, (unsigned char)message.palette_count);
}

Color_ *Bluetooth::Color_FromPB(Color_BT &color_bt, Speedometer *speedometer)
{
    // The function that recieves this color will be responsible for deallocating the memory after it is used
    Color_ *newColor;

    // First, get the type of Color_ that will be created
    if (color_bt.type == ColorType_STATIC_BT)
    {
        // TODO: SAFETY: Check the number of colorObjs?
        // If this is a static color, then we only ever need a single colorObj to describe it.
        newColor = new Color_Static(ColorObjFromPB(color_bt.color_objs[0]));
    }
    else
    {
        // If this is a dynamic color, then we're going to need an array of colorObj's, plus their metadata.  Create the array of the colorObjs and the blend_type's (the t array will be constructed once we know of what type the array is)
        // TODO: MEMORY: Pass a reference to this array, along with the size, but don't copy the entire array in the constructor, just record the pointer and the number of colorObj's that were created right HERE.
        colorObj *allC = new colorObj[color_bt.color_objs_count];
        BLEND_TYPE *allB = new BLEND_TYPE[color_bt.color_objs_count];
        for (unsigned char i = 0; i < color_bt.color_objs_count; i++)
        {
            allC[i] = ColorObjFromPB(color_bt.color_objs[i]);
            allB[i] = BlendTypeFromPB(color_bt.color_objs[i].bt);
        }

        switch (color_bt.type)
        {
        case ColorType_DTIME_BT:
        {
            // Create the T array and fill it
            unsigned long *allLongT = new unsigned long[color_bt.color_objs_count];
            for (unsigned char i = 0; i < color_bt.color_objs_count; i++)
            {
                allLongT[i] = (unsigned long)color_bt.color_objs[i].t;
            }

            newColor = new Color_dTime(allC, allLongT, allB, (unsigned char)color_bt.color_objs_count);
            break;
        }
        case ColorType_DVEL_BT:
        {
            // Create the T array and fill it
            float *allFloatT = new float[color_bt.color_objs_count];
            for (unsigned char i = 0; i < color_bt.color_objs_count; i++)
            {
                allFloatT[i] = (float)color_bt.color_objs[i].t;
            }

            newColor = new Color_dVel(speedometer, allC, allFloatT, allB, (unsigned char)color_bt.color_objs_count);
            break;
        }
        default:
        {
            // Uh oh...
            newColor = new Color_Static();
        }
        }
    }

    return newColor;
}

colorObj Bluetooth::ColorObjFromPB(ColorObj_BT &colorObj_bt)
{
    // Create a new colorObj object using the information stored in the ColorObj_BT Message
    return colorObj(colorObj_bt.r, colorObj_bt.g, colorObj_bt.b, colorObj_bt.w);
}

BLEND_TYPE Bluetooth::BlendTypeFromPB(BlendType_BT blendType_bt)
{
    // Convert a BlendType_BT to a local BLEND_TYPE (because reasons)
    switch (blendType_bt)
    {
    case BlendType_CONSTANT_BT:
        return B_CONSTANT;
        break;
    case BlendType_LINEAR_BT:
        return B_LINEAR;
        break;
    default:
        return B_LINEAR;
    }
}

void Bluetooth::processKalman(Kalman_BT &message)
{
    // Process the incoming Kalman message.

    // This message may not be "full" (i.e. the Android app may only be trying to send one or two parameters at a time).
    // So, we should check that each one is not the default value before anything.

    // TODO: SAFETY: Should we check the number of observable/state parameters that is included in the message and/or the sizes of R/P0?

    // Check if there's a new Q
    if (message.q != 0)
    {
        // A new value for Q was just added!
        speedometer->getKalman()->setQ(message.q);
    }

    // Check if there's a new R
    if (message.r != NULL)
    {
        // A new value for R was just added!
        speedometer->getKalman()->setR(message.r);
    }

    // Check if there's a new P0
    if (message.p0 != NULL)
    {
        // A new value for PPrior was just added!
        speedometer->getKalman()->setP0(message.p0);
    }
}

void Bluetooth::processBrightness(float brightness_scale)
{
    // Send the new brightness value to the Pattern_Handler
    pattern_handler->setBrightnessFactor(brightness_scale);
}

// Encoding methods
BWA_BT Bluetooth::PBFromPattern()
{
    // Generate a BWA_BT object from the information stored in Pattern_Handler
    BWA_BT messageOut = BWA_BT_default; // Create a new default BWA_BT object, as an initialization

    // TODO: Need to free this array using pb_release (or free()).  Still need memfree_BT()...?
    // First, encode the image(s)
    messageOut.image_idle = new pb_bytes_array_t[NUM_BYTES_PER_IMAGE];

    messageOut.image_main = (pb_bytes_array_t *)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(NUM_BYTES_PER_IMAGE));
    messageOut.image_main->size = NUM_BYTES_PER_IMAGE;
    if (pattern_handler->idlePattern != NULL)
    {
        messageOut.image_idle = (pb_bytes_array_t *)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(NUM_BYTES_PER_IMAGE));
        messageOut.image_idle->size = NUM_BYTES_PER_IMAGE;
    }

    for (unsigned char i = 0; i < NUM_BYTES_PER_IMAGE; i++)
    {
        // Encode each value in both main (and, if needed, idle) pattern(s)
        messageOut.image_main->bytes[i] = pattern_handler->mainPattern->getImageRawByte(i);
        if (pattern_handler->idlePattern != NULL)
        {
            messageOut.image_main->bytes[i] = pattern_handler->mainPattern->getImageRawByte(i);
        }
    }

    // Old
    // messageOut.image_main = new uint32_t[NUMLEDS];
    // messageOut.image_main_count = NUMLEDS;
    // if (pattern_handler->idlePattern != NULL)
    // {
    //     // If there is an idle image, add it to the message
    //     messageOut.image_idle = new uint32_t[NUMLEDS];
    //     messageOut.image_idle_count = NUMLEDS;
    // }
    // for (unsigned char i = 0; i < NUMLEDS; i++)
    // {
    //     // Encode each value in both main (and, if needed, idle) pattern(s)
    //     messageOut.image_main[i] = pattern_handler->mainPattern->getImageValInPos(i);
    //     if (pattern_handler->idlePattern != NULL)
    //     {
    //         messageOut.image_idle[i] = pattern_handler->idlePattern->getImageValInPos(i);
    //     }
    // }

    // Next, encode the image meta data
    messageOut.image_meta = pattern_handler->mainPattern->getImageType();
    // messageOut.image_meta.type = PBFromImageType(pattern_handler);
    // messageOut.image_meta.parameter_set = pattern_handler->mainPattern->getImageType();
    // TODO: Get paramter from idle animation, too!!!  (first, implement it on the Android side)
    // TODO: Also, must implement "stationary" (or whatever I'm going to call it, when the image doesn't move relative to the rest of the wheel) in both Android and Arduino

    // Then, encode the palette
    messageOut.palette = new Color_BT[pattern_handler->getNumColors()];
    messageOut.palette_count = pattern_handler->getNumColors();

    for (int i = 0; i < pattern_handler->getNumColors(); i++)
    {
        messageOut.palette[i] = PBFromColor_(pattern_handler->getColor(i));
    }

    // Finally, put in the number of LEDs on the wheel
    messageOut.num_leds = NUMLEDS;

    // Return the resulting object
    return messageOut;
}

Color_BT Bluetooth::PBFromColor_(Color_ *color_)
{
    // Intialize the Color_BT Message object
    Color_BT color_bt = Color_BT_default;

    // Depending on what kind of Color_ we're filling it with, do different things
    if (color_->getType() == COLOR_STATIC)
    {
        // If it's a static Color_, well...this is easy!
        color_bt.type = ColorType_STATIC_BT;

        // Add the only colorObj to the array
        const colorObj c = static_cast<Color_Static *>(color_)->getColor();
        color_bt.color_objs_count = 1;

        //
        //
        // TODO: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
        //
        //

        color_bt.color_objs = new ColorObj_BT[1];   // Create a new array, of size one
        color_bt.color_objs[0] = PBFromColorObj(c); // Assign the colorObj to the only position in the array
    }
    else
    {
        // The Color_ is a dynamic type, so it requires an array of colorObjs, with included metadata
        switch (color_->getType())
        {
        case COLOR_DTIME:
        {
            color_bt.type = ColorType_DTIME_BT;
            Color_dTime *thisColor_dt = (Color_dTime *)color_; // Get a pointer to the Color_ that's of the correct derived class type

            // Go through all of the colorObj's (and meta data), and add the information
            // TODO: Will need to convert T so that it can hold a float?  Or, just use long as for velocity, as well (no reason it couldn't work, right?)

            //
            //
            // TODO: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
            //
            //

            color_bt.color_objs = new ColorObj_BT[thisColor_dt->getNumColors()]; // Allocate space for the array of colorObjs that will be put into color_bt.color_objs
            color_bt.color_objs_count = thisColor_dt->getNumColors();            // Store the number of colorObjs that will be added
            for (unsigned char i = 0; i < thisColor_dt->getNumColors(); i++)
            {
                const colorObj c = thisColor_dt->getThisColorObj(i);                            // Get the colorObj (and don't modify it, because it's a reference!)
                color_bt.color_objs[i] = PBFromColorObj(c);                                     // Save the colorObj (Without metadata)
                color_bt.color_objs[i].bt = PBFromBlendType(thisColor_dt->getThisBlendType(i)); // Add the Blend type to the ColorObj_BT
                color_bt.color_objs[i].t = thisColor_dt->getThisTrigger(i);                     // Add the T value to the ColorObj_BT
            }

            break;
        }
        case COLOR_DVEL:
        {
            color_bt.type = ColorType_DVEL_BT;
            Color_dVel *thisColor_dv = (Color_dVel *)color_; // Get a pointer to the Color_ that's of the correct derived class type

            // Go through all of the colorObj's (and meta data), and add the information
            // TODO: Will need to convert T so that it can hold a float?  Or, just use long as for velocity, as well (no reason it couldn't work, right?)

            //
            //
            // TODO: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
            //
            //

            color_bt.color_objs = new ColorObj_BT[thisColor_dv->getNumColors()];
            color_bt.color_objs_count = thisColor_dv->getNumColors(); // Store the number of colorObjs that will be added
            for (unsigned char i = 0; i < thisColor_dv->getNumColors(); i++)
            {
                const colorObj c = thisColor_dv->getThisColorObj(i);                            // Get the colorObj (and don't modify it, because it's a reference!)
                color_bt.color_objs[i] = PBFromColorObj(c);                                     // Save the colorObj (Without metadata)
                color_bt.color_objs[i].bt = PBFromBlendType(thisColor_dv->getThisBlendType(i)); // Add the Blend type to the ColorObj_BT
                color_bt.color_objs[i].t = thisColor_dv->getThisTrigger(i);                     // Add the T value to the ColorObj_BT
            }

            break;
        }
        }
    }

    return color_bt;
}

void Bluetooth::memfree_BT(Message_BT *mIn)
{
    // May just need to use pb_release()...?

    // Call this function after encoding and writing any Message_BT that use dynamically allocated arrays!
    // Free any dynamically allocated arrays in the Message_BT object (really is only important for BWA_BT and Kalman_BT objects, but not reason not to make this function a quick one-stop-shop for memory freeing)
    // This is safe to do on a Message_BT that hasn't had any information allocated to it, because the default forms of all Messages initializes all pointers to NULL

    // First, figure out if this message contains anything that needs to be freed
    switch (mIn->type)
    {
    case MessageType_BWA:
    {
        // Get a pointer to the BWA part of the Message
        BWA_BT *thisM = &mIn->contents.bike_wheel_anim;

        // First, free each Image (if needed)
        delete[] thisM->image_main; // Delete the main image
        thisM->image_main = NULL;
        delete[] thisM->image_idle; // If this Image type supports an idle image, delete it (if it does not support image_idle, nothing will happen because image_idle is a NULL pointer)
        thisM->image_idle = NULL;

        // Next, go through each Color_ and free the colorObj arrays (and possibly the Color_'s themselves...?)
        for (unsigned char i = 0; i < thisM->palette_count; i++)
        {
            delete[] thisM->palette[i].color_objs;
            thisM->palette[i].color_objs = NULL;
        }

        break;
    }
    case MessageType_Kalman:
    {
        // Get a pointer to the Kalman part of the Message
        Kalman_BT *thisM = &mIn->contents.kalman;

        // Free the r and p0 parameters
        delete[] thisM->r;
        thisM->r = NULL;
        delete[] thisM->p0;
        thisM->p0 = NULL;

        break;
    }
    }
}

ColorObj_BT Bluetooth::PBFromColorObj(const colorObj &c)
{
    ColorObj_BT c_bt = ColorObj_BT_default;
    c_bt.r = (uint32_t)c.r();
    c_bt.b = (uint32_t)c.b();
    c_bt.g = (uint32_t)c.g();
    c_bt.w = (uint32_t)c.w();

    return c_bt;
}

Kalman_BT Bluetooth::PBFromKalman()
{
    Kalman_BT kalman_bt = Kalman_BT_default;

    // Fill in the scalar values
    kalman_bt.n_obs = N_OBS;
    kalman_bt.n_sta = N_STA;
    kalman_bt.q = speedometer->getKalman()->getQ();

    // Set the sizes of the matrix parameters
    kalman_bt.p0_count = N_STA * N_STA;
    kalman_bt.r_count = N_OBS * N_OBS;
    kalman_bt.p0 = new float[N_STA * N_STA];
    kalman_bt.r = new float[N_OBS * N_OBS];

    // Allocate the parameter arrays
    for (unsigned char col = 0; col < N_STA; col++)
    {
        for (unsigned char row = 0; row < N_STA; row++)
        {
            kalman_bt.p0[row + N_STA * col] = speedometer->getKalman()->getP0()[row][col]; // Oh god, I hope this crap doesn't bite me in the ass...
        }
    }

    for (unsigned char col = 0; col < N_OBS; col++)
    {
        for (unsigned char row = 0; row < N_OBS; row++)
        {
            kalman_bt.r[row + N_STA * col] = speedometer->getKalman()->getR()[row][col]; // Oh god, I hope this crap doesn't bite me in the ass again...
        }
    }

    // Return the newly constructed object
    return kalman_bt;
}

ImageType_BT Bluetooth::PBFromImageType(Pattern_Handler *pattern_handler_in)
{
    if (pattern_handler_in->mainPattern->doesAllowIdle())
    {
        return ImageType_CONSTANT_BT;
    }
    else
    {
        return ImageType_SPINNER_BT;
    }
}

BlendType_BT Bluetooth::PBFromBlendType(BLEND_TYPE blendType)
{
    // Convert a BLEND_TYPE to a local BlendType_BT (because reasons)
    switch (blendType)
    {
    case B_CONSTANT:
        return BlendType_CONSTANT_BT;
        break;
    case B_LINEAR:
        return BlendType_LINEAR_BT;
        break;
    default:
        // Uh oh...
        return BlendType_CONSTANT_BT;
    }
}

ColorType_BT Bluetooth::PBFromColorType(COLOR_TYPE colorType)
{
    // Convert a COLOR_TYPE to a local ColorType_BT (because reasons)
    switch (colorType)
    {
    case COLOR_STATIC:
        return ColorType_STATIC_BT;
        break;
    case COLOR_DTIME:
        return ColorType_DTIME_BT;
        break;
    case COLOR_DVEL:
        return ColorType_DVEL_BT;
        break;
    default:
        // Uh oh...
        return ColorType_STATIC_BT;
    }
}