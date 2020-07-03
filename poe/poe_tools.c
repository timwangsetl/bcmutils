/**
  ******************************************************************************
  * @file    poe_tools.c
  * @author  UTEPO Application Team
  * @version V0.0.1
  * @brief   This file provides all the poe functions operate on Si3482.
  * @Copyright:      2014 utepo, Inc. (http://www.utpvideo.com/)
  * Revision History:
  * Date        author       content
  * 04/06/2014  timwang      Initial release
  *
  ******************************************************************************
  */ 
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>

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
#define POESTATUSFILE		"/tmp/poe_status"
#define ALARMSTATUSFILE		"/tmp/alarm_status"
#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)




/* Packets */
PKT_SYSINFO  SysInfoPacket;
PKT_PORTINFO PortInfoPacket;

/* Error statistics */
UINT32 ReadFailed   = 0;
UINT32 BadChecksum  = 0;
UINT32 NoResponse   = 0;
UINT32 RetryFail    = 0;
UINT32 RetrySuccess = 0;


const UINT8 G_u8_PortMap[MAX_PORT_NUM]=
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


#ifdef EN_DEBUG
void DEBUG(char *format,...)
{
	va_list args;
	va_start(args,format);
	//vfprintf(stdout,format,args);
	vprintf(format,args);
	va_end(args);
}
#else
void DEBUG(char *format,...){}
#endif

