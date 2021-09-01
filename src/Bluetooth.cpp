#include "Bluetooth.h"
#include "Pattern.h"
#include "Speedometer.h"
#include "Definitions.h"

// Bit-wise reading functions
unsigned char getNUIntFromByte(unsigned char byte, unsigned char intSize, unsigned char firstBitPos)
{
    // Get some n-sized unsigned integer from a byte.
    // NOTE: intsize MUST be less than 8!!!  No checks will be performed!!!
    // For example, an unsigned int is defined by the 3 bytes indicated by X in 0boooXXXoo, this function should be called as getNIntFromByte(0boooXXXoo, 3, 2)
    unsigned char bitMask = (1U << intSize) - 1U; // Bit-mask generation idea from John Gietzan, on Stack Exchange (https://stackoverflow.com/a/1392065)
    return (byte >> firstBitPos) & bitMask;
}

signed char getNSIntFromByte(unsigned char byte, unsigned char intSize, unsigned char firstBitPos)
{
    // Get some n-sized signed integer from a byte.
    // NOTE: intsize MUST be less than 8!!!  No checks will be performed!!!
    // For example, an unsigned int is defined by the 3 bytes indicated by X in 0boooXXXoo, this function should be called as getUIntFromByte(0boooXXXoo, 3, 2)
    unsigned char bitMask = (1U << intSize) - 1U; // Bit-mask generation idea from John Gietzan, on Stack Exchange (https://stackoverflow.com/a/1392065)
    return (byte >> firstBitPos) & bitMask;
}

bool getBoolFromByte(unsigned char byte, unsigned char bitPos)
{
    // Extract a single bit from a byte, and interpret it as a boolean.
    // bitPos is the number of the bit (starting with the right-most bit as 0)
    return (byte >> bitPos) & 1U; // Shift the byte bitPos bits to the right, so the bit of interest is in the least-significant position.  Then, mask it with 1 (0b00000001)
}

// Byte-wise reading functions
void putZerosToByte(unsigned char &byteDest, unsigned char clearSize, unsigned char bitLoc)
{
    // Will write all zeros to a defined position in byteDest, to make writing a pattern of bits easier
    byteDest &= ~(((1U << clearSize) - 1U) << bitLoc);
}

void putMaskExceptDataToByte(unsigned char &byteDest, unsigned char dataSize, unsigned char bitLoc)
{
    // Will write all zeros to all but a defined position in byteDest, to make writing a pattern of bits more robust
    // byteDest &= ((1U << dataSize) - 1U) << bitLoc;
    
    byteDest &= ((1U << dataSize) - 1); // Mask out only the data we want from the byte, limited by size
    byteDest = byteDest << bitLoc; // Move the newly masked data to the bit location
}

unsigned long reverseBits(unsigned long num)
{
    // Taken from https://www.geeksforgeeks.org/reverse-actual-bits-given-number/
    unsigned long count = sizeof(num) * 8 - 1;
    unsigned long reverse_num = num;

    num >>= 1;
    while (num)
    {
        reverse_num <<= 1;
        reverse_num |= num & 1;
        num >>= 1;
        count--;
    }
    reverse_num <<= count;
    return reverse_num;
}

unsigned long getLongFromByteArray(unsigned char *byteArray, unsigned char firstBytePos)
{
    // TODO: CHECK THIS CODE!!!
    // Convert a series of 4 bytes, located at some point firstBytePos in the array pointed to by byteArray, to an unsigned long

    // Create an array to be reinterpretted
    unsigned char longByteArray[4];
    // Copy the 4 bytes from byteArray, starting at firstBytePos, to longByteArray
    for (int i = 0; i < 4; i++)
    {
        // I do not fucking trust the memory copying functions, probably due to my own incompetence, but still I'll do it the stupid way and let the goddamn compiler deal with it.
        longByteArray[i] = byteArray[i + firstBytePos];
    }

    // Convert the byteArray to a float pointer, then dereference the pointer to get a float
    return *((unsigned long *)longByteArray); // Converted to a long, because C++ is stupid and doesn't let me shift bytes (>> or <<) on a float...

    // return *((unsigned long *)longByteArray); // Type conversion idea from imreal, on Stack Exchange (https://stackoverflow.com/a/22029162)
}

float getFloatFromByteArray(unsigned char *byteArray, unsigned char firstBytePos)
{
    // Convert a series of 4 bytes, located at some point firstBytePos in the array pointed to by byteArray, to a float

    // Create an array to be reinterpretted
    unsigned char intByteArray[4];
    // Copy the 4 bytes from byteArray, starting at firstBytePos, to longByteArray
    for (int i = 0; i < 4; i++)
    {
        // I do not fucking trust the memory copying functions, probably due to my own incompetence, but still I'll do it the stupid way and let the goddamn compiler deal with it.
        intByteArray[i] = byteArray[i + firstBytePos];
    }

    unsigned long floatInt = reverseBits(*((unsigned long *)intByteArray)); // Converted to a long, because C++ is stupid and doesn't let me shift bytes (>> or <<) on a float...

    return *reinterpret_cast<float *>(&floatInt); // Reinterpret the reversed-bits long to a float and return
    // return *((float *)intByteArray); // Type conversion idea from imreal, on Stack Exchange (https://stackoverflow.com/a/22029162)
}

// Bit-wise writing functions
void putBoolToByte(unsigned char &byteDest, bool data, unsigned char bitLoc)
{
    // Right-most bit is 0, left-most is 7

    // Algorithm from Jeremy Ruten on Stack Exchange (https://stackoverflow.com/a/47990)
    if (data)
    {
        // If we are writing a 1 to the bit at bitLoc
        byteDest |= 1U << bitLoc;
    }
    else
    {
        // If we are writing a 0 to the bit at bitLoc
        byteDest &= ~(1U << bitLoc);
    }
}

void putDataToByte(unsigned char &byteDest, unsigned char data, unsigned char intSize, unsigned char bitLoc)
{
    // Will write an intSize-bit unsigned integer into the byte referenced by byteDest, at bit location bitLoc
    // bitLoc indicates the position of the right-most bit in the data, where the right-most bit in the byte is 0.  I.e. making a zero'd byte to look like (0b01111000) would be done by calling putDataToByte(zeroByte, 15, 4, 3)

    putZerosToByte(byteDest, intSize, bitLoc);      // First, clear the indicated bits in the destination, so that new data can be placed there
    putMaskExceptDataToByte(data, intSize, bitLoc); // Second, clear all except the indicated bits in data so that no spare bits interfere with placing the data
    byteDest |= data;                               // Data is clear everywhere except the data, and byteDest is clear in the data location, so only the data pattern will be written to byteDest
}

void putLongToByteArray(unsigned char outputCharArray[4], unsigned long inputLong)
{
    *((unsigned long *)outputCharArray) = inputLong; // TODO: Does this even goddamn work...?
}

void putFloatToByteArray(unsigned char outputCharArray[4], float inputFloat)
{
    *((float *)outputCharArray) = inputFloat; // TODO: Does this even goddamn work...?
}

unsigned char *putFloatMatToByteArray(const float **inputMat)
{
    // Reinterpret the incoming float 2D matric as an unsigned char array, where each group of 4 unsigned chars represents a float
    // TODO: CHECK THIS FREAKING CODE!!!
    return reinterpret_cast<unsigned char *>(inputMat);
}

BLEND_TYPE getBlendTypeFromByte(unsigned char byte)
{
    switch (byte)
    {
    case 0:
        return B_CONSTANT;
        break;
    case 1:
        return B_LINEAR;
        break;
    default:
        return B_CONSTANT;
    }
}

unsigned char getByteFromBlendType(BLEND_TYPE blend_type_in)
{
    switch (blend_type_in)
    {
    case B_CONSTANT:
        return 0U;
        break;

    case B_LINEAR:
        return 1U;
        break;
    default:
        return 0U;
    }
}

Bluetooth::Bluetooth(Pattern_Handler *pattern_handler, Speedometer *speedometer) : pattern_handler(pattern_handler), speedometer(speedometer)
{
    totalMemory = freeRam();
}

Bluetooth::~Bluetooth()
{
    delete btSer; // Delete the btSer object (always defined, because it is instantiated in the .h file)
}

