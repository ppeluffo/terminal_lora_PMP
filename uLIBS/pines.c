
#include "pines.h"

// -----------------------------------------------------------------------------
void ACGEN_init(void)
{
	// Configura los pines como output
    
	AC0_PORT.DIR |= AC0_PIN_bm;	
	CLEAR_AC0();

	AC1_PORT.DIR |= AC1_PIN_bm;	
	CLEAR_AC1();
  
}
// -----------------------------------------------------------------------------
bool ACGEN_set_pin( uint8_t pin_id, uint8_t pin_val )
{
       
    if ((pin_id == 0) && ( pin_val == 0) ) {
        CLEAR_AC0();
        return(true);
    }

    if ((pin_id == 0) && ( pin_val == 1) ) {
        SET_AC0();
        return(true);
    }

    if ((pin_id == 1) && ( pin_val == 0) ) {
        CLEAR_AC1();
        return(true);
    }

    if ((pin_id == 1) && ( pin_val == 1) ) {
        SET_AC1();
        return(true);
    }

    return(false);
    
}
// -----------------------------------------------------------------------------
