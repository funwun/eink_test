/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app_plaid.h"
#include "disp_driver.h"
#include "system_definitions.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback funtions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_SLEEP;
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, 0); // green LED off
    
    DRV_TMR0_Start();
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

    // TODO: turn on switching regulators
    // turn on (low) power supply mosfet from RB3 -- P-MOSFET inversion!
    // turn on (high) switching pos and neg via RF0 and RF1

    // NOTE:
    // RD0 -- outside, blue on board, active low
    // RD9 -- inside, green on board, active high
    // pin 21 - SW2 - RB8
    // pin 40 - SW1 - RC15

    // also NOTE:  This code has issues when optimization is set (greater than 0)
    // optimizations are re-set to 1 every time harmony is updated?
    

#define SW1INPUT            SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_C, 15)
#define SW2INPUT            SYS_PORTS_PinRead(PORTS_ID_0, PORT_CHANNEL_B, 8)
#define SWDOWNCOUNT         100

#define GREENLEDON()        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, 0)
#define GREENLEDOFF()       SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, 0)
#define GREENLEDTOGGLE()    SYS_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_D, 0)
#define BLUELEDON()         SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_D, 9)
#define BLUELEDOFF()        SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_D, 9)
#define BLUELEDTOGGLE()     SYS_PORTS_PinToggle(PORTS_ID_0, PORT_CHANNEL_D, 9)

void APP_Tasks ( void )
{
    volatile uint8_t switch1Counter, switch2Counter;
    volatile uint16_t counter;
    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_SLEEP:
        {
            // check button inputs
            if ( 0 == SW2INPUT ) {
                switch2Counter++;

                if ( switch2Counter > SWDOWNCOUNT ) {
                    GREENLEDON();
                    while( 0 == SW2INPUT );
                    switch2Counter = 0;
                    appData.state = APP_STATE_DRAW;
                    break;
                }

            } else {
                switch2Counter = 0;
                GREENLEDOFF();
            }
            if ( 0 == SW1INPUT ) {
                switch1Counter++;

                if ( switch1Counter > SWDOWNCOUNT ) {
                    BLUELEDON();
                    while( 0 == SW1INPUT );
                    switch1Counter = 0;
                    appData.state = APP_STATE_CLEAR;
                    break;
                }

            } else {
                switch1Counter = 0;
                BLUELEDOFF();
            }
            
            break;
        }
        case APP_STATE_DRAW:
        {
            
//            if ( 1 == IFS0bits.T1IF ) {
//                IFS0bits.T1IF = 0;
//                
//                if ( 0 == SW1INPUT ) {
//                    switch1Counter++;
//                    
//                    if ( switch1Counter > SWDOWNCOUNT ) {
//                        BLUELEDTOGGLE();
//                        while( 0 == SW1INPUT );
//                        switch1Counter = 0;
//                    }
//                } else {
//                    switch1Counter = 0;
//                }
//            
//                // check button input
//                if ( 0 == SW2INPUT ) {
//                    switch2Counter++;
//                    
//                    if ( switch2Counter > SWDOWNCOUNT ) {
//                        GREENLEDOFF();
//                        BLUELEDOFF();
//                        while( 0 == SW2INPUT );
//                        switch2Counter = 0;
//                        appData.state = APP_STATE_SLEEP;
//                        break;
//                    }
//                    
//                } else {
//                    switch2Counter = 0;
//                }
//            }
            
            GREENLEDON();
            
            eink_test_plaid(TRUE);
            
            appData.state = APP_STATE_SLEEP;
            
            GREENLEDOFF();
            
            
            break;
        }
        case APP_STATE_CLEAR:
        {
            BLUELEDON();
            
            eink_test_fill(BYTE_WHITE);
            
            appData.state = APP_STATE_SLEEP;
            
            BLUELEDOFF();
            
            
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}
 

/*******************************************************************************
 End of File
 */