void Bluetooth::mainLoop()
{
    // Check if there is anything on the serial line
    int count = btSer->available();
    if (count > 0)
    {
        btSer->resetCommunicationData(); // Initialize btSer for the incoming data (TODO: Make sure this is kosher...)

        if (DEBUGGING_BLUETOOTH)
        {
            Serial.println(F("Got data..."));
        }

        if (count > MAX_BT_BUFFER_SIZE)
        {
            // SOMETHING HAS GONE HORRIBLY WRONG!!!
            if (DEBUGGING_BLUETOOTH)
            {
                Serial.println(F("WE'RE ALL GONNA DIE..."));
                Serial.println();
                Serial.println(F("Received:"));
                
                for (int i = 0; i < count ;i++) {
                    unsigned char buffer[1];
                    btSer->stream->readBytes(buffer, 1);
                    printByte(*buffer);
                    Serial.println();
                }
            }
        }
        else
        {
            // Before any of this stuff, store the current amount of freeRam, in case the user asks for it (don't want to taint the answer will all of the memory we'll need to do the message processing)
            int freeRamNow = freeRam();

            // Read the meta-data from the information that is now in the Bluetooth buffer, to initialize btSer member variables.
            bool isValid = btSer->initReceiveMessage();
            if (!isValid) {
                // If it is "not valid", it means that it's either an error or a debug code, so no further processing is needed
                return;
            }

            // while (messageNum < totalNumMessages) {
            if (!btSer->isRequest())
            {
                if (DEBUGGING_BLUETOOTH)
                {
                    Serial.println(F("Message contains content..."));
                }
                // If this message contains content sent by the Android program, then read the message and update the Arduino
                switch (btSer->getContent())
                {
                case 0:
                {
                    // Receiving Bike wheel animation
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Receiving BWA..."));
                    }

                    // First, get the invariate information from the head of the BWA message, the meta-data of the palette and image

                    unsigned char incomingNumLEDs;
                    unsigned char totalNumColor_s;

                    btSer->readNextMessageByte(incomingNumLEDs); // Get the number of LEDs that the Android thinks we have
                    btSer->readNextMessageByte(totalNumColor_s); // Get the number of Color_'s that are in the palette

                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.print(incomingNumLEDs);
                        Serial.println(F(" LEDs"));
                        Serial.print(totalNumColor_s);
                        Serial.println(F(" Color_('s):"));
                        Serial.println();
                    }

                    pattern_handler->setupPalette(1); // Set up the palette with a single, static Color_, minimizing the RAM requirements

                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F("**********BT: Allocating Color_ Array: "));
                        Serial.print(freeRam());
                    }
                    Color_ **newColor_Array = new Color_ *[totalNumColor_s]; // Create an array of pointers to Color_ objects that will be creaforted soon.  This array will be *copied*.
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F(" -> "));
                        Serial.println(freeRam());
                    }

                    // Start saving the palette information
                    for (unsigned char thisColor_Num = 0; thisColor_Num < totalNumColor_s; thisColor_Num++)
                    {
                        if (DEBUGGING_PATTERN_SIZE) {
                            Serial.print(F("**********BT: Creating Color: "));
                            Serial.print(freeRam());
                        }
                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(thisColor_Num);
                            Serial.print(F(": "));
                        }

                        // First, get this Color_ type
                        unsigned char thisColor_Type;
                        btSer->readNextMessageByte(thisColor_Type);              // Get the color type from the message
                        thisColor_Type = getNUIntFromByte(thisColor_Type, 4, 4); // Save the first nibble to get the color type

                        // Depending on the type of Color_ we are expecting for this position in the palette, intepret the data differently
                        switch (thisColor_Type)
                        {
                        case 0:
                        {
                            // Static Color
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.print(F("Static Color: "));
                            }
                            // Get the RBGW values from the message
                            unsigned char incomingColors[NUMLIGHTSPERLED];
                            btSer->readNextMessageBytes(incomingColors, NUMLIGHTSPERLED);

                            if (DEBUGGING_BLUETOOTH)
                            {
                                serialPrintColor(incomingColors);
                                Serial.println();
                            }

                            // Create a new Static Color_ object from this array
                            newColor_Array[thisColor_Num] = new Color_Static(incomingColors);
                            break;
                        }

                        case 1:
                        {
                            // Time-based Color_
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F("Time-based Color"));
                            }

                            // This is a dynamic Color_, so the loading process will be quite similar
                            unsigned char numColorObj_metas;
                            btSer->readNextMessageByte(numColorObj_metas); // Get the number of colorObj_metas that describe this Color_d

                            // Set up the arrays that will be used to initialize the Color_d
                            colorObj *newColorObjs = new colorObj[numColorObj_metas];
                            BLEND_TYPE *newBs = new BLEND_TYPE[numColorObj_metas];
                            unsigned long *newTimeTs = new unsigned long[numColorObj_metas];

                            for (unsigned char colorObjNum = 0; colorObjNum < numColorObj_metas; colorObjNum++)
                            {
                                // For every colorObjNum, get all of the information (LED brightnesses, T, and blend types)
                                if (DEBUGGING_BLUETOOTH)
                                {
                                    Serial.print(colorObjNum);
                                    Serial.print(F(": "));
                                }

                                {
                                    // Get the colorObj for this index in the Color_d
                                    unsigned char thisColorObjArray[NUMLIGHTSPERLED];
                                    btSer->readNextMessageBytes(thisColorObjArray, NUMLIGHTSPERLED);
                                    newColorObjs[colorObjNum] = colorObj(thisColorObjArray);

                                    if (DEBUGGING_BLUETOOTH)
                                    {
                                        serialPrintColor(thisColorObjArray);
                                    }
                                }

                                {
                                    // Get the Blend type for this colorObj
                                    unsigned char thisBlendTypeByte;
                                    btSer->readNextMessageByte(thisBlendTypeByte);
                                    newBs[colorObjNum] = getBlendTypeFromByte(thisBlendTypeByte);

                                    if (DEBUGGING_BLUETOOTH)
                                    {
                                        Serial.print(F(", B = "));
                                        switch (newBs[colorObjNum])
                                        {
                                        case B_LINEAR:
                                            Serial.print(F("Lin"));
                                            break;
                                        case B_CONSTANT:
                                            Serial.print(F("Con"));
                                            break;
                                        }
                                    }
                                }

                                {
                                    // Get the T for this colorObj
                                    unsigned char thisTValArray[4];
                                    btSer->readNextMessageBytes(thisTValArray, 4);
                                    newTimeTs[colorObjNum] = getLongFromByteArray(thisTValArray, 0);

                                    if (DEBUGGING_BLUETOOTH)
                                    {
                                        Serial.print(F(", T = "));
                                        Serial.println(newTimeTs[colorObjNum]);
                                    }
                                }
                            }

                            // Create the Color_dTime object
                            newColor_Array[thisColor_Num] = new Color_dTime(newColorObjs, newTimeTs, newBs, numColorObj_metas); // These arrays are not deleted, because they will now be owned by the Color_dTime
                            break;
                        }

                        case 2:
                        {
                            // Velocity-based Color_
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F("Time-based Color"));
                            }

                            // This is a dynamic Color_, so the loading process will be quite similar
                            unsigned char numColorObj_metas;
                            btSer->readNextMessageByte(numColorObj_metas); // Get the number of colorObj_metas that describe this Color_d

                            // Set up the arrays that will be used to initialize the Color_d
                            colorObj *newColorObjs = new colorObj[numColorObj_metas];
                            BLEND_TYPE *newBs = new BLEND_TYPE[numColorObj_metas];
                            float *newVelTs = new float[numColorObj_metas];

                            for (unsigned char colorObjNum = 0; colorObjNum < numColorObj_metas; colorObjNum++)
                            {
                                // For every colorObjNum, get all of the information (LED brightnesses, T, and blend types)
                                if (DEBUGGING_BLUETOOTH)
                                {
                                    Serial.print(colorObjNum);
                                    Serial.print(F(": "));
                                }

                                {
                                    // Get the colorObj for this index in the Color_d
                                    unsigned char thisColorObjArray[NUMLIGHTSPERLED];
                                    btSer->readNextMessageBytes(thisColorObjArray, NUMLIGHTSPERLED);
                                    newColorObjs[colorObjNum] = colorObj(thisColorObjArray);

                                    if (DEBUGGING_BLUETOOTH)
                                    {
                                        serialPrintColor(thisColorObjArray);
                                    }
                                }

                                {
                                    // Get the Blend type for this colorObj
                                    unsigned char thisBlendTypeByte;
                                    btSer->readNextMessageByte(thisBlendTypeByte);
                                    newBs[colorObjNum] = getBlendTypeFromByte(thisBlendTypeByte);

                                    if (DEBUGGING_BLUETOOTH)
                                    {
                                        Serial.print(F(", B = "));
                                        switch (newBs[colorObjNum])
                                        {
                                        case B_LINEAR:
                                            Serial.print(F("Lin"));
                                            break;
                                        case B_CONSTANT:
                                            Serial.print(F("Con"));
                                            break;
                                        }
                                    }
                                }

                                {
                                    // Get the T for this colorObj
                                    unsigned char thisTValArray[4];
                                    btSer->readNextMessageBytes(thisTValArray, 4);
                                    newVelTs[colorObjNum] = getFloatFromByteArray(thisTValArray, 0);

                                    if (DEBUGGING_BLUETOOTH)
                                    {
                                        Serial.print(F(", T = "));
                                        Serial.println(newVelTs[colorObjNum]);
                                    }
                                }
                            }

                            // Create the Color_dVel object
                            newColor_Array[thisColor_Num] = new Color_dVel(speedometer, newColorObjs, newVelTs, newBs, numColorObj_metas); // These arrays are not deleted, because they will now be owned by the Color_dVel
                            break;
                        }
                        }

                        if (DEBUGGING_PATTERN_SIZE) {
                            Serial.print(F(" -> "));
                            Serial.println(freeRam());
                        }
                    }

                    // Now that we have a complete array of Color_'s, assign it to the Pattern_Handler
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F("**********BT: Setting up palette: "));
                        Serial.print(freeRam());
                    }
                    pattern_handler->setupPalette(newColor_Array, totalNumColor_s);
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F(" -> "));
                        Serial.println(freeRam());
                    }

                    // Next, start saving the Pattern information
                    // Delete the current patterns from the pattern_handler
                    // TODO: There IS a memory leak in here somewhere, but it was mitigated by the pattern_handler->deletePatterns() line.  It appears to be manageable (not exapanding beyond a certain size), so for now it is low priority
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F("**********BT: Deleting Patterns: "));
                        Serial.print(freeRam());
                    }
                    pattern_handler->deletePatterns();
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F(" -> "));
                        Serial.println(freeRam());
                    }

                    // First, get the palette-specific meta-data
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F("**********BT: Reading Image meta-data: "));
                        Serial.println(freeRam());
                    }
                    bool supportsIdle;
                    unsigned char mainImageType;
                    signed char mainImageParam; // The parameter for the image type (signed to allow for negative rotation rates, etc.)

                    {
                        // Get the supports idle bool, as well as the image type
                        unsigned char supportsIdleImageTypeByte;
                        btSer->readNextMessageByte(supportsIdleImageTypeByte);

                        supportsIdle = getBoolFromByte(supportsIdleImageTypeByte, 4);
                        mainImageType = getNUIntFromByte(supportsIdleImageTypeByte, 4, 0);

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.println(F("Main Image:"));
                            Serial.print(F("Supports idle: "));
                            Serial.println(supportsIdle);
                        }
                    }

                    {
                        // Get the parameter for this image type
                        unsigned char imageParamByte;
                        btSer->readNextMessageByte(imageParamByte);
                        mainImageParam = getNSIntFromByte(imageParamByte, 8, 0);

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Main Image Param: "));
                            Serial.println(mainImageParam);
                        }
                    }
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F(" -> "));
                        Serial.println(freeRam());
                    }

                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F("**********BT: Preparing Image Helper: "));
                        Serial.println(freeRam());
                    }
                    Image_Helper *main_image_helper; // Declare the image helper that will be defined below and used to create the Pattern
                    bool groundRel = true;           // Is the Pattern's motion defined relative to the wheel, or relative to the ground?
                    switch (mainImageType)
                    {
                    case 0:
                        // Wheel-relative motion
                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.println(F("Wheel-rel"));
                        }

                        groundRel = false; // The motion is relative to the wheel, not the ground

                        // Create a different Image_Helper depending on whether or not there is motion (if there is no motion, we can save some RAM)
                        if (mainImageParam == 0)
                        {
                            main_image_helper = new Static_Helper();
                        }
                        else
                        {
                            // If there is some movement relative to the wheel, use a slightly different Pattern type
                            main_image_helper = new Moving_Helper(mainImageParam);
                        }
                        break;
                    case 1:
                        // Ground-relative motion
                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.println(F("Ground-rel"));
                        }

                        // Create a different Image_Helper depending on whether or not there is motion (if there is no motion, we can save some RAM)
                        if (mainImageParam == 0)
                        {
                            main_image_helper = new Static_Helper();
                        }
                        else
                        {
                            // If there is some movement relative to the wheel, use a slightly different Pattern type
                            main_image_helper = new Moving_Helper(mainImageParam);
                        }
                        break;
                    case 2:
                        // Spinner wheel
                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.println(F("Spinner"));
                        }

                        groundRel = false; // The motion is relative to the wheel, not the ground
                        main_image_helper = new Spinner_Helper(mainImageParam);
                        break;
                    default:
                        main_image_helper = new Static_Helper();
                    }

                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F(" -> "));
                        Serial.println(freeRam());
                    }

                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F("**********BT: Creating Main Pattern: "));
                        Serial.println(freeRam());
                    }
                    Pattern *mainPattern = new Pattern(pattern_handler, main_image_helper, groundRel); // Define the new Pattern that will be constructed based on the information from the message, then sent to the Pattern_Handler
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F(" -> "));
                        Serial.print(freeRam());
                    }

                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.print(F("Before setting image: curMessageNum = "));
                        Serial.println(btSer->getCurMessageNum());
                    }
                    
                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F("**********BT: Setting Main Pattern: "));
                        Serial.print(freeRam());
                    }
                    // Read in the main Image (could probably change this into a one-liner by writing the data directly into the image of the Pattern...but whatever)
                    mainPattern->setImageFromBluetooth(btSer);
                    // Save this Pattern to the Pattern_Handler
                    pattern_handler->setMainPattern(mainPattern);

                    if (DEBUGGING_PATTERN_SIZE) {
                        Serial.print(F(" -> "));
                        Serial.println(freeRam());
                    }

                    // If this pattern supports an idle image, read the data from the message about the idle image
                    if (supportsIdle)
                    {
                        // First, get the image type for the idle image
                        unsigned char idleImageType;
                        signed char idleImageParam;

                        // Get the idle image type
                        btSer->readNextMessageByte(idleImageType);
                        {
                            // Get the idle image parameter
                            unsigned char idleImageParamByte;
                            btSer->readNextMessageByte(idleImageParamByte);
                            idleImageParam = getNSIntFromByte(idleImageParamByte, 8, 0);
                        }

                        Image_Helper *idle_image_helper; // Declare the image helper that will be defined below and used to create the Pattern
                        switch (idleImageType)
                        {
                        case 0:
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F("Wheel-rel"));
                            }
                            // Wheel-relative motion
                            // NOTE: This is the only idle pattern that is defined at the moment

                            // Create a different Image_Helper depending on whether or not there is motion (if there is no motion, we can save some RAM)
                            if (idleImageParam == 0)
                            {
                                // if (DEBUGGING_BLUETOOTH)
                                // {
                                //     Serial.println(F("Making Static Helper..."));
                                // }
                                idle_image_helper = new Static_Helper();
                            }
                            else
                            {
                                // If there is some movement relative to the wheel, use a slightly different Pattern type
                                idle_image_helper = new Moving_Helper(idleImageParam);
                                // if (DEBUGGING_BLUETOOTH)
                                // {
                                //     Serial.print(F("Making Moving Helper with "));
                                //     Serial.println(idle_image_helper->getHelperParameter());
                                // }
                            }
                            break;
                        default:
                            idle_image_helper = new Static_Helper();
                        }

                        if (DEBUGGING_PATTERN_SIZE) {
                            Serial.print(F("**********BT: Creating Idle Pattern: "));
                            Serial.print(freeRam());
                        }
                        Pattern *idlePattern = new Pattern(pattern_handler, idle_image_helper, false); // Define a Pattern using the Image_Helper we just defined, enforcing that it will be using wheel_relative motion
                        if (DEBUGGING_PATTERN_SIZE) {
                            Serial.print(F(" -> "));
                            Serial.println(freeRam());
                        }

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Idle Image Param: "));
                            Serial.println(idleImageParam);
                            // Serial.print(F("Idle Image Param from Helper: "));
                            // Serial.println(idle_image_helper->getHelperParameter());
                            // Serial.print(F("Idle Image Param from Pattern: "));
                            // Serial.println(idlePattern->getHelperParameter());
                        }

                        if (DEBUGGING_PATTERN_SIZE) {
                            Serial.print(F("**********BT: Setting Idle Pattern: "));
                            Serial.print(freeRam());
                        }
                        
                        // Read in the idle Image
                        idlePattern->setImageFromBluetooth(btSer);
                        pattern_handler->setIdlePattern(idlePattern);

                        if (DEBUGGING_PATTERN_SIZE) {
                            Serial.print(F(" -> "));
                            Serial.println(freeRam());
                        }
                    }

                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Resetting Kalman..."));
                    }
                    speedometer->getKalman()->resetFilter(); // Finally, reset the Kalman filter

                    break;
                }
                case 1:
                {
                    // Receiving Kalman Info
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Receiving Kalman..."));
                    }

                    unsigned char numObs;   // Number of observed parameters
                    unsigned char numState; // Number of state parameters

                    {
                        // First, get the number of observed and state parameters
                        unsigned char obsStateByte;
                        btSer->readNextMessageByte(obsStateByte);
                        numObs = getNUIntFromByte(obsStateByte, 4, 4);
                        numState = getNUIntFromByte(obsStateByte, 4, 0);

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("nObs: "));
                            Serial.println(numObs);
                            Serial.print(F("nState: "));
                            Serial.println(numState);
                        }

                        // TODO: Error check if the observation and state variable numbers are the same between Arduino and Android?
                    }

                    {
                        // Next, read in 4 bytes that will be saved as Q
                        unsigned char qByteArray[4];
                        btSer->readNextMessageBytes(qByteArray, 4);
                        speedometer->getKalman()->setPhi(getFloatFromByteArray(qByteArray, 0)); // Set the Q value in the Kalman filter

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Q: "));
                            Serial.println(getFloatFromByteArray(qByteArray, 0));
                        }
                    }

                    {
                        // Finally, read in the series of floats that will become the R and P0 matrices
                        unsigned char thisByteArray[4];

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.println(F("R:"));
                        }
                        for (int row = 0; row < numObs; row++)
                        {
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.print(F("[ "));
                            }

                            for (int col = 0; col < numObs; col++)
                            {
                                btSer->readNextMessageBytes(thisByteArray, 4); // Get the next float's worth of bytes

                                speedometer->getKalman()->setRElem(row, col, getFloatFromByteArray(thisByteArray, 0)); // Read this new float into the PPrior matrix for the Kalman filter

                                if (DEBUGGING_BLUETOOTH)
                                {
                                    Serial.print(getFloatFromByteArray(thisByteArray, 0));
                                    Serial.print(F(" "));
                                }
                            }

                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F(" ]"));
                            }
                        }

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.println(F("P0:"));
                        }
                        for (int row = 0; row < numState; row++)
                        {
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.print(F("[ "));
                            }

                            for (int col = 0; col < numState; col++)
                            {
                                btSer->readNextMessageBytes(thisByteArray, 4); // Get the next float's worth of bytes

                                speedometer->getKalman()->setP0Elem(row, col, getFloatFromByteArray(thisByteArray, 0)); // Read this new float into the PPrior matrix for the Kalman filter

                                if (DEBUGGING_BLUETOOTH)
                                {
                                    Serial.print(getFloatFromByteArray(thisByteArray, 0));
                                    Serial.print(F(" "));
                                }
                            }

                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F(" ]"));
                            }
                        }
                    }

                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Resetting Kalman..."));
                    }
                    speedometer->getKalman()->resetFilter(); // Reset the Kalman filter

                    break;
                }

                case 2:
                {
                    // Receiving Brightness scale
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Receiving Brightness..."));
                    }
                    {
                        // Read in the brightness factor from the Message
                        unsigned char brightnessFactorByteArray[4];
                        btSer->readNextMessageBytes(brightnessFactorByteArray, 4);
                        pattern_handler->setBrightnessFactor(getFloatFromByteArray(brightnessFactorByteArray, 0));

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Brightness: "));
                            Serial.println(getFloatFromByteArray(brightnessFactorByteArray, 0));
                        }
                    }
                    break;
                }

                case 5:
                {
                    // Receiving Power State
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Receiving Power State..."));
                    }
                    {
                        // Read in the power state from the Message
                        unsigned char powerStateByte;
                        btSer->readNextMessageByte(powerStateByte);
                        bool powerState = getBoolFromByte(powerStateByte, 7);
                        pattern_handler->setPowerState(powerState);

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Power state: "));
                            Serial.println(powerState);
                        }
                    }
                }
                }

                if (DEBUGGING_BLUETOOTH)
                {
                   Serial.println(F("Done receiving."));
                }

                // Send one last confirmation message, because readNextMessageBytes() doesn't have any way of knowing how many bytes are expected per message, and therefore can't send a confirmation when we're done reading
                btSer->sendConfirmation();
            }
            else
            {
                // This message is a request for information from the Arduino

                // btSer->sendConfirmation(); // Let the Android know that we just received the request, and will respond shortly

                if (DEBUGGING_BLUETOOTH)
                {
                    Serial.println(F("Message contains a request..."));
                }
                // Start sending a message back, starting with the metadata
                unsigned char content = btSer->getContent();
                btSer->writeMetadata(false, content);

                switch (content)
                {
                case 0:
                {
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Sending BWA..."));
                    }
                    // Sending Bike wheel animation
                    // Send the firsts byte with the BWA meta-data
                    btSer->writeNextMessageByte((unsigned char)NUMLEDS); // TODO: Update
                    btSer->writeNextMessageByte(pattern_handler->getNumColors());

                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.print(F("Number of LEDs: "));
                        Serial.println(NUMLEDS); // TODO: Update
                        Serial.print(F("Number of Colors: "));
                        Serial.println(pattern_handler->getNumColors());
                    }

                    // Go through each Color_ in the palette, and encode it into the message
                    for (unsigned char colorNum = 0; colorNum < pattern_handler->getNumColors(); colorNum++)
                    {
                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Color "));
                            Serial.print(colorNum + 1);
                            Serial.print(F(": "));
                        }

                        // First, get the type of color
                        switch (pattern_handler->getColor(colorNum)->getType())
                        {

                        case COLOR_STATIC:
                        {
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F("Static"));
                            }

                            // First, send the color type as an unsigned char
                            {
                                unsigned char colorNumByte = 0U; // An entire byte of zeros has a 0 in the correct place to denote a Static Color_
                                btSer->writeNextMessageByte(colorNumByte);
                            }

                            // Next and finally, send the RGBW values of the static color as a group of 4 bytes
                            btSer->writeNextMessageBytes(pattern_handler->getColor(colorNum)->getColor().c, 4);
                            
                            if (DEBUGGING_BLUETOOTH)
                            {
                                printColorBytes(pattern_handler->getColor(colorNum)->getColor().c);
                            }
                            break;
                        }
                        case COLOR_DTIME:
                        {
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F("dTime"));
                            }

                            const Color_dTime *thisColor_dTime = static_cast<const Color_dTime *>(pattern_handler->getColor(colorNum));

                            // First, send the color type as an unsigned char
                            {
                                unsigned char colorNumByte = 0U;
                                putDataToByte(colorNumByte, 1U, 4U, 4);
                                btSer->writeNextMessageByte(colorNumByte);
                            }

                            // Next, send the number of colorObj_meta's that are going to be sent
                            btSer->writeNextMessageByte(thisColor_dTime->getNumColors());

                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.print(thisColor_dTime->getNumColors());
                                Serial.println(F(" colors"));
                            }

                            // Go through each colorObj_meta held by this Color_, and send them one at a time
                            for (unsigned char colorObjNum = 0; colorObjNum < thisColor_dTime->getNumColors(); colorObjNum++)
                            {
                                // For each colorObj_meta held by this Color_, send the data over Bluetooth

                                // First, send the RGBW data
                                btSer->writeNextMessageBytes(thisColor_dTime->getThisColorObj(colorObjNum).c, 4);

                                // Next, send the Blend Type
                                btSer->writeNextMessageByte(getByteFromBlendType(thisColor_dTime->getThisBlendType(colorObjNum)));

                                // Finally, send the T value
                                {
                                    unsigned char thisLongCharArray[4];                                                  // A byte array to hold the byte-wise representation of the trigger value
                                    putLongToByteArray(thisLongCharArray, thisColor_dTime->getThisTrigger(colorObjNum)); // Assign the trigger value to the byte array
                                    btSer->writeNextMessageBytes(thisLongCharArray, 4);                                  // Send the byte array over the Bluetooth connection
                                }

                                if (DEBUGGING_BLUETOOTH)
                                {
                                    Serial.print(F("Color "));
                                    Serial.print(colorObjNum + 1);
                                    Serial.print(F(": "));
                                    printColorBytes(pattern_handler->getColor(colorNum)->getColor().c);
                                    Serial.print(F(", B: "));
                                    Serial.print(getByteFromBlendType(thisColor_dTime->getThisBlendType(colorObjNum)));
                                    Serial.print(F(", T: "));
                                    Serial.println(thisColor_dTime->getThisTrigger(colorObjNum));
                                }
                            }

                            break;
                        }
                        case COLOR_DVEL:
                        {
                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.println(F("dVel"));
                            }

                            const Color_dVel *thisColor_dVel = static_cast<const Color_dVel *>(pattern_handler->getColor(colorNum));

                            // First, send the color type as an unsigned char
                            {
                                unsigned char colorNumByte = 0U;
                                putDataToByte(colorNumByte, 2U, 4U, 4);
                                btSer->writeNextMessageByte(colorNumByte);
                            }

                            // Next, send the number of colorObj_meta's that are going to be sent
                            btSer->writeNextMessageByte(thisColor_dVel->getNumColors());

                            if (DEBUGGING_BLUETOOTH)
                            {
                                Serial.print(thisColor_dVel->getNumColors());
                                Serial.println(F(" colors"));
                            }

                            // Go through each colorObj_meta held by this Color_, and send them one at a time
                            for (unsigned char colorObjNum = 0; colorObjNum < thisColor_dVel->getNumColors(); colorObjNum++)
                            {
                                // For each colorObj_meta held by this Color_, send the data over Bluetooth

                                // First, send the RGBW data
                                btSer->writeNextMessageBytes(thisColor_dVel->getThisColorObj(colorObjNum).c, 4);

                                // Next, send the Blend Type
                                btSer->writeNextMessageByte(getByteFromBlendType(thisColor_dVel->getThisBlendType(colorObjNum)));

                                // Finally, send the T value
                                {
                                    unsigned char thisFloatCharArray[4];                                                  // A byte array to hold the byte-wise representation of the trigger value
                                    putFloatToByteArray(thisFloatCharArray, thisColor_dVel->getThisTrigger(colorObjNum)); // Assign the trigger value to the byte array
                                    btSer->writeNextMessageBytes(thisFloatCharArray, 4);                                  // Send the byte array over the Bluetooth connection
                                }

                                if (DEBUGGING_BLUETOOTH)
                                {
                                    Serial.print(F("Color "));
                                    Serial.print(colorObjNum + 1);
                                    Serial.print(F(": "));
                                    printColorBytes(pattern_handler->getColor(colorNum)->getColor().c);
                                    Serial.print(F(", B: "));
                                    Serial.print(getByteFromBlendType(thisColor_dVel->getThisBlendType(colorObjNum)));
                                    Serial.print(F(", T: "));
                                    Serial.println(thisColor_dVel->getThisTrigger(colorObjNum));
                                }
                            }

                            break;
                        }
                        }
                    }

                    {
                        // Send the first byte, which has information on whether or not an idle image is supported, and which image type is being used for the main image
                        unsigned char imageMainTypeByte = 0U;
                        putBoolToByte(imageMainTypeByte, pattern_handler->getMainPattern()->supportIdle(), 4);
                        putDataToByte(imageMainTypeByte, pattern_handler->getMainPattern()->getImageType(), 4, 0);
                        btSer->writeNextMessageByte(imageMainTypeByte);

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Main image type: "));
                            Serial.println(pattern_handler->getMainPattern()->getImageType());
                        }
                    }

                    // Send the parameter for the image type
                    btSer->writeNextMessageByte(pattern_handler->getMainPattern()->getHelperParameter());

                    // Finally, send the Main image
                    btSer->writeNextMessageBytes(pattern_handler->getMainPattern()->getImage(), NUM_BYTES_PER_IMAGE);

                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.print(F("Main Parameter: "));
                        Serial.println(pattern_handler->getMainPattern()->getHelperParameter());
                        // Serial.print(F("Main Image: "));
                        // for (int i = 0; i < NUM_BYTES_PER_IMAGE;i++) {
                        //     printByte(pattern_handler->getMainPattern()->getImage()[i]);
                        // }
                    }

                    // If there is an idle image, send the idle image information
                    if (pattern_handler->getMainPattern()->supportIdle())
                    {
                        // First, send the idle image type
                        btSer->writeNextMessageByte(pattern_handler->getIdlePattern()->getImageType());

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Idle image type: "));
                            Serial.println(pattern_handler->getIdlePattern()->getImageType());
                        }

                        // Next, send the idle image parameter
                        // if (DEBUGGING_BLUETOOTH) {
                        //     Serial.print(F("Idle Image Param: "));
                        //     Serial.println(pattern_handler->getIdlePattern()->getHelperParameter());
                        // }
                        btSer->writeNextMessageByte(pattern_handler->getIdlePattern()->getHelperParameter());

                        // Finally, send the idle image itself
                        btSer->writeNextMessageBytes(pattern_handler->getIdlePattern()->getImage(), NUM_BYTES_PER_IMAGE);

                        if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.print(F("Idle Parameter: "));
                        Serial.println(pattern_handler->getIdlePattern()->getHelperParameter());
                        // Serial.print(F("Idle Image: "));
                        // for (int i = 0; i < NUM_BYTES_PER_IMAGE;i++) {
                        //     printByte(pattern_handler->getIdlePattern()->getImage()[i]);
                        // }
                    }
                    }

                    break;
                }
                case 1:
                {
                    // Sending Kalman Info
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Sending Kalman..."));
                    }

                    {
                        // First, send the number of observation an state variables
                        unsigned char numObsStateByte = 0U;
                        putDataToByte(numObsStateByte, N_OBS, 4, 4);
                        putDataToByte(numObsStateByte, N_STA, 4, 0);
                        btSer->writeNextMessageByte(numObsStateByte);
                    }

                    {
                        // Next, send Q
                        unsigned char QFloatArray[4];
                        putFloatToByteArray(QFloatArray, speedometer->getKalman()->getPhi());
                        btSer->writeNextMessageBytes(QFloatArray, 4);
                    }

                    {
                        // Finally, send both R and P0

                        // First R
                        btSer->writeNextMessageBytes(putFloatMatToByteArray(speedometer->getKalman()->getR()), 4 * N_OBS * N_OBS); // An N_OBS x N_OBS matrix in which each element requires 4 bytes

                        // Then P0
                        btSer->writeNextMessageBytes(putFloatMatToByteArray(speedometer->getKalman()->getP0()), 4 * N_STA * N_STA); // An N_STA x N_STA matrix in which each element requires 4 bytes
                    }

                    break;
                }

                case 2:
                {
                    // Sending Brightness scale
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Sending Brightness..."));
                    }

                    {
                        // Send the brightness scale as an array of bytes, reprenting a single float
                        unsigned char brightnessFloatArray[4];
                        putFloatToByteArray(brightnessFloatArray, pattern_handler->getBrightnessFactor());
                        btSer->writeNextMessageBytes(brightnessFloatArray, 4);
                    }

                    break;
                }

                case 3:
                {
                    // Sending Storage
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Sending Storage info..."));
                    }

                    {
                        // First, send the amount of remaining RAM, recorded before this process started
                        unsigned char remainingRAMLongArray[4];
                        putLongToByteArray(remainingRAMLongArray, (unsigned long)freeRamNow);
                        btSer->writeNextMessageBytes(remainingRAMLongArray, 4);

                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Remaining = "));
                            Serial.println(freeRamNow);
                            Serial.println();
                        }
                    }

                    {
                        // Next, send the total amount of RAM available on the Arduino
                        if (DEBUGGING_BLUETOOTH)
                        {
                            Serial.print(F("Total = "));
                            Serial.println(totalMemory);
                            Serial.println();
                        }

                        unsigned char totalRAMLongArray[4];
                        putLongToByteArray(totalRAMLongArray, (unsigned long)totalMemory);
                        btSer->writeNextMessageBytes(totalRAMLongArray, 4);
                    }

                    break;
                }

                case 4:
                {
                    // Sending Battery
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Sending Battery info..."));
                    }

                    {
                        // TODO: ....uhhhhhh.....
                        btSer->writeNextMessageByte(255U);
                    }

                    break;
                }

                case 5:
                {
                    // Sending Power State
                    if (DEBUGGING_BLUETOOTH)
                    {
                        Serial.println(F("Sending Power State info..."));
                    }

                    {
                        unsigned char powerStateByte;
                        putBoolToByte(powerStateByte, pattern_handler->getPowerState(), 7);
                        btSer->writeNextMessageByte(powerStateByte);
                    }
                }
                }
                // REJOICE

                if (DEBUGGING_BLUETOOTH)
                {
                   Serial.println(F("Done sending."));
                }
            }

            // Finalize the connection
            btSer->sendCompletionSignal(); // Let Android know that we're done sending data
            btSer->resetCommunicationData();
        }

        // Flush the connection, to ensure there isn't any unaccounted for data
        btSer->stream->flush();
    }
}

