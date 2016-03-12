/* 
 * File:   disp_driver.h
 * Author: david
 *
 * Created on December 28, 2015, 4:57 PM
 * based on some code from essentialscrap for their eink display driver
 */

#ifndef DISP_H
#define	DISP_H

#include "system_definitions.h"

/* =================================
 *      Default configuration
 * ================================= */

/* Delay for generating clock pulses.
 * Unit is approximate clock cycles of the CPU (0 to 15).
 * This should be atleast 50 ns.
 */
#ifndef EINK_CLOCKDELAY
#       define EINK_CLOCKDELAY 0
#endif

#define DISP_HEIGHT 600

#define PIXELMASK 3
#define PIXEL_WHITE 2   // 10
#define PIXEL_BLACK 1   // 01
// for filling a row (one byte of color)
#define BYTE_WHITE 0xAA
#define BYTE_BLACK 0x55

// do I need this?
#define TRUE    true
#define FALSE   false

// private functions -- low level drivers for the screen
static inline void eink_delay(uint16_t microseconds);
static inline void setpower_vdd(bool on);
static inline void setpower_vneg(bool on);
static inline void setpower_vpos(bool on);
static inline void setpin_le(bool on);
static inline void setpin_oe(bool on);
static inline void setpin_cl(bool on);
static inline void setpin_gmode(bool on);
static inline void setpin_sph(bool on);
static inline void setpin_spv(bool on);
static inline void setpin_ckv(bool on);
static inline void setpins_data(uint8_t value);
static inline void clockdelay();
static void vclock_quick(void);
static void hclock(void);
static void vscan_start(void);
static void vscan_write(void);
static void vscan_bulkwrite(void);
static void vscan_skip(void);
static void vscan_stop(void);
static void hscan_start(void);
static void hscan_write(const uint8_t *data, int count);
static void hscan_stop(void);
static void power_on(void);
static void power_off(void);

// public functions
// test functions for evaluation
void eink_test_fill(uint8_t fill_color);
void eink_test_plaid(bool white_background);
// buffer interface functions, draw to memory before dumping it onto screen
void buffer_set_pixel(uint16_t xpos, uint16_t ypos, uint8_t color);
void buffer_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t color);
void eink_write_buffer(uint8_t *buffer);

#endif	/* DISP_H */

