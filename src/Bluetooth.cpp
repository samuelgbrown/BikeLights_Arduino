#include "Bluetooth.h"

Bluetooth::Bluetooth()
{
}

Bluetooth::~Bluetooth()
{
}

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

            bool status = pb_decode(&serStream, Message_BT_Fields, &message);

            // TODO: Put this into a callback: depending on what we're expecting, we may need to decode differently (i.e. getting a BWA or a Kalman, or getting/not getting an image_idle)
            if (message.request)
            {
                // The message is requesting information from the Arduino

                // Store the message type, but otherwise dismiss the information in the message.
                int message_type_which = message.which_contents;
                pb_release(Message_BT_Fields, &message);

                // Make room for a new Message_BT, to send.
                Message_BT message_out;

                switch (message.which_contents)
                {
                case BWA_BT_Tag:

                    message_out.type = MessageType_BWA;
                    break;
                case Kalman_BT_Tag:

                    message_out.type = MessageType_Kalman;
                    break;
                case Brightness_BT_Tag:

                    message_out.type = MessageType_Brightness;
                    break;
                case Storage_BT_Tag:

                    message_out.type = MessageType_Storage;
                    break;
                case Battery_BT_Tag:

                    message_out.type = MessageType_Battery;
                    break;
                }

                // Add in the required meta-data for the new message
                message_out.which_contents = message_type_which;
                message_out.request = false;
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

                    needRestart = true; // We got some new Kalman parameters, so we should probably just start fresh
                    break;
                case MessageType_Brightness:

                    break;
                }

                // Now that we are done reading the data, release the information from the Message
                // TODO: MEMORY: Maybe I can do this myself?  Hell, maybe memfree_BT() already does it?
            }

            // Test 2: Manual decoding
            // Define all of the decoding callbacks manually...why trust nanobp?
            // Similar to https://github.com/nanopb/nanopb/blob/master/tests/callbacks/decode_callbacks.c
            // Method 1: Define all of the callbacks such that they assign the incoming value to a class (maybe a new helper class?) pointed to by arg.  May not be possible, because fields in the Message are not known until we know which content is in there
            // Method 2: Manually decode the entire message, byte by suffering byte...ugh.  Fuck being memory efficient.  Maybe find (or make?) function that will decode one field at a time...except oneof's and fields that contain other Messages?  Have I said "ugh" yet?
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
        pattern_handler->idlePattern->setImage(message.image_idle);
        // bluetooth_BluetoothMessage m = bluetooth_BluetoothMessage_init_zero;

    case ImageType_SPINNER_BT:
        // TODO: Yet to be implemented!
        break;
    }

    // Save the images for the main pattern
    pattern_handler->mainPattern->setImage(message.image_main);

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
            // Create the T array and fill it
            unsigned long *allLongT = new unsigned long[color_bt.color_objs_count];
            for (unsigned char i = 0; i < color_bt.color_objs_count; i++)
            {
                allLongT[i] = (unsigned long)color_bt.color_objs[i].t;
            }

            newColor = new Color_dTime(allC, allLongT, allB, (unsigned char)color_bt.color_objs_count);
            break;
        case ColorType_DVEL_BT:
            // Create the T array and fill it
            float *allFloatT = new float[color_bt.color_objs_count];
            for (unsigned char i = 0; i < color_bt.color_objs_count; i++)
            {
                allFloatT[i] = (float)color_bt.color_objs[i].t;
            }

            newColor = new Color_dVel(speedometer, allC, allFloatT, allB, (unsigned char)color_bt.color_objs_count);
            break;
        default:
            // Uh oh...
            newColor = new Color_Static();
        }
    }

    return newColor;
}

colorObj Bluetooth::ColorObjFromPB(ColorObj_BT &colorObj_bt)
{
    // Create a new colorObj object using the information stored in the ColorObj_BT Message
    colorObj c = colorObj(colorObj_bt.r, colorObj_bt.g, colorObj_bt.b, colorObj_bt.w);
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

    // TODO: SAFETY: Should we check the number of observable/state parameters that is included in the message?
    
    // Check if there's a new Q
    if (message.q != 0)
    {
        // A new value for Q was just added!
        speedometer->setQ(message.q);
    }

    
}