void Bluetooth::sendStr(char * str) {
    btSer->printStr(str);
}

void Bluetooth::sendStrPROGMEM(const char * str) {
    // Print a string that is stored in program memory (if we just printed it on its own, it would read from the address that str points to in RAM, not PROGMEM)
    btSer->printPROGMEMStr(str);
}

bool Bluetooth::toggleSpeedometerDebugCode(unsigned char debugCode) {
    return speedometer->toggleDebugging(debugCode);
}

btSerialWrapper::btSerialWrapper(Stream *stream, Bluetooth * BT) : stream(stream), BT(BT)
{
#if BLUETOOTH_USE_HARDWARESERIAL
    ((HardwareSerial *)stream)->begin(BLUETOOTH_BAUD);
#else
    ((SoftwareSerial *)stream)->begin(BLUETOOTH_BAUD);
#endif

    if (DEBUGGING_BLUETOOTH_LOWLEVEL)
    {
        Serial.println(F("      Initialize btSerialWrapper..."));
    }

    stream->flush();
}

void btSerialWrapper::setCurMessageNum(int newCurMessageNum)
{
    curMessageNum = newCurMessageNum;

    if (DEBUGGING_BLUETOOTH_LOWLEVEL)
    {
        // Serial.print(F("***   curMessageNum = "));
        // Serial.println(getCurMessageNum());
    }
}

