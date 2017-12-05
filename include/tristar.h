#pragma ident "$Id: tristar.h,v 1.4 2016/06/06 21:44:39 dechavez Exp $"
/*======================================================================
 *
 *  Support for the IDA solar power system.
 *
 * The design of this system assumes that there will be a single MODBUS
 * server, with two slaves.  Slave 1 MUST be a charge device and it will
 * be one of (currently) two possible types: a Tristar-45/60 running in
 * charge mode, OR a Tristar-MPPT-45/60 (which only has charge mode).
 * Slave 2 MUST (currently) be a Tristar-45/60 running in load mode.  Any
 * other combination will trigger a library abort.
 *
 * The Tristar-45/60 devices are assumed to follow the MODBUS specification
 * in document:
 *                 TriStar MODBUS Specification
 *                   Morningstar Corporation
 *                             V 02
 *                       19 October 2005
 *
 * while the Tristar-MPPT-45/60 support was based on the document:
 *
 *                     TriStar MPPT
 *                  MODBUS Specification
 *                        V10.2
 *                   5 December 2011
 *
 * There are three things you can do with this library:
 *
 * 1) Read a single register
 * 2) Generate a state of health report
 *    Here, "state of health" is nothing more than a collection of
 *    readings taken one after another.  The selection of items to read
 *    is hard-coded in the library in the TS_SOH structure.
 * 3) Issue commands
 *    A MODBUS command is accomplished by writing to specific registers
 *    which are called "coils" in the spec.  All of the coils referred
 *    to in the documentation are supported.
 *
 * The MODBUS I/O is done using libmodbus 3.0.1 from libmodbus.org
 * That implementation does not support the Read Device Identification
 * function, and as of this writing neither do any of the subsequent
 * releases.  Therefore the way we distinguish between Tristar and
 * Tristar-MPPT devices is by taking advantage of the fact that for the
 * Tristar-45/60 devices the serial number starts at address 0xF000 which
 * is not a valid Tristar-MPPT address.  So, we start out by assumming
 * that the devices is a Tristar-45/60 and if the serial number read
 * results in an error then we try to read it at the Tristar-MPPT
 * address.  If it succeds then we are good, and if not then that
 * triggers a library failure.
 *
 * I/O is done by calling tristarCreateHandle() to allocate and initialize
 * an instance of a TS handle, and then passing its pointer to
 * tristarConnect(). 
 *
 * To read a single item, use tristarReadItem().  It takes as its 
 * arguments a connected TS handle, a pointer to a TS_DESCR
 * structure that has the registers and scale factors required to convert
 * the raw readings and a pointer to a TS_ITEM to store the result.
 * A TS_DESCR contains the address(es) and scale factors which
 * are taken directly from the MODBUS manual appropriate for the
 * device.
 *
 * Commands are sent to the device via tristarSendCommand() whose
 * arguments are a connected TS handle an a TS_CMD_x generic command
 * code described below.  The mapping of the generic library codes
 * to the appropriate coil address is handled by the library, using
 * the TS_COIL_MAP.
 *
 *====================================================================*/
#ifndef tristar_h_included
#define tristar_h_included

#include "platform.h"
#include "modbus.h"
#include "logio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Some basic constants */

#define TS_DEFAULT_PORT MODBUS_TCP_DEFAULT_PORT
#define TS_DEFAULT_SERVER "spc"
#define TS_SERIALNO_LEN 8
#define TS_DIPSWITCH_STRING_LEN 47 /* "1=off 2=off 3=off 4=off 5=off 6=off 7=off 8=off" */
#define TS_VERSION_NUMBER_STRING_LEN 5 /* "00.00" */

/* This library supports two models of hardware, each with its
 * own set of MODBUS addresses:
 *
 * TS_MODEL_TS => TriStar MODBUS Specification V02 19 October 2005
 * TS_MODEL_MP => TriStar MPPT MODBUS Specification V10.2 5 December 2011
 */

#define TS_MODEL_UNKNOWN 0
#define TS_MODEL_TS      1 /* TS-45/60 */
#define TS_MODEL_MP      2 /* TS-MPPT-45/60 */

/* By IDA *convention*, slave 1 is *always* charge and 2 is *always* load */

#define TS_SLAVE_NONE 0
#define TS_SLAVE_CHRG 1
#define TS_SLAVE_LOAD 2

/* Each instance of a TS handle, defined below, will allow you to connect
 * to one of three possible devices
 */

#define TS_DEVICE_NONE 0x00

#define TS_DEVICE_TSCH 0x01 /* TS-45/60 in charge mode */
#define TS_DEVICE_TSLD 0x02 /* TS-45/60 in load mode */
#define TS_DEVICE_MPPT 0x04 /* TS-MPPT-45/60 (only a charge controller) */

#define TS_DEVICE_TS45 0x08 /* TS-45 (only used by tristarDeviceString() */
#define TS_DEVICE_TS60 0x10 /* TS-60 (only used by tristarDeviceString() */
#define TS_DEVICE_MP45 0x20 /* TS-MPPT-45 (only used by tristarDeviceString() */
#define TS_DEVICE_MP60 0x40 /* TS-MPPT-60 (only used by tristarDeviceString() */

#define TS_DEVICE_ALL       (TS_DEVICE_TSCH | TS_DEVICE_TSLD | TS_DEVICE_MPPT)

/* The model, device and serial number are needed often enough that
 * combining them into this handy little structure is provided to
 * simplify passing them around.
 */

typedef struct {
    int model;                        /* TS_MODEL_x */
    int device;                       /* TS_DEVICE_x */
    char serialno[TS_SERIALNO_LEN+1]; /* serial number string */
} TS_IDENT;

/* The MODBUS protocol uses 16 bit "PDU" addresses to refer to 16 bit objects.
 * 32-bit objects are handled with a pair of MODBUS addresses, one each for
 * the low-order and high-order words. The library's notion of an address consists
 * of a pair of MODBUS addresses, of which the low order address is required
 * and the high order address is optional.
 */

#define TS_XXX 0xFFFF /* not a valid TS or TS-MPPT address */

typedef struct {
    UINT16 hi; /* address of high order word (if not TS_XXX) */
    UINT16 lo; /* address of low order word */
} TS_ADDR;

/* Possible units for the various items
 * These are given in the vendor supplied MODBUS documentation
 */

#define TS_UNITS_NA  0 /* not applicable (eg, strings and bitfields) */
#define TS_UNITS_V   1 /* volts */
#define TS_UNITS_A   2 /* amps */
#define TS_UNITS_C   3 /* deg C */
#define TS_UNITS_H   4 /* hours */
#define TS_UNITS_AH  5 /* amp-hours */
#define TS_UNITS_KWH 6 /* kilowatt-hours */
#define TS_UNITS_P   7 /* percent */
#define TS_UNITS_D   8 /* days */

