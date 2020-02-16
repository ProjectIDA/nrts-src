#pragma ident "$Id: string.c,v 1.2 2013/05/14 20:50:40 dechavez Exp $"
/*======================================================================
 * 
 * Generate various ADDOSS related strings
 *
 *====================================================================*/
#include "addoss.h"

char *addossTypeString(int code)
{
static struct {
    int code;
    char *string;
} StringMap[11] = {
    { 0, "reserved"},
    { 1, "sensor data, compressed"},
    { 2, "Request OSG Status"},
    { 3, "OSG Status"},
    { 4, "Request OBP Status"},
    { 5, "OBP Status"},
    { 6, "Set OBP Power"},
    { 7, "Set Trillium Power"},
    { 8, "Archived Sensor Data Request"},
    { 9, "Sensor Data, MiniSeed"},
    {10, "Sensor Data, 24-bit uncompressed"}
};
static char *unknown = "unknown payload ID";

    if (code > 0 && code < 11) return StringMap[code].string;
    return unknown;
}

/* Revision History
 *
 * $Log: string.c,v $
 * Revision 1.2  2013/05/14 20:50:40  dechavez
 * initial production release
 *
 */
