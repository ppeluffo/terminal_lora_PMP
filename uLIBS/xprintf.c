


#include "xprintf.h"
#include "string.h"
#include "stdio.h"
#include "frtos-io.h"
#include "printf.h"
#include "semphr.h"


#define PRINTF_BUFFER_SIZE        256U
static uint8_t stdout_buff[PRINTF_BUFFER_SIZE];

SemaphoreHandle_t sem_STDOUT;
StaticSemaphore_t STDOUT_xMutexBuffer;

//------------------------------------------------------------------------------
void XPRINTF_init(void)
{
   // La UART queda abierta con frtos_open()
    sem_STDOUT = xSemaphoreCreateMutexStatic( &STDOUT_xMutexBuffer );
   
}
//------------------------------------------------------------------------------
int xprintf( const char *fmt, ...)
{
 
va_list args;
int i = 0;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	//vsnprintf( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
    vsnprintf_( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
    va_end(args);
    i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );
   
    xSemaphoreGive( sem_STDOUT );
    
	return(i);

}
//------------------------------------------------------------------------------
int xprintf_P( PGM_P fmt, ...)
{
	// Imprime formateando en el uart fd.usando el buffer stdout_buff.
	// Como se controla con semaforo, nos permite ahorrar los buffers de c/tarea.
	// Si bien vsnprintf no es thread safe, al usarla aqui con el semaforo del buffer
	// queda thread safe !!!
	// Al usar esta funcion no es necesario controlar el tama�o de los buffers ya que
	// los limita a PRINTF_BUFFER_SIZE

va_list args;
int i;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);
	vsnprintf_P( (char *)stdout_buff, sizeof(stdout_buff),fmt, args);
    va_end(args);
	i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );
	// Espero que se vacie el buffer 10ms.
    vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-----------------------------------------------------------------------------------
int xfprintf( int fd, const char *fmt, ...)
{
	// Imprime formateando en el uart fd.usando el buffer stdout_buff.
	// Como se controla con semaforo, nos permite ahorrar los buffers de c/tarea.
	// Si bien vsnprintf no es thread safe, al usarla aqui con el semaforo del buffer
	// queda thread safe !!!

va_list args;
int i = -1;

	// Espero el semaforo del buffer en forma persistente.
	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	// Ahora tengo en stdout_buff formateado para imprimir
	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
	va_start(args, fmt);   
    vsnprintf( (char *)stdout_buff,sizeof(stdout_buff),fmt,args);
    va_end(args);
	i = frtos_write(fd, (char *)stdout_buff, strlen((char *)stdout_buff) );

	xSemaphoreGive( sem_STDOUT );
	return(i);

}
//-------------------------------------------------------------------------------
int xputs( const char *str )
{
 
int i = 0;

	while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );

	memset(stdout_buff,'\0',PRINTF_BUFFER_SIZE);
    strncpy((char *)stdout_buff, str, PRINTF_BUFFER_SIZE );
    i = frtos_write(fdTERM, (char *)stdout_buff, strlen((char *)stdout_buff) );
    
    xSemaphoreGive( sem_STDOUT );
    
	return(i);

}
//------------------------------------------------------------------------------
void xputChar(unsigned char c)
{

    while ( xSemaphoreTake( sem_STDOUT, ( TickType_t ) 5 ) != pdTRUE )
		vTaskDelay( ( TickType_t)( 5 ) );
   
    stdout_buff[0] = c;
    stdout_buff[1] = '\0';
    frtos_write( fdTERM, (char *)stdout_buff, 1 );
   
    xSemaphoreGive( sem_STDOUT );
}
//------------------------------------------------------------------------------
