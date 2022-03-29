/*
 * File:   tkCtl.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "terminal_lora_PMP.h"
#include "math.h"
#include "led.h"

#define TKCTL_DELAY_S	1


void sys_watchdog_check(void);

//------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
//uint16_t counter = 0;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    xprintf("Starting tkCtl..\r\n");
    WDG_INIT();

	for( ;; )
	{
        kick_wdt(CTL_WDG_bp);
		vTaskDelay( ( TickType_t)( 5000 / portTICK_PERIOD_MS ) );
        led_flash();
        sys_watchdog_check();
        //xprintf("Counter=%d\r\n", counter++);
        //xprintf_P( PSTR("PGMCounter=%d\r\n"), counter++);
        
	}
}
//------------------------------------------------------------------------------
void sys_watchdog_check(void)
{
    // El watchdog se inicializa en 7.
    // Cada tarea debe poner su bit en 0. Si alguna no puede, se resetea
    
    if ( sys_watchdog != 0 ) {  
        xprintf_P(PSTR("tkCtl: reset by wdg [0x%02d]\r\n"), sys_watchdog );
        vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        reset();
    } else {
        wdt_reset();
        WDG_INIT();
    }
}
//------------------------------------------------------------------------------