/*****************************************************************************/
/* InitPowerManager                                                          */
/*****************************************************************************/
INT8 init_poe_power_manager( EVENT_HANDLER Handler )
{
   INT8 Result;

   /* Initialize the serial port */
   Result = InitSerial();

   if ( Result == SUCCESS )
   {
      /* Initialzize the events task */
      #if SPP_INCLUDE_EVENTS == 1
         //InitEvents( Handler );
      #endif
   }

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* ClosePowerManager                                                         */
/*****************************************************************************/
INT8 close_poe_power_manager( void )
{ 
   /* Close the events task */
   #if SPP_INCLUDE_EVENTS == 1
      CloseEvents();
   #endif
   
   /* Close the serial port */
   CloseSerial();

   DEBUG("%s\n", __FUNCTION__);
   return 0;
}


/*****************************************************************************/
/* GetSystemStatus                                                           */
/*****************************************************************************/

INT8 get_poe_system_status(INT32 skfd, INT8 *sysStatus)
{
   UINT8 SystemStatus;
   INT8  Result;
   
   /* Protect against loopback */
   SystemStatus = STATUS_SYSTEM_COMM_LOST;
   
   /* Call GetSystemStatus() in the Power Manager */
   Result = CallRoutine( RTN_GETSYSTEMSTATUS, &SystemStatus, 0 );
   if ( Result != SUCCESS ) return STATUS_SYSTEM_COMM_LOST;

   *sysStatus = (INT8)SystemStatus;
 
   DEBUG("%s sysStatus=%d\n", __FUNCTION__, SystemStatus);
   return Result;
}


/*****************************************************************************/
/* GetSystemInfo                                                             */
/*****************************************************************************/

INT8 get_poe_system_info(INT32 skfd, SYSINFO *SystemInfo )
{
   PKT_PARMS ParmsPacket;
   INT8      Result;

   /* In case of error, init the version strings to question marks */
   SysInfoPacket.Data.PowerManagerVersion[0]    = '?';
   SysInfoPacket.Data.PowerManagerVersion[1]    =  0;
   SysInfoPacket.Data.PlatformSupportVersion[0] = '?';
   SysInfoPacket.Data.PlatformSupportVersion[1] =  0;

   /* Point SystemInfo to the version strings */
   SystemInfo->PowerManagerVersion    = SysInfoPacket.Data.PowerManagerVersion;
   SystemInfo->PlatformSupportVersion = SysInfoPacket.Data.PlatformSupportVersion;

   /* Build the request packet */
   ParmsPacket.Start      = SOP;
   ParmsPacket.Routine    = RTN_GETSYSTEMINFO;
   ParmsPacket.DataLength = sizeof(DATA_PARMS);

   /* Send the request packet */
   Result = SendPacket( &ParmsPacket );
   if ( Result != SUCCESS ) return;

   /* Receive the reponse packet */
   Result = ReceivePacket( (PKT_PARMS*)&SysInfoPacket );
   if ( Result != SUCCESS )
   {
      /* Keep error statistics */
      switch ( Result )
      {
         case ERROR_RESOURCE_READ:    ReadFailed++;  break;
         case ERROR_RESOURCE_INVALID: BadChecksum++; break;
         case ERROR_RESOURCE_NOT_FND: NoResponse++;  break;
      }
      
      /* Build a fresh request packet */
      ParmsPacket.Start      = SOP;
      ParmsPacket.Routine    = RTN_GETSYSTEMINFO;
      ParmsPacket.DataLength = sizeof(DATA_PARMS);
      
      /* Resend the request packet */
      Result = SendPacket( &ParmsPacket );
      if ( Result != SUCCESS ) return;

      /* Try to receive the response packet again */
      Result = ReceivePacket( (PKT_PARMS*)&SysInfoPacket );
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
      }
      else RetrySuccess++;
   }

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* GetTotalPowerConsumed                                                     */
/*****************************************************************************/

INT8 get_poe_total_power_consumed(INT32 skfd, INT32 *totalPC)
{
   UINT32 TotalPowerConsumed;
   INT8   Result;
   
   Result = CallRoutine( RTN_GETTOTALPOWERCONSUMED, 0, &TotalPowerConsumed );
   if ( Result != SUCCESS ) return Result;
 
   *totalPC = (INT32)TotalPowerConsumed;

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* GetTotalPowerGranted                                                      */
/*****************************************************************************/

INT8 get_poe_total_power_granted(INT32 skfd, INT32 *totalPG)
{
   UINT32 TotalPowerGranted;
   INT8   Result;
   
   Result = CallRoutine( RTN_GETTOTALPOWERGRANTED, 0, &TotalPowerGranted );
   if ( Result != SUCCESS ) return Result;
 
   *totalPG =(INT32)TotalPowerGranted;

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* GetTotalPowerProvided                                                     */
/*****************************************************************************/

INT8 get_poe_total_power_provided(INT32 skfd, INT32 *totalPP)
{
   UINT32 TotalPowerProvided;
   INT8   Result;
   
   Result = CallRoutine( RTN_GETTOTALPOWERPROVIDED, 0, &TotalPowerProvided );
   if ( Result != SUCCESS ) return Result;
 
   *totalPP = (INT32)TotalPowerProvided;

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* GetPortCount                                                              */
/*****************************************************************************/

INT8 get_poe_port_count(INT32 skfd, UINT8 *portCnt )
{
   UINT8 PortCount;
   INT8  Result;
   
   Result = CallRoutine( RTN_GETPORTCOUNT, &PortCount, 0 );
   if ( Result != SUCCESS ) return 0;
 
   *portCnt = PortCount;

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* GetPortStatus                                                             */
/*****************************************************************************/

INT8 get_poe_port_status(INT32 skfd, UINT8 Port, INT8 *status)
{
   INT8 Result;
   
   Result = CallRoutine( RTN_GETPORTSTATUS, &Port, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *status =(INT8)Port;  

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* GetPortInfo                                                               */
/*****************************************************************************/

INT8 get_poe_port_info(INT32 skfd, UINT8 Port, PORTINFO *PortInfo )
{
   PKT_PARMS ParmsPacket;
   INT8      Result;

   /* Build the request packet */
   ParmsPacket.Start      = SOP;
   ParmsPacket.Routine    = RTN_GETPORTINFO;
   ParmsPacket.DataLength = sizeof(DATA_PARMS);
   ParmsPacket.Data.Parm8 = Port;

   /* Send the request packet */
   Result = SendPacket( &ParmsPacket );
   if ( Result != SUCCESS ) return Result;

   /* Receive the reponse packet */
   Result = ReceivePacket( (PKT_PARMS*)&PortInfoPacket );
   if ( Result != SUCCESS )
   {
      /* Keep error statistics */
      switch ( Result )
      {
         case ERROR_RESOURCE_READ:    ReadFailed++;  break;
         case ERROR_RESOURCE_INVALID: BadChecksum++; break;
         case ERROR_RESOURCE_NOT_FND: NoResponse++;  break;
      }

      /* Build a fresh request packet */
      ParmsPacket.Start      = SOP;
      ParmsPacket.Routine    = RTN_GETPORTINFO;
      ParmsPacket.DataLength = sizeof(DATA_PARMS);
      ParmsPacket.Data.Parm8 = Port;
      
      /* Resend the request packet */
      Result = SendPacket( &ParmsPacket );
      if ( Result != SUCCESS ) return Result;

      /* Try to receive the response packet again */
      Result = ReceivePacket( (PKT_PARMS*)&PortInfoPacket );
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
         
         return Result;
      }
      else RetrySuccess++;
   }

   /* Verify that the correct response packet was received */
   if ( PortInfoPacket.Routine != RTN_GETPORTINFO )
      return ERROR_RESOURCE_INVALID;

   /* Fill in the PortInfo structure */
   PortInfo->Detection          = PortInfoPacket.Data.Detection;
   PortInfo->Classification     = PortInfoPacket.Data.Classification;
   PortInfo->Current            = Swap16(PortInfoPacket.Data.Current);
   PortInfo->PowerSupplyVoltage = Swap16(PortInfoPacket.Data.PowerSupplyVoltage);
   PortInfo->SiliconVersion     = PortInfoPacket.Data.SiliconVersion;
   PortInfo->FirmwareVersion    = PortInfoPacket.Data.FirmwareVersion;
      
   DEBUG("%s\n", __FUNCTION__);
   return PortInfoPacket.Data.Result;   
}


/*****************************************************************************/
/* GetPortPriorityStatus                                                     */
/*****************************************************************************/

INT8 get_poe_port_priority_status(INT32 skfd, UINT8 Port, INT8 *portpri )
{
   INT8 Result;
   
   Result = CallRoutine( RTN_GETPORTPRIORITYSTATUS, &Port, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *portpri = (INT8)Port;  /* Port Priority Status */

   DEBUG("%s\n", __FUNCTION__);
   return Result; 
}


/*****************************************************************************/
/* GetPortPowerConsumed                                                      */
/*****************************************************************************/

INT8 get_poe_port_power_consumed(INT32 skfd, UINT8 Port, INT32 *portPC)
{
   UINT32 PortPowerConsumed;
   INT8   Result;
   
   Result = CallRoutine( RTN_GETPORTPOWERCONSUMED, &Port, &PortPowerConsumed );
   if ( Result != SUCCESS ) return Result;
 
   *portPC = (INT32)PortPowerConsumed;
 
   DEBUG("%s\n", __FUNCTION__);
   return Result; 
}


/*****************************************************************************/
/* GetPortPowerGranted                                                       */
/*****************************************************************************/

INT8 get_poe_port_power_granted(INT32 skfd, UINT8 Port, INT32 *portPG )
{
   UINT32 PortPowerGranted;
   INT8   Result;
   
   Result = CallRoutine( RTN_GETPORTPOWERGRANTED, &Port, &PortPowerGranted );
   if ( Result != SUCCESS ) return Result;
 
   *portPG = (INT32)PortPowerGranted;

   DEBUG("%s\n", __FUNCTION__);
   return Result; 
}


/*****************************************************************************/
/* GetPortPowerRequested                                                     */
/*****************************************************************************/

INT8 get_poe_port_power_requested(INT32 skfd, UINT8 Port, INT32 *portPR )
{
   UINT32 PortPowerRequested;
   INT8   Result;
   
   Result = CallRoutine( RTN_GETPORTPOWERREQUESTED, &Port, &PortPowerRequested );
   if ( Result != SUCCESS ) return Result;
 
   *portPR =  (INT32)PortPowerRequested;

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


/*****************************************************************************/
/* GetPortPowerAvailable                                                     */
/*****************************************************************************/

INT8 get_poe_port_power_available(INT32 skfd, UINT8 Port , INT32 *portPA)
{
   UINT32 PortPowerAvailable;
   INT8   Result;
   
   Result = CallRoutine( RTN_GETPORTPOWERAVAILABLE, &Port, &PortPowerAvailable );
   if ( Result != SUCCESS ) return Result;
 
   *portPA = (INT32)PortPowerAvailable;

   DEBUG("%s\n", __FUNCTION__);
   return Result;
}


INT8 syn_poe_system( void )
{
   UINT8 Return;
   INT8  Result;
   
    DEBUG("%s\n", __FUNCTION__);
   /* Call the ResetSystem() routine in the Power Manager */
   Result = CallRoutine(RTN_SYNCHRONIZE, &Return, 0 );
   if ( Result != SUCCESS ) return Result;
 
   return (INT8)Return;
}



/*****************************************************************************/
/* ResetSystem                                                               */
/*****************************************************************************/

INT8 reset_poe_system( void )
{
   DEBUG("%s\n", __FUNCTION__);
   return SystemReset();
}


/*****************************************************************************/
/* RestoreFactoryDefaults                                                    */
/*****************************************************************************/

void restore_poe_factory_defaults( void )
{
   CallRoutine( RTN_RESTOREFACTORYDEFAULTS, 0, 0 );
}


/*****************************************************************************/
/* SetPortControl                                                            */
/*****************************************************************************/

INT8 set_poe_port_control(INT32 skfd, UINT8 Port, UINT8 Control )
{
   UINT32 Parm32;
   INT8   Result;

   /* Control must be passed in a UINT32 */
   Parm32 = Control;
 
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETPORTCONTROL, &Port, &Parm32 );
   if ( Result != SUCCESS ) return Result;

   return Result;
}


/*****************************************************************************/
/* AdjustPortPower                                                           */
/*****************************************************************************/

INT8 adjust_poe_port_power(INT32 skfd, UINT8 Port, INT32 Power )
{
   INT8 Result;

   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_ADJUSTPORTPOWER, &Port, (UINT32*)&Power );
   if ( Result != SUCCESS ) return Result;
 
   return Result;
}


/*****************************************************************************/
/* SetPowerProvided                                                          */
/*****************************************************************************/

INT8 set_poe_power_provided(INT32 skfd, UINT8 PowerSupply, INT32 Power )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETPOWERPROVIDED, &PowerSupply, (UINT32*)&Power );
   if ( Result != SUCCESS ) return Result;
  
   return Result;
}


/*****************************************************************************/
/* GetPowerProvided                                                          */
/*****************************************************************************/

INT8 get_poe_power_provided(INT32 skfd, UINT8 PowerSupply, INT32 *powerPro )
{
   UINT32 PowerProvided;
   INT8   Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETPOWERPROVIDED, &PowerSupply, &PowerProvided );
   if ( Result != SUCCESS ) return Result;
 
   *powerPro = (INT32)PowerProvided;
 
   return Result;
}


/*****************************************************************************/
/* SetReservedPower                                                          */
/*****************************************************************************/

INT8 set_poe_reserved_power(INT32 skfd, INT8 Reserved )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETRESERVEDPOWER, (UINT8*)&Reserved, 0 );
   if ( Result != SUCCESS ) return Result;

   return Result;
}


/*****************************************************************************/
/* GetReservedPower                                                          */
/*****************************************************************************/

INT8 get_poe_reserved_power(INT32 skfd, INT8 *reservedP )
{
   UINT8 ReservedPower;
   INT8  Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETRESERVEDPOWER, &ReservedPower, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *reservedP = (INT8)ReservedPower;

   return Result;
}


/*****************************************************************************/
/* SetOverloadLimit                                                          */
/*****************************************************************************/

INT8 set_poe_overload_limit(INT32 skfd, INT8 Limit )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETOVERLOADLIMIT, (UINT8*)&Limit, 0 );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  
}


