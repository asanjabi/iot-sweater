// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. 

#include "mbed.h"
#include "neopixel.h"

#define T1H 900 // Width of a 1 bit in ns
#define T1L 600 // Width of a 1 bit in ns
#define T0H 400 // Width of a 0 bit in ns
#define T0L 900 // Width of a 0 bit in ns

#define DEFAULT_NS_PER_NOP_DELAY 42 // Width of one nop loop in ns

#define RES 6000 // Width of the low gap between bits to cause a frame to latch, in ns

static uint32_t _nsT1H[] = {T1H / DEFAULT_NS_PER_NOP_DELAY};
static uint32_t _nsT1L[] = {T1L / DEFAULT_NS_PER_NOP_DELAY};
static uint32_t _nsT0H[] = {T0H / DEFAULT_NS_PER_NOP_DELAY};
static uint32_t _nsT0L[] = {T0L / DEFAULT_NS_PER_NOP_DELAY};

NeoPixel::NeoPixel(PinName pin, int num) : DigitalOut(pin)
{
    _pixels = (Pixel *)malloc(num * sizeof(Pixel));
    memset(_pixels, 0, num * sizeof(Pixel));
    _npixels = num;
    nsPerNopLoop = DEFAULT_NS_PER_NOP_DELAY;
    setTiming();
    gpio_write(&gpio, 0);
}

void NeoPixel::setTiming()
{
    _nsT1H[0] = T1H / nsPerNopLoop;
    _nsT1L[0] = T1L / nsPerNopLoop;
    _nsT0H[0] = T0H / nsPerNopLoop;
    _nsT0L[0] = T0L / nsPerNopLoop;
}

bool NeoPixel::init()
{
    _nsT1H[0] = 100000; // loop 100000 times

    // Calculate the width of one nop loop in ns
    uint32_t us = us_ticker_read();
    __disable_irq();
    asm volatile(
        "       ldr r4, %0;"
        "loop:; "
        "       nop;"
        "       subs r4, r4, #1;"
        "       bne loop"
        :
        : "m"(_nsT1H)
        : "r4", "cc", "memory");
    __enable_irq();
    us = us_ticker_read() - us;

    nsPerNopLoop = us / (_nsT1H[0] / 1000);
    if (nsPerNopLoop == 0)
    {
        nsPerNopLoop = DEFAULT_NS_PER_NOP_DELAY;
        return false;
    }

    setTiming();
    return true;
}

void NeoPixel::sendByte(register uint8_t byte)
{
    for (int i = 0; i < 8; i++)
    {
        if (byte & 0x80)
        {
            // one
            gpio_write(&gpio, 1);
            asm volatile(
                "       ldr r4, %0;"
                "loop1:; "
                "       nop;"
                "       subs r4, r4, #1;"
                "       bne loop1"
                :
                : "m"(_nsT1H)
                : "r4", "cc", "memory");

            gpio_write(&gpio, 0);
            asm volatile(
                "       ldr r4, %0;"
                "loop2:; "
                "       nop;"
                "       subs r4, r4, #1;"
                "       bne loop2"
                :
                : "m"(_nsT1L)
                : "r4", "cc", "memory");
        }
        else
        {
            // zero
            gpio_write(&gpio, 1);
            asm volatile(
                "       ldr r4, %0;"
                "loop3:; "
                "       nop;"
                "       subs r4, r4, #1;"
                "       bne loop3"
                :
                : "m"(_nsT0H)
                : "r4", "cc", "memory");

            gpio_write(&gpio, 0);
            asm volatile(
                "       ldr r4, %0;"
                "loop4:; "
                "       nop;"
                "       subs r4, r4, #1;"
                "       bne loop4"
                :
                : "m"(_nsT0L)
                : "r4", "cc", "memory");
        }

        byte = byte << 1; // shift to next bit
    }
}

