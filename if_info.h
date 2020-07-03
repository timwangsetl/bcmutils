/******************************************************************************
*  Copyright (c) 2002 Linux UPnP Internet Gateway Device Project              *    
*  All rights reserved.                                                       *
*                                                                             *   
*  This file is part of The Linux UPnP Internet Gateway Device (IGD).         *
*                                                                             *
*  The Linux UPnP IGD is free software; you can redistribute it and/or modify *
*  it under the terms of the GNU General Public License as published by       *
*  the Free Software Foundation; either version 2 of the License, or          *
*  (at your option) any later version.                                        *
*                                                                             *    
*  The Linux UPnP IGD is distributed in the hope that it will be useful,      *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*  GNU General Public License for more details.                               *
*                                                                             *   
*  You should have received a copy of the GNU General Public License          * 
*  along with Foobar; if not, write to the Free Software                      *
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *
*                                                                             *  
*                                                                             *  
******************************************************************************/
#ifndef _TOOL_H_
#define _TOOL_H_

#include "bcmutils.h"

#define IPV6_ADDR_ANY           0x0000U
	
#define IPV6_ADDR_UNICAST       0x0001U
#define IPV6_ADDR_MULTICAST     0x0002U
#define IPV6_ADDR_ANYCAST       0x0004U
	
#define IPV6_ADDR_LOOPBACK      0x0010U
#define IPV6_ADDR_LINKLOCAL     0x0020U
#define IPV6_ADDR_SITELOCAL     0x0040U
	
#define IPV6_ADDR_COMPATv4      0x0080U
	
#define IPV6_ADDR_SCOPE_MASK    0x00f0U
	
#define IPV6_ADDR_MAPPED        0x1000U
#define IPV6_ADDR_RESERVED      0x2000U	/* reserved address space */

typedef struct {
	char addr6[128];
	char devname[20];
	int plen;
	char scope[32];
	int scope_type;
	int dad_status;
	int if_idx;
	
	struct if_ipv6_t *next;
}if_ipv6_t;

typedef struct if_info_s{
	char ifname[16];
	char ipaddr[16];
	unsigned hexip;
	unsigned hexmask;
	char mac[18];
    char hexmac[6];
    char bcask[16];
	char mask[16];
	char gateway[16];
	int  mtu;
	
}if_info_t;

typedef struct{
    unsigned crc;
    char   mac[12];
	char   rsvd[32];
	char   sn[32];
	unsigned fiber;
}sw_info_t;


typedef struct{
	uint64   mac;
	unsigned int 	crc;
	unsigned fiber;
	unsigned fixtype;
}MAC_INFO;
#define SWITCH_CFG "/dev/mtdblock6"

//extern SWINFO;
/* Obtain a ip address string from ifname , maybe we can get it from nvram 
   without using open socket method 
 */
int getIFInfo(if_info_t *if_info);
int getSWInfo(MAC_INFO *sw_info);

unsigned long crc32(char *data, int length);

#endif/*_TOOL_H_*/
