#include <Arduino.h>
#include <Adafruit_TinyUSB.h> 
#include <IRremote.hpp>
#include "KeyMap.h"
#include "StatusLED.h"

// --- CONFIGURATION ---
#define DEBUG 1            // Set to 1 to enable Serial console, 0 to disable
#define IR_PIN 2           // GPIO pin for IR Receiver
#define NEO_PIN 16         // Onboard NeoPixel (GPIO 16 for RP2040-Zero)

// --- HARDWARE ROM LOOKUP ---
typedef void* (*rom_table_lookup_fn)(uint16_t *table, uint32_t code);
bool get_bootsel_button() {
    rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn)(uintptr_t)(*(uint16_t *)0x18);
    uint16_t *func_table = (uint16_t *)(uintptr_t)(*(uint16_t *)0x14);
    typedef bool (*bootsel_fn_t)(void);
    static bootsel_fn_t bootsel_func = (bootsel_fn_t)rom_table_lookup(func_table, 'B' | ('S' << 8));
    return bootsel_func ? bootsel_func() : false;
}

// --- USB HID DESCRIPTOR ---
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(1) ),
    TUD_HID_REPORT_DESC_GENERIC_INOUT( 64, HID_REPORT_ID(2) )
};

// --- GLOBALS ---
Adafruit_USBD_HID usb_hid;
StatusLED led(NEO_PIN);
volatile bool cirMode = true; // Volatile for safe cross-core access
bool lastBtnState = false;
bool lastUsbState = false;

/**
 * @brief Monitor USB Enumeration status
 */
void checkUSBConnection() {
    bool currentUsbState = TinyUSBDevice.mounted();
    if (currentUsbState != lastUsbState) {
        #if DEBUG
        if (currentUsbState) {
            Serial.println(F("\n[SYSTEM] USB Connected & Enumerated (Host Ready)"));
            Serial.print(F("[SYSTEM] Identity: Microsoft eHome IR Transceiver\n"));
        } else {
            Serial.println(F("\n[SYSTEM] USB Disconnected or Suspended"));
        }
        #endif
        lastUsbState = currentUsbState;
    }
}

/**
 * @brief Dispatches IR codes to HID and Serial
 * Called from Core 1
 */
void handleIR(uint32_t code) {
    #if DEBUG
    Serial.print(F("[IR] Received Raw Hex: 0x"));
    Serial.println(code, HEX);
    #endif

    if (cirMode) {
        // WINDOWS MODE: Find mapping and send Media Key
        bool found = false;
        for (int i = 0; i < MAP_SIZE; i++) {
            if (irToMediaMap[i].irCode == code) {
                uint16_t usage = irToMediaMap[i].usageId;
                usb_hid.sendReport(1, &usage, 2);
                delay(10);
                uint16_t empty = 0;
                usb_hid.sendReport(1, &empty, 2);
                
                #if DEBUG
                Serial.printf("[CIR] Mapping Found: %s -> Sent to Windows\n", irToMediaMap[i].description);
                #endif
                found = true;
                break;
            }
        }
        #if DEBUG
        if(!found) Serial.println(F("[CIR] No mapping found for this code."));
        #endif
    } else {
        // LINUX RAW MODE: Send via HID Report ID 2
        uint8_t packet[4];
        packet[0] = (code >> 24) & 0xFF;
        packet[1] = (code >> 16) & 0xFF;
        packet[2] = (code >> 8) & 0xFF;
        packet[3] = code & 0xFF;
        
        if(usb_hid.sendReport(2, packet, 4)) {
            #if DEBUG
            Serial.println(F("[RAW] Code sent via HID Packet (Report ID 2) to Linux"));
            #endif
        }
    }
}

// ==========================================
// CORE 0: USB Stack, UI, and System Checks
// ==========================================
void setup() {
    // 1. Spoof Identity
    USBDevice.setID(0x045E, 0x006D); 
    USBDevice.setManufacturerDescriptor("Microsoft");
    USBDevice.setProductDescriptor("eHome Infrared Transceiver");
    USBDevice.setSerialDescriptor("CIR-RP2040-PUNE");
    USBDevice.setVersion(0x0100);

    #if DEBUG
    Serial.begin(115200);
    #endif

    // 2. Init HID
    usb_hid.setPollInterval(2);
    usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
    usb_hid.begin();

    // 3. Init NeoPixel
    led.begin();
    led.showMode(cirMode);

    #if DEBUG
    delay(2000); 
    Serial.println(F("--- RP2040 Multi-Core IR Ready ---"));
    Serial.printf("Core 0: USB & System Management\n");
    Serial.printf("Core 1: IR Pulse Sampling\n");
    #endif
}

void loop() {
    led.update();
    checkUSBConnection();

    // Handle Mode Toggle (BOOTSEL Button)
    bool currentBtn = get_bootsel_button();
    if (currentBtn && !lastBtnState) {
        cirMode = !cirMode;
        led.showMode(cirMode);
        #if DEBUG
        Serial.printf("\n[MODE] Switched to: %s\n", cirMode ? "CIR (Windows Media)" : "RAW (Linux HID)");
        #endif
        delay(300); 
    }
    lastBtnState = currentBtn;
}

// ==========================================
// CORE 1: Dedicated IR Decoding Loop
// ==========================================
void setup1() {
    // Start the IR Receiver exclusively on Core 1
    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
}

void loop1() {
    // Handle Incoming IR pulses on Core 1
    if (IrReceiver.decode()) {
        if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) && 
            IrReceiver.decodedIRData.decodedRawData != 0) {
            
            // Note: TinyUSB sendReport is thread-safe on RP2040 Philhower core
            handleIR(IrReceiver.decodedIRData.decodedRawData);
        }
        IrReceiver.resume();
    }
}