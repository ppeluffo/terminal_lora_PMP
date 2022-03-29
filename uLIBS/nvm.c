/*
 * l_nvm.c
 *
 *  Created on: 18 feb. 2019
 *      Author: pablo
 */

#include "nvm.h"

//------------------------------------------------------------------------------
void nvm_read_print_id(void)
{
nvm_device_serial_id_t avr_id;

    NVM_ID_read( &avr_id );
    xprintf_P(PSTR("AVR_ID: 0x%02x 0x%02x 0x%02x\r\n"), avr_id.devid0, avr_id.devid1, avr_id.devid2);
    xprintf_P(PSTR("SIGNATURE: 0x%02x 0x%02x 0x%02x 0x%02x "), avr_id.sernum0, avr_id.sernum1, avr_id.sernum2, avr_id.sernum3);
    xprintf_P(PSTR("0x%02x 0x%02x 0x%02x 0x%02x "), avr_id.sernum4, avr_id.sernum5, avr_id.sernum6, avr_id.sernum7);
    xprintf_P(PSTR("0x%02x 0x%02x 0x%02x 0x%02x "), avr_id.sernum8, avr_id.sernum9, avr_id.sernum10, avr_id.sernum11);
    xprintf_P(PSTR("0x%02x 0x%02x 0x%02x 0x%02x\r\n"), avr_id.sernum12, avr_id.sernum13, avr_id.sernum14, avr_id.sernum15);

}
//------------------------------------------------------------------------------
void NVM_ID_read( nvm_device_serial_id_t *avr_id )
{
	// El signature lo leo una sola vez.
	// Luego, como lo tengo en la memoria, no lo leo mas.

    avr_id->devid0 = SIGROW.DEVICEID0;
    avr_id->devid1 = SIGROW.DEVICEID1;
    avr_id->devid2 = SIGROW.DEVICEID2;
    
    avr_id->sernum0 = SIGROW.SERNUM0;
    avr_id->sernum1 = SIGROW.SERNUM1;
    avr_id->sernum2 = SIGROW.SERNUM2;
    avr_id->sernum3 = SIGROW.SERNUM3;
    avr_id->sernum4 = SIGROW.SERNUM4;
    avr_id->sernum5 = SIGROW.SERNUM5;
    avr_id->sernum6 = SIGROW.SERNUM6;
    avr_id->sernum7 = SIGROW.SERNUM7;
    avr_id->sernum8 = SIGROW.SERNUM8;
    avr_id->sernum9 = SIGROW.SERNUM9;
    avr_id->sernum10 = SIGROW.SERNUM10;
    avr_id->sernum11 = SIGROW.SERNUM11;
    avr_id->sernum12 = SIGROW.SERNUM12;
    avr_id->sernum13 = SIGROW.SERNUM13; 
    avr_id->sernum14 = SIGROW.SERNUM14;
    avr_id->sernum15 = SIGROW.SERNUM15;
                 
}
//------------------------------------------------------------------------------------
int8_t NVM_EE_read( uint16_t eeRdAddr, char *data, size_t size )
{
    frtos_ioctl( fdNVM, ioctl_OBTAIN_BUS_SEMPH, NULL);
    frtos_ioctl( fdNVM, ioctl_NVM_SET_EEADDRESS, &eeRdAddr);
   
    frtos_read( fdNVM, data , size );
    
    frtos_ioctl( fdNVM,ioctl_RELEASE_BUS_SEMPH, NULL);
    
    return(size);
    
}
//------------------------------------------------------------------------------
int8_t NVM_EE_write( uint16_t eeWrAddr, char *data, size_t size )
{

int16_t retValue = -1;
    
    frtos_ioctl( fdNVM, ioctl_OBTAIN_BUS_SEMPH, NULL);
    frtos_ioctl( fdNVM, ioctl_NVM_SET_EEADDRESS, &eeWrAddr);
    
    retValue = frtos_write( fdNVM, data , size );
    
    frtos_ioctl( fdNVM,ioctl_RELEASE_BUS_SEMPH, NULL);
    
    return(retValue);
	
}
//------------------------------------------------------------------------------