// Encoding methods

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
            color_bt.type = ColorType_DTIME_BT;
            Color_dTime *thisColor_ = (Color_dTime *)color_; // Get a pointer to the Color_ that's of the correct derived class type

            // Go through all of the colorObj's (and meta data), and add the information
            // TODO: Will need to convert T so that it can hold a float?  Or, just use long as for velocity, as well (no reason it couldn't work, right?)

            //
            //
            // TODO: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
            //
            //

            color_bt.color_objs = new ColorObj_BT[thisColor_->getNumColors()]; // Allocate space for the array of colorObjs that will be put into color_bt.color_objs
            color_bt.color_objs_count = thisColor_->getNumColors();            // Store the number of colorObjs that will be added
            for (unsigned char i = 0; i < thisColor_->getNumColors(); i++)
            {
                const colorObj c = thisColor_->getThisColorObj(i);                            // Get the colorObj (and don't modify it, because it's a reference!)
                color_bt.color_objs[i] = PBFromColorObj(c);                                   // Save the colorObj (Without metadata)
                color_bt.color_objs[i].bt = PBFromBlendType(thisColor_->getThisBlendType(i)); // Add the Blend type to the ColorObj_BT
                color_bt.color_objs[i].t = thisColor_->getThisTrigger(i);                     // Add the T value to the ColorObj_BT
            }

            break;
        case COLOR_DVEL:
            color_bt.type = ColorType_DVEL_BT;
            Color_dVel *thisColor_ = (Color_dVel *)color_; // Get a pointer to the Color_ that's of the correct derived class type

            // Go through all of the colorObj's (and meta data), and add the information
            // TODO: Will need to convert T so that it can hold a float?  Or, just use long as for velocity, as well (no reason it couldn't work, right?)

            //
            //
            // TODO: REMEMBER TO PROPERLY delete[] THE ColorObj_BT ARRAY BY CALLING memfree_BT!!!
            //
            //

            color_bt.color_objs = new ColorObj_BT[thisColor_->getNumColors()];
            color_bt.color_objs_count = thisColor_->getNumColors(); // Store the number of colorObjs that will be added
            for (unsigned char i = 0; i < thisColor_->getNumColors(); i++)
            {
                const colorObj c = thisColor_->getThisColorObj(i);                            // Get the colorObj (and don't modify it, because it's a reference!)
                color_bt.color_objs[i] = PBFromColorObj(c);                                   // Save the colorObj (Without metadata)
                color_bt.color_objs[i].bt = PBFromBlendType(thisColor_->getThisBlendType(i)); // Add the Blend type to the ColorObj_BT
                color_bt.color_objs[i].t = thisColor_->getThisTrigger(i);                     // Add the T value to the ColorObj_BT
            }

            break;
        default:
            // Uh oh...
        }
    }

    return color_bt;
}

void Bluetooth::memfree_BT(Message_BT *mIn)
{
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

        // First, free each Image
        delete[] thisM->image_main; // Delete the main image
        thisM->image_main = NULL;
        delete[] thisM->image_idle; // If this Image type supports an idle image, delete it (if it does not support image_idle, nothing will happen because image_idle is a NULL pointer)
        thisM->image_idle = NULL;

        // Next, go through each Color_ and free the colorObj arrays (and possibly the Color_'s themselves...?)
        for (unsigned char i = 0; i < thisM->palette_count; i++)
        {
            delete[] thisM->palette[i].color_objs;
            thisM->palette[i] = NULL;
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
        thisM->r = NULL;

        break;
    }
    }
}

ColorObj_BT Bluetooth::PBFromColorObj(const colorObj &c)
{
    ColorObj_BT c_bt = ColorObj_BT_default;
    c_bt.r = (uint32_t)c.r;
    c_bt.b = (uint32_t)c.b;
    c_bt.g = (uint32_t)c.g;
    c_bt.w = (uint32_t)c.w;
}

Kalman_BT Bluetooth::PBFromKalman(Kalman *kalman)
{
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