bool btSerialWrapper::initReceiveMessage()
{
    // This function reads the meta-data from the next message, if it exists, and returns if the meta-data was successfully read
    if (nextByteNum == 0)
    {
        // We have not yet read the meta-data.  Extract this first

        if (DEBUGGING_BLUETOOTH_LOWLEVEL || DEBUGGING_BTDEBUG)
        {
            Serial.println(F("      Got message..."));
        }
        // Read the first two bytes into a temporary buffer
        unsigned char tmpStreamBuf[2];
        size_t numIncomingBytes = stream->readBytes(tmpStreamBuf, 2); // Minus one to get rid of the null terminator

        // Read the data from this temporary buffer
        if (tmpStreamBuf[0] == 'd') // NOTE: 'd' == 0b01100100.  This byte series should not occur unless via terminal debugging
        {
            // If this is a debug request
            if (numIncomingBytes == 1)
            {
                // If we only read one byte, then the user is requesting the help info.
                debugCode = DEBUG_HELP_0;
            }
            else
            {
                unsigned char curBufLoc = 0;
                if (tmpStreamBuf[1] != ASCII_SPACE_32)
                {
                    // Ignore spaces
                    debugRequestChar[curBufLoc] = tmpStreamBuf[1];
                    curBufLoc++;
                }

                // Read the debug request
                stream->readBytesUntil('\n', &debugRequestChar[curBufLoc], DEBUG_BUF_SIZE - curBufLoc - 1);

                // Extract the debug code (and message, if applicable)
                extractDebugCode();
            }

            // Process the debug request
            processDebugCode();

            // Return an "error" so the remaining communication processing doesn't occur, because we've done everything we need to
            return false;
        }
        request = getBoolFromByte(tmpStreamBuf[0], 7);                          // Determine if this is a request for information, or a message containing infromation
        content = getNUIntFromByte(tmpStreamBuf[0], 3, 4);                      // Get the type of content that is being requested or sent
        totalMessages = getNUIntFromByte(tmpStreamBuf[1], 4, 4);                // Get the total number of messages
        unsigned char readMessageNum = getNUIntFromByte(tmpStreamBuf[1], 4, 0); // Get the message number, according to the message

        // Check the message number
        if (readMessageNum != getCurMessageNum())
        {
            // Something has gone horribly wrong...
            if (DEBUGGING_BLUETOOTH_LOWLEVEL)
            {
                Serial.print(F("      Mismatch between readMessageNum ("));
                Serial.print(readMessageNum);
                Serial.print(F(") and curMessageNum ("));
                Serial.print(getCurMessageNum());
                Serial.println(F(")"));
            }
        }

        // Set the byte number so that we are after the meta data
        nextByteNum = 2;

        if (DEBUGGING_BLUETOOTH_LOWLEVEL)
        {
            Serial.print(F("      Initialized message: "));
            Serial.print(readMessageNum + 1);
            Serial.print(F(" of "));
            Serial.print(totalMessages);
            Serial.print(F(" (curMessageNum = "));
            Serial.print(getCurMessageNum());
            Serial.println(F(")"));
        }

        return true;
    }
    else
    {
        return false; // If nextByteNum is not 0, then we are not EXPECTING to be at the beginning of a new message, so skip this function and return an error
    }
}

