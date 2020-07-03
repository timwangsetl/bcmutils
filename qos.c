#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

/* ENABLE CPU_Ctrl and Qos_ENA*/
int set_qos_enable_cli(int skfd, int enable)
{
}

/* QOS CONTROL REGISTER (PAGE 30H/ADDR 10H-17H) */
int set_qos_pause_enable(int skfd, int enable)
{
}

/* QoS IEEE 802.1p Enable Register (Page 30h: Address 04h¨C05h)*/
int set_qos_8021p_enable(int skfd, int enable)
{
}

/*
   set port base priority
   portId: 0-28, 0=IMP port
   priority: range 0-7
*/
int set_qos_port_priority(int skfd, int i, int priority)
{
    return 0; 
}


/* PRIORITY THRESHOLD REGISTER (PAGE 30H/ADDR 30H-31H) */
int set_qos_8021p_priority(int skfd, int tag, int priority)
{
}

/* DIFFSERV DSCP PRIORITY REGISTER (PAGE 30H/ADDR 40H-4FH) */
int set_qos_dscp_priority(int skfd, int tag, int priority)
{    
}

//Table 173:  TX Queue Control Register (Page 30h: Address 80h)
int set_qos_schedule_mode(int skfd, int schedule)
{
}


int set_qos_mac_priority(int skfd, uint64_t mac, int enable, int priority, int port, int vid)
{
}

//QoS DiffServ Enable Register (Page 30h: Address 06h¨C07h
int set_qos_dscp_enable(int skfd, int enable)
{
}

int set_wrr_weight(int queue_id, int weight)
{
}

int bcm_config_set_wrr_weight(int queue_id, int weight)
{
    int wrrnum[8];
    char line[128], *wrr = nvram_safe_get("qos_wrr_config");
    
    sscanf(wrr, "%d,%d,%d,%d,%d,%d,%d,%d,", &wrrnum[0], &wrrnum[1], &wrrnum[2], &wrrnum[3],
        &wrrnum[4], &wrrnum[5], &wrrnum[6], &wrrnum[7]);
        
    wrrnum[queue_id-1] =  weight;
         
    memset(line, '\0', sizeof(line));
    sprintf(line, "%d,%d,%d,%d,%d,%d,%d,%d,", wrrnum[0], wrrnum[1], wrrnum[2], wrrnum[3],
        wrrnum[4], wrrnum[5], wrrnum[6], wrrnum[7]);
    
    scfgmgr_set("qos_wrr_config", line);
    free(wrr);   
    
    return 0;
}

int bcm_drop_profile_set(int skfd, int mode, int delay)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
	data[0] = mode;
	data[1] = delay;
	
 	//printf("%s %d\n",__FUNCTION__,__LINE__);
	if (ioctl(skfd, QOS_DROP_PROFILE, &data) < 0) {
		return -1;	  
	}
	
	return 0;
}

int bcm_cosq_sched_set(int skfd, int mode, int delay,uint32 *weight)
{
    uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = mode;
    data[1] = delay;
	data[2] = weight;
   
	if (ioctl(skfd, COSQ_SCHED_SET, &data) < 0) {
		return -1;    
	}
	
	return 0;
}

int bcm_cosq_sched_get(int skfd, int *mode, int *delay)
{
    uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = mode;
    data[1] = delay;
 
	if (ioctl(skfd, COSQ_SCHED_GET, &data) < 0) {
		return -1;    
	}
	
	return 0;
}


int bcm_cosq_mapping_set(int skfd, int priority, int cosq)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = priority;
    data[1] = cosq;
 
	if (ioctl(skfd, COSQ_MAPPING_SET, &data) < 0) {
		return -1;    
	}
	
	return 0;
}

int bcm_cosq_mapping_get(int skfd, int priority, int *cosq)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = priority;
    data[1] = cosq;
 
	if (ioctl(skfd, COSQ_MAPPING_GET, &data) < 0) {
		return -1;    
	}
	
	return 0;
}

int bcm_cosq_init(int skfd, int numq)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = numq;
    
	if (ioctl(skfd, COSQ_INIT, &data) < 0) {
		return -1;    
	}
	
	return 0;
}

int bcm_set_qos_port_priority(int skfd, int port, int priority)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = port;
    data[1] = priority;
 
	if (ioctl(skfd, PORT_COSQ, &data) < 0) {
		return -1;    
	}
	
    return 0; 
}

int bcm_dscp_mapping_set(int skfd, int dscp, int priority, int enable)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = dscp;
    data[1] = priority;
    data[2] = enable;
 
	if (ioctl(skfd, DSCP_MAPPING_SET, &data) < 0) {
		return -1;    
	}
	
	return 0;
}

int bcm_port_prority_set(int skfd, int port, int priority)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(uint32) * 8);
    data[0] = port;
    data[1] = priority;
 
	if (ioctl(skfd, QOS_PORT_PRORITY, &data) < 0) {
		return -1;    
	}
	
	return 0;
}