/* A TS_DESC contains all the information needed to read/write a particular
 * value.  It is basically the contents of the registers as described in
 * the MODBUS document. The scale factor is taken from the "Scaling or Range"
 * field in the document.  For TS-45/60 devices, that is THE scale factor
 * to convert the raw reading to real units.  For TS-MPPT devices *some*
 * quantities need to further multiply by a second scale factor that
 * is read from the device (called V_PU and I_PU factors.  The "pu" field
 * contains one of the following T_PU_x flags to indicate which of these
 * additionalscale factors (if any) should be applied.
 */

#define TS_PU_0 0 /* no further scaling required */
#define TS_PU_V 1 /* scale factor must be multiplied by V_PU */
#define TS_PU_I 2 /* scale factor must be multiplied by I_PU */
#define TS_PU_B 3 /* scale factor must be multiplied by both V_PU and I_PU */

typedef struct {
    TS_ADDR addr; /* PDU address(es) with which to load register reading */
    int units;    /* TS_UNITS_x */
    REAL64 scale; /* base unit conversion factor (0 => TS_UNITS_NA) */
    int pu;       /* TS_PU_x */
} TS_DESC;

/* Some handy constants for use as TS_DESC scale values */

#define TS_SCALE___1_____2M15 0.000030517578125000 /*   1     * 2^-15 */
#define TS_SCALE___3_____2M15 0.000091552734375000 /*   3     * 2^-15 */
#define TS_SCALE___6_6___2M15 0.000567993164062500 /*   6.6   * 2^-15 */
#define TS_SCALE__18_612_2M15 0.000567993164062500 /*  18.612 * 2^-15 */
#define TS_SCALE__96_667_2M15 0.002950042724609375 /*  96.667 * 2^-15 */
#define TS_SCALE__66_667_2M15 0.002034515380859375 /*  66.667 * 2^-15 */
#define TS_SCALE_139_15__2M15 0.004246520996093750 /* 139.15  * 2^-15 */
#define TS_SCALE_316_67__2M15 0.009664001464843750 /* 316.67  * 2^-15 */

/* Below are the TS_DESC contants for all the items this library supports.
 * The names of these constants are of the form TS_DESC_XX_name where
 * XX=TS for the TS-45/60 and XX=MP for the TS-MPPT and 'name' is what
 * is used in the MODBUS document.  
 * 
 * TS-MPPT-45/60 descriptions
 * Page numbers refer to "Tristar MODBUS document V 02 19 October 2005"
 * 
 * RAM - page 4 */

#define TS_DESC_TS_adc_vb_f        { {TS_XXX, 0x0008}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }
#define TS_DESC_TS_adc_vs_f        { {TS_XXX, 0x0009}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }
#define TS_DESC_TS_adc_vx_f        { {TS_XXX, 0x000A}, TS_UNITS_V,   TS_SCALE_139_15__2M15, TS_PU_0 }
#define TS_DESC_TS_adc_ipv_f       { {TS_XXX, 0x000B}, TS_UNITS_A,   TS_SCALE__66_667_2M15, TS_PU_0 }
#define TS_DESC_TS_adc_iload_f     { {TS_XXX, 0x000C}, TS_UNITS_A,   TS_SCALE_316_67__2M15, TS_PU_0 }
#define TS_DESC_TS_Vb_f            { {TS_XXX, 0x000D}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }
#define TS_DESC_TS_T_hs            { {TS_XXX, 0x000E}, TS_UNITS_C,   1.0,                   TS_PU_0 }
#define TS_DESC_TS_T_batt          { {TS_XXX, 0x000F}, TS_UNITS_C,   1.0,                   TS_PU_0 }
#define TS_DESC_TS_V_ref           { {TS_XXX, 0x0010}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }
#define TS_DESC_TS_Ah_r            { {0x0011, 0x0012}, TS_UNITS_AH,  0.1,                   TS_PU_0 }
#define TS_DESC_TS_Ah_t            { {0x0013, 0x0014}, TS_UNITS_AH,  0.1,                   TS_PU_0 }
#define TS_DESC_TS_hourmeter       { {0x0015, 0x0016}, TS_UNITS_H,   1.0,                   TS_PU_0 }
#define TS_DESC_TS_alarm           { {0x001D, 0x0017}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_fault           { {TS_XXX, 0x0018}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_dip_switch      { {TS_XXX, 0x0019}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_control_mode    { {TS_XXX, 0x001A}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_control_state   { {TS_XXX, 0x001B}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_d_filt          { {TS_XXX, 0x001C}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
/* These next two don't appear in the document, but actually, while EVb_min, EVb_max (below) give zeros. */
/* I'm not sure where I found them, I must have used a different version of the document the first time. */
#define TS_DESC_TS_min_batt_v      { {TS_XXX, 0x001F}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }
#define TS_DESC_TS_max_batt_v      { {TS_XXX, 0x0020}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }

/* EEPROM - pages 5, 6 */

#define TS_DESC_TS_EVb_min         { {TS_XXX, 0xE02B}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }
#define TS_DESC_TS_EVb_max         { {TS_XXX, 0xE02C}, TS_UNITS_V,   TS_SCALE__96_667_2M15, TS_PU_0 }

/* Calibration values - page 6 */

#define TS_DESC_TS_serial01        { {TS_XXX, 0xF000}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_serial23        { {TS_XXX, 0xF001}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_serial45        { {TS_XXX, 0xF002}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_serial67        { {TS_XXX, 0xF003}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_K_hw_ver        { {TS_XXX, 0xF00A}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_TS_K_amp60         { {TS_XXX, 0xF00B}, TS_UNITS_NA,  0.0,                   TS_PU_0 }

/* TS-MPPT-45/60 descriptions
 * Page numbers refer to "Tristar MPPT MODBUS document V10.2 5 December 2011"
 *
 * RAM - page 3 */

#define TS_DESC_MP_V_PU_hi         { {TS_XXX, 0x0000}, TS_UNITS_NA,  1.0,                   TS_PU_0 }
#define TS_DESC_MP_V_PU_lo         { {TS_XXX, 0x0001}, TS_UNITS_NA,  1.0,                   TS_PU_0 }
#define TS_DESC_MP_I_PU_hi         { {TS_XXX, 0x0002}, TS_UNITS_NA,  1.0,                   TS_PU_0 }
#define TS_DESC_MP_I_PU_lo         { {TS_XXX, 0x0003}, TS_UNITS_NA,  1.0,                   TS_PU_0 }
#define TS_DESC_MP_ver_sw          { {TS_XXX, 0x0004}, TS_UNITS_NA,  1.0,                   TS_PU_0 }