// boolean btSerialWrapper::readNextMessageByte(unsigned char & byteDestination) {
//     // Check if there is any available data
//     if (available() > 0) {
//         // We have data avilable
//         // Check if we've already read the meta-data from this message
//         if (nextByteNum == 0) {
//             initReceiveMessage(); // If this message has not been read yet, get the meta-data from this one
//         }

//         // Now that we definitely have read the meta-data, read the next byte from the stream into byteDestination
//         unsigned char tmpStreamBuf[1]; // A small temporary buffer to read bytes into before being moved into byteDestination
//         stream->readBytes(tmpStreamBuf, 1); // (Always reading a single byte at a time.  Perhaps this is inefficient...)
//         byteDestination = tmpStreamBuf[0]; // Get the first (only) byte from thisStreamBuf, and put it into the output bytesDestination

//         // Iterate the byte number (is this actually used...?  Perhaps for debugging...)
//         nextByteNum++;

//         // Finally, return success
//         return true;

//     } else {
//         // If no data is available

//         // Check if we're expecting more content for this communication
//         if (curMessageNum <= totalMessages) {
//             // We are expecting more content from this communication, ask for the next bit from Android
//             // TODO_JAVA: Send a message to Android requesting the next message in this communication

//             // TODO_JAVA: Wait for the new communication (If nothing comes for a bit of time, return a 0 due to timeout, and abandon the entire reading operation)

