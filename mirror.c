#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

//MIRROR CAPTURE CONTROL REGISTER (PAGE 03H/ADDR 10H-17H)
int get_mirror_capture_enable(int skfd)
{
    return 0;
}

//Mirror Capture Control Register (Page 02h: Address 10h每11h)
int set_mirror_capture_enable(int skfd, int enable, int dst, uint64_t *in, uint64_t *out)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = 0;
    data[1] = enable;
    data[2] = dst;
    data[3] = in;
    data[4] = out;

	if (ioctl(skfd, MIRROR_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

/* Mirror Capture Control Register (Page 02h: Address 10h每11h) */
/* port = 0 means no capture port, others: has capture port */
int set_mirror_capture_port(int skfd, int port)
{
	return 0;
}

/* Mirror Capture Control Register (Page 02h: Address 10h每11h) */
/* port = 0 means no capture port, others: has capture port */
int get_mirror_capture_port(int skfd)
{
	return 0;
}
    
/* Ingress Mirror Control Register (Page 02h: Address 12h每13h) */
int set_mirror_ingress(int skfd, uint64_t *phymaps)
{
      return 0;
}

/* Ingress Mirror Control Register (Page 02h: Address 12h每13h) */
int get_mirror_ingress(int skfd, uint64_t *phymaps)
{      
      return 0;
}

/* Egress Mirror Control Register (Page 02h: Address 1Ch每1Dh) */
int set_mirror_egress(int skfd,  uint64_t *phymaps)
{
      return 0;
}    

/* Egress Mirror Control Register (Page 02h: Address 1Ch每1Dh) */
int get_mirror_egress(int skfd,  uint64_t *phymaps)
{
      return 0;
}    

int set_mirror_ingress_div(int skfd, int enable)
{
    return 0;
}

int set_mirror_egress_div(int skfd, int enable)
{
    return 0;
}

int set_mirror_div(int skfd, uint16 val, int type)
{
    return 0;
}

//Mirror Capture Control Register (Page 02h: Address 10h每11h)
int bcm_mirror_vlan_set(int skfd, int enable, int session, int dst, int vlan)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;	
    data[1] = session;
    data[2] = dst;
    data[3] = vlan;

	if (ioctl(skfd, MIRROR_SET_VLAN, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int set_bcm_mirror_vlan(int skfd, int enable, int session, int dst, int vlan)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;
    data[1] = session;
    data[2] = dst;	
    data[3] = vlan;

	if (ioctl(skfd, MIRROR_SET_VLAN, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int del_bcm_mirror_vlan(int skfd, int session)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = session;

	if (ioctl(skfd, MIRROR_SESSION_DEL, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}


