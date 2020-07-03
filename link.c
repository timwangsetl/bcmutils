#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int bcm_get_swlink_status(uint64 * link)
{ 
    int skfd;
	uint32 data[8];
	
	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
        return -1;
	
    data[0] = link;
	if (ioctl(skfd, GET_SWITCH_LINK, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
        close(skfd);
		return -1;    
	}
	
	DEBUG("[%s:%d] get port link: 0x%08x%08x", __FUNCTION__, __LINE__, (uint32)((*link) >> 32), (uint32)(*link));

    close(skfd);
    return 0;      
}

int bcm_get_port_link(int skfd, int unit, int port, int *up)
{ 
	uint32 data[8];
	
    data[0] = unit;
    data[1] = port;
    data[2] = up;
    
	if (ioctl(skfd, PORT_LINK_GET, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//	DEBUG("[%s:%d] get port link: %d", __FUNCTION__, __LINE__, *up);
	
	return 0;
}

int get_link_status(int skfd, int unit, uint64 * link)
{ 
	uint32 data[8];

    data[0] = (uint64 *)link;
	if (ioctl(skfd, GET_SWITCH_LINK, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//	DEBUG("[%s:%d] get port link: 0x%08x%08x", __FUNCTION__, __LINE__, (uint32)((*link) >> 32), (uint32)(*link));
    return 0;      
}
int bcm_get_link(int skfd, uint64 * bmaps, uint64 * link)
{ 
	uint32 data[8];

    data[0] = (uint64 *)bmaps;
    data[1] = (uint64 *)link;
    
	if (ioctl(skfd, GET_LINK, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	//DEBUG("[%s:%d] get port link: 0x%08x%08x", __FUNCTION__, __LINE__, (uint32)((*link) >> 32), (uint32)(*link));
    return 0;      
}


int bcm_init_led(int skfd)
{ 
	uint32 data[8];
	
	if (ioctl(skfd, LED_INIT, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}