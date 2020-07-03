#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int bcm_set_rstp_stp(int skfd, int unit, int port, int status)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = port;	
    data[2] = status;
 
	if (ioctl(skfd, SET_PORT_STP_STATUS, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	DEBUG("[%s,%d] set port %d, stp status %d",__FUNCTION__,__LINE__, port, status);
	return 0;
}

int bcm_set_erps_bpdu(int skfd,int md_level, int control_vlan, int block1, int  block2, int primary_port, int second_port)
{

	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = md_level;
    data[1] = block1;	
    data[2] = block1;
	data[3] = control_vlan;	
    data[4] = primary_port;
	data[5] = second_port;
 
	if (ioctl(skfd, ERPS_BPDU_LEVEL_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	DEBUG("[%s,%d] set port %d, stp status %d",__FUNCTION__,__LINE__, md_level, control_vlan);
	return 0;
	   
}
int bcm_get_rstp_stp(int skfd, int unit, int port, int * status)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = port;	
    data[2] = status;
 
	if (ioctl(skfd, GET_PORT_STP_STATUS, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	DEBUG("[%s,%d] get port %d, stp status %d",__FUNCTION__,__LINE__, port, *status);
	return 0;
}

int get_port_stp_status(int port, char *ring)
{
	int skfd, res;
	int stp;
	
	if(*ring != '1')
	   return 0;
	   	
	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
		return -1;
		
	bcm_get_rstp_stp(skfd, 0, phy[port], &stp);
	close(skfd);
	
	if(BCM_PORT_STP_FORWARD == stp)
		res = 3;
	else if(BCM_PORT_STP_BLOCK == stp)
		res = 2;
	else 
		res = 1;	
	
	close(skfd);
	return res;		
}


int bcm_set_bpdu_mac(int skfd, int unit, int which)
{
	uint32 data[8];
	uint64 bpdu[3] = {0x0180c2000000ULL, 0x01505e00006cULL, 0x01505e00006dULL, 0x00ULL};
	

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = &bpdu[which];
 
	if (ioctl(skfd, BPDU_MAC_MODIFY, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	DEBUG("[%s,%d] BPDU is 0x%08x%08x, which is %d",__FUNCTION__,__LINE__, (uint32)(bpdu[which] >> 32), (uint32)bpdu[which], which);

	return 0;
}

int bcm_enable_bpdu_rx(int skfd, int unit, int port, int enable)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = port;
    data[2] = enable;
 
	if (ioctl(skfd, PORT_BPDU_ENABLE, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_mstp_enable(int skfd, char enable)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;
 
	if (ioctl(skfd, MSTP_ENABLE, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int bcm_mstp_set_port_state(int skfd, uint16 msti, int port, uint8 state)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = msti;
    data[1] = port;
    data[2] = state;
 
	if (ioctl(skfd, MSTP_PORT_SET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int bcm_mstp_get_port_state(int skfd, uint16 msti, int port, uint8 *state)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = msti;
    data[1] = port;
    data[2] = state;
 
	if (ioctl(skfd, MSTP_PORT_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int bcm_mstp_msti_map_vlan(int skfd, uint16 msti, uint16 vid)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = msti;
    data[1] = vid;
 
	if (ioctl(skfd, MSTP_ADD_VLAN, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_mstp_msti_remove_vlan(int skfd, uint16 msti, uint16 vid)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = msti;
    data[1] = vid;
 
	if (ioctl(skfd, MSTP_REMOVE_VLAN, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}
int bcm_set_stg_init(int skfd, int unit)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
  
       #if 1 
	if (ioctl(skfd, BCM_STG_INIT, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	#endif
	DEBUG("[%s,%d] bcm_set_stg_init",__FUNCTION__,__LINE__);
	return 0;
}
int bcm_set_stg_create_id(int skfd, int unit, int stg)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = stg;	
       #if 1 
	if (ioctl(skfd, ERPS_STG_CREATE, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	#endif
	DEBUG("[%s,%d] set port %d, stp status %d",__FUNCTION__,__LINE__, unit, stg);
	return 0;
}


int bcm_set_stg_stp(int skfd, int unit, int stg,int port, int status)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;   	
    data[1] = port;
	data[2] = status;
	data[3] = stg;
       #if 1 
	if (ioctl(skfd, ERPS_STG_STATE_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	#endif
	//DEBUG("[%s,%d] set port %d, stp status %d",__FUNCTION__,__LINE__, port, status);
	return 0;
}
int bcm_set_stg_add_vlan(int skfd, int unit, int stg,int vlan)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = stg;	
    data[2] = vlan;
	
       #if 1 
	if (ioctl(skfd, ERPS_STG_ADD_VLAN, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	#endif
	//DEBUG("[%s,%d] set port %d, stp status %d",__FUNCTION__,__LINE__, port, status);
	return 0;
}
int bcm_set_stg_remove_vlan(int skfd, int unit, int stg,int vlan)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = stg;	
    data[2] = vlan;
	
       #if 1 
	if (ioctl(skfd, ERPS_STG_REMOVE_VLAN, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	#endif
	//DEBUG("[%s,%d] set port %d, stp status %d",__FUNCTION__,__LINE__, port, status);
	return 0;
}

