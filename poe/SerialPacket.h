/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Power Manager
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      SerialPacket.h
*
* Description:    Serial Packet Protocol (SPP) Packets
*
* Revision History:
*
*   02/25/10  QHS  File created
*
*----------------------------------------------------------------------------*/

/* Start-of-Packet character */
#define SOP               0xAC

/* Maximum length of a version string */
#define MAX_VER_SIZE      8

/* Maximum number of events in an Events packet */
#define MAX_EVENT_COUNT   85

/* Routines */
#define RTN_GETSYSTEMSTATUS         1 
#define RTN_GETSYSTEMINFO           2 
#define RTN_GETTOTALPOWERCONSUMED   3 
#define RTN_GETTOTALPOWERGRANTED    4 
#define RTN_GETTOTALPOWERPROVIDED   5 
#define RTN_GETPORTCOUNT            6 
#define RTN_GETPORTSTATUS           7 
#define RTN_GETPORTINFO             8 
#define RTN_GETPORTPRIORITYSTATUS   9 
#define RTN_GETPORTPOWERCONSUMED    10
#define RTN_GETPORTPOWERGRANTED     11
#define RTN_GETPORTPOWERREQUESTED   12
#define RTN_GETPORTPOWERAVAILABLE   13
#define RTN_RESETSYSTEM             14
#define RTN_RESTOREFACTORYDEFAULTS  15
#define RTN_SETPORTCONTROL          16
#define RTN_ADJUSTPORTPOWER         17
#define RTN_SETPOWERPROVIDED        18
#define RTN_GETPOWERPROVIDED        19
#define RTN_SETRESERVEDPOWER        20
#define RTN_GETRESERVEDPOWER        21
#define RTN_SETOVERLOADLIMIT        22
#define RTN_GETOVERLOADLIMIT        23
#define RTN_SETGRANTINGPOLICY       24
#define RTN_GETGRANTINGPOLICY       25
#define RTN_SETRETRYPOLICY          26
#define RTN_GETRETRYPOLICY          27
#define RTN_SETPOWERLOCATION        28
#define RTN_GETPOWERLOCATION        29
#define RTN_SETPORTENABLE           30
#define RTN_GETPORTENABLE           31
#define RTN_SETPORTCAPABILITY       32
#define RTN_GETPORTCAPABILITY       33
#define RTN_SETPORTPRIORITY         34
#define RTN_GETPORTPRIORITY         35
#define RTN_SETPORTPOWERLIMIT       36
#define RTN_GETPORTPOWERLIMIT       37
#define RTN_SETPOWERSUPPLYSTATUS    38
#define RTN_GETPOWERSUPPLYSTATUS    39
#define RTN_GETEVENTS               40
#define RTN_SYNCHRONIZE						  41


/* Parameters format */
typedef struct
{
   UINT8  Parm8;
   UINT32 Parm32;
} DATA_PARMS;

/* System Information format */
typedef struct
{
   char   PowerManagerVersion[MAX_VER_SIZE];
   char   PlatformSupportVersion[MAX_VER_SIZE];
} DATA_SYSINFO;

/* Port Information format */
typedef struct
{
   INT8   Result;
   UINT8  Detection;
   UINT8  Classification;
   UINT16 Current;
   UINT16 PowerSupplyVoltage;
   char   SiliconVersion[2];
   char   FirmwareVersion[MAX_VER_SIZE];
} DATA_PORTINFO;


/* Parameters packet */
typedef struct
{
   UINT8      Start;          /* SOP */
   UINT8      Routine;        /* RTN_? */
   UINT8      DataLength;     /* sizeof(DATA_PARMS) */
   DATA_PARMS Data;
   UINT8      Checksum;
} PKT_PARMS;

/* System Information packet */
typedef struct
{
   UINT8        Start;        /* SOP */
   UINT8        Routine;      /* RTN_GETSYSTEMINFO */
   UINT8        DataLength;   /* sizeof(DATA_SYSINFO) */
   DATA_SYSINFO Data;
   UINT8        Checksum;
} PKT_SYSINFO;

/* Port Information packet */
typedef struct
{
   UINT8         Start;       /* SOP */
   UINT8         Routine;     /* RTN_GETPORTINFO */
   UINT8         DataLength;  /* sizeof(DATA_PORTINFO) */
   DATA_PORTINFO Data;
   UINT8         Checksum;
} PKT_PORTINFO;

/* Events packet */
typedef struct
{
   UINT8  Start;              /* SOP */
   UINT8  Routine;            /* RTN_GETEVENTS */
   UINT8  DataLength;         /* Number of events * sizeof(EVENT) */
   EVENT  Events[MAX_EVENT_COUNT];
   UINT8  Checksum;
} PKT_EVENTS;

