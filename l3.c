#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"
#include "if_info.h"
#include "kputils.h"

int bcm_l3_host(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = 0;

	if (ioctl(skfd, L3_HOST_DUMP, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}

int bcm_l3_intf_create(int skfd, int vlan)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(data));
    data[0] = vlan;	
 
	if (ioctl(skfd, L3_INTF_CREATE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}

int bcm_l3_intf_delete_all(int skfd, int unit)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(data));
    data[0] = unit;
 
	if (ioctl(skfd, L3_INTF_DELETE_ALL, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}

int bcm_l3_intf_delete(int skfd, int vlan)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(data));
    data[0] = vlan;	
 
	if(ioctl(skfd, L3_INTF_DELETE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}


int bcm_l3_intf_dump(int skfd)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(data));
    data[0] = 0;
 
	if (ioctl(skfd, L3_INTF_DUMP, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}

int bcm_l3_host_create(int skfd, int dir, int port, int vlan, uint32 ip, uint64 *l2addr, int pri)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = dir;
    data[1] = port;	
    data[2] = vlan;
    data[3] = ip;	
    data[4] = l2addr;
    data[5] = pri;
 
	if (ioctl(skfd, L3_HOST_CREATE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}


int bcm_l3_host_delete(int skfd, uint32 ip)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = ip;
 
	if (ioctl(skfd, L3_HOST_DELETE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_l3_host_find(int skfd, uint32 ip, int *vlan, int *port, uint64 *l2addr, int *priority)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = ip;
    data[1] = vlan;	
    data[2] = port;
    data[3] = l2addr;
    data[4] = priority;
 
    *l2addr = 0x00ULL;
	if (ioctl(skfd, L3_HOST_FIND, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//	DEBUG("[%s:%d]: find %d.%d.%d.%d port %d, hwaddr: %04X%08X\n", __FUNCTION__, __LINE__, 
//	    (uint8)(ip>>24), (uint8)(ip>>16), (uint8)(ip>>8), (uint8)ip, *port, (uint32)((*l2addr) >> 32), (uint32)(*l2addr));

	return 0;
} 

int bcm_l3_host_delete_all(int skfd)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
	
	if (ioctl(skfd, L3_HOST_DELETE_ALL, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_l3_route_add(int skfd, int unit, uint32 network, uint32 netmask, uint32 gateway, uint64 *l2addr, int port, int vlan)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = unit;
    data[1] = network;	
    data[2] = netmask;
    data[3] = gateway;	
    data[4] = l2addr;
    data[5] = port;	
    data[6] = vlan;
 
	if (ioctl(skfd, L3_ROUTE_SET, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_l3_route_delete(int skfd, int unit, uint32 network, uint32 netmask, int vlan)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = unit;
    data[1] = network;	
    data[2] = netmask;
    data[3] = vlan;
 
	if (ioctl(skfd, L3_ROUTE_DEL, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int bcm_l3_route_get()
{
    return 0;
}

int bcm_l3_route_delete_all(int skfd, int unit)
{
	uint32 data[8];
    
	memset(data, '\0', sizeof(data));
    data[0] = unit;

	if (ioctl(skfd, L3_ROUTE_DELETE_ALL, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_l3_route_dump(int skfd, int unit)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(data));
    data[0] = unit;
 
	if (ioctl(skfd, L3_ROUTE_DUMP, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}


int bcm_switch_control_arp_get(int skfd, int *flag)
{
	uint32 data[8];
	
	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = flag;	
 
	if (ioctl(skfd, L3_ARP_GET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}

int bcm_switch_control_arp_set(int skfd, int flag)
{
	uint32 data[8];
	
	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = flag;
 
	if (ioctl(skfd, L3_ARP_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}
