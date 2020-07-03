/*-----------------------------------------------------------------------------
*
* Project:        Silicon Labs Serial Packet Protocol
*
* Copyright:      2010 Silicon Labs, Inc. (www.silabs.com)
*
* File Name:      PoE.c
*
* Description:    Entry point for PoE CLI application for Linux
*
* Revision History:
*
*   12/06/10  QHS  Initial release
*
*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#if 0
#include "CLI.h"
#endif
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

#include "poe_tools.h"

BOOL G_CheckPoe;
void SPPEventHandler(EVENT *Event)
{
}

void HandlePauseSig(int signo){
	int pid;
	G_CheckPoe=FALSE;
	if(!find_pid_by_name("sendSig", &pid)){
		kill(pid, SIG_PAUSE_POE_ECHO);
	}
}

void HandleContinueSig(int signo){
	int pid;
	G_CheckPoe=TRUE;
	if(!find_pid_by_name("sendSig", &pid)){
		kill(pid, SIG_CONTINUE_POE_ECHO);
	}
}

int main( void )
{
	INT8 u8_state = 0xff;
	int result;

	if(signal(SIG_PAUSE_POE_BG, HandlePauseSig)==SIG_ERR){
		perror("SIG_PAUSE_POE_BG");
		exit(0);
	}
	if(signal(SIG_CONTINUE_POE_BG, HandleContinueSig)==SIG_ERR){
		perror("SIG_CONTINUE_POE_BG");
		exit(0);
	}

	result = init_poe_power_manager(SPPEventHandler);
	if(result==SUCCESS){
		if(get_poe_system_status(0, (INT8 *)&u8_state)==SUCCESS);{
			if(u8_state==0){
				printf("POE system on!\n");
			}else{
				printf("poe system error!\n");
			}	
		}
	}

	while(1){
		Delay(2000);
		if(G_CheckPoe){
			get_poe_power_consume_and_line_status();
		 }
	}
}   

