/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Serial Packet Protocol
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      SPP_Events.h
*
* Description:    SPP Events Task
*
* Revision History:
*
*   10/04/10  QHS  Initial release
*   10/18/10  QHS  Move GetSystemStatus() prototype from the header file.
*
*----------------------------------------------------------------------------*/


/* If PowerManager.h is not included */
#ifndef SYSTEM_EVENT

/* Useful definitions */
#define SYSTEM_EVENT             1
#define STATUS_SYSTEM_COMM_LOST -4

/* Event Structure */
typedef struct
{
   UINT8  Type;
   INT8   Parm1;
   UINT8  Parm2;
   INT32  PowerConsumed;
} EVENT;

/* Event Handler */
typedef void (*EVENT_HANDLER)( EVENT *Event );

#endif  /* SYSTEM_EVENT */


/* Prototypes */
INT8 InitEvents( EVENT_HANDLER Handler );
void CloseEvents( void );