//             // Finally, call this function once more since we just got a new message
//             return readNextMessageByte(byteDestination);

//             // TODO: Give this function the same treatment as the readMessageBytes function

//         } else {
//             // We are not expecting any more information from this communication

//             // Reset the communication, and return nothing
//             resetCommunicationData(); // Should have been reset by this point already, but may as well just confirm

//             byteDestination = 0;
//             return false; // Error, because there was nothing to read
//         }
//     }
// }

unsigned char btSerialWrapper::getDebugCode() {
    return debugCode;
}

void btSerialWrapper::extractDebugCode() {
    // Now parse the string to get the debug code (and info on the debug message, if applicable)
    unsigned char debugCodeBuf[DEBUG_MAX_DIGITS] = {0}; // The debug code buffer (used to calculate the debug code)
    int debugCodeLoc = DEBUG_MAX_DIGITS - 1;               // The location in the debug code buffer
    unsigned curBufLoc = 0;                       // The location in the request buffer as it is processed


    for (curBufLoc = 0; curBufLoc < DEBUG_BUF_SIZE; curBufLoc++)
    {
        // For each character in the request buffer
        if (debugCodeLoc < 0)
        {
            // If we got all of the possible digits, calculate the debug code
            debugCode = 0;
            for (debugCodeLoc = DEBUG_MAX_DIGITS - 1; debugCodeLoc >= 0; debugCodeLoc--)
            {
                debugCode += debugCodeBuf[debugCodeLoc] * pow(10, DEBUG_MAX_DIGITS - debugCodeLoc - 1);
            }

            // If this is for renaming the bluetooth device, store the location of the message start (skip spaces, if necessary)
            if (debugCode == DEBUG_RENAME_BLUETOOTH_3 || debugCode == DEBUG_DEBUG_9)
            {
                debugMessageStart = curBufLoc - 1; // The debug message starts right after this value
                while (debugRequestChar[debugMessageStart] == ASCII_SPACE_32)
                {
                    // Skip spaces where required
                    debugMessageStart++;
                }
            }

            // After calculating the debug code, we can break from this loop
            break;
        }

        unsigned char curVal = ((unsigned char)debugRequestChar[curBufLoc]) - ASCII_ZERO_48;
        if (0 <= curVal && curVal <= 9)
        {
            // If this is a single digit, add it to our sum
            debugCodeBuf[debugCodeLoc] = curVal;
            debugCodeLoc--;
        }
        else
        {
            // This is no longer a digit, so say that we've got all of the debugNum digits
            debugCodeLoc = -1;
        }
    }

    // TODO: The debug code is now stored in debugCode, and the message for changing the bluetooth name is now stored at debugRequestChar[debugMessageStart] (is c-string, ends in null terminator)
}

void btSerialWrapper::printStr(char * str)
{
    stream->write(str);
    stream->write("\n");
}

void btSerialWrapper::printPROGMEMStr(const char * str)
{
    // Write the PROGMEM string over bluetooth (if we just printed it on its own, it would read from the address that str points to in RAM, not PROGMEM)
    for (byte i = 0; i < strlen_P(str); i++)
    {
        stream->write(pgm_read_byte_near(str + i));
    }
}

void btSerialWrapper::processDebugCode() {
    switch(debugCode)
    {
    case DEBUG_HELP_0:
    {
        // Send instructions on included debug codes
        const static char debugHelp[] PROGMEM = "Debug info:\nd or d0: Brings up this menu.\nd1: Tic debugging\nd2: Block extra reference tics\nd3: (NOT IN USE) Follow with a name to rename the Bluetooth module.\nd4: Block extra standard tics\nd5: Use adaptive debouncing\n";
        printPROGMEMStr(debugHelp);
        break;
    }
    case DEBUG_TIC_INFO_1:
    {
        // Toggle using debug tic info
        // First, make sure the speedometer has a pointer to this btSerialWrapper object
        bool status = BT->toggleSpeedometerDebugCode(DEBUG_TIC_INFO_1);

        const static char ticMessage[] PROGMEM = "Toggling tic monitoring.\n    Now set to: ";
        printPROGMEMStr(ticMessage);
        char returnVal[3];
        sprintf(returnVal, "%d\n", status ? 1 : 0);
        printStr(returnVal);
        break;
    }
    case DEBUG_BLOCK_EXTRA_REF_2:
    {
        // Toggle blocking extra reference signals
        // First, make sure the speedometer has a pointer to this btSerialWrapper object
        bool status = BT->toggleSpeedometerDebugCode(DEBUG_BLOCK_EXTRA_REF_2);

        const static char blockRefMessage[] PROGMEM = "Toggling Blocking extra reference tics.\n    Now set to: ";
        printPROGMEMStr(blockRefMessage);
        char returnVal[3];
        sprintf(returnVal, "%d\n", status ? 1 : 0);
        printStr(returnVal);
        break;
    }
    case DEBUG_BLOCK_EXTRA_TIC_4:
    {
        // Toggle blocking extra standard tic signals
        // First, make sure the speedometer has a pointer to this btSerialWrapper object
        bool status = BT->toggleSpeedometerDebugCode(DEBUG_BLOCK_EXTRA_TIC_4);

        const static char blockRefMessage[] PROGMEM = "Toggling Blocking extra standard tics.\n    Now set to: ";
        printPROGMEMStr(blockRefMessage);
        char returnVal[3];
        sprintf(returnVal, "%d\n", status ? 1 : 0);
        printStr(returnVal);
        break;
    }
    case DEBUG_ADAPTIVE_DEBOUNCE_5:
    {
        // Toggle using adaptive debouncing
        // First, make sure the speedometer has a pointer to this btSerialWrapper object
        bool status = BT->toggleSpeedometerDebugCode(DEBUG_ADAPTIVE_DEBOUNCE_5);

        const static char adaptiveDebounceMessage[] PROGMEM = "Toggling Adaptive debouncing.\n    Now set to: ";
        printPROGMEMStr(adaptiveDebounceMessage);
        char returnVal[3];
        sprintf(returnVal, "%d\n", status ? 1 : 0);
        printStr(returnVal);
        break;
    }
    case DEBUG_DEBUG_9:
    {
        // Return the debug message
        const static char debugDebugPreMsg[] PROGMEM = "Got debug message:\n\n";
        printPROGMEMStr(debugDebugPreMsg);

        stream->write(&debugRequestChar[debugMessageStart]);

        const static char debugDebugPostMsg[] PROGMEM = "\n\n";
        printPROGMEMStr(debugDebugPostMsg);
        break;
    }
    case DEBUG_NONE:
    default:
    {
        // Do nothing
        const static char unknownDebug[] PROGMEM = "Debug code not recognized!\nEnter 'd0' for help.\n";
        printPROGMEMStr(unknownDebug);
    }
    }

    debugCode = DEBUG_NONE;
}

