/*================================================================
 *
 * Abstraction layer for Q330 DB calls
 *
 * ==============================================================*/
#include "isi330.h"

int q330db_get_sn(UINT64 *snval)
{
    *snval = 0x010000044c8Ba181u;
    return 0;
}

int q330db_get_dp(UINT16 *dp)
{
    *dp = (UINT16) 3;
    return 0;
}

int q330db_get_station_code(char *stabuf)
{
    memset(stabuf, 0, ISI330_STATION_CODE_SIZE + 1);
    strncpy(stabuf, "YYY3", ISI330_STATION_CODE_SIZE);
    return 0;
}