/*****************************************************************************/
/* GetOverloadLimit                                                          */
/*****************************************************************************/

INT8 get_poe_overload_limit(INT32 skfd, INT8 *oLlimit )
{
   UINT8 Limit;
   INT8  Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETOVERLOADLIMIT, &Limit, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *oLlimit = (INT8)Limit;

   return Result;  
}


/*****************************************************************************/
/* SetGrantingPolicy                                                         */
/*****************************************************************************/

INT8 set_poe_granting_policy(INT32 skfd, INT8 Policy )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETGRANTINGPOLICY, (UINT8*)&Policy, 0 );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  
}


/*****************************************************************************/
/* GetGrantingPolicy                                                         */
/*****************************************************************************/

INT8 get_poe_granting_policy(INT32 skfd, INT8 *pol )
{
   UINT8 Policy;
   INT8  Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETGRANTINGPOLICY, &Policy, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *pol = (INT8)Policy;
   return Result; 
}


/*****************************************************************************/
/* SetRetryPolicy                                                            */
/*****************************************************************************/

INT8 set_poe_retry_policy(INT32 skfd, INT8 Policy )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETRETRYPOLICY, (UINT8*)&Policy, 0 );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  /* Result */
}


/*****************************************************************************/
/* GetRetryPolicy                                                            */
/*****************************************************************************/

