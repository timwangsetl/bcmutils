#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int bcm_rate_set(int skfd, int pps, int flags)
{
	uint32 data[8];
		
	memset(data, '\0', sizeof(data));;
    data[0] = pps;
    if(0 == pps)
        data[1] = 0x00;
    else    
        data[1] = flags;

	if (ioctl(skfd, STORM_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_rate_get(int skfd, int *pps, int *flags)
{
	uint32 data[8];
		
	memset(data, '\0', sizeof(data));
    data[0] = pps;
    data[1] = flags;

	if (ioctl(skfd, STORM_GET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
} 

int bcm_set_broadcast_data(int skfd, int port, int rate)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = rate;

	if (ioctl(skfd, STORM_BCAST, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_set_multicast_data(int skfd, int port, int rate)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = rate;

	if (ioctl(skfd, STORM_MCAST, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_set_dlfunicast_data(int skfd, int port, int rate)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = rate;

	if (ioctl(skfd, STORM_UCAST, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_rate_bandwidth_ingress_set(int skfd, int port, uint32 kbits_sec, uint32 kbits_burst)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = kbits_sec;
    data[2] = kbits_burst;

	if (ioctl(skfd, RATE_INGRESS_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}
  
int bcm_rate_bandwidth_ingress_get(int skfd, int port, uint32 *kbits_sec, uint32 *kbits_burst)
{
    return 0;
}

int bcm_rate_bandwidth_egress_set(int skfd, int port, uint32 kbits_sec, uint32 kbits_burst)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = kbits_sec;
    data[2] = kbits_burst;

	if (ioctl(skfd, RATE_EGRESS_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}
  
int bcm_rate_bandwidth_egress_get(int skfd, int port, uint32 *kbits_sec, uint32 *kbits_burst)
{
    return 0;
} 