#define TS_DESC_MP_adc_vb_f_med    { {TS_XXX, 0x0018}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }
#define TS_DESC_MP_adc_vbterm_f    { {TS_XXX, 0x0019}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }
#define TS_DESC_MP_adc_vbs_f       { {TS_XXX, 0x001A}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }
#define TS_DESC_MP_adc_va_f        { {TS_XXX, 0x001B}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }
#define TS_DESC_MP_adc_ib_f_shadow { {TS_XXX, 0x001C}, TS_UNITS_A,   TS_SCALE___1_____2M15, TS_PU_I }
#define TS_DESC_MP_adc_ia_f_shadow { {TS_XXX, 0x001D}, TS_UNITS_A,   TS_SCALE___1_____2M15, TS_PU_I }
#define TS_DESC_MP_adc_p12_f       { {TS_XXX, 0x001E}, TS_UNITS_V,   TS_SCALE__18_612_2M15, TS_PU_0 }
#define TS_DESC_MP_adc_p3_f        { {TS_XXX, 0x001F}, TS_UNITS_V,   TS_SCALE___6_6___2M15, TS_PU_0 }
#define TS_DESC_MP_adc_pmeter_f    { {TS_XXX, 0x0020}, TS_UNITS_V,   TS_SCALE__18_612_2M15, TS_PU_0 }
#define TS_DESC_MP_adc_p18_f       { {TS_XXX, 0x0021}, TS_UNITS_V,   TS_SCALE___3_____2M15, TS_PU_V }
#define TS_DESC_MP_adc_v_ref       { {TS_XXX, 0x0022}, TS_UNITS_V,   TS_SCALE___3_____2M15, TS_PU_V }

#define TS_DESC_MP_T_hs            { {TS_XXX, 0x0023}, TS_UNITS_C,   1.0,                   TS_PU_0 }
#define TS_DESC_MP_T_rts           { {TS_XXX, 0x0024}, TS_UNITS_C,   1.0,                   TS_PU_0 }
#define TS_DESC_MP_T_batt          { {TS_XXX, 0x0025}, TS_UNITS_C,   1.0,                   TS_PU_0 }

#define TS_DESC_MP_adc_vb_f_1m     { {TS_XXX, 0x0026}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }
#define TS_DESC_MP_adc_ib_f_1m     { {TS_XXX, 0x0027}, TS_UNITS_A,   TS_SCALE___1_____2M15, TS_PU_I }
#define TS_DESC_MP_vb_min          { {TS_XXX, 0x0028}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }
#define TS_DESC_MP_vb_max          { {TS_XXX, 0x0029}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }
#define TS_DESC_MP_hourmeter       { {0x002A, 0x002B}, TS_UNITS_H,   1.0,                   TS_PU_0 }
#define TS_DESC_MP_fault           { {TS_XXX, 0x002C}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_alarm           { {0x002E, 0x002F}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_dip_all         { {TS_XXX, 0x0030}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_led_state       { {TS_XXX, 0x0031}, TS_UNITS_NA,  0.0,                   TS_PU_0 }

#define TS_DESC_MP_charge_state    { {TS_XXX, 0x0032}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_vb_ref          { {TS_XXX, 0x0033}, TS_UNITS_V,   TS_SCALE___1_____2M15, TS_PU_V }

/* TS_MPPT RAM - page 4 */

#define TS_DESC_MP_Ahc_r           { {0x0034, 0x0035}, TS_UNITS_AH,  0.1,                   TS_PU_0 }
#define TS_DESC_MP_Ahc_t           { {0x0036, 0x0037}, TS_UNITS_AH,  0.1,                   TS_PU_0 }
#define TS_DESC_MP_kwhc_r          { {TS_XXX, 0x0038}, TS_UNITS_KWH, 1.0,                   TS_PU_0 }
#define TS_DESC_MP_kwhc_t          { {TS_XXX, 0x0039}, TS_UNITS_KWH, 1.0,                   TS_PU_0 }

/* TS_MPPT EEPROM - page 6 */

#define TS_DESC_MP_serial01        { {TS_XXX, 0xE0C0}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_serial23        { {TS_XXX, 0xE0C1}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_serial45        { {TS_XXX, 0xE0C2}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_serial67        { {TS_XXX, 0xE0C3}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_Emodel          { {TS_XXX, 0xE0CC}, TS_UNITS_NA,  0.0,                   TS_PU_0 }
#define TS_DESC_MP_Ehw_version     { {TS_XXX, 0xE0CD}, TS_UNITS_NA,  0.0,                   TS_PU_0 }

/* All the supported descriptors in one handy structure */

typedef struct {
    TS_DESC TS_adc_vb_f;
    TS_DESC TS_adc_vs_f;
    TS_DESC TS_adc_vx_f;
    TS_DESC TS_adc_ipv_f;
    TS_DESC TS_adc_iload_f;
    TS_DESC TS_Vb_f;
    TS_DESC TS_T_hs;
    TS_DESC TS_T_batt;
    TS_DESC TS_V_ref;
    TS_DESC TS_Ah_r;
    TS_DESC TS_Ah_t;
    TS_DESC TS_hourmeter;
    TS_DESC TS_alarm;
    TS_DESC TS_fault;
    TS_DESC TS_dip_switch;
    TS_DESC TS_control_mode;
    TS_DESC TS_control_state;
    TS_DESC TS_d_filt;
    TS_DESC TS_min_batt_v;
    TS_DESC TS_max_batt_v;
    TS_DESC TS_EVb_min;
    TS_DESC TS_EVb_max;
    TS_DESC TS_serial01;
    TS_DESC TS_serial23;
    TS_DESC TS_serial45;
    TS_DESC TS_serial67;
    TS_DESC TS_K_hw_ver;
    TS_DESC TS_K_amp60;
    TS_DESC MP_V_PU_hi;
    TS_DESC MP_V_PU_lo;
    TS_DESC MP_I_PU_hi;
    TS_DESC MP_I_PU_lo;
    TS_DESC MP_ver_sw;
    TS_DESC MP_adc_vb_f_med;
    TS_DESC MP_adc_vbterm_f;
    TS_DESC MP_adc_vbs_f;
    TS_DESC MP_adc_va_f;
    TS_DESC MP_adc_ib_f_shadow;
    TS_DESC MP_adc_ia_f_shadow;
    TS_DESC MP_adc_p12_f;
    TS_DESC MP_adc_p3_f;
    TS_DESC MP_adc_pmeter_f;
    TS_DESC MP_adc_p18_f;
    TS_DESC MP_adc_v_ref;
    TS_DESC MP_T_hs;
    TS_DESC MP_T_rts;
    TS_DESC MP_T_batt;
    TS_DESC MP_adc_vb_f_1m;
    TS_DESC MP_adc_ib_f_1m;
    TS_DESC MP_vb_min;
    TS_DESC MP_vb_max;
    TS_DESC MP_hourmeter;
    TS_DESC MP_fault;
    TS_DESC MP_alarm;
    TS_DESC MP_dip_all;
    TS_DESC MP_led_state;
    TS_DESC MP_charge_state;
    TS_DESC MP_vb_ref;
    TS_DESC MP_Ahc_r;
    TS_DESC MP_Ahc_t;
    TS_DESC MP_kwhc_r;
    TS_DESC MP_kwhc_t;
    TS_DESC MP_serial01;
    TS_DESC MP_serial23;
    TS_DESC MP_serial45;
    TS_DESC MP_serial67;
    TS_DESC MP_Emodel;
    TS_DESC MP_Ehw_version;
} TS_SUPPORTED_DESCRIPTORS;

