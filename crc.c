#include "crc.h"

uint16_t crc16_calc(uint8_t *pData, uint32_t len)
{
    unsigned char ucCRCHi = 0xFF;
    unsigned char ucCRCLo = 0xFF;
    int iIndex;
    if(pData == NULL || len == 0)
    {
        return 0;
    }
    while( len-- )
    {
        iIndex = ucCRCLo ^ *( pData++ );
        ucCRCLo = ( unsigned char )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( unsigned short )( ucCRCHi << 8 | ucCRCLo );
}
