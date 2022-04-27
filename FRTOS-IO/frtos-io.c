/*
 * frtos-io.c
 *
 *  Created on: 11 jul. 2018
 *      Author: pablo
 */

#include "frtos-io.h"
//#include "util/delay.h"

//#define GPRS_IsTXDataRegisterEmpty() ((USARTE0.STATUS & USART_DREIF_bm) != 0)

#define USART_IsTXDataRegisterEmpty(_usart) (((_usart)->STATUS & USART_DREIF_bm) != 0)
#define USART_IsTXShiftRegisterEmpty(_usart) (((_usart)->STATUS & USART_TXCIF_bm) != 0)

#define USART_PutChar(_usart, _data) ((_usart)->TXDATAL = _data)

//------------------------------------------------------------------------------------
int16_t frtos_open( file_descriptor_t fd, uint32_t flags)
{
	// Funcion general para abrir el puerto que invoca a una mas
	// especializada para c/periferico.
	// Es la que invoca la aplicacion.
	// Retorna -1 en error o un nro positivo ( fd )

int16_t xRet = -1;

	switch(fd) {
        
	case fdTERM:
		xRet = frtos_uart_open( &xComTERM, fd, iUART3, flags );
		break;
 
    case fdLORA:
		xRet = frtos_uart_open( &xComLORA, fd, iUART4, flags );
		break;
 
    case fdI2C:
		xRet = frtos_i2c_open( &xBusI2C, fd, &I2C_xMutexBuffer, flags );
		break;

    case fdNVM:
		xRet = frtos_nvm_open( &xNVM, fd, &NVM_xMutexBuffer, flags );
		break;
        
	default:
		break;
	}


	return(xRet);
}
//------------------------------------------------------------------------------------
int16_t frtos_ioctl( file_descriptor_t fd, uint32_t ulRequest, void *pvValue )
{

int16_t xRet = -1;

	switch(fd) {
	case fdTERM:
		xRet = frtos_uart_ioctl( &xComTERM, ulRequest, pvValue );
		break;
      
    case fdLORA:
		xRet = frtos_uart_ioctl( &xComLORA, ulRequest, pvValue );
		break;
    
    case fdI2C:
		xRet = frtos_i2c_ioctl( &xBusI2C, ulRequest, pvValue );
		break;

    case fdNVM:
		xRet = frtos_nvm_ioctl( &xNVM, ulRequest, pvValue );
		break;
        
	default:
		break;
	}

	return(xRet);
}
//------------------------------------------------------------------------------------
int16_t frtos_write( file_descriptor_t fd ,const char *pvBuffer, const uint16_t xBytes )
{

int16_t xRet = -1;

	switch(fd) {
        
	case fdTERM:
		xRet = frtos_uart_write( &xComTERM, pvBuffer, xBytes );
		break;
    
    case fdLORA:
		xRet = frtos_uart_write( &xComLORA, pvBuffer, xBytes );
		break;
    
    case fdI2C:
		xRet = frtos_i2c_write( &xBusI2C, pvBuffer, xBytes );
		break; 

    case fdNVM:
		xRet = frtos_nvm_write( &xNVM, pvBuffer, xBytes );
		break;
        
	default:
		break;
	}

	return(xRet);
}
//------------------------------------------------------------------------------------
int16_t frtos_read( file_descriptor_t fd , char *pvBuffer, uint16_t xBytes )
{

int16_t xRet = -1;

	switch(fd) {
	case fdTERM:
		xRet = frtos_uart_read( &xComTERM, pvBuffer, xBytes );
		break;
    
    case fdLORA:
		xRet = frtos_uart_read( &xComLORA, pvBuffer, xBytes );
		break;
     
    case fdI2C:
		xRet = frtos_i2c_read( &xBusI2C, pvBuffer, xBytes );
		break;

    case fdNVM:
		xRet = frtos_nvm_read( &xNVM, pvBuffer, xBytes );
		break;
        
	default:
		break;
	}

	return(xRet);
}
//------------------------------------------------------------------------------------
// FUNCIONES ESPECIFICAS DE UART's
//------------------------------------------------------------------------------------
int16_t frtos_uart_open( periferico_serial_port_t *xCom, file_descriptor_t fd, uart_id_t uart_id, uint32_t flags)
{

	xCom->fd = fd;
	xCom->xBlockTime = (10 / portTICK_PERIOD_MS );
	// Inicializo la uart del usb (iUART_USB) y la asocio al periferico
	xCom->uart = drv_uart_init( uart_id, flags );
    
    frtos_ioctl(fd, ioctl_UART_ENABLE_RX, NULL );
    frtos_ioctl(fd, ioctl_UART_ENABLE_TX, NULL );
    //frtos_ioctl(fd, ioctl_UART_ENABLE_RX_INT, NULL );
    //frtos_ioctl(fd, ioctl_UART_ENABLE_TX_INT, NULL );
    
	return(xCom->fd);

}
//------------------------------------------------------------------------------------
int16_t frtos_uart_write( periferico_serial_port_t *xCom, const char *pvBuffer, const uint16_t xBytes )
{
	// Esta funcion debe poner los caracteres apuntados en pvBuffer en la cola de trasmision del
	// puerto serial apuntado por xCom
	// Actua como si fuese rprintfStr.
	// Debe tomar el semaforo antes de trasmitir. Los semaforos los manejamos en la capa FreeRTOS
	// y no en la de los drivers.


    USART3_sendString( (char *)pvBuffer);
    vTaskDelay( ( TickType_t)( 10 ) );
    return(0);

int16_t wBytes = 0;
char cChar = '\0';
char *p = NULL;
uint16_t bytes2tx = 0;
bool exit_flag;


	// Espero que los buffers esten vacios. ( La uart se va limpiando al trasmitir )
	while  ( rBchar_GetCount( &xCom->uart->TXringBuffer ) > 0 )
		vTaskDelay( ( TickType_t)( 1 ) );

    
	// Controlo no hacer overflow en la cola de trasmision
	bytes2tx = xBytes;
    p = (char *)pvBuffer;
    
    // PASO 1: Almaceno todo lo que pueda en el txBuffer
    while(1) {
        cChar = *p++;
		bytes2tx--;
        wBytes++;	// Cuento los bytes que voy trasmitiendo
		rBchar_Poke( &xCom->uart->TXringBuffer, &cChar  );
        
        // Condiciones de salida:
        // No mas bytes.
        if ( bytes2tx == 0 ) {
            break;
        }
        // NULL
        if ( *p == '\0') {
            break;
        }
        // RINGBUFFER FULL
        if (  rBchar_ReachHighWaterMark( &xCom->uart->TXringBuffer ) ) {
            break;
        }
    }
    
    // PASO 2: Arranco a transmitir
    rBchar_Pop( &xCom->uart->TXringBuffer, &cChar  );
    drv_uart_SendByte( xCom->uart->uart_id, cChar);
    drv_uart_enable_tx_int( xCom->uart->uart_id );
    
    // PASO 3: Si hay mas datos, monitoreo el rxBuffer para seguir alimentandolo
    while(1) {
        // Condiciones de salida:
        // No mas bytes.
        if ( bytes2tx == 0 ) {
            break;
        }
        // NULL
        if ( *p == '\0') {
            break;
        }   
        
        // Si hay datos y lugar los encolo
        if ( rBchar_ReachLowWaterMark( &xCom->uart->TXringBuffer ) ) {
            exit_flag = rBchar_ReachHighWaterMark( &xCom->uart->TXringBuffer ) || ( bytes2tx == 0);
            while(!exit_flag) {
                cChar = *p++;
                bytes2tx--;
                wBytes++;	// Cuento los bytes que voy trasmitiendo
                rBchar_Poke( &xCom->uart->TXringBuffer, &cChar  );
                exit_flag = rBchar_ReachHighWaterMark( &xCom->uart->TXringBuffer ) || ( bytes2tx == 0);
            }
        }
        
        // Espero( Simula sacando bytes)
        vTaskDelay( ( TickType_t)( 1 ) );
    }
       
    // Y doy 1 ms para que se vacie el shift register.
	vTaskDelay( ( TickType_t)( 1 ) );

	return (wBytes);   
}
//------------------------------------------------------------------------------------
int16_t frtos_uart_write_poll( periferico_serial_port_t *xCom, const char *pvBuffer, const uint16_t xBytes )
{
	// Transmite los datos del pvBuffer de a uno. No usa interrupcion de TX.
	//
    
int16_t wBytes = 0;
char cChar = '\0';
char *p = NULL;
uint16_t bytes2tx = 0;
int timeout;
 
 
	// Controlo no hacer overflow en la cola de trasmision
	bytes2tx = xBytes;

	// Trasmito.
	// Espero que los buffers esten vacios. ( La uart se va limpiando al trasmitir )
	while  ( rBchar_GetCount( &xCom->uart->TXringBuffer ) > 0 )
		vTaskDelay( ( TickType_t)( 1 ) );

	// Cargo el buffer en la cola de trasmision.
	p = (char *)pvBuffer;

	while ( bytes2tx-- > 0 ) {
		// Voy cargando la cola de a uno.
		cChar = *p;
		timeout = 10;	// Espero 10 ticks maximo
		while( --timeout > 0) {

			if ( USART_IsTXDataRegisterEmpty(xCom->uart->usart) ) {
				//USART_PutChar(xCom->uart->usart, cChar);
                USART3_sendChar(cChar);
				p++;
				wBytes++;	// Cuento los bytes que voy trasmitiendo
				break;
			} else {
				// Espero
				vTaskDelay( ( TickType_t)( 1 ) );
			}

			if ( timeout == 0 ) {
				// Error de transmision: SALGO
				return(-1);
			}

		}
	}

	return (wBytes);

    
}
//------------------------------------------------------------------------------------
int16_t frtos_uart_ioctl( periferico_serial_port_t *xCom, uint32_t ulRequest, void *pvValue )
{

int16_t xReturn = 0;

	switch( ulRequest )
	{

		case ioctl_SET_TIMEOUT:
			xCom->xBlockTime = *((uint8_t *)pvValue);
			break;
		case ioctl_UART_CLEAR_RX_BUFFER:
			rBchar_Flush(&xCom->uart->RXringBuffer);
			break;
		case ioctl_UART_CLEAR_TX_BUFFER:
			rBchar_Flush(&xCom->uart->TXringBuffer);
			break;
		case ioctl_UART_ENABLE_TX_INT:
			drv_uart_enable_tx_int( xCom->uart->uart_id );
			break;
		case ioctl_UART_DISABLE_TX_INT:
			drv_uart_disable_tx_int( xCom->uart->uart_id );
			break;
		case ioctl_UART_ENABLE_RX_INT:
			drv_uart_enable_rx_int( xCom->uart->uart_id );
			break;
		case ioctl_UART_DISABLE_RX_INT:
			drv_uart_disable_rx_int( xCom->uart->uart_id );
			break;
		case ioctl_UART_ENABLE_TX:
			drv_uart_enable_tx( xCom->uart->uart_id );
			break;
		case ioctl_UART_DISABLE_TX:
			drv_uart_disable_tx( xCom->uart->uart_id );
			break;
		case ioctl_UART_ENABLE_RX:
			drv_uart_enable_rx( xCom->uart->uart_id );
			break;
		case ioctl_UART_DISABLE_RX:
			drv_uart_disable_rx( xCom->uart->uart_id );
			break;
		default :
			xReturn = -1;
			break;
	}

	return xReturn;

}
//------------------------------------------------------------------------------------
int16_t frtos_uart_read( periferico_serial_port_t *xCom, char *pvBuffer, uint16_t xBytes )
{
	// Lee caracteres de la cola de recepcion y los deja en el buffer.
	// El timeout lo fijo con ioctl.

int16_t xBytesReceived = 0U;
TickType_t xTicksToWait = 10;
TimeOut_t xTimeOut;

     /* Initialize xTimeOut.  This records the time at which this function was
        entered. 
      */
	vTaskSetTimeOutState( &xTimeOut );

	// Are there any more bytes to be received?
	while( xBytesReceived < xBytes )
	{

		if( rBchar_Pop( &xCom->uart->RXringBuffer, &((char *)pvBuffer)[ xBytesReceived ] ) == true ) {
			xBytesReceived++;
            /*
             Recibi un byte. Re-inicio el timeout.
             */
            vTaskSetTimeOutState( &xTimeOut );
			//taskYIELD();
            //vTaskDelay( ( TickType_t)( 1 ) );
		} else {
			// Espero xTicksToWait antes de volver a chequear
			vTaskDelay( ( TickType_t)( 1 ) );

            // Time out has expired ?
            if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) != pdFALSE )
            {
                break;
            }
        }
    }

	return ( xBytesReceived );

}
//------------------------------------------------------------------------------
// FUNCIONES ESPECIFICAS DEL BUS I2C/TWI
//------------------------------------------------------------------------------
int16_t frtos_i2c_open( periferico_i2c_port_t *xI2c, file_descriptor_t fd, StaticSemaphore_t *i2c_semph, uint32_t flags)
{
	// Asigno las funciones particulares ed write,read,ioctl
	xI2c->fd = fd;
	xI2c->xBusSemaphore = xSemaphoreCreateMutexStatic( i2c_semph );
	xI2c->xBlockTime = (10 / portTICK_PERIOD_MS );
	xI2c->i2c_error_code = I2C_OK;
	//
	// Abro e inicializo el puerto I2C solo la primera vez que soy invocado
	drv_I2C_init();
	return(1);
}
//------------------------------------------------------------------------------
int16_t frtos_i2c_ioctl( periferico_i2c_port_t *xI2c, uint32_t ulRequest, void *pvValue )
{

int16_t xReturn = 0;
    
uint32_t *p = NULL;

	p = pvValue;

	switch( ulRequest )
	{
		case ioctl_OBTAIN_BUS_SEMPH:
			// Espero el semaforo en forma persistente.
			while ( xSemaphoreTake(xI2c->xBusSemaphore, ( TickType_t ) 5 ) != pdTRUE )
				taskYIELD();
			break;
			case ioctl_RELEASE_BUS_SEMPH:
				xSemaphoreGive( xI2c->xBusSemaphore );
				break;
			case ioctl_SET_TIMEOUT:
				xI2c->xBlockTime = *p;
				break;
			case ioctl_I2C_SET_DEVADDRESS:
				xI2c->devAddress = (int8_t)(*p);
				break;
			case ioctl_I2C_SET_DATAADDRESS:
				xI2c->dataAddress = (uint16_t)(*p);
				break;
			case ioctl_I2C_SET_DATAADDRESSLENGTH:
				xI2c->dataAddress_length = (int8_t)(*p);
				break;
			case ioctl_I2C_GET_LAST_ERROR:
				xReturn = xI2c->i2c_error_code;
				break;
            case ioctl_I2C_SET_DEBUG:
				drv_I2C_config_debug(true);
				break;
            case ioctl_I2C_CLEAR_DEBUG:
				drv_I2C_config_debug(false);
				break;
            case ioctl_I2C_RESET:
				drv_I2C_reset();
				break;                
			default :
				xReturn = -1;
				break;
		}

	return xReturn;

}
//------------------------------------------------------------------------------
int16_t frtos_i2c_read( periferico_i2c_port_t *xI2c, char *pvBuffer, const uint16_t xBytes )
{

int16_t xReturn = 0U;

	if ( ( xReturn = drv_I2C_master_read(xI2c->devAddress, xI2c->dataAddress, xI2c->dataAddress_length, (char *)pvBuffer, xBytes)) > 0 ) {
		xI2c->i2c_error_code = I2C_OK;
	} else {
		// Error de escritura indicado por el driver.
		xI2c->i2c_error_code = I2C_RD_ERROR;
	}
    return(xReturn);
}
//------------------------------------------------------------------------------
int16_t frtos_i2c_write( periferico_i2c_port_t *xI2c, const char *pvBuffer, const uint16_t xBytes )
{

int16_t xReturn = 0U;

	if ( ( xReturn = drv_I2C_master_write(xI2c->devAddress, xI2c->dataAddress, xI2c->dataAddress_length, (char *)pvBuffer, xBytes) ) > 0 ) {
		xI2c->i2c_error_code = I2C_OK;
	} else {
		// Error de escritura indicado por el driver.
		xI2c->i2c_error_code = I2C_WR_ERROR;
	}

	return(xReturn);

}
//------------------------------------------------------------------------------
// FUNCIONES ESPECIFICAS DE NVM
//------------------------------------------------------------------------------
int16_t frtos_nvm_open( periferico_nvm_t *xNVM, file_descriptor_t fd, StaticSemaphore_t *nvm_semph, uint32_t flags)
{
	// Asigno las funciones particulares ed write,read,ioctl
	xNVM->fd = fd;
	xNVM->xBusSemaphore = xSemaphoreCreateMutexStatic( nvm_semph );
	xNVM->xBlockTime = (10 / portTICK_PERIOD_MS );
	//
	// Abro e inicializo el puerto I2C solo la primera vez que soy invocado
	//drv_I2C_init();
	return(1);
}
//------------------------------------------------------------------------------
int16_t frtos_nvm_ioctl( periferico_nvm_t *xNVM, uint32_t ulRequest, void *pvValue )
{

int16_t xReturn = 0;
    
uint32_t *p = NULL;

	p = pvValue;

	switch( ulRequest )
	{
		case ioctl_OBTAIN_BUS_SEMPH:
			// Espero el semaforo en forma persistente.
			while ( xSemaphoreTake(xNVM->xBusSemaphore, ( TickType_t ) 5 ) != pdTRUE )
				taskYIELD();
			break;
			case ioctl_RELEASE_BUS_SEMPH:
				xSemaphoreGive( xNVM->xBusSemaphore );
				break;
			case ioctl_SET_TIMEOUT:
				xNVM->xBlockTime = *p;
				break;
			case ioctl_NVM_SET_EEADDRESS:
				xNVM->eeAddress = *p;
				break;                
			default :
				xReturn = -1;
				break;
		}

	return xReturn;

}
//------------------------------------------------------------------------------
int16_t frtos_nvm_read( periferico_nvm_t *xNVM, char *pvBuffer, const uint16_t xBytes )
{

int16_t xReturn = xBytes;

    FLASH_0_read_eeprom_block( xNVM->eeAddress, (uint8_t *)pvBuffer, xBytes );
    return(xReturn);
}
//------------------------------------------------------------------------------
int16_t frtos_nvm_write( periferico_nvm_t *xNVM, const char *pvBuffer, const uint16_t xBytes )
{

nvmctrl_status_t nvm_status;

    nvm_status = FLASH_0_write_eeprom_block( xNVM->eeAddress, (uint8_t *)pvBuffer, xBytes);
    if ( nvm_status == NVM_OK) {
        return(xBytes);
    }
    return(-1);

}
//------------------------------------------------------------------------------