#ifdef TS_INCLUDE_STATIC_TS_DESCRIPTOR

static TS_SUPPORTED_DESCRIPTORS TS_DESCRIPTOR = {
    TS_DESC_TS_adc_vb_f,
    TS_DESC_TS_adc_vs_f,
    TS_DESC_TS_adc_vx_f,
    TS_DESC_TS_adc_ipv_f,
    TS_DESC_TS_adc_iload_f,
    TS_DESC_TS_Vb_f,
    TS_DESC_TS_T_hs,
    TS_DESC_TS_T_batt,
    TS_DESC_TS_V_ref,
    TS_DESC_TS_Ah_r,
    TS_DESC_TS_Ah_t,
    TS_DESC_TS_hourmeter,
    TS_DESC_TS_alarm,
    TS_DESC_TS_fault,
    TS_DESC_TS_dip_switch,
    TS_DESC_TS_control_mode,
    TS_DESC_TS_control_state,
    TS_DESC_TS_d_filt,
    TS_DESC_TS_min_batt_v,
    TS_DESC_TS_max_batt_v,
    TS_DESC_TS_EVb_min,
    TS_DESC_TS_EVb_max,
    TS_DESC_TS_serial01,
    TS_DESC_TS_serial23,
    TS_DESC_TS_serial45,
    TS_DESC_TS_serial67,
    TS_DESC_TS_K_hw_ver,
    TS_DESC_TS_K_amp60,
    TS_DESC_MP_V_PU_hi,
    TS_DESC_MP_V_PU_lo,
    TS_DESC_MP_I_PU_hi,
    TS_DESC_MP_I_PU_lo,
    TS_DESC_MP_ver_sw,
    TS_DESC_MP_adc_vb_f_med,
    TS_DESC_MP_adc_vbterm_f,
    TS_DESC_MP_adc_vbs_f,
    TS_DESC_MP_adc_va_f,
    TS_DESC_MP_adc_ib_f_shadow,
    TS_DESC_MP_adc_ia_f_shadow,
    TS_DESC_MP_adc_p12_f,
    TS_DESC_MP_adc_p3_f,
    TS_DESC_MP_adc_pmeter_f,
    TS_DESC_MP_adc_p18_f,
    TS_DESC_MP_adc_v_ref,
    TS_DESC_MP_T_hs,
    TS_DESC_MP_T_rts,
    TS_DESC_MP_T_batt,
    TS_DESC_MP_adc_vb_f_1m,
    TS_DESC_MP_adc_ib_f_1m,
    TS_DESC_MP_vb_min,
    TS_DESC_MP_vb_max,
    TS_DESC_MP_hourmeter,
    TS_DESC_MP_fault,
    TS_DESC_MP_alarm,
    TS_DESC_MP_dip_all,
    TS_DESC_MP_led_state,
    TS_DESC_MP_charge_state,
    TS_DESC_MP_vb_ref,
    TS_DESC_MP_Ahc_r,
    TS_DESC_MP_Ahc_t,
    TS_DESC_MP_kwhc_r,
    TS_DESC_MP_kwhc_t,
    TS_DESC_MP_serial01,
    TS_DESC_MP_serial23,
    TS_DESC_MP_serial45,
    TS_DESC_MP_serial67,
    TS_DESC_MP_Emodel,
    TS_DESC_MP_Ehw_version
};

#endif /* TS_INCLUDE_STATIC_TS_DESCRIPTOR */

/* The TS_ITEM structure is used to hold the quantities described by their corresponding TS_DESC */

typedef struct {
    INT32 raw;    /* raw register reading (INT32 in case of hi/lo addresses present) */
    REAL64 value; /* converted register reading */
    BOOL valid;   /* set to TRUE by tristarReadItem(), when read and conversion is successful */
} TS_ITEM;

/* TS_DEVICE_TSCH specific state of health items */

#define TS_MODEL_45  0x00 /* K_amp60, page 6 */
#define TS_MODEL_60  0x01 /* K_amp60, page 6 */

typedef struct {
    TS_ITEM K_hw_ver;
    TS_ITEM control_state;
    TS_ITEM hourmeter;
    TS_ITEM dip_switch;
    TS_ITEM T_hs;
    TS_ITEM T_batt;
    TS_ITEM min_batt_v;
    TS_ITEM max_batt_v;
    TS_ITEM adc_vb_f;
    TS_ITEM adc_vs_f;
    TS_ITEM adc_vx_f;
    TS_ITEM adc_ipv_f;
    TS_ITEM alarm;
    TS_ITEM fault;
    TS_ITEM K_amp60;
} TS_SOH_TSCH;

/* TS_DEVICE_TSLD specific state of health items */

typedef struct {
    TS_ITEM K_hw_ver;
    TS_ITEM control_state;
    TS_ITEM hourmeter;
    TS_ITEM dip_switch;
    TS_ITEM T_hs;
    TS_ITEM min_batt_v;
    TS_ITEM max_batt_v;
    TS_ITEM adc_vx_f;
    TS_ITEM adc_iload_f;
    TS_ITEM alarm;
    TS_ITEM fault;
    TS_ITEM K_amp60;
} TS_SOH_TSLD;

/* TS_DEVICE_MPPT specific state of health items */

#define TS_MPPT_EMODEL_45  0 /* page 20 */
#define TS_MPPT_EMODEL_60  1 /* page 20 */

typedef struct {
    TS_ITEM ver_sw;
    TS_ITEM adc_vbterm_f;
    TS_ITEM adc_vbs_f;
    TS_ITEM adc_va_f;
    TS_ITEM adc_ib_f_shadow;
    TS_ITEM T_hs;
    TS_ITEM T_rts;
    TS_ITEM vb_min;
    TS_ITEM vb_max;
    TS_ITEM hourmeter;
    TS_ITEM fault;
    TS_ITEM alarm;
    TS_ITEM dip_all;
    TS_ITEM charge_state;
    TS_ITEM Emodel;
    TS_ITEM Ehw_version;
} TS_SOH_MPPT;

/* device independent state of health report */

typedef struct {
    int which;                        /* TS_DEVICE_x */
    char serialno[TS_SERIALNO_LEN+1]; /* serial number */
    union {
        TS_SOH_TSCH tsch; /* for TS_DEVICE_TSCH */
        TS_SOH_TSLD tsld; /* for TS_DEVICE_TSLD */
        TS_SOH_MPPT mppt; /* for TS_DEVICE_MPPT */
    } device;
} TS_SOH;

/* TS-45/60 coils ("coils" are MODBUS-speak for registers we can write to)
 * From page 6 of Tristar MODBUS document V 02 19 October 2005
 */