INT8 get_poe_retry_policy(INT32 skfd, INT8 *rePolicy )
{
   UINT8 Policy;
   INT8  Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETRETRYPOLICY, &Policy, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *rePolicy = (INT8)Policy;
   return Result;  /* Result */
}


/*****************************************************************************/
/* SetPowerLocation                                                          */
/*****************************************************************************/

INT8 set_poe_power_location(INT32 skfd, INT8 Location )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETPOWERLOCATION, (UINT8*)&Location, 0 );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  /* Result */
}


/*****************************************************************************/
/* GetPowerLocation                                                          */
/*****************************************************************************/

INT8 get_poe_power_location(INT32 skfd, INT8 *local )
{
   UINT8 Location;
   INT8  Result;
  
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETPOWERLOCATION, &Location, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *local = (INT8)Location;
   return Result;  /* Result */
}


/*****************************************************************************/
/* SetPortEnable                                                             */
/*****************************************************************************/

INT8 set_poe_port_enable(INT32 skfd, UINT8 Port, INT8 Enable )
{
   UINT32 Parm32;
   INT8   Result;

   /* Must pass Enable in a UINT32 */
   Parm32 = Enable;
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETPORTENABLE, &Port, &Parm32 );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  /* Result */
}


/*****************************************************************************/
/* GetPortEnable                                                             */
/*****************************************************************************/

