
#include "lora.h"
#include "frtos_cmd.h"

// -----------------------------------------------------------------------------
void LORA_init(void)
{
    LORA_RTS_CONFIG();
    LORA_CTS_CONFIG();
    LORA_RESET_CONFIG();
    
    lBchar_CreateStatic(&lora_rx_sdata, (char *)&lora_rx_buffer, LORA_RX_BUFFER_SIZE );
}
// -----------------------------------------------------------------------------
void loraRx_process( char c)
{
    /*
     * Guardo todos los caracteres en el buffer.
     * Si termina en \n lo proceso.
     *  
     */
    lora_push_RxBuffer( &c);
    if ( c == '\n') {
        
        lora_print_RxBuffer();
        lora_flush_RxBuffer();
    }
}
// -----------------------------------------------------------------------------
void lora_flush_RxBuffer(void)
{
    lBchar_Flush(&lora_rx_sdata);

    //memset( lora_rx_data.rx_buffer, '\0', LORA_RX_BUFFER_SIZE );
    //lora_rx_data.ptr = 0;
    
}
// -----------------------------------------------------------------------------
void lora_flash_led(void)
{
    xprintf( "Lora test led.\r\n");

    xfprintf( fdLORA, "sys set pindig GPIO5 1\r\n");
    vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );
    xfprintf( fdLORA, "sys set pindig GPIO5 0\r\n");
}
// -----------------------------------------------------------------------------
void lora_print_RxBuffer(void)
{
    xprintf( "[%s]\r\n", lBchar_get_buffer(&lora_rx_sdata));
    //xprintf( "[%s]\r\n", lora_rx_data.rx_buffer  );
}
//------------------------------------------------------------------------------
void lora_push_RxBuffer( char *c)
{
    lBchar_Poke(&lora_rx_sdata,  (char *)&c);
    //if ( lora_rx_data.ptr < LORA_RX_BUFFER_SIZE ) {
    //    lora_rx_data.rx_buffer[lora_rx_data.ptr++] = *c;    
    //}
}
//------------------------------------------------------------------------------
void lora_write_cmd(void)
{
    /*
     * Uso el cmdLine_History_buffer porque el cmdLine tiene los \0 del
     * parseo por lo que me corta en el primer texto.
     */
    xprintf("LORA[%s]\r\n", cmdLine_History_buffer );
}
//------------------------------------------------------------------------------
