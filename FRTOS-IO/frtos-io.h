/*
 * frtos-io.h
 *
 *  Created on: 11 jul. 2018
 *      Author: pablo
 *
 * Funcionamiento:
 * Usamos los servicios que nos brindan los drivers.
 * Definimos para cada periferico una estructura de control que depende del periferico.
 * En el caso del los puertos seriales es periferico_serial_port_t.
 * Este tiene un elemento que es un puntero a una uart definida en el driver.
 *
 * Cada periferico se asocia a un file descriptor de modo que las funciones genericas
 * frtos_open/ioctl/read/write por medio de un switch redirigen a funciones mas especializadas
 * en cada tipo de periferico.
 *
 */

#ifndef SRC_FRTOS_IO_FRTOS_IO_H_
#define SRC_FRTOS_IO_FRTOS_IO_H_

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "drv_uart_avrDX.h"
#include "drv_i2c_avrDX.h"
#include "drv_nvm_avrDX.h"

// Identificador de los file descriptor.
typedef enum {
	fdTERM = 1,
    fdLORA,  
    fdI2C,
    fdNVM,
} file_descriptor_t;


// Estructuctura generica de un periferico tipo bus i2c.
typedef struct {
	file_descriptor_t fd;
	SemaphoreHandle_t xBusSemaphore;		//
	uint8_t xBlockTime;						// ticks to block in read operations. Set by ioctl
	uint8_t devAddress;
	uint16_t dataAddress;
	uint8_t dataAddress_length;
	uint8_t i2c_error_code;

} periferico_i2c_port_t;

// Estructuctura generica de un periferico tipo bus i2c.
typedef struct {
	file_descriptor_t fd;
	SemaphoreHandle_t xBusSemaphore;		//
	uint8_t xBlockTime;						// ticks to block in read operations. Set by ioctl
    uint16_t eeAddress;
} periferico_nvm_t;

// Perifericos real.
periferico_i2c_port_t xBusI2C;
periferico_nvm_t xNVM;

StaticSemaphore_t I2C_xMutexBuffer;
StaticSemaphore_t NVM_xMutexBuffer;

#define ioctl_OBTAIN_BUS_SEMPH			1
#define ioctl_RELEASE_BUS_SEMPH			2
#define ioctl_SET_TIMEOUT				3

#define ioctl_UART_CLEAR_RX_BUFFER		4
#define ioctl_UART_CLEAR_TX_BUFFER		5
#define ioctl_UART_ENABLE_TX_INT		6
#define ioctl_UART_DISABLE_TX_INT		7
#define ioctl_UART_ENABLE_RX_INT		8
#define ioctl_UART_DISABLE_RX_INT		9
#define ioctl_UART_ENABLE_TX			10
#define ioctl_UART_DISABLE_TX			11
#define ioctl_UART_ENABLE_RX			12
#define ioctl_UART_DISABLE_RX			13

#define ioctl_I2C_SET_DEVADDRESS		14
//#define ioctl_I2C_SET_DEVADDRESSLENGTH	15
#define ioctl_I2C_SET_DATAADDRESS		16
#define ioctl_I2C_SET_DATAADDRESSLENGTH	17
#define ioctl_I2C_GET_LAST_ERROR		18
//#define ioctl_I2C_SCAN					19
#define ioctl_I2C_SET_DEBUG				20
#define ioctl_I2C_CLEAR_DEBUG           21
#define ioctl_I2C_RESET                 22

#define ioctl_NVM_SET_EEADDRESS         30

#define I2C_OK			0
#define I2C_RD_ERROR	1
#define I2C_WR_ERROR	2

int16_t frtos_open( file_descriptor_t fd, uint32_t flags);
void frtos_open_uart3(uint32_t baudrate);
int16_t frtos_i2c_open( periferico_i2c_port_t *xI2c, file_descriptor_t fd, StaticSemaphore_t *i2c_semph, uint32_t flags);
int16_t frtos_nvm_open( periferico_nvm_t *xNVM, file_descriptor_t fd, StaticSemaphore_t *i2c_semph, uint32_t flags);

int16_t frtos_write( file_descriptor_t fd ,const char *pvBuffer, const uint16_t xBytes );
int16_t frtos_uart3_write( const char *pvBuffer, const uint16_t xBytes );
int16_t frtos_i2c_write( periferico_i2c_port_t *xI2c, const char *pvBuffer, const uint16_t xBytes );
int16_t frtos_nvm_write( periferico_nvm_t *xNVM, const char *pvBuffer, const uint16_t xBytes );

int16_t frtos_ioctl( file_descriptor_t fd, uint32_t ulRequest, void *pvValue );
int16_t frtos_ioctl_uart3( uint32_t ulRequest, void *pvValue );
int16_t frtos_i2c_ioctl( periferico_i2c_port_t *xI2c, uint32_t ulRequest, void *pvValue );
int16_t frtos_nvm_ioctl( periferico_nvm_t *xNVM, uint32_t ulRequest, void *pvValue );

int16_t frtos_read( file_descriptor_t fd , char *pvBuffer, uint16_t xBytes );
int16_t frtos_read_uart3( char *pvBuffer, uint16_t xBytes );
int16_t frtos_i2c_read( periferico_i2c_port_t *xI2c, char *pvBuffer, const uint16_t xBytes );
int16_t frtos_nvm_read( periferico_nvm_t *xNVM, char *pvBuffer, const uint16_t xBytes );





#endif /* SRC_FRTOS_IO_FRTOS_IO_H_ */
