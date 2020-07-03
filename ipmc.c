#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int bcm_ipmc_enable(int skfd, int enable)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = enable;

	if (ioctl(skfd, IPMC_ENABLE, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_add(int skfd, int vid, uint32 maddr, uint32 ipaddr, int *group)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = vid;
    data[1] = maddr;
    data[2] = ipaddr;
    data[3] = group;

	if (ioctl(skfd, IPMC_ADD, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_remove(int skfd, int vid, uint32 maddr, uint32 ipaddr, int group)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = vid;
    data[1] = maddr;
    data[2] = ipaddr;
    data[3] = group;

	if (ioctl(skfd, IPMC_DEL, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_remove_all(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));

	if (ioctl(skfd, IPMC_DEL_ALL, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_egress_add(int skfd, int group_id, int port, int vlan, int *encap_id)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = group_id;
    data[1] = port;
    data[2] = vlan;
    data[3] = encap_id;

	if (ioctl(skfd, IPMC_EGRESS_ADD, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_egress_remove(int skfd, int group_id, int port, int vlan)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = group_id;
    data[1] = port;
    data[2] = vlan;

	if (ioctl(skfd, IPMC_EGRESS_DEL, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_egress_remove_all(int skfd, int group_id)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = group_id;

	if (ioctl(skfd, IPMC_EGRESS_DEL_ALL, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_egress_port_set(int skfd, int port, int untag, int vid)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = untag;
    data[2] = vid;

	if (ioctl(skfd, IPMC_EGRESS_PORT, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_dir_egress_port_set(int skfd, int port, int untag, int vid)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = untag;
    data[2] = vid;

	if (ioctl(skfd, IPMC_ZEGRESS_PORT, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}
int bcm_ipmc_dir_egress_add(int skfd, int group_id, int port, int vlan, int *encap_id)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = group_id;
    data[1] = port;
    data[2] = vlan;
    data[3] = encap_id;

	if (ioctl(skfd, IPMC_ZEGRESS_ADD, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_dir_egress_remove(int skfd, int group_id, int port, int vlan)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = group_id;
    data[1] = port;
    data[2] = vlan;

	if (ioctl(skfd, IPMC_ZEGRESS_DEL, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_ipmc_dump(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));

	if (ioctl(skfd, IPMC_DUMP, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_get_ipmc_number(char *igmp_config)
{
    int i, num = 0;
    char *p0 = igmp_config;

    while(1) 
    {
        p0 = strchr(++p0, ';');
        
        if(NULL == p0)
            break;
        else    
            num++;
    }

    return num;
}

int bcm_get_ipmc_entry(char *ipmc_config, IPMC_ENTRY *ipmc)
{
    int i = 0;
    IPMC_ENTRY * ipmc_entry = ipmc;
    char *p, *p1, *p2, *p3, *p4, tmp[64];
    
    p = ipmc_config;
    while((p4=strchr(p, ';')) != NULL)
    {    
        ipmc_entry->vlanid = atoi(p);      
        p1 = strchr(p, ',');
        p1++;
        ipmc_entry->version = atoi(p1);
        p2 = strchr(p1, ',');      
        p2++;
        ipmc_entry->query = atoi(p2);      
        p3 = strchr(p2, ',');      
        p3++;
        ipmc_entry->timeout = atoi(p3);      
        p = p4 + 1;
        ipmc_entry++;
    }
    return 0;
}



int bcm_mcast_join(int skfd, uint64 * mcMacAddr, int srcPort)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = mcMacAddr;
    data[1] = srcPort;
 
	if (ioctl(skfd, MULMAC_ADD_PORT, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//    DEBUG("[%s:%d] clear mac of port %d", __FUNCTION__, __LINE__, port);
	return 0;
}

int bcm_mcast_bitmap_set(int skfd, uint64 * mcMacAddr, uint64 * maps)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = mcMacAddr;
    data[1] = maps;
 
	if (ioctl(skfd, MULMAC_ADD_PMAP, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//    DEBUG("[%s:%d] bcm_mcast_leave mac of port %d", __FUNCTION__, __LINE__, port);
	return 0;
}

int bcm_mcast_destroy(int skfd)
{
	uint32 data[8];

	if (ioctl(skfd, MULMAC_DEL_ALL, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//    DEBUG("[%s:%d] clear mac of port %d", __FUNCTION__, __LINE__, port);
	return 0;
}

int bcm_mcast_addr_del(int skfd, uint64 *mcMacAddr)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(data));
    data[0] = mcMacAddr;
    
	if (ioctl(skfd, MULMAC_DEL_ADDR, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

//GLOBAL MANAGEMENT CONFIGURATION REGISTER (PAGE 03H/ADDR 00H)
int bcm_igmp_snoop_set(int skfd, int imgp_enable)
{
	uint32 data[8];
	
	memset(data, '\0', sizeof(data));
    data[0] = imgp_enable;
    
	if (ioctl(skfd, IGMP_SNOOP_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

 