INT8 get_poe_port_enable(INT32 skfd, UINT8 Port, INT8 *enable )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETPORTENABLE, &Port, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *enable = (INT8)Port;  /* Enable */
   return Result;  /* Result */
}


/*****************************************************************************/
/* SetPortCapability                                                         */
/*****************************************************************************/

INT8 set_poe_port_capability(INT32 skfd, UINT8 Port, INT8 Capability )
{
   UINT32 Parm32;
   INT8   Result;
   
   /* Must pass Capability in a UINT32 */
   Parm32 = Capability;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETPORTCAPABILITY, &Port, &Parm32 );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  /* Result */
}


/*****************************************************************************/
/* GetPortCapability                                                         */
/*****************************************************************************/

INT8 get_poe_port_capability(INT32 skfd, UINT8 Port, INT8 *portCap )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETPORTCAPABILITY, &Port, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *portCap = (INT8)Port;  /* Capability */

   return Result;  /* Result */
}


/*****************************************************************************/
/* SetPortPriority                                                           */
/*****************************************************************************/

INT8 set_poe_port_priority(INT32 skfd, UINT8 Port, INT8 Priority )
{
   UINT32 Parm32;
   INT8   Result;
   
   /* Must pass Priority in a UINT32 */
   Parm32 = Priority;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETPORTPRIORITY, &Port, &Parm32 );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  /* Result */
}


/*****************************************************************************/
/* GetPortPriority                                                           */
/*****************************************************************************/

INT8 get_poe_port_priority(INT32 skfd, UINT8 Port, INT8 *portPri )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETPORTPRIORITY, &Port, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *portPri = (INT8)Port;  /* Priority */
   return Result;  /* Result */
}


/*****************************************************************************/
/* SetPortPowerLimit                                                         */
/*****************************************************************************/

INT8 set_poe_port_power_limit(INT32 skfd, UINT8 Port, INT32 Limit )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_SETPORTPOWERLIMIT, &Port, (UINT32*)&Limit );
   if ( Result != SUCCESS ) return Result;
 
   return Result;  /* Result */
}


/*****************************************************************************/
/* GetPortPowerLimit                                                         */
/*****************************************************************************/

INT32 get_poe_port_power_limit(INT32 skfd, UINT8 Port, INT8 *portPL )
{
   UINT32 Limit;
   INT8   Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETPORTPOWERLIMIT, &Port, &Limit );
   if ( Result != SUCCESS ) return Result;
 
   *portPL = (INT32)Limit;
	return Result;  /* Result */
}


/*****************************************************************************/
/* SetPowerSupplyStatus                                                      */
/*****************************************************************************/

INT8 set_poe_power_supply_status(INT32 skfd, UINT8 PowerSupply, INT8 Status )
{
   DEBUG("%s\n", __FUNCTION__);
   return SetPowerSupplyPresent( PowerSupply, Status );
}


/*****************************************************************************/
/* GetPowerSupplyStatus                                                      */
/*****************************************************************************/

INT8 get_poe_power_supply_status(INT32 skfd, UINT8 PowerSupply, INT8 *powerS )
{
   INT8 Result;
   
   DEBUG("%s\n", __FUNCTION__);
   Result = CallRoutine( RTN_GETPOWERSUPPLYSTATUS, &PowerSupply, 0 );
   if ( Result != SUCCESS ) return Result;
 
   *powerS = (INT8)PowerSupply;  /* Status */
   return Result;  /* Result */
}


/*****************************************************************************/
/* CallRoutine                                                               */
/*****************************************************************************/

