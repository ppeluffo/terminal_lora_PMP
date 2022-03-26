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


//------------------------------------------------------------------------------
void tkCtl(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;
//uint16_t counter = 0;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    xprintf("Starting tkCtl..\r\n");

	for( ;; )
	{
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        led_flash();
        wdt_reset();
        //xprintf("Counter=%d\r\n", counter++);
        //xprintf_P( PSTR("PGMCounter=%d\r\n"), counter++);
        
	}
}
//------------------------------------------------------------------------------