bool btSerialWrapper::readNextMessageByte(unsigned char &byteDestination)
{
    return readNextMessageBytes(&byteDestination, 1);
}

bool btSerialWrapper::readNextMessageBytes(unsigned char *byteDestinationArray, unsigned char numBytes)
{
    // TODO: TEST THIS OUT!!!  If it doesn't work, then just loop through calls to readNextMessageByte
    int numBytesRemaining = numBytes;
    unsigned char curByteDestInd = 0; // Initialize the current location in the byteDestination array

    while (numBytesRemaining > 0)
    {
        // if (DEBUGGING_BLUETOOTH_LOWLEVEL)
        // {
        //     Serial.print(F("    Starting rnmb loop with numBytesRemaining = "));
        //     Serial.print(numBytesRemaining);
        //     Serial.print(F(", and curMessageNum = "));
        //     Serial.println(getCurMessageNum());
        // }
        // While we are still trying to read data from the bluetooth message
        if (available() > 0)
        {
            // if (DEBUGGING_BLUETOOTH_LOWLEVEL)
            // {
            //     Serial.print(F("    Data is available = "));
            //     Serial.println(available());
            // }

            // We have data available
            // Check if we've already read meta-data from this message
            if (nextByteNum == 0)
            {
                initReceiveMessage(); // If this message has not been read yet, get the meta-data from this one
            }

            // Calculate how many bytes to read from the stream
            unsigned char numBytesLeftInMessage = MAX_BT_BUFFER_SIZE - nextByteNum;           // Calculate the number of bytes that are left in this Bluetooth message
            unsigned char thisNumBytesToRead = min(numBytesRemaining, numBytesLeftInMessage); // Calculate the number of bytes to read from this message

            // Read a series of bytes from the stream
            stream->readBytes(byteDestinationArray + curByteDestInd, thisNumBytesToRead); // Read this number of bytes from the stream into byteDestination, offset by the number of bytes we have already read in
            nextByteNum += thisNumBytesToRead;                                            // Update the pointer to the next byte location

            if (DEBUGGING_BLUETOOTH_LOWLEVEL) 
            {
                for (int i = curByteDestInd;i < thisNumBytesToRead;i++) {
                    printByte(byteDestinationArray[i]);
                }
                Serial.println();
            }

            // Prepare for the next loop, if needed
            curByteDestInd += thisNumBytesToRead;    // Update our location in byteDestination
            numBytesRemaining -= thisNumBytesToRead; // Update the number of bytes that still need to be read into byteDestination

            // if (DEBUGGING_BLUETOOTH_LOWLEVEL)
            // {
            //     Serial.print(F("rnbm  curMessageNum = "));
            //     Serial.print(getCurMessageNum());
            //     Serial.print(F(", numBytesRemaining = "));
            //     Serial.print(numBytesRemaining);
            //     Serial.print(F(", thisNumBytesToRead = "));
            //     Serial.println(thisNumBytesToRead);
            // }

            // Check if we just read the last of the bytes in the most recent message
            if (MAX_BT_BUFFER_SIZE - nextByteNum <= 0)
            {
                // Send a quick message to Java to let it know that we finished reading its last message.  It will work on getting the next message to us while we either a) exit this function, or b) swing around to the "else" statement and wait in the while loop for a minute
                sendConfirmation();
            }

            // if (DEBUGGING_BLUETOOTH_LOWLEVEL)
            // {
            //     Serial.print(F("rnbm  curMessageNum = "));
            //     Serial.print(getCurMessageNum());
            //     Serial.print(F(", numBytesRemaining = "));
            //     Serial.print(numBytesRemaining);
            //     Serial.print(F(", thisNumBytesToRead = "));
            //     Serial.println(thisNumBytesToRead);
            // }
        }
        else
        {
            // If there is no data available

            // Check if we're expecting more content for this communication
            if (getCurMessageNum() <= totalMessages)
            {
                // if (DEBUGGING_BLUETOOTH_LOWLEVEL)
                // {
                //     Serial.println(F("      No data available; waiting for next message..."));
                // }
                // We are expecting more content from this communication, so try reading the next message from Android (since we already sent a confirmation)
                // TODO_JAVA: Wait for the new communication (If nothing comes for a bit of time, return a 0 due to timeout, and abandon the entire reading operation)
                unsigned long startTime = millis();
                while ((((unsigned long)available()) == 0) & (millis() < (startTime + BLUETOOTH_TIMEOUT_MILLI)))
                {
                    // While there is nothing to read, wait for BLUETOOTH_TIMEOUT_MILLI milliseconds
                }

                if (available() == 0)
                {
                    // If we timed out of the communication loop, abandon the connection
                    if (DEBUGGING_BLUETOOTH_LOWLEVEL)
                    {
                        Serial.println(F("      Did not get new message."));
                    }
                    return false;
                }

                if (DEBUGGING_BLUETOOTH_LOWLEVEL)
                {
                    Serial.println(F("      Got new message, continuing..."));
                }

                // If we just got a new message, continue in the main while loop, to read the data
                continue;
            }
            else
            {
                // We are not expecting any more information from this communication

                // Reset the communication, and return nothing
                resetCommunicationData(); // Should have been reset by this point already, but may as well just confirm

                byteDestinationArray = 0;
                // if (DEBUGGING_BLUETOOTH_LOWLEVEL)
                // {
                //     Serial.println(F("      Done."));
                // }
                return false; // Error, because there was nothing to read
            }
        }
    }

    // If we've gotten to this point, this has gone well
    return true;
}

unsigned char btSerialWrapper::getTotalMessages()
{
    return totalMessages;
}

unsigned char btSerialWrapper::getContent()
{
    return content;
}

bool btSerialWrapper::isRequest()
{
    return request;
}

int btSerialWrapper::getCurMessageNum()
{
    return curMessageNum;
}

int btSerialWrapper::available()
{
    return stream->available();
}

bool btSerialWrapper::sendConfirmation()
{
    // Create a "header" (with no body) with the meta data of the data currently being received.
    // Request is false, content is given by the member variable, and confirmation is true
    unsigned char firstByte = 0U;
    // putDataToByte(firstByte, content, 3, 4);
    putBoolToByte(firstByte, true, 3);

    // The current/total message numbers are given by the member variables
    // unsigned char messageNumByte = 0U;
    // putDataToByte(messageNumByte, totalMessages, 4, 4);
    // putDataToByte(messageNumByte, curMessageNum, 4, 0);

    // Send the two bytes of data
    stream->write(firstByte);
    sendCompletionSignal(); // Let Android know that we're done sending data
    // stream->write(messageNumByte);

    if (DEBUGGING_BLUETOOTH_LOWLEVEL)
    {
        Serial.println(F("      Sent confirmation message..."));
    }

    // Finally, because this represents us being done with the message we were just reading, reset the byte number and iterate the curMessageNum
    nextByteNum = 0;
    setCurMessageNum(getCurMessageNum() + 1);

    return true; // TODO: Add some kind of error handling, probably from the stream->write() command?
}

bool btSerialWrapper::writeMetadata(bool thisRequest, unsigned char thisContent)
{
    // First, reset the communication meta-data
    resetCommunicationData();

    // Next, Send the meta-data for this transmission
    if (nextByteNum == 0)
    {
        // If this is the very beginning of the data transmission, then encode some meta-data to the first byte
        // if (stream->availableForWrite())
        // {
            unsigned char firstByte = 0U;
            putBoolToByte(firstByte, thisRequest, 7);
            putDataToByte(firstByte, thisContent, 3, 4);

            stream->write(firstByte);

            if (DEBUGGING_BLUETOOTH) {
                Serial.println(F("Wrote Meta-data..."));
                Serial.print(F("Content = "));
                Serial.println(thisContent);
            }

            if (DEBUGGING_BLUETOOTH_LOWLEVEL) {
                printByte(firstByte);
                Serial.println();
            }

            return true;
        // }
        // else
        // {
        //     // The stream is not available for writing for some reason
        //     return false;
        // }
    }
    else
    {
        // We are not at the beginning of the byte
        return false;
    }
}