INT8 CallRoutine( UINT8 Routine, UINT8 *pParm8, UINT32 *pParm32 )
{
   PKT_PARMS ParmsPacket;
   INT8      Result;
   
   DEBUG("%s\n", __FUNCTION__);
   /* Build the request packet */
   ParmsPacket.Start       = SOP;
   ParmsPacket.Routine     = Routine;
   ParmsPacket.DataLength  = sizeof(DATA_PARMS);
   if ( pParm8  ) ParmsPacket.Data.Parm8  = *pParm8;
   if ( pParm32 ) ParmsPacket.Data.Parm32 = Swap32(*pParm32);

   /* Send the request packet */
   Result = SendPacket( &ParmsPacket );
   if ( Result != SUCCESS ) return Result;

   /* Receive the reponse packet */
   Result = ReceivePacket( &ParmsPacket );
   if ( Result != SUCCESS )
   {
   	return ERROR_RESOURCE_NOT_FND;
#if 0
      /* Keep error statistics */
      switch ( Result )
      {
         case ERROR_RESOURCE_READ:    ReadFailed++;  break;
         case ERROR_RESOURCE_INVALID: BadChecksum++; break;
         case ERROR_RESOURCE_NOT_FND: NoResponse++;  break;
      }

      /* Build a fresh request packet */
      ParmsPacket.Start       = SOP;
      ParmsPacket.Routine     = Routine;
      ParmsPacket.DataLength  = sizeof(DATA_PARMS);
      if ( pParm8  ) ParmsPacket.Data.Parm8  = *pParm8;
      if ( pParm32 ) ParmsPacket.Data.Parm32 = Swap32(*pParm32);
      
      /* Resend the request packet */
      Result = SendPacket( &ParmsPacket );
      if ( Result != SUCCESS ) return Result;

      /* Try to receive the response packet again */
      Result = ReceivePacket( &ParmsPacket );
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

         return Result;
      }
      else RetrySuccess++;
#endif 
   }

   /* Verify that the correct response packet was received */
   if ( ParmsPacket.Routine != Routine )
      return ERROR_RESOURCE_INVALID;

   /* Return received parameters */
   if ( pParm8  ) *pParm8  = ParmsPacket.Data.Parm8;
   if ( pParm32 ) *pParm32 = Swap32(ParmsPacket.Data.Parm32);
      
   return SUCCESS;   
}


/*****************************************************************************/
/* SendPacket                                                                */
/*****************************************************************************/

INT8 SendPacket( PKT_PARMS *pPacket )
{
   UINT8 *pByte;
   UINT8 *pChecksum;
   UINT8  Sum;
   INT16  Result;

	DEBUG("%s\n", __FUNCTION__);
   /* The checksum comes after the data field */   
   pChecksum = &pPacket->Data.Parm8 + pPacket->DataLength;

   /* Calculate the checksum */      
   for ( pByte=&pPacket->Routine,Sum=0; pByte<pChecksum; pByte++ )
      Sum += *pByte;      

   /* Store the checksum in the packet */
   *pChecksum = (~Sum) + 1;
   
   /* Only one task at a time can use the serial port */
   SerialLock();

   /* Send the packet */   
   Result = SerialSend( (UINT8*)pPacket, pPacket->DataLength+4 );
   if ( Result != pPacket->DataLength+4 )
   {
      SerialUnlock();
      return ERROR_RESOURCE_WRITE;
   }   

   return SUCCESS;   
}


/*****************************************************************************/
/* ReceivePacket                                                             */
/*****************************************************************************/

INT8 ReceivePacket( PKT_PARMS *pPacket )
{
   UINT32 StartTime;
   UINT8 *pByte;
   UINT8 *pLimit;
   UINT8  Sum;
   INT16  Result;
   
   DEBUG("%s\n", __FUNCTION__);
   /* Clear the start field */
   pPacket->Start = 0;

   /* Stop searching for SOP after 1 second */
   StartTime = CurrentTime();

   /* Receive until a Start-Of-Packet character is found */   
   while ( pPacket->Start != SOP )
   {
      Result = SerialReceive( &pPacket->Start, 1 );
      if ( Result < 0 ) { SerialUnlock(); return ERROR_RESOURCE_READ; }
      if ( ElapsedTime(StartTime) > 1000 ) { SerialUnlock(); return ERROR_RESOURCE_NOT_FND; }
   } 

   /* Receive the routine and data length fields */
   for ( pByte=&pPacket->Routine,pLimit=pByte+2; pByte<pLimit; pByte+=Result )
   {
      Result = SerialReceive( pByte, pLimit-pByte );
      if ( Result < 0 ) { SerialUnlock(); return ERROR_RESOURCE_READ; }
      if ( ElapsedTime(StartTime) > 1000 ) { SerialUnlock(); return ERROR_RESOURCE_NOT_FND; }
   }
   
   /* Receive the data and checksum fields */
   for ( pByte=&pPacket->Data.Parm8,pLimit=pByte+pPacket->DataLength+1; pByte<pLimit; pByte+=Result )
   {
      Result = SerialReceive( pByte, pLimit-pByte );
      if ( Result < 0 ) { SerialUnlock(); return ERROR_RESOURCE_READ; }
      if ( ElapsedTime(StartTime) > 1000 ) { SerialUnlock(); return ERROR_RESOURCE_NOT_FND; }
   }

   /* Another task can use the serial port now */
   SerialUnlock();
   
   /* The checksum comes after the data field */   
   pLimit = &pPacket->Data.Parm8 + pPacket->DataLength;

   /* Calculate the checksum */
   for ( pByte=&pPacket->Routine,Sum=0; pByte<=pLimit; pByte++ )
      Sum += *pByte;      

   /* Verify the checksum */
   if ( Sum != 0 )
      return ERROR_RESOURCE_INVALID;
   else
      return SUCCESS;   
}


