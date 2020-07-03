#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include "if_info.h"

int bcm_i2c_read(int skfd, int addr, int reg, uint8 *value)
{
	uint32 data[8];
	
    data[0] = addr;
    data[1] = reg;
    data[2] = value;
    
	if (ioctl(skfd, I2C_READ, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;
}

int bcm_i2c_write(int skfd, int addr, int reg, uint8 value)
{
	uint32 data[8];
	
    data[0] = addr;
    data[1] = reg;
    data[2] = value;
    
	if (ioctl(skfd, I2C_WRITE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;
}


int bcm_ddm_read(int skfd, int channel, int addr, int reg, int size, uint8 *value)
{
	uint32 data[8];
	
    data[0] = channel;
    data[1] = addr;
    data[2] = reg;
    data[3] = size;
    data[4] = value;
    
	if (ioctl(skfd, SFP_READ, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;
}
