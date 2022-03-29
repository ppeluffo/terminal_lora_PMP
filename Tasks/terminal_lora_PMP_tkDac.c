/*
 * File:   tkTerm.c
 * Author: pablo
 *
 * Created on 25 de octubre de 2021, 12:50 PM
 */


#include "terminal_lora_PMP.h"
#include "dac.h"

//------------------------------------------------------------------------------
void tkDac(void * pvParameters)
{

	/* Mantiene una variable local dac.
     * Una vez por segundo chequea si el valor del sysytemVars.dac cambio.
     * En caso afirmativo, actualiza al variable local y
     * setea la salida con el nuevo valor
     */
    

( void ) pvParameters;
uint16_t dac;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
    xprintf("Starting tkDAC..\r\n");

    systemVars.dac = 0;
    dac = 0;
    
	for( ;; )
	{
   
        kick_wdt(DAC_WDG_bp);
		vTaskDelay( ( TickType_t)( 1000 / portTICK_PERIOD_MS ) );
        
        while ( xSemaphoreTake( sem_SYSVars, ( TickType_t ) 5 ) != pdTRUE )
  			taskYIELD();
        
		if ( systemVars.dac != dac ) {
            dac = systemVars.dac;
            DAC_setVal(dac);
            //xprintf("New dac=%d\r\n", dac);
        } 
         
        xSemaphoreGive( sem_SYSVars );
	}
}
//------------------------------------------------------------------------------