#if SPP_LITTLE_ENDIAN == 1

/*****************************************************************************/
/* Swap16                                                                    */
/*****************************************************************************/

UINT16 Swap16( UINT16 Word )
{
   DATA16 In;
   DATA16 Out;

   if ( Word == 0 ) return 0;

   In.Word = Word;
   
   Out.Byte[0] = In.Byte[1];
   Out.Byte[1] = In.Byte[0];
   
   return Out.Word;
}


/*****************************************************************************/
/* Swap32                                                                    */
/*****************************************************************************/

UINT32 Swap32( UINT32 Long )
{
   DATA32 In;
   DATA32 Out;
   
   if ( Long == 0 ) return 0;

   In.Long = Long;
   
   Out.Word[0] = Swap16(In.Word[1]);
   Out.Word[1] = Swap16(In.Word[0]);
   
   return Out.Long;
}

#endif  /* SPP_LITTLE_ENDIAN */


void saveResult(void){
	FILE *fp;
	if((fp=fopen("./PowerLinkStatus", "w"))==NULL){
		perror("fopen");
		return;
	}
	if(fwrite(&G_PowerLinkStatus, sizeof(st_PC_LS), MAX_PORT_NUM+1, fp)<=0){
		perror("fwrite");
		fclose(fp);
		return;
	}
	fflush(fp);
	fclose(fp);
	//printf("date update!\n");
}

void get_poe_power_consume_and_line_status(void)
{
	INT32 powerConsumed;
	UINT8 index=0;
	UINT8 cnt=0;

	if(get_poe_total_power_consumed(0, &powerConsumed)==SUCCESS){ //total power consumed
		G_PowerLinkStatus[MAX_PORT_NUM].Type=MAX_PORT_NUM;
		G_PowerLinkStatus[MAX_PORT_NUM].PowerConsumed=(powerConsumed==-1?0:powerConsumed);
		cnt++;
	}

	powerConsumed=0;
	for(index=0; index<MAX_PORT_NUM; index++){  //port power consumed
		UINT8 Port=G_u8_PortMap[index];
		INT8 status;
		if(get_poe_port_power_consumed(0, Port, &powerConsumed)==SUCCESS){
			G_PowerLinkStatus[index].Type=index;
			G_PowerLinkStatus[index].PowerConsumed=(powerConsumed==-1?0:powerConsumed);
			cnt++;
		}
		if(get_poe_port_status(0, Port, &status)==SUCCESS){
			G_PowerLinkStatus[index].LinkStatus=(status==-1?0:status);
			cnt++;
		}
	}
	if(cnt==2*MAX_PORT_NUM+1){
		saveResult();
	}
}


INT8 find_pid_by_name( char* ProcName, int* pidFound)
{
        DIR             *dir;
        struct dirent   *d;
        int             pid, i, j;
        char            *s;
        int pnlen;
		int foundpid[128];

        i = 0;
        foundpid[0] = 0;
        pnlen = strlen(ProcName);

        /* Open the /proc directory. */
        dir = opendir("/proc");
        if (!dir)
        {
                printf("cannot open /proc");
                return -1;
        }

        /* Walk through the directory. */
        while ((d = readdir(dir)) != NULL) {

                char exe [PATH_MAX+1];
                char path[PATH_MAX+1];
                int len;
                int namelen;

                /* See if this is a process */
                if ((pid = atoi(d->d_name)) == 0)       continue;

                snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
                if ((len = readlink(exe, path, PATH_MAX)) < 0)
                        continue;
                path[len] = '\0';

                /* Find ProcName */
                s = strrchr(path, '/');
                if(s == NULL) continue;
                s++;

                /* we don't need small name len */
                namelen = strlen(s);
                if(namelen < pnlen)     continue;

                if(!strncmp(ProcName, s, pnlen)) {
                        /* to avoid subname like search proc tao but proc taolinke matched */
                        if(s[pnlen] == ' ' || s[pnlen] == '\0') {
                                foundpid[i] = pid;
                                i++;
                        }
                }
        }

        foundpid[i] = 0;
        closedir(dir);

		*pidFound=0;
		for(i=0; foundpid[i] != 0; i++);
		for(j=0; j<i; j++){
           *pidFound += foundpid[j]*((int)pow(10,(i-1)));
        }
	
        return  0;
}

