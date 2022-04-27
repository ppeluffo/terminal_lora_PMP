#include "terminal_lora_PMP.h"
#include "frtos_cmd.h"
#include "pines.h"
#include "ina3221.h"
#include "i2c.h"
#include "nvm.h"
#include "lora.h"


static void cmdClsFunction(void);
static void cmdHelpFunction(void);
static void cmdResetFunction(void);
static void cmdStatusFunction(void);
static void cmdWriteFunction(void);
static void cmdReadFunction(void);
static void cmdConfigFunction(void);

static void pv_snprintfP_OK(void );
static void pv_snprintfP_ERR(void );

//------------------------------------------------------------------------------
void tkCmd(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

	vTaskDelay( ( TickType_t)( 500 / portTICK_PERIOD_MS ) );

uint8_t c = 0;
uint8_t ticks = 0;
//uint16_t counter=0;

    FRTOS_CMD_init();

    FRTOS_CMD_register( "cls", cmdClsFunction );
	FRTOS_CMD_register( "help", cmdHelpFunction );
    FRTOS_CMD_register( "reset", cmdResetFunction );
    FRTOS_CMD_register( "status", cmdStatusFunction );
    FRTOS_CMD_register( "write", cmdWriteFunction );
    FRTOS_CMD_register( "read", cmdReadFunction );
    FRTOS_CMD_register( "config", cmdConfigFunction );
    
    
	// Fijo el timeout del READ
	ticks = 5;
	frtos_ioctl( fdTERM, ioctl_SET_TIMEOUT, &ticks );

    xprintf( "Starting tkCmd..\r\n" );
    
	// loop
	for( ;; )
	{
        kick_wdt(CMD_WDG_bp);
		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdTERM, (char *)&c, 1 ) > 0 ) {
            //FRTOS_CMD_process(c);
            xputChar(c);
        }
	}    
}
//------------------------------------------------------------------------------
static void cmdHelpFunction(void)
{

    FRTOS_CMD_makeArgv();
        
    if ( strcmp( strupr(argv[1]), "WRITE") == 0 ) {
		xprintf( "-write:\r\n");
        xprintf("   dac{val}\r\n");
        xprintf("   ina rconfValue\r\n");
        xprintf("   lora {cmdString}\r\n");
        
    }  else if ( strcmp( strupr(argv[1]), "CONFIG") == 0 ) {
		xprintf_P(PSTR("-config:\r\n"));
		xprintf_P(PSTR("   default,save,load\r\n"));
        xprintf_P(PSTR("   modo {central,remoto}, timerpoll, txwindow, ANoutputChannel, linkTimeout\r\n"));

    }  else if ( strcmp( strupr(argv[1]), "READ") == 0 ) {
		xprintf("-read:\r\n");
		xprintf("   dac\r\n");
//        xprintf("   lora rsp, buffer\r\n");
        xprintf("   ina {regName}\r\n");
        xprintf("   id\r\n");
   
    }  else {
        // HELP GENERAL
        xprintf_P(PSTR("Available commands are:\r\n"));
        xprintf_P(PSTR("-cls\r\n"));
        xprintf_P(PSTR("-help\r\n"));
        xprintf_P(PSTR("-status\r\n"));
        xprintf_P(PSTR("-reset\r\n"));
        xprintf_P(PSTR("-write...\r\n"));
        xprintf_P(PSTR("-config...\r\n"));
        xprintf_P(PSTR("-read...\r\n"));

    }
   
	xprintf("Exit help \r\n");

}
//------------------------------------------------------------------------------
static void cmdClsFunction(void)
{
	// ESC [ 2 J
	xprintf("\x1B[2J\0");
}
//------------------------------------------------------------------------------
static void cmdResetFunction(void)
{
    reset();
}
//------------------------------------------------------------------------------
static void cmdStatusFunction(void)
{

    // https://stackoverflow.com/questions/12844117/printing-defined-constants
    
	//xprintf("\r\nSpymovil HW_MODELO" "FRTOS_VERSION" "FW_REV" "FW_DATE"\r\n" , );"
    //xprintf("\r\nSpymovil %s\r\n" , HW_MODELO );
    xprintf("\r\nSpymovil V1.3\r\n");
    xprintf_P(PSTR("Configuracion:\r\n"));
    if ( systemConf.tipo_nodo == CENTRAL ) {
        xprintf_P(PSTR(" Nodo: CENTRAL\r\n"));
    } else {
        xprintf_P(PSTR(" Nodo: REMOTO\r\n"));
    }
    xprintf_P(PSTR(" TimerPoll: %d(s)\r\n"), systemConf.timerPoll);
    xprintf_P(PSTR(" TXwindowSize: %d\r\n"), systemConf.tx_window_size);
    xprintf_P(PSTR(" ANchannelXconvert: %d\r\n"), systemConf.an_channel_for_convert);
    xprintf_P(PSTR(" Timeout link %d(s):\r\n"), systemConf.max_inactivity_link);
    
    xprintf_P(PSTR("Estado:\r\n"));
    xprintf(" DAC=%d\r\n", systemVars.dac);
}
//------------------------------------------------------------------------------
static void cmdWriteFunction(void)
{

    FRTOS_CMD_makeArgv();
    
    if ( strcmp( strupr(argv[1]),"LORA") == 0 ) {
        lora_write_cmd();
        pv_snprintfP_OK();
        return;
    }
    
    // INA
	// write ina id rconfValue
	// Solo escribimos el registro 0 de configuracion.
	if (strcmp( strupr(argv[1]), "INA") == 0)  {
		( INA_test_write ( "0", argv[2] ) > 0)?  pv_snprintfP_OK() : pv_snprintfP_ERR();
		return;
	}
    
    if ( strcmp( strupr(argv[1]),"DAC") == 0 ) {
        systemVars.dac = atoi(argv[2]);
        pv_snprintfP_OK();
        return;
    }
        
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void cmdReadFunction(void)
{
  
    FRTOS_CMD_makeArgv();
       
    // ID
	// read id
	if ( strcmp( strupr(argv[1]),"ID") == 0  ) {
		nvm_read_print_id();
		return;
	}
    
    // INA
	// read ina id regName
	if ( strcmp( strupr(argv[1]),"INA") == 0  ) {
		INA_test_read ( "0" , argv[2] );
		return;
	}
    
    if ( strcmp( strupr(argv[1]),"DAC") == 0 ) {
        xprintf("dac=%d\r\n", systemVars.dac );
        pv_snprintfP_OK();
        return;
    }
        
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void cmdConfigFunction(void)
{
  
    FRTOS_CMD_makeArgv();
       
    // modo {central,remoto}
	if ( strcmp( strupr(argv[1]),"MODO") == 0  ) {
        config_modo(argv[2]) ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
	}
    
    //timerpoll
	if ( strcmp( strupr(argv[1]),"TIMERPOLL") == 0  ) {
        config_timerpoll(argv[2]) ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
	}
    
    // txwindow
    if ( strcmp( strupr(argv[1]),"TXWINDOW") == 0  ) {
        config_txwindow(argv[2]) ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
	}
    
    // ANoutputChannel
    if ( strcmp( strupr(argv[1]),"ANOUTPUTCHANNEL") == 0  ) {
        config_ANoutputChannel(argv[2]) ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
	}
    
    // Linktimeout
    if ( strcmp( strupr(argv[1]),"LINKTIMEOUT") == 0  ) {
        config_linktimeout(argv[2]) ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
	}
    
    // default
	if ( strcmp( strupr(argv[1]),"DEFAULT") == 0  ) {
		config_default();
		return;
	}
    
    // save
    if ( strcmp( strupr(argv[1]),"SAVE") == 0 ) {
        save_config_in_NVM() ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
    }
    
    // load
    if ( strcmp( strupr(argv[1]),"LOAD") == 0 ) {
        load_config_from_NVM() ? pv_snprintfP_OK() : pv_snprintfP_ERR();
        return;
    }
    
        
    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;
 
}
//------------------------------------------------------------------------------
static void pv_snprintfP_OK(void )
{
	xprintf("ok\r\n\0");
}
//------------------------------------------------------------------------------
static void pv_snprintfP_ERR(void)
{
	xprintf("error\r\n\0");
}
//------------------------------------------------------------------------------

