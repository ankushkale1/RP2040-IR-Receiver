#ifndef KEYMAP_H
#define KEYMAP_H

#include <Arduino.h>
#include <class/hid/hid.h> 

enum KeyType { MEDIA, KEYBOARD };

struct IRMapping {
    uint32_t irCode;
    uint16_t usageId; 
    KeyType type;
    const char* description;
    uint8_t modifier; // For Alt, Shift, etc. (0 if none)
};

const IRMapping irToMediaMap[] = {
    // --- Original Media Keys ---
    {0x4FB0FB04, HID_USAGE_CONSUMER_PLAY_PAUSE, MEDIA, "Play/Pause", 0},
    {0xBB44BF00, HID_USAGE_CONSUMER_VOLUME_INCREMENT, MEDIA, "Vol Up", 0},
    {0xBC43BF00, HID_USAGE_CONSUMER_VOLUME_DECREMENT, MEDIA, "Vol Down", 0},
    {0xB54ABF00, HID_USAGE_CONSUMER_SCAN_NEXT_TRACK, MEDIA, "Next", 0},
    {0xB44BBF00, HID_USAGE_CONSUMER_SCAN_PREVIOUS_TRACK, MEDIA, "Prev", 0},
    {0xF10EBF00, HID_USAGE_CONSUMER_MUTE, MEDIA, "Mute", 0}, // Add your code

    // --- Seeking ---
    {0x718EFB04, HID_KEY_ARROW_RIGHT, KEYBOARD, "Seek Forward (+5s)", 0},
    {0x708FFB04, HID_KEY_ARROW_LEFT,  KEYBOARD, "Seek Backward (-5s)", 0},

    // --- 10s Skip or more (YouTube Native 'L' and 'J') ---
    {0xE718BF00, HID_KEY_L, KEYBOARD, "Big Seek +10s", 0},
    {0xE619BF00, HID_KEY_J, KEYBOARD, "Big Seek -10s", 0},

    // --- Power & System ---
    {0x8D72FB04, HID_USAGE_CONSUMER_SLEEP, MEDIA, "Suspend/Resume", 0},
    {0xF20DBF00, HID_USAGE_CONSUMER_POWER, MEDIA, "Power/Shutdown", 0}, 
    {0x2FD0707, HID_KEY_L, KEYBOARD, "Lock PC (Win+L)", KEYBOARD_MODIFIER_LEFTGUI},
    //{0x000000, HID_USAGE_CONSUMER_AL_TERMINATE_RUNNING_APPLICATION, MEDIA, "Close App", 0},

    // --- Keyboard Macros (Using Modifiers) ---
    // Alt(0x04) + Tab(0x2B)
    {0xEA15BF00, HID_KEY_TAB, KEYBOARD, "Alt+Tab", KEYBOARD_MODIFIER_LEFTALT}, 
    // Alt(0x04) + F4(0x3D)
    {0xA45BFB04, HID_KEY_F4, KEYBOARD, "Close Window (Alt+F4)", KEYBOARD_MODIFIER_LEFTALT},
    // F11 (Fullscreen)
    {0xDF20BF00, HID_KEY_F, KEYBOARD, "Toggle Fullscreen", 0},

    // --- Numbers 1-9 ---
    {0xEF10FB04, HID_KEY_0, KEYBOARD, "Num 0", 0},
    {0xEE11FB04, HID_KEY_1, KEYBOARD, "Num 1", 0},
    {0xED12FB04, HID_KEY_2, KEYBOARD, "Num 2", 0},
    {0xEC13FB04, HID_KEY_3, KEYBOARD, "Num 3", 0},
    {0xEB14FB04, HID_KEY_4, KEYBOARD, "Num 4", 0},
    {0xEA15FB04, HID_KEY_5, KEYBOARD, "Num 5", 0},
    {0xE916FB04, HID_KEY_6, KEYBOARD, "Num 6", 0},
    {0xE817FB04, HID_KEY_7, KEYBOARD, "Num 7", 0},
    {0xE718FB04, HID_KEY_8, KEYBOARD, "Num 8", 0},
    {0xE619FB04, HID_KEY_9, KEYBOARD, "Num 9", 0}
};

const int MAP_SIZE = sizeof(irToMediaMap) / sizeof(IRMapping);

#endif