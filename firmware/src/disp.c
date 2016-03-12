
#include "disp.h"
#include "system_definitions.h"

inline void eink_delay( volatile uint16_t microseconds) {
    while( microseconds > 0 ) {
        microseconds--;
    }
}

// display voltage supplies
// SMPS_CTRL RB3, active low
// VNEG_CTRL RF0, active high
// VPOS_CTRL RF1, active high
// eink_VDD - RD6, active high

static inline void setpower_vdd ( volatile bool on ) {
    // SMPS_CTRL - RB3 - active low
    SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_B, 3);
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, !on, PORT_CHANNEL_B, 3);
    // EINK_VDD - RD6 - active high
    SYS_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_D, 6, on);
}

static inline void setpower_vneg ( volatile bool on ) {
    //VNEG_CTRL - RF0 - active high
    SYS_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_F, 0, on);
}

static inline void setpower_vpos ( volatile bool on ) {
    //VPOS_CTRL - RF1 - active high
    SYS_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_F, 1, on);
}

// display source driver
// CL - RD3
// OE - RD2
// LE - RD7

static inline void setpin_le ( volatile bool on ) {
    // LE - RD7
    LATDbits.LATD7 = on;
}

static inline void setpin_oe ( volatile bool on ) {
    // RD2
    LATDbits.LATD2 = on;
}

static inline void setpin_cl ( volatile bool on ) {
    // RD3
    LATDbits.LATD3 = on;
//    SYS_PORTS_PinWrite(PORTS_ID_0, PORT_CHANNEL_D, 3, on);
}

// display gate driver
// GMODE - RD1 - high at start of frame through row to end of frame
// SPH - RD8
// SPV - RC14 - rework
// CKV - RC13 - rework

static inline void setpin_gmode ( volatile bool on ) {
    // RD1
    LATDbits.LATD1 = on;
}

static inline void setpin_sph ( volatile bool on ) {
    // RD8
    LATDbits.LATD8 = on;
}

static inline void setpin_spv ( volatile bool on ) {
    // RC14
    LATCbits.LATC14 = on;
}

static inline void setpin_ckv ( volatile bool on ) {
    // RC13
    LATCbits.LATC13 = on;
}

// display data pins
// RE0-RE7

static inline void setpins_data ( volatile uint8_t value ) {
    LATE = value;
}

/* ====================================
 *      Lower level driver functions
 * ==================================== */

/** Delay between signal changes, to give time for IO pins to change state. */
static inline void clockdelay()
{
#if EINK_CLOCKDELAY & 1
    asm("nop");
#endif
#if EINK_CLOCKDELAY & 2
    asm("nop");
    asm("nop");
#endif
#if EINK_CLOCKDELAY & 4
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
#endif
#if EINK_CLOCKDELAY & 8
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
#endif
}

/** Fast vertical clock pulse for gate driver, used during initializations */
static void vclock_quick()
{
    //setpin_ckv(TRUE;)
    LATCbits.LATC13 = 1;
    // delay ~10us (easiest with nop, although processor speed may change)
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    //setpin_ckv(FALSE);
    LATCbits.LATC13 = 0;
    // delay ~5us?
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
    _nop();
}

/** Horizontal clock pulse for clocking data into source driver */
static void hclock()
{
    //clockdelay();
    setpin_cl(TRUE);
    clockdelay();
    setpin_cl(FALSE);
}

/** Start a new vertical gate driver scan from top.
 * Note: Does not clear any previous bits in the shift register,
 *       so you should always scan through the whole display before
 *       starting a new scan.
 */
static void vscan_start()
{
    setpin_gmode(TRUE);
    vclock_quick();
    setpin_spv(FALSE);
    vclock_quick();
    setpin_spv(TRUE);
    vclock_quick();
    setpin_oe(TRUE);
}

/** Waveform for strobing a row of data onto the display.
 * Attempts to minimize the leaking of color to other rows by having
 * a long idle period after a medium-length strobe period.
 */
static void vscan_write()
{
    // ckv on
    setpin_ckv(TRUE);
    // delay 5us -- don't need much, setpin call
    _nop();
    _nop();
    // ckv off
    setpin_ckv(FALSE);
    // delay 200us
    eink_delay(70);
}

