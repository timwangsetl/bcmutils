/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Power Manager
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      PowerManager.h
*
* Description:    Power Manager API
*
* Revision History:
*
*   09/25/09  QHS  Pre-release
*   11/20/09  QHS  Ver 1.0 - Initial Release
*   12/22/09  QHS  Ver 1.1 - Change port power parameters to INT32 to support
*                            40W per port.  Remove 'const' from version string.
*
*----------------------------------------------------------------------------*/

/**************/
/* Management */
/**************/
#ifndef __POWER_MANAGER_H__
#define __POWER_MANAGER_H__
#include <signal.h>
#include "PMTypes.h"

#if (defined SZG12M)

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
#define MAX_PORT_NUM  24

#elif((defined HGM28)||(defined HGM24GE4GF)||(defined HGM24GE4GF_P))

#define PORT_NUM24 23
#define PORT_NUM23 24
#define PORT_NUM22 21
#define PORT_NUM21 22
#define PORT_NUM20 19
#define PORT_NUM19 20
#define PORT_NUM18 17
#define PORT_NUM17 18

#define PORT_NUM16  15
#define PORT_NUM15  16
#define PORT_NUM14  13
#define PORT_NUM13  14
#define PORT_NUM12  11
#define PORT_NUM11  12
#define PORT_NUM10  9
#define PORT_NUM9  	10

#define PORT_NUM8 7
#define PORT_NUM7 8
#define PORT_NUM6 5
#define PORT_NUM5 6
#define PORT_NUM4 3
#define PORT_NUM3 4
#define PORT_NUM2 1
#define PORT_NUM1 2
#define MAX_PORT_NUM  24

#elif ((defined HGM20)||(defined HGM16GE4GF))

#define PORT_NUM24 23
#define PORT_NUM23 24
#define PORT_NUM22 21
#define PORT_NUM21 22
#define PORT_NUM20 19
#define PORT_NUM19 20
#define PORT_NUM18 17
#define PORT_NUM17 18

#define PORT_NUM16  15
#define PORT_NUM15  16
#define PORT_NUM14  13
#define PORT_NUM13  14
#define PORT_NUM12  11
#define PORT_NUM11  12
#define PORT_NUM10  9
#define PORT_NUM9  	10

#define PORT_NUM8 7
#define PORT_NUM7 8
#define PORT_NUM6 5
#define PORT_NUM5 6
#define PORT_NUM4 3
#define PORT_NUM3 4
#define PORT_NUM2 1
#define PORT_NUM1 2
#define MAX_PORT_NUM  16
#endif

typedef struct{
	UINT8 Type; 
	UINT8 LinkStatus;
	INT32 PowerConsumed;
}st_PC_LS;


st_PC_LS G_PowerLinkStatus[MAX_PORT_NUM+1];

#define   SIG_PAUSE_POE_BG		SIGRTMIN+2   
#define   SIG_CONTINUE_POE_BG	SIGRTMIN+3  
#define   SIG_PAUSE_POE_ECHO	SIGRTMIN+4   
#define   SIG_CONTINUE_POE_ECHO	SIGRTMIN+5 

/* Event Structure */
typedef struct
{
   UINT8  Type;
   INT8   Parm1;
   UINT8  Parm2;
} EVENT;


/* Event Handler */
typedef void (*EVENT_HANDLER)( EVENT *Event );

/* Event Types */
#define SYSTEM_EVENT        1
#define PORT_EVENT          2
#define POWER_SUPPLY_EVENT  4
#define ERROR_EVENT         8
#define INFO_EVENT          16
#define POWER_CONSUME_EVENT 32



/* Management Prototypes */
INT8 init_poe_power_manager( EVENT_HANDLER Handler );
INT8 close_poe_power_manager( void );

/*****************/
/* System Status */
/*****************/

/* System Status */
#define STATUS_SYSTEM_OK           0
#define STATUS_SYSTEM_INIT_FAIL   -1
#define STATUS_SYSTEM_UNDER_VOLT  -2
#define STATUS_SYSTEM_OVER_TEMP   -3
#define STATUS_SYSTEM_COMM_LOST   -4

/* System Info Structure */
typedef struct
{
   char  *PowerManagerVersion;
   char  *PlatformSupportVersion;
} SYSINFO;

/* System Status Prototypes */
INT8 get_poe_system_status(INT32 skfd, INT8 *sysStatus);
INT8 get_poe_system_info(INT32 skfd, SYSINFO *SystemInfo );
INT8 get_poe_total_power_consumed(INT32 skfd, INT32 *totalPC);
INT8 get_poe_total_power_granted(INT32 skfd, INT32 *totalPG);
INT8 get_poe_total_power_provided(INT32 skfd, INT32 *totalPP);

/***************/
/* Port Status */
/***************/

/* Port Status */
#define STATUS_PORT_DISABLED     0
#define STATUS_PORT_POWERED_ON   1
#define STATUS_PORT_POWERED_OFF  2
#define STATUS_PORT_DENIED       3
#define STATUS_PORT_BLOCKED      4
#define STATUS_PORT_FORCED_ON    5
#define STATUS_PORT_FORCED_OFF   6

/* Port Info Structure */
typedef struct
{
   UINT8   Detection;
   UINT8   Classification;
   UINT16  Current;             /* mA */
   UINT16  PowerSupplyVoltage;  /* mV */
   char   *SiliconVersion;
   char   *FirmwareVersion;
} PORTINFO;