#define TS_COIL_TS_EQTRIG         0x0000
#define TS_COIL_TS_DISCONNECT     0x0001
#define TS_COIL_TS_CLEAR_AH_R     0x0010
#define TS_COIL_TS_CLEAR_AH_T     0x0011
#define TS_COIL_TS_CLEAR_KWH      0x0012
#define TS_COIL_TS_RESET_BATT_SVC 0x0013
#define TS_COIL_TS_CLEAR_FAULTS   0x0014
#define TS_COIL_TS_CLEAR_ALARMS   0x0015
#define TS_COIL_TS_EEPROM_UPDATE  0x0016
#define TS_COIL_TS_LVD_OVERRIDE   0x0017
#define TS_COIL_TS_RESET_CONTROL  0x00FF

/* TS-MPPT-45/60 coils
 * From page 7 of Tristar MPPT MODBUS document V10.2 5 December 2011
 */

#define TS_COIL_MP_EQTRIG         0x0000
#define TS_COIL_MP_DISCONNECT     0x0002
#define TS_COIL_MP_CLEAR_AH_R     0x0010
#define TS_COIL_MP_CLEAR_AH_T     0x0011
#define TS_COIL_MP_CLEAR_KWH      0x0012
#define TS_COIL_MP_RESET_BATT_SVC 0x0013
#define TS_COIL_MP_CLEAR_FAULTS   0x0014
#define TS_COIL_MP_CLEAR_ALARMS   0x0015
#define TS_COIL_MP_EEPROM_UPDATE  0x0016
#define TS_COIL_MP_CLEAR_KWH_T    0x0018
#define TS_COIL_MP_CLEAR_VB_MM    0x0019
#define TS_COIL_MP_TEST_PHASE     0x00F0
#define TS_COIL_MP_RESET_CONTROL  0x00FF
#define TS_COIL_MP_SEND_TEST_1    0x1000
#define TS_COIL_MP_SEND_TEST_2    0x1001
#define TS_COIL_MP_SEND_TEST_3    0x1002
#define TS_COIL_MP_SEND_TEST_4    0x1003
#define TS_COIL_MP_RESET_SERVER   0x10FF

#define TS_COIL_UNSUPPORTED       0xFFFF /* none of the above */

/* Device independent command codes */

#define TS_CMD_ILLEGAL        -2 /* command not implemented by this library */
#define TS_CMD_UNSUPPORTED    -1 /* command not supported by the particular device */

/* library commands are common to all devices */

#define TS_CMD_NONE            0 /* no op */
#define TS_CMD_LIST            1 /* list commands supported by this device */
#define TS_CMD_SOH             2 /* generate SOH report */

/* MODBUS commands which common to all controllers */

#define TS_CMD_EQTRIG         11 /* equalize trigger */
#define TS_CMD_DISCONNECT     12 /* force control into disconnect state */
#define TS_CMD_CLEAR_AH_R     13 /* clear Ah resetable */
#define TS_CMD_CLEAR_AH_T     14 /* clear Ah total */
#define TS_CMD_CLEAR_KWH      15 /* clear kWh */
#define TS_CMD_RESET_BATT_SVC 16 /* reset battery service calendar */
#define TS_CMD_CLEAR_FAULTS   17 /* clear faults */
#define TS_CMD_CLEAR_ALARMS   18 /* clear alarms */
#define TS_CMD_EEPROM_UPDATE  19 /* force EEPROM update */
#define TS_CMD_RESET_CONTROL  20 /* reset controller */

/* MODBUS commands which are supported by load devices only */

#define TS_CMD_LVD_OVERRIDE  101 /* LVD override */

/* MODBUS commands supported by MPPT devices only */

#define TS_CMD_CLEAR_KWH_T   201 /* clear kWh total */
#define TS_CMD_CLEAR_VB_MM   202 /* clear Vb min and Vb max */
#define TS_CMD_TEST_PHASE    203 /* test single phase */
#define TS_CMD_SEND_TEST_1   204 /* send test notification 1 */
#define TS_CMD_SEND_TEST_2   205 /* send test notification 2 */
#define TS_CMD_SEND_TEST_3   206 /* send test notification 3 */
#define TS_CMD_SEND_TEST_4   207 /* send test notification 4 */
#define TS_CMD_RESET_SERVER  209 /* reset communications server */

/* for code development convenience */

#define TS_CMD_DEBUG        1000 /* trigger debug code */

#define TS_DEFAULT_CMD TS_CMD_SOH

#ifdef TS_INCLUDE_STATIC_CMD_MAP

typedef struct {
    int code;    /* TS_CMD_X command code */
    int device;  /* bit mask of which devices support this command */
    char *name;  /* command name */
    char *desc;  /* command description */
} TS_CMD_MAP_ENTRY;

static TS_CMD_MAP_ENTRY TS_CMD_MAP[] = {
    { TS_CMD_LIST,           TS_DEVICE_ALL,  "list",         "list supported commands"       },
    { TS_CMD_SOH,            TS_DEVICE_ALL,  "soh",          "state of health report"        },
    { TS_CMD_EQTRIG,         TS_DEVICE_ALL,  "eqtrig",       "equalize trigger"              },
    { TS_CMD_DISCONNECT,     TS_DEVICE_ALL,  "disconnect",   "disconnect"                    },
    { TS_CMD_CLEAR_AH_R,     TS_DEVICE_ALL,  "clearAhr",     "clear Ah resettable"           },
    { TS_CMD_CLEAR_AH_T,     TS_DEVICE_ALL,  "clearAhT",     "clear AH total"                },
    { TS_CMD_CLEAR_KWH,      TS_DEVICE_ALL,  "clearKwh",     "clear kwh resettable"          },
    { TS_CMD_CLEAR_KWH_T,    TS_DEVICE_MPPT, "clearKwhT",    "clear kwh total"               },
    { TS_CMD_RESET_BATT_SVC, TS_DEVICE_ALL,  "resetBattSvc", "reset battery service calendar"},
    { TS_CMD_CLEAR_FAULTS,   TS_DEVICE_ALL,  "clearFaults",  "clear faults"                  },
    { TS_CMD_CLEAR_ALARMS,   TS_DEVICE_ALL,  "clearAlarms",  "clear alarms"                  },
    { TS_CMD_EEPROM_UPDATE,  TS_DEVICE_ALL,  "eepromUpdate", "force EEPROM update"           },
    { TS_CMD_RESET_CONTROL,  TS_DEVICE_ALL,  "reset",        "reset controller"              },
    { TS_CMD_LVD_OVERRIDE,   TS_DEVICE_TSLD, "lvdOverride",  "LVD override for one cycle"    },
    { TS_CMD_CLEAR_VB_MM,    TS_DEVICE_MPPT, "clearVb",      "clear Vb min and max"          },
    { TS_CMD_TEST_PHASE,     TS_DEVICE_MPPT, "testPhase",    "test a single phase"           },
    { TS_CMD_SEND_TEST_1,    TS_DEVICE_MPPT, "test1",        "send test notification 1"      },
    { TS_CMD_SEND_TEST_2,    TS_DEVICE_MPPT, "test2",        "send test notification 2"      },
    { TS_CMD_SEND_TEST_3,    TS_DEVICE_MPPT, "test3",        "send test notification 3"      },
    { TS_CMD_SEND_TEST_4,    TS_DEVICE_MPPT, "test4",        "send test notification 4"      },
    { TS_CMD_RESET_SERVER,   TS_DEVICE_MPPT, "resetServer",  "reset communications server"   },
    { TS_CMD_DEBUG,          TS_DEVICE_ALL,  "debug",        "trigger debug code"            },
    { TS_CMD_NONE,           TS_DEVICE_NONE, NULL,           NULL                            }
};

