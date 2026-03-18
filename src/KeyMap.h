#ifndef KEYMAP_H
#define KEYMAP_H

#include <Arduino.h>
// This header contains the standard USB-IF HID Usage IDs
#include <class/hid/hid.h> 

struct IRMapping {
    uint32_t irCode;
    uint16_t usageId; 
    const char* description;
};

/**
 * Using standard HID_USAGE_CONSUMER_* constants.
 * These map directly to the USB-IF HID Specification.
 */
const IRMapping irToMediaMap[] = {
    {0xFF30CF, HID_USAGE_CONSUMER_PLAY_PAUSE, "Play/Pause"},
    {0xFF18E7, HID_USAGE_CONSUMER_STOP,       "Stop"},
    {0xFF7A85, HID_USAGE_CONSUMER_VOLUME_INCREMENT, "Vol Up"},
    {0xFF10EF, HID_USAGE_CONSUMER_VOLUME_DECREMENT, "Vol Down"},
    {0xFF5AA5, HID_USAGE_CONSUMER_SCAN_NEXT_TRACK,  "Next"},
    {0xFF12ED, HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK, "Prev"}
};

const int MAP_SIZE = sizeof(irToMediaMap) / sizeof(IRMapping);

#endif