/* Detection */
#define DETECT_UNKNOWN      0
#define DETECT_SHORT        1
#define DETECT_LOW          3
#define DETECT_GOOD         4
#define DETECT_HIGH         5
#define DETECT_OPEN         6

/* Classification */
#define CLASS_UNKNOWN       0
#define CLASS_1             1
#define CLASS_2             2
#define CLASS_3             3
#define CLASS_4             4
#define CLASS_UNEQ_FINGERS  5
#define CLASS_0             6
#define CLASS_OVERLOAD      7

/* Port Priority */
#define PRIORITY_LOW        0
#define PRIORITY_HIGH       1
#define PRIORITY_FORCED     2
#define PRIORITY_CRITICAL   3

/* Port Status Prototypes */
INT8 get_poe_port_count(INT32 skfd, UINT8 *portCnt );
INT8 get_poe_port_status(INT32 skfd, UINT8 Port, INT8 *status);
INT8 get_poe_port_info(INT32 skfd, UINT8 Port, PORTINFO *PortInfo );
INT8 get_poe_port_priority_status(INT32 skfd, UINT8 Port, INT8 *portpri );
INT8 get_poe_port_power_consumed(INT32 skfd, UINT8 Port, INT32 *portPC);
INT8 get_poe_port_power_granted(INT32 skfd, UINT8 Port, INT32 *portPG );   
INT8 get_poe_port_power_requested(INT32 skfd, UINT8 Port, INT32 *portPR );
INT8 get_poe_port_power_available(INT32 skfd, UINT8 Port , INT32 *portPA);

/******************/
/* System Control */
/******************/

/* System Control Prototypes */
INT8  reset_poe_system( void );
void  restore_poe_factory_defaults( void );

/****************/
/* Port Control */
/****************/
#define PORT_CAPABILITY_HIGH   1
#define PORT_CAPABILITY_LOW    0

/* Port Control */
#define PORT_CTRL_AUTOMATIC  0
#define PORT_CTRL_FORCE_ON   1
#define PORT_CTRL_FORCE_OFF  2

/* Port Control Prototypes */
INT8  set_poe_port_control(INT32 skfd, UINT8 Port, UINT8 Control );
INT8  adjust_poe_port_power(INT32 skfd, UINT8 Port, INT32 Power );

/************************/
/* System Configuration */
/************************/

/* Granting Policy */
#define GRANT_POLICY_GRANT_BASED        0
#define GRANT_POLICY_CONSUMPTION_BASED  1

/* Retry Policy */
#define RETRY_IMMEDIATELY       0
#define RETRY_AFTER_RECONNECT   1
#define RETRY_AFTER_REENABLE    2

/* Power Location */
#define LOCATION_ENDPOINT       0
#define LOCATION_MIDSPAN        1

/* System Configuration Prototypes */
INT8  set_poe_power_provided(INT32 skfd, UINT8 PowerSupply, INT32 Power );
INT8  get_poe_power_provided(INT32 skfd, UINT8 PowerSupply, INT32 *powerPro );
INT8  set_poe_reserved_power(INT32 skfd, INT8 Reserved );
INT8  get_poe_reserved_power(INT32 skfd, INT8 *reservedP );
INT8  set_poe_overload_limit(INT32 skfd, INT8 Limit );
INT8  get_poe_overload_limit(INT32 skfd, INT8 *oLlimit );
INT8  set_poe_granting_policy(INT32 skfd, INT8 Policy );
INT8  get_poe_granting_policy(INT32 skfd, INT8 *pol );
INT8  set_poe_retry_policy(INT32 skfd, INT8 Policy );
INT8  get_poe_retry_policy(INT32 skfd, INT8 *rePolicy );
INT8  set_poe_power_location(INT32 skfd, INT8 Location );
INT8  get_poe_power_location(INT32 skfd, INT8 *local );

/**********************/
/* Port Configuration */
/**********************/

/* Capability */
#define CAPABILITY_LOW_POWER    0
#define CAPABILITY_HIGH_POWER   1

/* Port Configuration Prototypes */
INT8  set_poe_port_enable(INT32 skfd, UINT8 Port, INT8 Enable );
INT8  get_poe_port_enable(INT32 skfd, UINT8 Port, INT8 *enable );
INT8  set_poe_port_capability(INT32 skfd, UINT8 Port, INT8 Capability );
INT8  get_poe_port_capability(INT32 skfd, UINT8 Port, INT8 *portCap );
INT8  set_poe_port_priority(INT32 skfd, UINT8 Port, INT8 Priority );
INT8  get_poe_port_priority(INT32 skfd, UINT8 Port, INT8 *portPri );
INT8  set_poe_port_power_limit(INT32 skfd, UINT8 Port, INT32 Limit );
INT32 get_poe_port_power_limit(INT32 skfd, UINT8 Port, INT8 *portPL );

INT8 set_poe_power_supply_status(INT32 skfd, UINT8 PowerSupply, INT8 Status );
INT8 get_poe_power_supply_status(INT32 skfd, UINT8 PowerSupply, INT8 *powerS );

INT8 find_pid_by_name( char* ProcName, int* pidFound);
void get_poe_power_consume_and_line_status(void);
#endif

