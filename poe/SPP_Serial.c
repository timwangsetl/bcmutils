/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Serial Packet Protocol
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      SPP_Serial.c
*
* Description:    Serial Support Routines for Linux
*
* Revision History:
*
*   12/06/10  QHS  Initial release
*
*----------------------------------------------------------------------------*/

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <semaphore.h>
#include "PMTypes.h"
#include "PMErrors.h"
#include "SPP.h"
#include "SPP_Support.h"
#include "SPP_Events.h"


/* Serial port device name */
const char *SerialName = "/dev/ttyS0";

/* Handle to the serial port */
int SerialPort;

/* Mutual exclusion semaphore */
sem_t SerialMutex;

/* Forward reference */
INT8 OpenSerial( void );
INT8 get_poe_system_status(INT32 skfd, INT8 *sysStatus);


/*****************************************************************************/
/* InitSerial                                                                */
/*****************************************************************************/

INT8 InitSerial( void )
{
   INT8 Result;

   /* Open the serial port */
   Result = OpenSerial();
   if ( Result == SUCCESS )
   {
	  INT8 systemStatus;
	  get_poe_system_status(0, &systemStatus);
      /* Try to get the system status */
      if ( systemStatus == STATUS_SYSTEM_COMM_LOST )
      {
         //CloseSerial();
         Result = ERROR_RESOURCE_NOT_FND;
      } 
   }
   
   return Result;
}


/*****************************************************************************/
/* OpenSerial                                                                */
/*****************************************************************************/

INT8 OpenSerial( void )
{
   struct termios Attributes;
   int Result;

   /* Open the serial port */
   SerialPort = open( SerialName, O_RDWR | O_NOCTTY );
   if ( SerialPort == -1 ) return ERROR_RESOURCE_CREATE;

   /* Get the attributes of the serial port */
   Result = tcgetattr( SerialPort, &Attributes );
   if ( Result == -1 )
   {
      close( SerialPort );
      return ERROR_RESOURCE_CONFIG;
   }

   /* Configure the serial port for raw mode */
   cfmakeraw( &Attributes );

#if defined MV3035_M28
   /* Configure the serial port for 115200 baud */
   cfsetspeed( &Attributes, B115200 );
#else
   /* Configure the serial port for 19200 baud */
   cfsetspeed( &Attributes, B19200 );
#endif
   
   /* Configure the serial port for 8-N-1 */
   Attributes.c_cflag &= ~( CSIZE | PARENB | CSTOPB );
   Attributes.c_cflag |=  ( CS8 | CREAD | CLOCAL );
   
   /* Configure the serial port read timout to 1 second */
   Attributes.c_cc[VMIN]  = 0;
   Attributes.c_cc[VTIME] = 10;

   /* Set the attributes of the serial port */
   Result = tcsetattr( SerialPort, TCSANOW, &Attributes );
   if ( Result == -1 )
   {
      close( SerialPort );
      return ERROR_RESOURCE_CONFIG;
   }

   /* Create the mutual exclusion semaphore */
   Result = sem_init( &SerialMutex, 0, 1 );
   if ( Result == -1 )
   {
      close( SerialPort );
      return ERROR_RESOURCE_CONFIG;
   }
   
   return SUCCESS;
}


/*****************************************************************************/
/* CloseSerial                                                               */
/*****************************************************************************/

void CloseSerial( void )
{
   sem_destroy( &SerialMutex );
   close( SerialPort );
}


/*****************************************************************************/
/* SerialSend                                                                */
/*****************************************************************************/

INT16 SerialSend( UINT8 *Data, UINT8 DataSize )
{
   ssize_t DataSent;

   /* Write data to the serial port */
   DataSent = write( SerialPort, Data, DataSize ); 
   if ( DataSent != DataSize ) return ERROR_RESOURCE_WRITE;

   return (INT16)DataSent;
}


/*****************************************************************************/
/* SerialReceive                                                             */
/*****************************************************************************/

INT16 SerialReceive( UINT8 *Data, UINT8 DataSize )
{
   ssize_t DataReceived;
   
   /* Read the data from the serial port */
   DataReceived = read( SerialPort, Data, DataSize );
   if ( DataReceived == -1 ) return ERROR_RESOURCE_READ;
   
   return (INT16)DataReceived;
}


/*****************************************************************************/
/* SerialLock                                                                */
/*****************************************************************************/

void SerialLock( void )
{
   /* Only one task at a time can use the serial port */
   sem_wait( &SerialMutex );
}


/*****************************************************************************/
/* SerialUnlock                                                              */
/*****************************************************************************/

void SerialUnlock( void )
{
   /* Allow another task to use the serial port */
   sem_post( &SerialMutex );
}