int lockfileWR(int fd)
{
    struct flock fl;
    fl.l_type = F_WRLCK;  /* write lock */
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;  //lock the whole file
    return(fcntl(fd, F_SETLK, &fl));
}

int lockfileRD(int fd)
{
    struct flock fl;
    fl.l_type =  F_RDLCK;  /* read lock */
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;  //lock the whole file
    return(fcntl(fd, F_SETLK, &fl));
}

int unlockfile(int fd)
{
    struct flock fl;
    fl.l_type =  F_UNLCK;  /* unlock */
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;  //lock the whole file
    return(fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *filename)
{
    int fd;
    char buf[16];

    fd = open(filename, O_RDWR|O_CREAT, LOCKMODE);
    if (fd < 0) {
            //pDEBUG("can't open %s: %m\n", filename);
            exit(1);
    }

    /* 先获取文件锁 */
    if (lockfileWR(fd) == -1) {
      if (errno == EACCES || errno == EAGAIN) {
        //pDEBUG("file: %s already locked", filename);
        close(fd);
        return 1;
      }
//      pDEBUG("can't lock %s: %m\n", filename);
      exit(1);
    }
    /* 写入运行实例的pid */
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return 0;
}

int already_stoprunning(const char *filename)
{
    int fd;
    struct flock lock;
    
    fd = open(filename,  O_RDONLY, LOCKMODE);
    if (fd < 0) {
     	exit(1);
    }
		fcntl(fd, F_GETLK,&lock);
		if(lock.l_type != F_UNLCK){
			close(fd);
    	return -1;
		}
		
    close(fd);
    return 0;
}

int get_alarm_status_from_file(char **data)
{
	int total;
	int fd=0;
	
  if((fd=open(ALARMSTATUSFILE, O_RDONLY, LOCKMODE))<0){//O_RDWR
		return -1;
	}
	if (lockfileRD(fd) == -1) {
      if (errno == EACCES || errno == EAGAIN) {
        close(fd);
        return -1;
      }
      return -1;
  }
  
	total=lseek(fd,0,SEEK_END);
	lseek(fd,0,0);
	
	if((*data=malloc(total))==NULL){
		unlockfile(fd);
	  return -1;
	}
	if(read(fd,*data,total)<0){ 
		free(*data);
		unlockfile(fd);
		return -1;
	}
	
	unlockfile(fd);
	close(fd);
  return total;
}


int set_poe_status(const char *poe_status)
{
	int fd;
  fd = open(POESTATUSFILE, O_WRONLY|O_CREAT, LOCKMODE);//O_RDWR
  if (fd < 0) {
      return -1;
  }
  if(lockfileWR(fd) == -1) {
      if (errno == EACCES || errno == EAGAIN) {
        close(fd);
        return -1;
      }
       return -1;
  }

	ftruncate(fd, 0);
	write(fd, poe_status, strlen(poe_status) + 1);
	unlockfile(fd);
	close(fd);
	return 0;
}


int get_poe_status_from_file(char **data)
{
	int total;
	int fd=0;
	char *poe_status;
	
  if((fd=open(POESTATUSFILE, O_RDONLY, LOCKMODE))<0){//O_RDWR
		return -1;
	}
	if (lockfileRD(fd) == -1) {
      if (errno == EACCES || errno == EAGAIN) {
        close(fd);
        return -1;
      }
      return -1;
  }
  
	total=lseek(fd,0,SEEK_END);
	lseek(fd,0,0);
	
	if((*data=malloc(total))==NULL){
		unlockfile(fd);
	  return -1;
	}
	if(read(fd,*data,total)<0){ 
		free(*data);
		unlockfile(fd);
		return -1;
	}
	
	unlockfile(fd);
	close(fd);
  return total;
}





