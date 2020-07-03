/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Serial Packet Protocol
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      SPP_Support.h
*
* Description:    Routines to support the Serial Packet Client
*
* Revision History:
*
*   10/04/10  QHS  Initial release
*
*----------------------------------------------------------------------------*/


/* Pointer to the Events Task routine */
typedef void (*EVENTS_TASK)( void );


/* Client Support Routines */
INT8   InitSerial( void );
void   CloseSerial( void );
INT16  SerialSend( UINT8 *Data, UINT8 DataSize );
INT16  SerialReceive( UINT8 *Data, UINT8 DataSize );
void   SerialLock( void );
void   SerialUnlock( void );
INT8   SystemReset( void );
INT8   SetPowerSupplyPresent( UINT8 PowerSupply, INT8 Status );
UINT32 CurrentTime( void );
UINT32 ElapsedTime( UINT32 StartTime );

/* Events Support Routines */
//#if SPP_INCLUDE_EVENTS == 1
UINT32 Spawn( EVENTS_TASK EventsTask );
void   Join( UINT32 TaskID );
void   Delay( UINT32 Milliseconds );
//#endif

/* Console Support Routines */
#if SPP_INCLUDE_CONSOLE == 1
INT8   InitConsole( void );
void   CloseConsole( void );
UINT8  Send( UINT8 *Data, UINT8 DataSize );
UINT8  Receive( UINT8 *Data, UINT8 DataSize );
UINT8  ReceiveAvailable( void );
#endif