#endif /* TS_INCLUDE_STATIC_CMD_MAP */

#ifdef TS_INCLUDE_STATIC_COIL_MAPS

typedef struct {
    int cmd;      /* TS_CMD_x */
    UINT16 coil;  /* TS_COIL_x */ 
} TS_COIL_MAP;

static TS_COIL_MAP TS_COIL_MAP_TSCH[] = {
    { TS_CMD_EQTRIG,         TS_COIL_TS_EQTRIG        },
    { TS_CMD_DISCONNECT,     TS_COIL_TS_DISCONNECT    },
    { TS_CMD_CLEAR_AH_R,     TS_COIL_TS_CLEAR_AH_R    },
    { TS_CMD_CLEAR_AH_T,     TS_COIL_TS_CLEAR_AH_T    },
    { TS_CMD_CLEAR_KWH,      TS_COIL_TS_CLEAR_KWH     },
    { TS_CMD_RESET_BATT_SVC, TS_COIL_TS_RESET_BATT_SVC},
    { TS_CMD_CLEAR_FAULTS,   TS_COIL_TS_CLEAR_FAULTS  },
    { TS_CMD_CLEAR_ALARMS,   TS_COIL_TS_CLEAR_ALARMS  },
    { TS_CMD_EEPROM_UPDATE,  TS_COIL_TS_EEPROM_UPDATE },
    { TS_CMD_RESET_CONTROL,  TS_COIL_TS_RESET_CONTROL },
    { TS_CMD_NONE,           TS_COIL_UNSUPPORTED      }
};

static TS_COIL_MAP TS_COIL_MAP_TSLD[] = {
    { TS_CMD_EQTRIG,         TS_COIL_TS_EQTRIG        },
    { TS_CMD_DISCONNECT,     TS_COIL_TS_DISCONNECT    },
    { TS_CMD_CLEAR_AH_R,     TS_COIL_TS_CLEAR_AH_R    },
    { TS_CMD_CLEAR_AH_T,     TS_COIL_TS_CLEAR_AH_T    },
    { TS_CMD_CLEAR_KWH,      TS_COIL_TS_CLEAR_KWH     },
    { TS_CMD_RESET_BATT_SVC, TS_COIL_TS_RESET_BATT_SVC},
    { TS_CMD_CLEAR_FAULTS,   TS_COIL_TS_CLEAR_FAULTS  },
    { TS_CMD_CLEAR_ALARMS,   TS_COIL_TS_CLEAR_ALARMS  },
    { TS_CMD_EEPROM_UPDATE,  TS_COIL_TS_EEPROM_UPDATE },
    { TS_CMD_RESET_CONTROL,  TS_COIL_TS_RESET_CONTROL },
    { TS_CMD_LVD_OVERRIDE,   TS_COIL_TS_LVD_OVERRIDE  },
    { TS_CMD_NONE,           TS_COIL_UNSUPPORTED      }
};

static TS_COIL_MAP TS_COIL_MAP_MPPT[] = {
    { TS_CMD_EQTRIG,         TS_COIL_MP_EQTRIG        },
    { TS_CMD_DISCONNECT,     TS_COIL_MP_DISCONNECT    },
    { TS_CMD_CLEAR_AH_R,     TS_COIL_MP_CLEAR_AH_R    },
    { TS_CMD_CLEAR_AH_T,     TS_COIL_MP_CLEAR_AH_T    },
    { TS_CMD_CLEAR_KWH,      TS_COIL_MP_CLEAR_KWH     },
    { TS_CMD_CLEAR_KWH_T,    TS_COIL_MP_CLEAR_KWH_T   },
    { TS_CMD_RESET_BATT_SVC, TS_COIL_MP_RESET_BATT_SVC},
    { TS_CMD_CLEAR_FAULTS,   TS_COIL_MP_CLEAR_FAULTS  },
    { TS_CMD_CLEAR_ALARMS,   TS_COIL_MP_CLEAR_ALARMS  },
    { TS_CMD_EEPROM_UPDATE,  TS_COIL_MP_EEPROM_UPDATE },
    { TS_CMD_RESET_CONTROL,  TS_COIL_MP_RESET_CONTROL },
    { TS_CMD_CLEAR_VB_MM,    TS_COIL_MP_CLEAR_VB_MM   },
    { TS_CMD_TEST_PHASE,     TS_COIL_MP_TEST_PHASE    },
    { TS_CMD_SEND_TEST_1,    TS_COIL_MP_SEND_TEST_1   },
    { TS_CMD_SEND_TEST_2,    TS_COIL_MP_SEND_TEST_2   },
    { TS_CMD_SEND_TEST_3,    TS_COIL_MP_SEND_TEST_3   },
    { TS_CMD_SEND_TEST_4,    TS_COIL_MP_SEND_TEST_4   },
    { TS_CMD_RESET_SERVER,   TS_COIL_MP_RESET_SERVER  },
    { TS_CMD_NONE,           TS_COIL_UNSUPPORTED      }
};

#endif /* TS_INCLUDE_STATIC_COIL_MAPS */

/* TS-45/64 control states
 * From page 10 of Tristar MODBUS document V 02 19 October 2005
 */

#define TS_TSCH_STATE_START        0
#define TS_TSCH_STATE_NIGHT_CHECK  1
#define TS_TSCH_STATE_DISCONNECT   2
#define TS_TSCH_STATE_NIGHT        3
#define TS_TSCH_STATE_FAULT        4
#define TS_TSCH_STATE_BULK         5
#define TS_TSCH_STATE_PWM          6
#define TS_TSCH_STATE_FLOAT        7
#define TS_TSCH_STATE_EQUALIZE     8

#define TS_TSLD_STATE_START        0
#define TS_TSLD_STATE_NORMAL       1
#define TS_TSLD_STATE_LVD_WARN     2
#define TS_TSLD_STATE_LVD          3
#define TS_TSLD_STATE_FAULT        4
#define TS_TSLD_STATE_DISCONNECT   5
#define TS_TSLD_STATE_NORMAL_OFF   6
#define TS_TSLD_STATE_OVERRIDE_LVD 7

/* TS-45/64 control states
 * From page 13 of Tristar MPPT MODBUS document V10.2 5 December 2011
 */

