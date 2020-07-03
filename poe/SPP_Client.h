/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Serial Packet Protocol
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      SPP_Client.h
*
* Description:    Serial Packect Client
*
* Revision History:
*
*   10/04/10  QHS  Initial release
*
*----------------------------------------------------------------------------*/


#if SPP_LITTLE_ENDIAN == 1

   /* Union for byte swapping */
   typedef union
   {
      UINT16 Word;
      UINT8  Byte[2];
   } DATA16;

   /* Union for word swapping */
   typedef union
   {
      UINT32 Long;
      UINT16 Word[2];
   } DATA32;

   /* Prototype of swap routines */
   UINT16 Swap16( UINT16 Word );
   UINT32 Swap32( UINT32 Long );

#else /* Big endian */

   /* Swapping is not needed */
   #define Swap16(Word) Word
   #define Swap32(Long) Long

#endif

/* Error statistics */
extern UINT32 ReadFailed;
extern UINT32 BadChecksum;
extern UINT32 NoResponse;
extern UINT32 RetryFail;
extern UINT32 RetrySuccess;

/* Prototypes */
INT8 CallRoutine( UINT8 Routine, UINT8 *pParm8, UINT32 *pParm32 );
INT8 SendPacket( PKT_PARMS *pPacket );
INT8 ReceivePacket( PKT_PARMS *pPacket );

