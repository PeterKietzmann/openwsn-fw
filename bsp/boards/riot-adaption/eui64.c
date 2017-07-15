/**
\brief riot-adaption definition of the "eui64" bsp module.
*/

#include "string.h"
#include "eui64.h"

//=========================== defines =========================================

// stm32f103rey, 96-bit unique ID address : pointer to 8 first bytes of the 12: 0->8
#define UNIQUE_ID_BASE_ADDRESS          0x1FFFF7E8
// stm32f103rey, 96-bit unique ID address : pointer to 8 last bytes of the 12: 4->12
#define UNIQUE_ID_LAST_ADDRESS          0x1FFFF7EC

//=========================== variables =======================================

// this is assuming the eui64 is store in the memory.
const uint8_t *uid  = (const uint8_t *) UNIQUE_ID_BASE_ADDRESS;
const uint8_t *luid = (const uint8_t *) UNIQUE_ID_LAST_ADDRESS;

//=========================== prototypes ======================================

//=========================== public ==========================================

void eui64_get(uint8_t* addressToWrite){
/*
    //first byte of IoT-Lab 2-bytes uid
    uint8_t iotlab=(luid[4] | (luid[6] << 7));
    //Pseudo-OUI 
    addressToWrite[0] = uid[3];
    addressToWrite[1] = luid[7];
    addressToWrite[2] = uid[0];
    addressToWrite[3] = uid[1];
    //some variable bytes in the register
    addressToWrite[4] = luid[6];
    addressToWrite[5] = uid[2];
    //IoT-Lab uid
    addressToWrite[6] = iotlab;//luid[4];
    addressToWrite[7] = luid[5];
*/
}

//=========================== private =========================================

//TODO: uint8_t * hash96to64(uint8_t *uid){
   
//}
