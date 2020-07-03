/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Serial Packet Protocol
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      SPP_Events.c
*
* Description:    SPP Events Task
*
* Revision History:
*
*   10/04/10  QHS  Initial release
*   10/18/10  QHS  Move GetSystemStatus() prototype from the header file.
*
*----------------------------------------------------------------------------*/

#include "PMTypes.h"
#include "PMErrors.h"
#include "PowerManager.h"
#pragma pack(1)
#include "SerialPacket.h"
#ifdef __GNUC__
   #pragma pack(0)
#else
   #pragma pack()
#endif
#include "SPP.h"
#include "SPP_Client.h"
#include "SPP_Events.h"
#include "SPP_Support.h"


#if SPP_INCLUDE_EVENTS == 1

/* Address of the user interface component's event handler */
EVENT_HANDLER EventHandler;

/* True if the events task is running */
BOOL EventsRunning;

#define PORT_NUM24 24
#define PORT_NUM23 22
#define PORT_NUM22 20
#define PORT_NUM21 18
#define PORT_NUM20 16
#define PORT_NUM19 14
#define PORT_NUM18 12
#define PORT_NUM17 10

#define PORT_NUM16  8
#define PORT_NUM15  6
#define PORT_NUM14  4
#define PORT_NUM13  2
#define PORT_NUM12  23
#define PORT_NUM11  21
#define PORT_NUM10  19
#define PORT_NUM9  	17

#define PORT_NUM8 15
#define PORT_NUM7 13
#define PORT_NUM6 11
#define PORT_NUM5 9
#define PORT_NUM4 7
#define PORT_NUM3 5
#define PORT_NUM2 3
#define PORT_NUM1 1

const UINT8 G_u8_PortMap[24]=
{
	PORT_NUM1,
	PORT_NUM2,
	PORT_NUM3,
	PORT_NUM4,
	PORT_NUM5,
	PORT_NUM6,
	PORT_NUM7,
	PORT_NUM8,
	
	PORT_NUM9, 
	PORT_NUM10, 
	PORT_NUM11, 
	PORT_NUM12, 
	PORT_NUM13, 
	PORT_NUM14, 
	PORT_NUM15, 
	PORT_NUM16, 
       
	PORT_NUM17, 
	PORT_NUM18, 
	PORT_NUM19, 
	PORT_NUM20, 
	PORT_NUM21, 
	PORT_NUM22, 
	PORT_NUM23, 
	PORT_NUM24, 
}
;



/* Handle to the thread for the events task */
UINT32 EventsThread;

/* Forward reference */
void EventsTask( void );
INT8 get_poe_system_status(INT32 skfd, INT8 *sysStatus);


/*****************************************************************************/
/* InitEvents                                                                */
/*****************************************************************************/

INT8 InitEvents( EVENT_HANDLER Handler )
{
   /* Save the address of the user interface component's event handler */
   EventHandler = Handler;

   /* If the user interface component provided an event handler */
   if ( Handler != NULL )
   {
      /* Let the events task run */
      EventsRunning = TRUE;

      /* Create the events task */
      EventsThread = Spawn( EventsTask );
      if ( EventsThread == 0 )
      {
         EventsRunning = FALSE;
         return ERROR_RESOURCE_CREATE;
      } 
   }
   else EventsRunning = FALSE;

   return SUCCESS;
}


/*****************************************************************************/
/* CloseEvents                                                               */
/*****************************************************************************/

void CloseEvents( void )
{
   /* If the events task is running */
   if ( EventsRunning )
   {
      /* Stop the events task */
      EventsRunning = FALSE;

      /* Wait for the events task to end */
      Join( EventsThread );
   }
}


/*****************************************************************************/
/* EventsTask                                                                */
/*****************************************************************************/