void NeoPixel::show(void)
{
    // Disable interrupts in the critical section
    __disable_irq();

    for (int i = 0; i < _npixels; i++)
    {
        sendPixel((_pixels + i)->r, (_pixels + i)->g, (_pixels + i)->b);
    }

    __enable_irq();
    wait_us(RES / 1000);
}

void NeoPixel::setColor(int idx, uint8_t r, uint8_t g, uint8_t b)
{
    if ((idx >= 0) && (idx < _npixels))
    {
        (_pixels + idx)->g = g;
        (_pixels + idx)->r = r;
        (_pixels + idx)->b = b;
    }
}

void NeoPixel::setColorAll(uint8_t r, uint8_t g, uint8_t b)
{
    for (int idx = 0; idx < _npixels; idx++)
    {
        (_pixels + idx)->g = g;
        (_pixels + idx)->r = r;
        (_pixels + idx)->b = b;
    }
}

void NeoPixel::clear(void)
{
    for (int i = 0; i < _npixels; i++)
    {
        (_pixels + i)->hex = 0;
    }
}

void NeoPixel::showColor(uint8_t r, uint8_t g, uint8_t b)
{
    // Disable interrupts in the critical section
    __disable_irq();

    for (int i = 0; i < _npixels; i++)
    {
        sendPixel(r, g, b);
    }

    __enable_irq();
    wait_us(RES / 1000);
}

// Rainbow cycle.
void NeoPixel::showRainbowCycle(uint8_t frames, uint32_t frameAdvance, uint32_t pixelAdvance, uint32_t intervalms)
{
    // Hue is a number between 0 and 3*256 than defines a mix of r->g->b where
    // hue of 0 = Full red
    // hue of 128 = 1/2 red and 1/2 green
    // hue of 256 = Full Green
    // hue of 384 = 1/2 green and 1/2 blue
    // ...
    uint32_t firstPixelHue = 0; // Color for the first pixel in the string

    for (uint8_t j = 0; j < frames; j++)
    {
        uint32_t currentPixelHue = firstPixelHue;

        __disable_irq();
        for (int i = 0; i < _npixels; i++)
        {
            if (currentPixelHue >= (3 * 256))
            { // Normalize back down incase we incremented and overflowed

                currentPixelHue -= (3 * 256);
            }
            uint8_t phase = currentPixelHue >> 8;
            uint8_t step = currentPixelHue & 0xff;

            switch (phase)
            {
            case 0:
                sendPixel(~step, step, 0);
                break;
            case 1:
                sendPixel(0, ~step, step);
                break;
            case 2:
                sendPixel(step, 0, ~step);
                break;
            }

            currentPixelHue += pixelAdvance;
        }
        __enable_irq();

        firstPixelHue += frameAdvance;

        wait_ms(intervalms);
    }
}

// Theatre-style crawling lights.
void NeoPixel::showTheaterChase(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalms)
{
    uint32_t theaterSpacing = _npixels / 20;
    for (uint32_t j = 0; j < 3; j++)
    {
        for (uint32_t q = 0; q < theaterSpacing; q++)
        {
            uint32_t step = 0;

            __disable_irq();
            for (int i = 0; i < _npixels; i++)
            {
                if (step == q)
                {

                    sendPixel(r, g, b);
                }
                else
                {

                    sendPixel(0, 0, 0);
                }

                step++;
                if (step == theaterSpacing)
                {
                    step = 0;
                }
            }

            __enable_irq();

            wait_ms(intervalms);
        }
    }
}

// Detonate
void NeoPixel::showDetonate(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalms)
{
    uint32_t fadeIntervalms = intervalms / 256;
    // Flash
    while (intervalms)
    {
        showColor(0, 0, 0);
        wait_ms(intervalms);
        showColor(r, g, b);
        wait_ms(intervalms);
        intervalms = (intervalms * 4) / 5;
    }

    // Then fade to black
    for (int fade = 256; fade > 0; fade--)
    {
        showColor((r * fade) / 256, (g * fade) / 256, (b * fade) / 256);
        wait_ms(fadeIntervalms);
    }
    showColor(0, 0, 0);
}