/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Serial Packet Protocol
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      SPP_Util.c
*
* Description:    Miscellaneous Support Routines for Linux
*
* Revision History:
*
*   12/06/10  QHS  Initial release
*
*----------------------------------------------------------------------------*/

#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include "PMTypes.h"
#include "PMErrors.h"
#include "SPP.h"
#include "SPP_Support.h"
#include "SPP_Events.h"
#include "SerialPacket.h"
#include "SPP_Client.h"


/*****************************************************************************/
/* SystemReset                                                               */
/*****************************************************************************/

INT8 SystemReset( void )
{
   UINT8 Return;
   INT8  Result;
   
   /* Call the ResetSystem() routine in the Power Manager */
   Result = CallRoutine( RTN_RESETSYSTEM, &Return, 0 );
   if ( Result != SUCCESS ) return Result;
 
   return (INT8)Return;
}


/*****************************************************************************/
/* SetPowerSupplyPresent                                                     */
/*****************************************************************************/

INT8 SetPowerSupplyPresent( UINT8 PowerSupply, INT8 Status )
{
   /* Do nothing and return SUCCESS */
   return SUCCESS;
}


/*****************************************************************************/
/* CurrentTime                                                               */
/*****************************************************************************/

UINT32 CurrentTime( void )
{
   struct timespec Time;
   UINT32 Milliseconds;
   int    Result;

   /* Get the time from the monotonic clock */
   Result = clock_gettime( CLOCK_MONOTONIC, &Time );
   if ( Result == -1 ) return 0;

   /* Convert the time to milliseconds */
   Milliseconds = Time.tv_sec*1000 + Time.tv_nsec/1000000;
   if ( Time.tv_nsec%1000000 >= 500000 ) Milliseconds++;

   return Milliseconds;
}


/*****************************************************************************/
/* ElapsedTime                                                               */
/*****************************************************************************/

UINT32 ElapsedTime( UINT32 StartTime )
{
   UINT32 Time = CurrentTime();

   if ( Time >= StartTime )
      return Time - StartTime;
   else
      return (0xFFFFFFFF - StartTime) + Time + 1;
}


//#if SPP_INCLUDE_EVENTS == 1


/*****************************************************************************/
/* TaskWrapper                                                               */
/*****************************************************************************/

void *TaskWrapper( void *EventsTask )
{
   /* Call the EventsTask() routine */
   ((EVENTS_TASK)EventsTask)();

   /* Terminate the thread */   
   pthread_exit( NULL );
}


/*****************************************************************************/
/* Spawn                                                                     */
/*****************************************************************************/

UINT32 Spawn( EVENTS_TASK EventsTask )
{
   pthread_t TaskID;
   int Result;
   
   Result = pthread_create( &TaskID, NULL, TaskWrapper, EventsTask );
   if ( Result != 0 ) return 0;
   
   return (UINT32)TaskID;
}


/*****************************************************************************/
/* Join                                                                      */
/*****************************************************************************/

void Join( UINT32 TaskID )
{
   pthread_join( (pthread_t)TaskID, NULL );
}


/*****************************************************************************/
/* Delay                                                                     */
/*****************************************************************************/

void Delay( UINT32 Milliseconds )
{
   struct timespec Time;

   /* Convert milliseconds to time */
   Time.tv_sec  =  Milliseconds / 1000;
   Time.tv_nsec = (Milliseconds % 1000) * 1000000;

   /* Sleep for the specified time */
   nanosleep( &Time, NULL );
}


//#endif  /* SPP_INCLUDE_EVENTS */