void EventsTask( void )
{
   PKT_PARMS  RequestPacket;
   PKT_EVENTS EventsPacket;
   INT8   SystemStatus;
   EVENT  SystemEvent;
   EVENT *pEvent;
   EVENT *pLimit;
   INT8   Result;
   static UINT8 getPortConsumed=0;
   EVENT  PowerConsumeEvent;

   PowerConsumeEvent.Type = POWER_CONSUME_EVENT;
   PowerConsumeEvent.Parm2 = 0;

   /* Get the initial system status */
   get_poe_system_status(0, &SystemStatus);
   
   /* Build the system event */
   SystemEvent.Type  = SYSTEM_EVENT;
   SystemEvent.Parm1 = SystemStatus;
   SystemEvent.Parm2 = 0;

   /* Send the initial system event */
   EventHandler( &SystemEvent );

   /* Build the request packet */
   RequestPacket.Start      = SOP;
   RequestPacket.Routine    = RTN_GETEVENTS;
   RequestPacket.DataLength = sizeof(DATA_PARMS);

   /* If the events task has not been stopped */
   while ( EventsRunning )
   {
      /* Get events every 200ms */
      Delay( 200 );

      /* If communication is lost */
      if ( SystemStatus == STATUS_SYSTEM_COMM_LOST )
      {
         /* Try to get the system status */
          get_poe_system_status(0, &SystemStatus);

         /* If communication is still lost */
         if ( SystemStatus == STATUS_SYSTEM_COMM_LOST )
            continue;

         /* Send an event reporting the current system status */
         SystemEvent.Parm1 = SystemStatus;
         EventHandler( &SystemEvent );
      }

      /* Send the request packet */
      Result = SendPacket( &RequestPacket );
      if ( Result != SUCCESS ) continue;

      /* Receive the events packet */
      Result = ReceivePacket( (PKT_PARMS*)&EventsPacket );
      if ( Result != SUCCESS )
      {
         /* Keep error statistics */
         switch ( Result )
         {
            case ERROR_RESOURCE_READ:    ReadFailed++;  break;
            case ERROR_RESOURCE_INVALID: BadChecksum++; break;
            case ERROR_RESOURCE_NOT_FND: NoResponse++;  break;
         }
         
         /* Resend the request packet */
         Result = SendPacket( &RequestPacket );
         if ( Result != SUCCESS ) continue;

         /* Try to receive the events packet again */
         Result = ReceivePacket( (PKT_PARMS*)&EventsPacket );
         if ( Result != SUCCESS )
         {
            /* Retry was not successful */
            RetryFail++;
            
            /* Keep error statistics */
            switch ( Result )
            {
               case ERROR_RESOURCE_READ:    ReadFailed++;  break;
               case ERROR_RESOURCE_INVALID: BadChecksum++; break;
               case ERROR_RESOURCE_NOT_FND: NoResponse++;  break;
            }
            
            /* Communication is lost */
            SystemStatus = STATUS_SYSTEM_COMM_LOST;

            /* Send an event reporting that communication is lost */
            SystemEvent.Parm1 = SystemStatus;
            EventHandler( &SystemEvent );
            continue;
         }
         else RetrySuccess++;
      }

      /* Verify that the correct response packet was received */
      if ( EventsPacket.Routine != RTN_GETEVENTS )
         continue;

      /* Point just beyond the last event */
      pLimit = EventsPacket.Events + (EventsPacket.DataLength/sizeof(EVENT));

      /* Call the event handler for each event */
      for ( pEvent=EventsPacket.Events; pEvent<pLimit; pEvent++ )
         EventHandler( pEvent );

	 if(getPortConsumed>=24){
		INT32 totalPowerConsumed;
		if(get_poe_total_power_consumed(0, &totalPowerConsumed)==SUCCESS){

			PowerConsumeEvent.Parm1=getPortConsumed;
			PowerConsumeEvent.PowerConsumed=(totalPowerConsumed==-1?0:totalPowerConsumed);
			EventHandler(&PowerConsumeEvent);
			getPortConsumed=0;
		}
	  }else{
		UINT8 Port=G_u8_PortMap[getPortConsumed];
		INT32 portPowerConsumed;
		if(get_poe_port_power_consumed(0, Port, &portPowerConsumed)==SUCCESS){
			PowerConsumeEvent.Parm1=getPortConsumed;
			PowerConsumeEvent.PowerConsumed=(portPowerConsumed==-1?0:portPowerConsumed);
			EventHandler(&PowerConsumeEvent);
			getPortConsumed++;
		}
	  }
   }
}

#endif  /* SPP_INCLUDE_EVENTS */

