/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WS2812_INC
#define WS2812_INC

#define IS_RGBW false
#define NUM_PIXELS 10

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 2
#endif

void pixel(uint8_t r, uint8_t g, uint8_t b);
void pattern_snakes(uint len, uint t);
void pattern_random(uint len, uint t);
void pattern_sparkle(uint len, uint t);
void pattern_greys(uint len, uint t);
void ws2812_init();



#endif
