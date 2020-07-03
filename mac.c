#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include "bcmutils.h"

int port2phy(int pmap)
{
    int i, bitmaps = 0;

    for(i = 0; i < 32; i++)
    {
        if((pmap & (1<<i)) && (phy[i] >= 0))
           bitmaps |= 1<<(phy[i]);
    }
    
    return bitmaps;
}

uint64_t str2mac(char *name)
{
	int len;
	uint64_t val = 0;
	
	for(len = 12; len > 0; name++, len--)
	{
		switch(*name)
		{
			case '0'...'9':
				val = val*16 + (*name - '0');
				break;
			case 'a'...'f':
				val = val*16 + (*name - 'a') + 10;
				break;
			case 'A'...'F':
				val = val*16 + (*name - 'A') + 10;
				break;
			default:
				return val;
		}
	}
	return val;
}

uint64_t str2mac_blackhole(char *name)
{
	int len;
	uint64_t val = 0;
	
	for(len = 18; len > 0; name++, len--)
	{
		switch(*name)
		{
			case '0'...'9':
				val = val*16 + (*name - '0');
				break;
			case 'a'...'f':
				val = val*16 + (*name - 'a') + 10;
				break;
			case 'A'...'F':
				val = val*16 + (*name - 'A') + 10;
				break;
			case ':':
				break;
			case '-':
				break;
			default:
				return val;
		}
	}
	return val;
}

int bcm_l2_learn_limit_by_port(int port, int max ,int enflag)
{
	uint32 data[8] = {0};
	int skfd;
  
  	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
	  return -1;  
		  
	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = 0;//uint
    data[1] = port;
    data[2] = max;
    data[3] = enflag;
    
	if (ioctl(skfd, MAC_LIMIT, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
	}
	
	close(skfd);
	
	return 0;
}

//get mac information and print the mac table
int cli_get_l2_mac_list_new(arl_table *cur_arl_table)
{
    FILE *fp;
    uint8 mac[6];
    char line[128], *p;
    int skfd, count = 0, port,vid, tgid, mtype; 

	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
		return -1;
	
    bcm_get_all_mac(skfd);
	close(skfd);
		
    fp=fopen("/var/mac","r+");
    if( fp == NULL) return 0;
    
    fseek(fp,0,SEEK_SET);
    memset(line, '\0', 128); 
    
    while(fgets(line,128,fp)!=NULL)
    {
        p = line;       
        sscanf(p,"%02x-%02x", &mac[0], &mac[1]);
        p += 6;       
        sscanf(p,"%02x-%02x", &mac[2], &mac[3]);     
        p += 6;       
        sscanf(p,"%02x-%02x", &mac[4], &mac[5]);       
        p += 6;    
        sscanf(p, "%d %d %d %d", &vid, &port, &tgid, &mtype);    
        
        DEBUG("[%s:%d] mac %02x-%02x-%02x-%02x-%02x-%02x vid %d,  port %d, tgid %d, method %d",__FUNCTION__,__LINE__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vid, port, tgid, mtype);

	    if(phy2port[port]!=0 && phy2port[port]!= -1){
			cur_arl_table->mac_address[0] = mac[0];
			cur_arl_table->mac_address[1] = mac[1];
			cur_arl_table->mac_address[2] = mac[2];
			cur_arl_table->mac_address[3] = mac[3];
			cur_arl_table->mac_address[4] = mac[4];
			cur_arl_table->mac_address[5] = mac[5];

			cur_arl_table->port = phy2port[port];
			cur_arl_table->type = mtype;

			/* The max size of arl table is 8K */
			if(count > 8190)
				break;

			cur_arl_table++;
			count++;
		}
		memset(line, '\0', 128); 
    }
    
    fclose(fp);    		  
    unlink("/var/mac");
	return count;	
}

int bcm_get_all_mac(int skfd)
{
    uint64 pmaps = 0x00ULL;
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = 0;
    data[1] = 0;
    data[2] = &pmaps;

	if (ioctl(skfd, MAC_LIST_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_get_special_mac(int skfd, int type, int vlan, uint64_t * pmaps)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = type;
    data[1] = vlan;
    data[2] = pmaps;

	if (ioctl(skfd, MAC_LIST_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_get_multi_mac(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = 0;

	if (ioctl(skfd, MULMAC_LIST_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int cli_get_l2_mac_list(int cli_type, int cli_port, int cli_vid, uint64_t cli_mac, uint64_t cli_multi)
{
	return 0;	
}

int bcm_l2_addr_add(int skfd, int unit, int vlan, int port, uint64 *l2addr)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = vlan;	
    data[2] = port;
    data[3] = l2addr;
 
	if (ioctl(skfd, MAC_STATIC_ADD, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_l2_addr_blackhole(int skfd, int vlan, uint64 *l2addr)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = vlan;
    //data[1] = port;
    data[2] = l2addr;
    
	if (ioctl(skfd, BCM_MAC_BLACKHOLE_SET, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}

int bcm_set_age_time(int skfd, uint32 time)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = time;

	if (ioctl(skfd, AGE_TIME, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_l2_addr_del(int skfd, int unit, int vlan, uint64 *l2addr)
{
	uint32 data[8];
    
	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
		return -1;

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = vlan;
    data[2] = l2addr;
 
	if (ioctl(skfd, MAC_STATIC_DEL, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));		    
	}
	close(skfd);

	return 0;
}

int bcm_add_static_lock_mac_with_vlan(uint64 l2addr, int vlan, int port)
{
    int skfd;
	uint64 mac = l2addr;
	
	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
        return -1;	
        
    if (bcm_l2_addr_add(skfd, 0, vlan, port, &mac) < 0) {
        close(skfd);
		return -1;    
	}
	
    close(skfd);
    return 0;   
}

int bcm_del_static_lock_mac_with_vlan(uint64 l2addr, int vlan)
{
    int skfd;
	uint64 mac = l2addr;
	
	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
        return -1;	
        
    if (bcm_l2_addr_del(skfd, 0, vlan, &mac) < 0) {
        close(skfd);
		return -1;    
	}
	
    close(skfd);
    return 0;   
}

int bcm_l2_addr_delete_by_port(int skfd, int unit, int port)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = port;
 
	if (ioctl(skfd, MAC_DEL_BY_PORT, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    DEBUG("[%s:%d] clear mac of port %d", __FUNCTION__, __LINE__, port);
	return 0;
}

int bcm_l2_addr_delete_by_vlan(int skfd, int vlan)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = vlan;
 
//	if (ioctl(skfd, MAC_DEL_BY_PORT, data) < 0) {
//		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
//		return -1;    
//	}
	
//    DEBUG("[%s:%d] clear mac of port %d", __FUNCTION__, __LINE__, port);
	return 0;
}

int bcm_l2_macaddr_add(int skfd, int vlan, int port, uint64 *l2addr)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = vlan;	
    data[1] = port;
    data[2] = l2addr;
 
	if (ioctl(skfd, FAST_MAC_STATIC_ADD, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}

int bcm_l2_macaddr_del(int skfd, int vlan, uint64 *l2addr)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = vlan;
    data[1] = l2addr;
 
	if (ioctl(skfd, FAST_MAC_STATIC_DEL, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	return 0;
}


