#pragma ident "$Id: version.c,v 1.3 2016/06/23 20:14:57 dechavez Exp $"
/*======================================================================
 * 
 * library version management
 *
 *====================================================================*/
#include "modbus.h"

static VERSION version = {3, 0, 2};

/* modbus library release notes

3.0.2   06/23/2016
        use SetHostAddr() to allow specifying host by name

3.0.1   10/13/2011
        new sources from libmodbus.org (3.0.1)

1.0.0   05/14/2009
        modbus.c: http://copyleft.free.fr/wordpress/index.php/libmodbus/

 */

char *modbusVersionString()
{
static char string[] = "modbus library version 100.100.100 and slop";

    snprintf(string, strlen(string), "modbus library version %d.%d.%d",
        version.major, version.minor, version.patch
    );

    return string;
}

VERSION *modbusVersion()
{
    return &version;
}