#define TS_MPPT_STATE_START        0
#define TS_MPPT_STATE_NIGHT_CHECK  1
#define TS_MPPT_STATE_DISCONNECT   2
#define TS_MPPT_STATE_NIGHT        3
#define TS_MPPT_STATE_FAULT        4
#define TS_MPPT_STATE_MPPT         5
#define TS_MPPT_STATE_ABSORPTION   6
#define TS_MPPT_STATE_FLOAT        7
#define TS_MPPT_STATE_EQUALIZE     8
#define TS_MPPT_STATE_SLAVE        9

/* TS-45/60 alarm bits
 * From page 24 of Tristar MODBUS document V 02 19 October 2005
 * The same alarm bits are used for both charge and load modes.
 */

#define TS_LDCH_ALARM_RTS_OPEN         0x00000001
#define TS_LDCH_ALARM_RTS_SHORTED      0x00000002
#define TS_LDCH_ALARM_RTS_DISCONNECTED 0x00000004
#define TS_LDCH_ALARM_THS_DISCONNECTED 0x00000008
#define TS_LDCH_ALARM_THS_SHORTED      0x00000010
#define TS_LDCH_ALARM_TRISTAR_HOT      0x00000020
#define TS_LDCH_ALARM_CURRENT_LIMIT    0x00000040
#define TS_LDCH_ALARM_CURRENT_OFFSET   0x00000080
#define TS_LDCH_ALARM_BATTERY_SENSE    0x00000100
#define TS_LDCH_ALARM_BATT_SENSE_DISC  0x00000200
#define TS_LDCH_ALARM_UNCALIBRATED     0x00000400
#define TS_LDCH_ALARM_RTS_MISWIRE      0x00000800
#define TS_LDCH_ALARM_HVD              0x00001000
#define TS_LDCH_ALARM_HIGH_D           0x00002000
#define TS_LDCH_ALARM_MISWIRE          0x00004000
#define TS_LDCH_ALARM_FET_OPEN         0x00008000
#define TS_LDCH_ALARM_P12              0x00010000
#define TS_LDCH_ALARM_LOAD_DISC        0x00020000
#define TS_LDCH_ALARM_ALARM_19         0x00040000
#define TS_LDCH_ALARM_ALARM_20         0x00080000
#define TS_LDCH_ALARM_ALARM_21         0x00100000
#define TS_LDCH_ALARM_ALARM_22         0x00200000
#define TS_LDCH_ALARM_ALARM_23         0x00400000
#define TS_LDCH_ALARM_ALARM_24         0x00800000

/* TS-MPPT alarm bits
 * From page 12 of Tristar MPPT MODBUS document V10.2 5 December 2011
 */

#define TS_MPPT_ALARM_RTS_OPEN                         0x00000001
#define TS_MPPT_ALARM_RTS_SHORTED                      0x00000002
#define TS_MPPT_ALARM_RTS_DISCONNECTED                 0x00000004
#define TS_MPPT_ALARM_HEATSINK_SENSOR_OPEN             0x00000008
#define TS_MPPT_ALARM_HEATSINK_SENSOR_SHORTED          0x00000010
#define TS_MPPT_ALARM_HIGH_TEMP_CURRENT_LIMIT          0x00000020
#define TS_MPPT_ALARM_CURRENT_LIMIT                    0x00000040
#define TS_MPPT_ALARM_CURRENT_OFFSET                   0x00000080
#define TS_MPPT_ALARM_BATTERY_SENSE_OUT_OF_RANGE       0x00000100
#define TS_MPPT_ALARM_BATTER_SENSE_DISCONNECTED        0x00000200
#define TS_MPPT_ALARM_UNCALIBRATED                     0x00000400
#define TS_MPPT_ALARM_RTS_MISWIRE                      0x00000800
#define TS_MPPT_ALARM_HVD                              0x00001000
#define TS_MPPT_ALARM_ALARM13                          0x00002000
#define TS_MPPT_ALARM_SYSTEM_MISWIRE                   0x00004000
#define TS_MPPT_ALARM_MOSFET_OPEN                      0x00008000
#define TS_MPPT_ALARM_P12_VOLTAGE_OFF                  0x00010000
#define TS_MPPT_ALARM_HIGH_INPUT_VOLTAGE_CURRENT_LIMIT 0x00020000
#define TS_MPPT_ALARM_ADC_INPUT_MAX                    0x00040000
#define TS_MPPT_ALARM_CONTROLLER_RESET                 0x00080000
#define TS_MPPT_ALARM_ALARM21                          0x00100000
#define TS_MPPT_ALARM_ALARM22                          0x00200000
#define TS_MPPT_ALARM_ALARM23                          0x00400000
#define TS_MPPT_ALARM_ALARM24                          0x00800000

/* TS-45/60 fault bits
 * From page 24 of Tristar MODBUS document V 02 19 October 2005
 * The same alarm bits are used for both charge and load modes.
 */

#define TS_LDCH_FAULT_EXTERNAL_SHORT   0x0001
#define TS_LDCH_FAULT_OVERCURRENT      0x0002
#define TS_LDCH_FAULT_FET_SHORT        0x0004
#define TS_LDCH_FAULT_SOFTWARE         0x0008
#define TS_LDCH_FAULT_HVD              0x0010
#define TS_LDCH_FAULT_TRISTAR_HOT      0x0020
#define TS_LDCH_FAULT_DIP_SW_CHANGED   0x0040
#define TS_LDCH_FAULT_SETTING_EDIT     0x0080
#define TS_LDCH_FAULT_RESET            0x0100
#define TS_LDCH_FAULT_MISWIRE          0x0200
#define TS_LDCH_FAULT_RTS_SHORTED      0x0400
#define TS_LDCH_FAULT_RTS_DISCONNECTED 0x0800
#define TS_LDCH_FAULT_FAULT_12         0x1000
#define TS_LDCH_FAULT_FAULT_13         0x2000
#define TS_LDCH_FAULT_FAULT_14         0x4000
#define TS_LDCH_FAULT_FAULT_15         0x8000

/* TS-MPPT fault bits
 * From page 11 of Tristar MPPT MODBUS document V10.2 5 December 2011
 */

#define TS_MPPT_FAULT_OVERCURRENT           0x0001
#define TS_MPPT_FAULT_FETS_SHORTED          0x0002
#define TS_MPPT_FAULT_SOFTWARE              0x0004
#define TS_MPPT_FAULT_BATTERY_HVD           0x0008
#define TS_MPPT_FAULT_ARRAY_HVD             0x0010
#define TS_MPPT_FAULT_DIP_SW_CHANGED        0x0020
#define TS_MPPT_FAULT_SETTING_EDIT          0x0040
#define TS_MPPT_FAULT_RTS_SHORTED           0x0080
#define TS_MPPT_FAULT_RTS_DISCONNECTED      0x0100
#define TS_MPPT_FAULT_EEPROM_RETRY_LIMIT    0x0200
#define TS_MPPT_FAULT_RESERVED              0x0400
#define TS_MPPT_FAULT_SLAVE_CONTROL_TIMEOUT 0x0800
#define TS_MPPT_FAULT_FAULT13               0x1000
#define TS_MPPT_FAULT_FAULT14               0x2000
#define TS_MPPT_FAULT_FAULT15               0x4000
#define TS_MPPT_FAULT_FAULT16               0x8000