bool btSerialWrapper::writeNextMessageByte(unsigned char byteSource)
{
    // if (stream->availableForWrite())
    // {
        stream->write(byteSource);
        
        if (DEBUGGING_BLUETOOTH_LOWLEVEL) 
        {
            printByte(byteSource);
            Serial.println();
        }

        return true;
    // }
    // else
    // {
    //     return false;
    // }
}

bool btSerialWrapper::writeNextMessageBytes(const unsigned char *byteSourceArray, const unsigned char numBytes)
{
    // Write a series of bytes to the Bluetooth stream
    // if (stream->availableForWrite())
    // {
        stream->write(byteSourceArray, numBytes);

        if (DEBUGGING_BLUETOOTH_LOWLEVEL) 
        {
            for (int i = 0;i < numBytes; i++) 
            {
                printByte(byteSourceArray[i]);
                Serial.println();
            }
        }

        return true;
    // }
    // else
    // {
    //     return false;
    // }
}

void btSerialWrapper::sendCompletionSignal() {
    for (unsigned char byteNum = 0;byteNum < BLUETOOTH_COMPLETION_SIGNAL_SIZE;byteNum++) {
        stream->write(byteNum);
    }
}

void btSerialWrapper::resetCommunicationData()
{
    if (DEBUGGING_BLUETOOTH_LOWLEVEL)
    {
        Serial.println(F("      Resetting communication metadata..."));
    }
    totalMessages = 0;
    content = 0;
    request = false;
    setCurMessageNum(0);
    nextByteNum = 0;

    debugCode = DEBUG_NONE;
    for (unsigned char i = 0;i < DEBUG_BUF_SIZE;i++) {
        debugRequestChar[i] = '\0';
    }
}

#if USE_NANOPB
bool bluetooth_decode_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    // A decode callback for a pb_istream_t built from a Serial stream
    SoftwareSerial *thisBTSer = (SoftwareSerial *)stream->state;     // Get the Bluetooth stream
    size_t bytesToRead = min((size_t)thisBTSer->available(), count); // Get the minimum between the size of the buffer (buf) and the number of bytes in the Serial buffer (thisBTSer->available())

    for (unsigned int byteNum = 0; byteNum < bytesToRead; byteNum++)
    {
        // For each byte in the input stream, add it to the buffer
        buf[byteNum] = thisBTSer->read();
    }

    // Record the number of bytes left in the Serial stream
    stream->bytes_left = thisBTSer->available();

    return bytesToRead == count; // If we read the correct number of bytes, return a positive
}

void mainLoop()
{
    // Check if there is anything on the serial line
    int count = btSer->available();
    if (count > 0)
    {
        if (count > MAX_BT_BUFFER_SIZE)
        {
            // SOMETHING HAS GONE HORRIBLY WRONG!!!
        }
        else
        {
            // TODO_OLD: Do a RAM check?

            // // Process using the protocol buffer
            // uint8_t buffer[64]; // Buffer that will read from the Software Serial stream

            // // Read into a local buffer (horribly inefficient, wonder if there's a way to fix...)
            // for (int bufLoc = 0; bufLoc < count; bufLoc++)
            // {
            //     buffer[bufLoc] = btSer->read();
            // }

            // Create a protocol buffer stream
            // pb_istream_t stream;
            // stream = pb_istream_from_buffer(buffer, count);

            // Before any of this stuff, store the current amount of freeRam, in case the user asks for it (don't want to taint the answer will all of the memory we'll need to do the message processing)
            int freeRamNow = freeRam();

            Message_BT message;
            {
                // Read the message from a serial stream (do so inside of a block, so the large serStream object is dismissed immediately after)
                pb_istream_t serStream = {&bluetooth_decode_callback, &btSer, 256}; // TODO_OLD: Memory: Is this enough?  Do I need more/less?
                message = bluetooth_BluetoothMessage_init_default;                  // Initialize the message
                bool status = pb_decode(&serStream, Message_BT_Fields, &message);
            }

            // Do some tests
            // Test 1: Automatic decoding
            // Let nanopb do the decoding for us...what could possibly go wrong?
            // Similar to http://zakovinko.com/57-how-to-speak-from-arduino-to-python-and-back.html
            // See if we can transfer the information from the Message to the respective Bike_Lights objects without using up too much extra memory

            // First, try setting no_unions:true!!!
            // Next, try setting type:FT_POINTER in .options and #define PB_ENABLE_MALLOC=1 in pb.h, see how that changes things, should now be able to use dynamic arrays?
            // Try using https://github.com/nanopb/nanopb/blob/master/examples/using_union_messages/decode.c to figure out which content is there (try setting no_unions:true ?).  Perhaps try this first as a standard C++ program to see what happens if we just decode the thing without setting arg/funcs...may require writing encoding code first
            // message.contents.bike_wheel_anim.image_main.funcs = MAKE A FUNCTION THAT WILL READ FROM THE SERIAL INPUT AND PLACE THE VALUES DIRECTLY INTO THE ARG VARIABLE, WHICH IS A POINTER TO THE PATTERN_HANDLER, TO GET THE IMAGE_MAIN ARRAY (and possibly the image_idle?)

            // TODO_OLD: Put this into a callback: depending on what we're expecting, we may need to decode differently (i.e. getting a BWA or a Kalman, or getting/not getting an image_idle)
            if (message.request)
            {
                // The message is requesting information from the Arduino

                // Store the message type, but otherwise dismiss the information in the message.
                int message_type_which = message.which_contents;
                pb_release(Message_BT_Fields, &message);

                // Make room for a new Message_BT, to send.
                Message_BT message_out;

                //  TODO_OLD: Populate message_out
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
                    // TODO_OLD: Uhhhhh...
                    message_out.type = MessageType_Battery;
                    break;
                }

                // Add in the required meta-data for the new message
                message_out.which_contents = message_type_which;
                message_out.request = false;

                // TODO_OLD: Send the message

                // TODO_OLD: Release the memory held by the message we just sent
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
            pattern_handler->setMainPattern(M_GREL_STILL);
            pattern_handler->setIdlePattern(I_STILL); // TODO_OLD: May need to be revised, once I update Android to have still/moving for idle/main individually
        }
        else
        {
            int rotationSpeed = message.image_meta.parameter_set.p1;
            pattern_handler->setMainPattern(M_GREL_MOVING);
            pattern_handler->setIdlePattern(I_STILL); // TODO_OLD: May need to be revised, once I update Android to have still/moving for idle/main individually

            // Now set the speed
            static_cast<Moving_Helper_Main *>(pattern_handler->mainPattern)->setRotateSpeed(rotationSpeed);
        }
        break;

        // Set the image for the idle pattern (the main pattern's image will be set in just a moment, because Spinners don't have idle images)
        pattern_handler->idlePattern->setImage(message.image_idle->bytes);
        // bluetooth_BluetoothMessage m = bluetooth_BluetoothMessage_init_zero;

    case ImageType_SPINNER_BT:
        // TODO_OLD: Yet to be implemented!
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
        // TODO_OLD: SAFETY: Check the number of colorObjs?
        // If this is a static color, then we only ever need a single colorObj to describe it.
        newColor = new Color_Static(ColorObjFromPB(color_bt.color_objs[0]));
    }
    else
    {
        // If this is a dynamic color, then we're going to need an array of colorObj's, plus their metadata.  Create the array of the colorObjs and the blend_type's (the t array will be constructed once we know of what type the array is)
        // TODO_OLD: MEMORY: Pass a reference to this array, along with the size, but don't copy the entire array in the constructor, just record the pointer and the number of colorObj's that were created right HERE.
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

    // TODO_OLD: SAFETY: Should we check the number of observable/state parameters that is included in the message and/or the sizes of R/P0?

    // Check if there's a new Q
    if (message.q != 0)
    {
        // A new value for Q was just added!
        speedometer->getKalman()->setPhi(message.q);
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

    // TODO_OLD: Need to free this array using pb_release (or free()).  Still need memfree_BT()...?
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
    // TODO_OLD: Get paramter from idle animation, too!!!  (first, implement it on the Android side)
    // TODO_OLD: Also, must implement "stationary" (or whatever I'm going to call it, when the image doesn't move relative to the rest of the wheel) in both Android and Arduino

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
        // TODO_OLD: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
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
            // TODO_OLD: Will need to convert T so that it can hold a float?  Or, just use long as for velocity, as well (no reason it couldn't work, right?)

            //
            //
            // TODO_OLD: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
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
            // TODO_OLD: Will need to convert T so that it can hold a float?  Or, just use long as for velocity, as well (no reason it couldn't work, right?)

            //
            //
            // TODO_OLD: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
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
    kalman_bt.q = speedometer->getKalman()->getPhi();

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
#endif