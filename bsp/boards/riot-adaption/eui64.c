/**
\brief riot-adaption definition of the "eui64" bsp module.
*/

#include "string.h"
#include "eui64.h"

#include "luid.h"
#include "net/ieee802154.h"

//=========================== public ==========================================

void eui64_get(uint8_t* addressToWrite){
    luid_get((void *)addressToWrite, IEEE802154_LONG_ADDRESS_LEN);
}
