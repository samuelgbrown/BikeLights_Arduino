/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.3 at Wed Jul 03 17:53:49 2019. */

#include "bluetooth.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t bluetooth_BluetoothMessage_fields[8] = {
    PB_FIELD(  1, UENUM   , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothMessage, type, type, 0),
    PB_FIELD(  2, BOOL    , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage, request, type, 0),
    PB_ONEOF_FIELD(contents,   3, MESSAGE , ONEOF, STATIC  , OTHER, bluetooth_BluetoothMessage, bike_wheel_anim, request, &bluetooth_BluetoothMessage_BikeWheelAnim_fields),
    PB_ONEOF_FIELD(contents,   4, MESSAGE , ONEOF, STATIC  , UNION, bluetooth_BluetoothMessage, kalman, request, &bluetooth_BluetoothMessage_Kalman_fields),
    PB_ONEOF_FIELD(contents,   5, FLOAT   , ONEOF, STATIC  , UNION, bluetooth_BluetoothMessage, brightness_scale, request, 0),
    PB_ONEOF_FIELD(contents,   6, MESSAGE , ONEOF, STATIC  , UNION, bluetooth_BluetoothMessage, storage, request, &bluetooth_BluetoothMessage_Storage_fields),
    PB_ONEOF_FIELD(contents,   7, UINT32  , ONEOF, STATIC  , UNION, bluetooth_BluetoothMessage, battery, request, 0),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothMessage_BikeWheelAnim_fields[6] = {
    PB_FIELD(  1, UINT32  , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothMessage_BikeWheelAnim, num_leds, num_leds, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, POINTER , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim, palette, num_leds, &bluetooth_BluetoothMessage_BikeWheelAnim_Color__fields),
    PB_FIELD(  3, UINT32  , REPEATED, POINTER , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim, image_main, palette, 0),
    PB_FIELD(  4, UINT32  , REPEATED, POINTER , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim, image_idle, image_main, 0),
    PB_FIELD(  5, MESSAGE , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim, image_meta, image_idle, &bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_fields),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothMessage_BikeWheelAnim_Color__fields[3] = {
    PB_FIELD(  1, MESSAGE , REPEATED, POINTER , FIRST, bluetooth_BluetoothMessage_BikeWheelAnim_Color_, color_objs, color_objs, &bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_fields),
    PB_FIELD(  2, UENUM   , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim_Color_, type, color_objs, 0),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_fields[7] = {
    PB_FIELD(  1, UINT32  , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj, r, r, 0),
    PB_FIELD(  2, UINT32  , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj, g, r, 0),
    PB_FIELD(  3, UINT32  , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj, b, g, 0),
    PB_FIELD(  4, UINT32  , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj, w, b, 0),
    PB_FIELD(  5, UINT32  , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj, t, w, 0),
    PB_FIELD(  6, UENUM   , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj, bt, t, 0),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_fields[3] = {
    PB_FIELD(  1, UENUM   , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta, type, type, 0),
    PB_FIELD(  2, MESSAGE , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta, parameter_set, type, &bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageMetaParameter_fields),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageMetaParameter_fields[2] = {
    PB_FIELD(  1, UINT32  , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageMetaParameter, p1, p1, 0),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothMessage_Kalman_fields[6] = {
    PB_FIELD(  1, UINT32  , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothMessage_Kalman, n_obs, n_obs, 0),
    PB_FIELD(  2, UINT32  , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_Kalman, n_sta, n_obs, 0),
    PB_FIELD(  3, FLOAT   , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_Kalman, q, n_sta, 0),
    PB_FIELD(  4, FLOAT   , REPEATED, POINTER , OTHER, bluetooth_BluetoothMessage_Kalman, r, q, 0),
    PB_FIELD(  5, FLOAT   , REPEATED, POINTER , OTHER, bluetooth_BluetoothMessage_Kalman, p0, r, 0),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothMessage_Storage_fields[3] = {
    PB_FIELD(  1, UINT32  , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothMessage_Storage, remaining, remaining, 0),
    PB_FIELD(  2, UINT32  , SINGULAR, STATIC  , OTHER, bluetooth_BluetoothMessage_Storage, total, remaining, 0),
    PB_LAST_FIELD
};

const pb_field_t bluetooth_BluetoothLength_fields[2] = {
    PB_FIELD(  1, FIXED32 , SINGULAR, STATIC  , FIRST, bluetooth_BluetoothLength, message_length, message_length, 0),
    PB_LAST_FIELD
};






/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(bluetooth_BluetoothMessage, contents.bike_wheel_anim) < 65536 && pb_membersize(bluetooth_BluetoothMessage, contents.kalman) < 65536 && pb_membersize(bluetooth_BluetoothMessage, contents.storage) < 65536 && pb_membersize(bluetooth_BluetoothMessage_BikeWheelAnim, image_meta) < 65536 && pb_membersize(bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta, parameter_set) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_bluetooth_BluetoothMessage_bluetooth_BluetoothMessage_BikeWheelAnim_bluetooth_BluetoothMessage_BikeWheelAnim_Color__bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageMetaParameter_bluetooth_BluetoothMessage_Kalman_bluetooth_BluetoothMessage_Storage_bluetooth_BluetoothLength)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(bluetooth_BluetoothMessage, contents.bike_wheel_anim) < 256 && pb_membersize(bluetooth_BluetoothMessage, contents.kalman) < 256 && pb_membersize(bluetooth_BluetoothMessage, contents.storage) < 256 && pb_membersize(bluetooth_BluetoothMessage_BikeWheelAnim, image_meta) < 256 && pb_membersize(bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta, parameter_set) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_bluetooth_BluetoothMessage_bluetooth_BluetoothMessage_BikeWheelAnim_bluetooth_BluetoothMessage_BikeWheelAnim_Color__bluetooth_BluetoothMessage_BikeWheelAnim_Color__ColorObj_bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_bluetooth_BluetoothMessage_BikeWheelAnim_ImageMeta_ImageMetaParameter_bluetooth_BluetoothMessage_Kalman_bluetooth_BluetoothMessage_Storage_bluetooth_BluetoothLength)
#endif


/* @@protoc_insertion_point(eof) */
