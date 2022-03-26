/*
 * File:   tkLora.c
 * Author: pablo
 *
 * 
 */

#include "terminal_lora_PMP.h"
#include "lora.h"

//------------------------------------------------------------------------------
void tkLora(void * pvParameters)
{

	/*
     * Se encarga de leer la UART del lora y dejarlo en un buffer.
     */

( void ) pvParameters;
uint8_t c = 0;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );
   
    //lora_reset_on();
	xprintf( "\r\n\r\nstarting tkLora..\r\n" );
            
	// loop
	for( ;; )
	{
		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdLORA, (char *)&c, 1 ) == 1 ) {
            lora_push_RxBuffer( (char *)&c );
        }
	}   
}
//------------------------------------------------------------------------------
