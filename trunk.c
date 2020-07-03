#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include "if_info.h"

/*
 * cli get port trunk status, group start 1
 */
int cli_get_port_trunk_status(int skfd, int group, uint64_t *val64)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = group;
    data[1] = val64;
    
	if (ioctl(skfd, TRUNK_GET_PORT, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//	DEBUG("[%s:%d] get trunk member: 0x%08x%08x", __FUNCTION__, __LINE__, (uint32)((*val64) >> 32), (uint32)(*val64));
	return 0;
}	

/*
 * get port aggreation group number
 */
int bcm_get_port_trunk_member(int skfd, int port, uint64_t *val64)
{
	int reg_base = 0x10, group;
	uint64_t u_port_map = 0x0ULL;
	
	for(group = 1; group <= TNUM; group++)
	{
		cli_get_port_trunk_status(skfd, group, &u_port_map);
		if(u_port_map & (0x1ULL << port)) 
        {
			*val64 = u_port_map;
			return group;
		}
	}
		
	return 0;
}

int bcm_set_trunk_port(int skfd, int group, int enable, int port, int hash)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = group;
    data[1] = enable;
    data[2] = port;
    data[3] = hash;
 
	if (ioctl(skfd, TRUNK_SET_PORT, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}	

//void static_lacp_port_status(int skfd, char *link_status, char *real_speed){}


int bcm_trunk_set(int skfd, int hash, int tid, uint64 * maps)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = hash;
    data[1] = tid;
    data[2] = maps;
 
	if (ioctl(skfd, TRUNK_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}	

int bcm_trunk_disable(int skfd, int unit, int tid)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = unit;
    data[1] = tid;

	if (ioctl(skfd, TRUNK_DEL, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}	

int bcm_trunk_remove_all(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
	if (ioctl(skfd, TRUNK_DEL_ALL, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}	