/** Waveform used when clearing the display. Strobes a row of data to the
 * screen, but does not mind some of it leaking to other rows.
 */
static void vscan_bulkwrite()
{
    //setpin_oe(TRUE);
    setpin_ckv(TRUE);
    //eink_delay(20);
    eink_delay(10);
    //setpin_oe(FALSE);
    setpin_ckv(FALSE);
    eink_delay(200);
}

/** Waveform for skipping a vertical row without writing anything.
 * Attempts to minimize the amount of change in any row.
 */
static void vscan_skip()
{
    //setpin_ckv(TRUE);
    //eink_delay(1);
    //setpin_ckv(FALSE);
    LATCbits.LATC13 = 1;
    _nop();
    _nop();
    LATCbits.LATC13 = 0;
    eink_delay(35);
}

/** Stop the vertical scan. The significance of this escapes me, but it seems
 * necessary or the next vertical scan may be corrupted.
 */
static void vscan_stop()
{
    setpin_gmode(FALSE);
    setpin_oe(FALSE);
    vclock_quick();
    vclock_quick();
    vclock_quick();
    vclock_quick();
    vclock_quick();
}

/** Start updating the source driver data (from left to right). */
static void hscan_start()
{
    /* Disable latching and output enable while we are modifying the row. */
    setpin_le(FALSE);
    //setpin_oe(FALSE);
    
    /* The start pulse should remain low for the duration of the row. */
    setpin_sph(FALSE);
}

/** Write data to the horizontal row. */
static void hscan_write(const uint8_t *data, int count)
{
    while (count--)
    {
        /* Set the next byte on the data pins */
        setpins_data(*data++);
        
        /* Give a clock pulse to the shift register */
        hclock();
    }
}

/** Finish and transfer the row to the source drivers.
 * Does not set the output enable, so the drivers are not yet active. */
static void hscan_stop()
{
    /* End the scan */
    setpin_sph(TRUE);
    hclock();
    
    /* Latch the new data */
    setpin_le(TRUE);
    clockdelay();
    setpin_le(FALSE);
}

/** Turn on the power to the E-Ink panel, observing proper power sequencing. */
static void power_on()
{
    volatile unsigned i;
    
    /* First the digital power supply and signal levels. */
    setpower_vdd(TRUE);
    setpin_le(FALSE);
    setpin_oe(FALSE);
    setpin_cl(FALSE);
    setpin_sph(TRUE);
    setpins_data(0);
    setpin_ckv(FALSE);
    setpin_gmode(FALSE);
    setpin_spv(TRUE);
    
    /* Min. 100 microsecond delay after digital supply */
    eink_delay(1000);
    
    /* Then negative voltages and min. 1000 microsecond delay. */
    setpower_vneg(TRUE);
    eink_delay(3000);
    
    /* Finally the positive voltages. */
    setpower_vpos(TRUE);
    eink_delay(3000);
    
    /* Clear the vscan shift register */
    vscan_start();
    for (i = 0; i < DISP_HEIGHT; i++)
        vclock_quick();
    vscan_stop();
}

/** Turn off the power, observing proper power sequencing. */
static void power_off()
{
    /* First the high voltages */
    setpower_vpos(FALSE);
    setpower_vneg(FALSE);
    
    /* Wait for any capacitors to drain */
    eink_delay(5000);
    
    /* Then put all signals and digital supply to ground. */
    setpin_le(FALSE);
    setpin_oe(FALSE);
    setpin_cl(FALSE);
    setpin_sph(FALSE);
    setpins_data(0);
    setpin_ckv(FALSE);
    setpin_gmode(FALSE);
    setpin_spv(FALSE);
    setpower_vdd(FALSE);
}

/* ===============================
 *         Public functions
 *  static keyword prevents external access
 * =============================== */

void eink_test_fill (volatile uint8_t fill_color) {
    volatile uint16_t xCounter, yCounter;

    power_on();
    
    vscan_start();

    for ( yCounter = 0; yCounter < 600; yCounter++ ) {
        hscan_start();
        for ( xCounter = 0; xCounter < 200; xCounter++ ) {
            setpins_data(fill_color);
            hclock();
        }
        hscan_stop();
        vscan_write();
    }
    vscan_stop();
    
    power_off();
}