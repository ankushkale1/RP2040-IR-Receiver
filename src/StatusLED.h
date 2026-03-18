#ifndef STATUSLED_H
#define STATUSLED_H

#include <Adafruit_NeoPixel.h>

class StatusLED {
private:
    Adafruit_NeoPixel pixel;
    uint32_t timer = 0;
    bool isFlashing = false;

public:
    StatusLED(int pin) : pixel(1, pin, NEO_GRB + NEO_KHZ800) {}

    void begin() {
        pixel.begin();
        pixel.setBrightness(50);
        pixel.show();
    }

    void showMode(bool isCIR) {
        // CIR = Green, RAW = Blue
        if (isCIR) pixel.setPixelColor(0, pixel.Color(0, 255, 0)); 
        else pixel.setPixelColor(0, pixel.Color(0, 0, 255));
        
        pixel.show();
        timer = millis();
        isFlashing = true;
    }

    void update() {
        if (isFlashing && (millis() - timer > 3000)) {
            pixel.setPixelColor(0, 0); // Turn off after 3s
            pixel.show();
            isFlashing = false;
        }
    }
};

#endif