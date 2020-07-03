#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int bcm_auth_mode_set(int skfd, int port, uint32 mode)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = port;	
    data[1] = mode;
 
	if (ioctl(skfd, PORT_AUTH_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}
 
int bcm_auth_mode_get(int skfd, int port, uint32 *mode)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = port;	
    data[1] = mode;
 
//	if (ioctl(skfd, PORT_AUTH_GET, &data) < 0) {
//		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
//		return -1;    
//	}

	return 0;
}
int bcm_set_max_mac_num(int skfd, int port, int number)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;	
    data[1] = number;
 
//	if (ioctl(skfd, PORT_AUTH_GET, &data) < 0) {
//		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
//		return -1;    
//	}

	return 0;
}

int bcm_dhcp_snooping_set(int skfd, int enable)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;	
 
	if (ioctl(skfd, DHCP_SNOOP_ENABLE, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_arp_snooping_set(int skfd, int enable, uint64 maps)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;
    data[1] = &maps;		
 
	if (ioctl(skfd, ARP_SNOOP_ENABLE, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_ports_ip_drop_set(int skfd, int enable, uint64 maps)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;	
    data[1] = &maps;	
 
	if (ioctl(skfd, IPADDR_POLICE_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_access_ip_init(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
 
	if (ioctl(skfd, IPADDR_GUARD_INIT, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_access_ip_set(int skfd, int enable, int index, uint32 ip, int vlan, int port, uint8 *macaddr)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;	
    data[1] = index;	
    data[2] = ip;	
    data[3] = vlan;	
    data[4] = port;	
    data[5] = macaddr;	
 
	if (ioctl(skfd, IPADDR_GUARD_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_access_ip_clear(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
 
	if (ioctl(skfd, IPADDR_GUARD_INIT, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}