/* connection options */

typedef struct {
    LOGIO *lp;    /* logging facility */
    UINT32 bto;   /* MODBUS byte timeout, msec */
    UINT32 rto;   /* MODBUS byte timeout, msec */
    UINT32 retry; /* modbus_connect() retry interval, msec */
    BOOL   debug; /* turns on libmodbus debugging logging */
} TS_OPT;

#define TS_DEFAULT_OPT_LP    NULL
#define TS_DEFAULT_OPT_BTO   0
#define TS_DEFAULT_OPT_RTO   0
#define TS_DEFAULT_OPT_RETRY 0
#define TS_DEFAULT_OPT_DEBUG FALSE

#ifdef TS_INCLUDE_STATIC_TS_DEFAULT_OPT

static TS_OPT TS_DEFAULT_OPT = {
    TS_DEFAULT_OPT_LP,
    TS_DEFAULT_OPT_BTO,
    TS_DEFAULT_OPT_RTO,
    TS_DEFAULT_OPT_RETRY,
    TS_DEFAULT_OPT_DEBUG
};

#endif /* TS_INCLUDE_STATIC_TS_DEFAULT_OPT */

/* handle */

typedef struct {
    /* these are provided by the app through tristarCreateHandle() */
    char server[MAXPATHLEN+1]; /* server name */
    int port;                  /* server port */
    int slave;                 /* TS_SLAVE_x */
    TS_OPT opt;                /* connection options */
    char peerID[MAXPATHLEN+1]; /* server:port string */
    /* these get filled in by the library inside tristarConnect() */
    modbus_t *ctx;             /* MODBUS conext */
    REAL64 V_PU;               /* V_PU scale factor, if applicable */
    REAL64 I_PU;               /* I_PU scale factor, if applicable */
    TS_IDENT ident;            /* model, device, and serial number */
}  TS;

/* used by strings.c for converting various codes to text strings and vice versa */

typedef struct {
    char *text;
    int code;
} TS_TEXT_MAP;

/* function prototypes */

/* cmd.c */
#define TS_COMMAND_UNKNOWN_DEVICE -4 /* not a recognized device */
#define TS_COMMAND_UNSUPPORTED    -3 /* command not supported by this device */
#define TS_COMMAND_FAIL           -2 /* modbus_write_bit() failed */
#define TS_COMMAND_EINVAL         -1 /* NULL handle or unexpected contents */
#define TS_COMMAND_OK              0
int tristarSendCommand(TS *handle, int cmd);
int tristarCommandCode(char *string);
BOOL tristarSupportedCommand(int device, int cmd);

/* connect.c */
#define TS_CONNECT_UNKOWN_DEVICE    -7 /* if we can't figure out the device type */
#define TS_CONNECT_HANDSHAKE_FAIL   -6 /* can't read MPPT scale values */
#define TS_CONNECT_MODBUS_NEW_ERROR -5 /* modbus_new_tcp() error */
#define TS_CONNECT_FAIL             -4 /* modbus_connect() failed and retry not set */
#define TS_CONNECT_MISWIRE          -3 /* MPPT device found on the LOAD line */
#define TS_CONNECT_ILLEGAL_SLAVE    -2 /* slave other than TS_SLAVE_CHRG or TS_SLAVE_LOAD */
#define TS_CONNECT_EINVAL           -1 /* NULL handle or unexpected contents */
#define TS_CONNECT_OK                0 /* MODBUS connect and handshake OK */
int tristarConnect(TS *handle);
void tristarDisconnect(TS *handle);

/* handle.c */
TS *tristarDestroyHandle(TS *handle);
TS *tristarCreateHandle(char *string, int slave, TS_OPT *opt);

/* log.c */
void tristarLog(TS *handle, char *format, ...);

/* print.c */

/* read.c */
#define TS_READ_FAIL   -2 /* modbus_read_registers() failed */
#define TS_READ_EINVAL -1 /* NULL arguments or unexpected contents */
#define TS_READ_OK      0 /* successful read and conversion */
int tristarReadItem(TS *handle, TS_DESC *desc, TS_ITEM *item);

/* soh.c */
int tristarReadSOH(TS *handle, TS_SOH *dest);
void tristarPrintSOH(FILE *fp, TS_SOH *soh);

/* string.c */
char *tristarSlaveString(int code);
char *tristarDeviceString(int code);
char *tristarControlStateStringTSCH(int code);
char *tristarControlStateStringTSLD(int code);
char *tristarControlStateStringMPPT(int code);
char *tristarAlarmStringTSCH(int code);
char *tristarAlarmStringTSLD(int code);
char *tristarAlarmStringMPPT(int code);
char *tristarFaultStringTSCH(int code);
char *tristarFaultStringTSLD(int code);
char *tristarFaultStringMPPT(int code);
char *tristarConnectErrorString(int code);
char *tristarCommandErrorString(int code);
char *tristarDipSwitchString(UINT8 bitfield, char *dest);
char *tristarVersionNumberString(UINT16 version, char *dest);

/* version.c */

VERSION *tristarVersion(void);
char *tristarVersionString(void);

#ifdef __cplusplus
}
#endif

#endif /* tristar_h_included */

/*-----------------------------------------------------------------------+
 |                                                                       |
 | Copyright (C) 2011 Regents of the University of California            |
 |                                                                       |
 | This software is provided 'as-is', without any express or implied     |
 | warranty.  In no event will the authors be held liable for any        |
 | damages arising from the use of this software.                        |
 |                                                                       |
 | Permission is granted to anyone to use this software for any purpose, |
 | including commercial applications, and to alter it and redistribute   |
 | it freely, subject to the following restrictions:                     |
 |                                                                       |
 | 1. The origin of this software must not be misrepresented; you must   |
 |    not claim that you wrote the original software. If you use this    |
 |    software in a product, an acknowledgment in the product            |
 |    documentation of the contribution by Project IDA, UCSD would be    |
 |    appreciated but is not required.                                   |
 | 2. Altered source versions must be plainly marked as such, and must   |
 |    not be misrepresented as being the original software.              |
 | 3. This notice may not be removed or altered from any source          |
 |    distribution.                                                      |
 |                                                                       |
 +-----------------------------------------------------------------------*/

/* Revision History
 *
 * $Log: tristar.h,v $
 * Revision 1.4  2016/06/06 21:44:39  dechavez
 * complete redesign, TS-MPPT support added
 *
 * Revision 1.3  2012/05/02 18:19:33  dechavez
 * added valid field to TRISTAR_REG and serialno
 *
 * Revision 1.2  2012/04/25 21:01:37  dechavez
 * added function prototypes
 *
 * Revision 1.1  2011/10/14 17:50:53  dechavez
 * initial version
 *
 */
