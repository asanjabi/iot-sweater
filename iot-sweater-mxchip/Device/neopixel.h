// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. 

#ifndef __NEOPIXEL_H__
#define __NEOPIXEL_H__
#include "mbed.h"

union Pixel {
    struct __attribute__((packed))
    {
        uint8_t b;
        uint8_t g;
        uint8_t r;
        uint8_t a; // no use
    };
    uint32_t hex;
};

class NeoPixel : DigitalOut
{
  public:
    NeoPixel(PinName pin, int num);
    
    bool init();

    void show(void);
    void setColor(int idx, uint8_t r, uint8_t g, uint8_t b);
    void setColorAll(uint8_t r, uint8_t g, uint8_t b);
    void clear(void);

    void showColor(uint8_t r, uint8_t g, uint8_t b);
    void showRainbowCycle(uint8_t frames, uint32_t frameAdvance, uint32_t pixelAdvance, uint32_t intervalms);
    void showTheaterChase(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalms);
    void showDetonate(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalms);

  private:
    void setTiming();
    void sendByte(register uint8_t b);
    inline void sendPixel(uint8_t r, uint8_t g, uint8_t b)
    {
        sendByte(g);
        sendByte(r);
        sendByte(b);
    }

    int _npixels;
    Pixel *_pixels;

    uint32_t nsPerNopLoop;
};

#endif // __NEOPIXEL_H__
