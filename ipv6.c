#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int copy_dhcp6_client_to_imp(int skfd, int index, uint64 phymaps)
{
	return 0;
}

int drop_untrust_dhcp6_server_packet(int skfd, int index, uint64 phymaps)
{
	return 0;
}

int copy_dhcp6_server_to_imp(int skfd, int index, uint64 phymaps)
{
	return 0;
}

int bcm_l3v6_host_create(int skfd, int port, int vlan, uint8 *ipv6, uint64 *l2addr, int untag)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = port;	
    data[1] = vlan;
    data[2] = ipv6;	
    data[3] = l2addr;
    data[4] = untag;
 
	if (ioctl(skfd, L3V6_HOST_CREATE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}


int bcm_l3v6_host_delete(int skfd, int port, int vlan, uint8 *ipv6, uint64 *l2addr)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = port;	
    data[1] = vlan;
    data[2] = ipv6;	
    data[3] = l2addr;
 
	if (ioctl(skfd, L3V6_HOST_DELETE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_l3v6_host_find(int skfd, uint8 *ipv6, int *port, uint64 *l2addr)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = ipv6;	
    data[1] = port;
    data[2] = l2addr;
 
    *l2addr = 0x00ULL;
	if (ioctl(skfd, L3V6_HOST_FIND, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//	DEBUG("[%s:%d]: find %d.%d.%d.%d port %d, hwaddr: %04X%08X\n", __FUNCTION__, __LINE__, 
//	    (uint8)(ip>>24), (uint8)(ip>>16), (uint8)(ip>>8), (uint8)ip, *port, (uint32)((*l2addr) >> 32), (uint32)(*l2addr));

	return 0;
} 

int bcm_l3v6_host_init(int skfd)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
	
	if (ioctl(skfd, L3V6_HOST_INIT, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//	DEBUG("[%s:%d]: find %d.%d.%d.%d port %d, hwaddr: %04X%08X\n", __FUNCTION__, __LINE__, 
//	    (uint8)(ip>>24), (uint8)(ip>>16), (uint8)(ip>>8), (uint8)ip, *port, (uint32)((*l2addr) >> 32), (uint32)(*l2addr));

	return 0;
} 

 