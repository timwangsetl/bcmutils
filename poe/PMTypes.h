/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Power Manager
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      PMTypes.h
*
* Description:    Power Manager Types for Linux
*
* Revision History:
*
*   12/06/10  QHS  Ver 1.0 - Initial Release
*
*----------------------------------------------------------------------------*/
#ifndef __POWER_TYPE_H__
#define __POWER_TYPE_H__

/* Data types used by the Power Manager */
typedef signed char INT8;
typedef unsigned char  UINT8;
typedef signed short   INT16;
typedef unsigned short UINT16;
typedef signed int     INT32;
typedef unsigned int   UINT32;
typedef int            BOOL;


/* Common definitions */
#define TRUE   1
#define FALSE  0
#define NULL  ((void *)0)

/* Windows has no FAR or ROM data */
#undef FAR
#define FAR
#define ROM

/* Number of Ethernet ports and power supplies */
#define MAX_PORT_COUNT      48
#define POWER_SUPPLY_COUNT  3

#endif