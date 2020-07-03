#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int set_vlan_entry(int skfd, uint16 id, uint64_t fmaps, uint64_t umaps){}

//set default vlan1
int set_default_vlan(int skfd){}

int set_vlan_8021q_enable(int skfd, int enable){}

 //802.1Q DEFAULT PORT TAG REGISTER (PAGE 34H/ADDR 40H¨C75H)
int set_vlan_default_id(int skfd, int port, int val){}

//802.1Q CONTROL 2 REGISTER (PAGE 34H/ADDR 02H)
int set_MII_Frm_FwdMap_Chk(int skfd, int enable){}

//802.1Q CONTROL 3 REGISTER (PAGE 34H/ADDR 08H-0FH)
int set_Drop_Non_1Q_Frame(int skfd, uint64_t bitmaps) {}

/* Global Double Tagging Control Register (Pages: 34h, Address 90h) */
int set_vlan_qinq_enable(int skfd, int enable){}

/* TPID CFP Global Control 1 Register (Page 21h: Address 10h)*/
int set_vlan_qinq_tpid(int skfd,int port,uint16 tpid)
{
    uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    
    data[0] = phy[port];
    data[1] = tpid;
	if (ioctl(skfd, VLAN_DTAG_TPID, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
}

int get_vlan_qinq_tpid(int skfd,int port,uint16 *tpid)
{
    uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = 0;
    data[1] = phy[port];
	if (ioctl(skfd, VLAN_PVID_SET, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
}
/* ISP Portmap Selection Register (Pages: 34h, Address 98-9Fh)*/
int set_vlan_qinq_portmap(int skfd, uint64_t bmaps)
{
   
}
int set_vlan_qinq_port_mode(int skfd, int port,uint16 mode)
{

   uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = port;
    data[1] = mode;
   if (ioctl(skfd, VLAN_DTAG_MODE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	} 
   return 0;
}

//TRUSTED CUSTOMER VLAN REGISTER (PAGE 34H: ADDRESS B8H¨CBFH)
int set_trusted_cvlan_portmap_enable(int skfd, int enable){}


int bcm_port_pvid_get(int skfd, int unit, int port, int *pvid)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = phy[port];
    data[2] = pvid;

	if (ioctl(skfd, VLAN_PVID_GET, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_port_pvid_set(int skfd, int unit, int port, int pvid)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = phy[port];
    data[2] = pvid;

	if (ioctl(skfd, VLAN_PVID_SET, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}


int bcm_get_vlan_entry(int skfd, int unit, int vid, uint64_t *mem, uint64_t *untag)
{ 
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = vid;
    data[2] = mem;
    data[3] = untag;
 
	if (ioctl(skfd, VLAN_GET, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	DEBUG("[%s:%d] vid: 0x%03x mem: 0x%08x%08x untag: 0x%08x%08x", __FUNCTION__, __LINE__, vid, (uint32)((*mem) >> 32), (uint32)(*mem), (uint32)((*untag) >> 32), (uint32)((*untag)));
 
    return 0;       
}

int bcm_set_vlan_entry(int skfd, int unit, int vid, uint64_t *mem, uint64_t *untag)
{ 
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = vid;
    data[2] = mem;
    data[3] = untag;
 
	if (ioctl(skfd, VLAN_SET, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

	DEBUG("[%s:%d] vid: 0x%03x mem: 0x%08x%08x untag: 0x%08x%08x", __FUNCTION__, __LINE__, vid, (uint32)((*mem) >> 32), (uint32)(*mem), (uint32)((*untag) >> 32), (uint32)((*untag)));
 
    return 0;       
}

int bcm_del_vlan_entry(int skfd, int unit, int vid)
{ 
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = vid;
 
	if (ioctl(skfd, VLAN_DEL, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;      
}

int bcm_delete_all_vlan(int skfd, int unit)
{ 
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
 
	if (ioctl(skfd, VLAN_DEL_ALL, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;      
}

int bcm_get_all_vlan(int skfd, int type, int *count)
{ 
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = type;
    data[1] = count;
 
	if (ioctl(skfd, VLAN_GET_ALL, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;      
}
int bcm_voice_vlan(int skfd, int unit)
{ 
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
 
	if (ioctl(skfd, MAC_VOICE_VLAN, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;      
}

int bcm_mac_vlan_create(int skfd, int unit,int pri,char *mac,int vid)
{ 
	uint32 data[32];
	int i;

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = vid;
	data[1] = pri;

    for(i = 0 ; i < 6 ;i++)
    {
        data[i+2] = mac[i];
	}
	if (ioctl(skfd, MAC_VLAN_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;      
}

int bcm_mac_vlan_delete(int skfd, int unit,char *mac)
{ 
	uint32 data[8];
    int i; 
	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
	for(i = 0 ; i < 6 ;i++)
    {
        data[i+1] = mac[i];
	}
 
	if (ioctl(skfd, MAC_VLAN_DELETE, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;      
}
int bcm_get_pvid_from_entry(char *pvid_entry, int *p)
{
    int i, vid;
    char *p0 = pvid_entry;
    
    if(strlen(pvid_entry) < 2*PNUM) //defalut value
    {    
        for(i = 0; i < PNUM; i++) 
            *(p+i) = 1;
        
        return -1; //must init vlan entry    
    }        
    else
    {
        for(i = 0; i < PNUM; i++) 
        {
            if(p0 == NULL)
                vid = 1;
            else{
                vid = atoi(p0);
                if(vid == 0)
                    vid = 1;
            }

            *(p+i) = vid;

			p0 = strchr(p0, ',');
			if(p0 != NULL)
				p0++;
        }
    }
    
    return 0;
}

int bcm_get_entry_number(char *fwd_entry)
{
    int i, num = 0;
    char *p0 = fwd_entry;
    
    //if(strlen(fwd_entry) < 0) //defalut value
    //    return 0;   
      
    while(1) 
    {
        p0 = strchr(p0, ';');
        
        if(NULL == p0)
            break;
        else    
            num++;
			p0+=1;
    }

    return num;
}

int bcm_vlanfwd_entry(char *fwd_entry, VLAN_ENTRY *fwd)
{
    int i = 0;
    VLAN_ENTRY * vlan_entry = fwd;
    char *p, *p1, *p2, *p3, tmp[64];
    
    p = fwd_entry;
    while((p3=strchr(p, ';')) != NULL)
    {    
        p1 = strchr(p, ':');
        p1++;
        vlan_entry->vlanid = atoi(p1);

        p = ++p1;
        p1 = strchr(p, ':');
        p = ++p1;
        p2 = strchr(p, ':');      
        memcpy(vlan_entry->name, p1, p2-p1);
       
        p = p2;
        p1 = strchr(p, ':');
        p = ++p1;
        p2 = strchr(p, ':');
        memset(tmp, '\0', sizeof(tmp));
        memcpy(tmp, p1, p2-p1);
       
        str2bit(tmp, &(vlan_entry->forward));
        p = p2;
               
        p1 = strchr(p, ':');
        p1++;
        memset(tmp, '\0', sizeof(tmp));
        memcpy(tmp, p1, p3-p1);
        str2bit(tmp, &(vlan_entry->untag));
         
        p = p3 + 1;
        vlan_entry++;   
    }
    return 0;
}
 
int str2vlan_info(char *input, int *value, int *mmin, int *mmax)
{
    char port[128][256], *p, *p1 = input;
    int i = 0, j, k, min, max, tmin = 0, tmax = 0;

    memset(port, '\0', sizeof(port));
	while(1)
	{
		if((p = strchr(p1, ',')) != NULL)
		{
			memcpy(port[i], p1, p - p1);
			p1 = p+1;
			i++;
		}else
		{
			memcpy(port[i], p1, strlen(p1));
			break;
		}
	}

    for(j = 0; j <= i; j++)
    {
        min = atoi(port[j]);

        if((p = strchr(port[j], '-')) != NULL) 
        {
            p++;
            max = atoi(p);
        }else
            max = min;
            
        if(0 == tmin) 
            tmin = min;
        else if(min < tmin)
            tmin = min;
          
        if(0 == tmax) 
            tmax = max;
        else if(tmax < max)
            tmax = max;  
                               
        for(k = min; k <= max; k++)
        {  
            *(value+k) = 1;   
        } 
    }
    
    *mmin = tmin;
    *mmax = tmax;
   
    return 0;
}

int find_vlan_intf_exit(int vid)
{
    FILE * fp;
    char line[128];
    int vlan_id, found = 0; 
    
    if((fp=fopen("/proc/net/vlan/config","r")) != NULL)
    { 
        memset(line, '\0', sizeof(line));
    	while(fgets(line, sizeof(line), fp)!=NULL)
    	{		
    	    if(line[4] != '.')
    	        continue;
    	        
            vlan_id = atoi(line+5);				
            if(vid == vlan_id)
            {    
                found = 1;
                break;
            }    
    	}
    	fclose(fp);   
    } 
    
    return found;
}

int bcm_port_tpid_set(int skfd, int port, int tpid)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = tpid;

	if (ioctl(skfd, VLAN_DTAG_TPID, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_port_doubletag_set(int skfd, int port, int oldvid, int oldvid_high, int newid, int mode)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = oldvid;	
    data[2] = oldvid_high;
    data[3] = newid;
    data[4] = mode;

	if (ioctl(skfd, VLAN_DTAG_SET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_port_doubletag_delete_all(int skfd, int mode)
{
	uint32 data[8];
	memset(data, '\0', sizeof(data));
    data[0] = mode;

	if (ioctl(skfd, VLAN_DTAG_DELETE, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_port_qinq_mode_set(int skfd, int port, int mode)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = mode;

	if (ioctl(skfd, VLAN_DTAG_MODE, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}
int bcm_port_protocol_vlan_set(int skfd, int port, int frame,int ethertype,int vid)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = frame;
	data[2] = ethertype;
	data[3] = vid;
	if (ioctl(skfd, PROTOCOL_VLAN_ADD, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_port_protocol_vlan_delete(int skfd, int port, int frame,int ethertype)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = frame;
	data[2] = ethertype;
	if (ioctl(skfd, PROTOCOL_VLAN_DELETE, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_dtag_mode_dump(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
	if (ioctl(skfd, DTAG_DUMP_MODE, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_dtag_port_dump(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
	if (ioctl(skfd, DTAG_DUMP_PORT, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_dtag_entry_dump(int skfd)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
	if (ioctl(skfd, DTAG_DUMP_ENTRY, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int vlan_port_get(unsigned char *addr)
{
	int portid;
	char *p, *p1, *p2, *p3, *p4;
    char *pvid = nvram_safe_get_mirror("pvid_config");
    char *link_type = nvram_safe_get_mirror("vlan_link_type");
    char *allow = nvram_safe_get_mirror("trunk_vlan_allowed");
    char *untag = nvram_safe_get_mirror("trunk_vlan_untagged");

	PORT_CONF_ENTRY *c_port = NULL;
	c_port = (PORT_CONF_ENTRY *)addr;
    
	p = pvid;
	p1 = allow;
	p2 = untag;

	for(portid = 1; portid<=PNUM; portid++) 
	{
		c_port->mode = *(link_type+portid-1)-'0';
		c_port->pvid = atoi(p);
		p = strchr(p, ',');
		p++;
		p3 = strchr(p1, ':');
		p3++;
		p1 = strchr(p1, ';');
		memcpy(c_port->allow, p3, p1-p3);
		p1++;
		p3 = strchr(p2, ':');
		p3++;
		p2 = strchr(p2, ';');
		memcpy(c_port->untag, p3, p2-p3);
		p2++;
		
		c_port++;
	}
			
	free(link_type);
	free(pvid);
	free(allow);
	free(untag);
	
	return 0;
}

int bcm_get_no_PNUM_entry_number(char *fwd_entry)
{
    int i, num = 0;
    char *p0 = fwd_entry;
    
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

int bcm_port_based_vlan_set(int skfd,uint32 port_based)
{
	if (ioctl(skfd, PORT_BASED_VLAN_SET, &port_based) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

    return 0;      
}

int bcm_port_based_vlan_del(int skfd)
{
	if (ioctl(skfd, PORT_BASED_VLAN_DEL) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

    return 0;      
}

int bcm_port_based_vlan_get(char *voice_config, PORT_BASED_V *p){
	char *ps = NULL;
	char *pt = NULL;
	char *pc = NULL;
	char *pe = NULL;
	char *pd = NULL;
	char *pk = NULL;
	
	ps = voice_config;
	while(((pt = strchr(ps,';')) != NULL) && ((pc = strchr(ps,':')) != NULL)){
		if(pc > pt)
			break;
		////printf("\n%d ps %s\n",__LINE__,ps);
		if((atoi(ps) != 0) && (atoi(ps) < PNUM)){
			////printf("line:%d %d \n",__LINE__,atoi(ps));
			p->pbmap |= (0x1U << atoi(ps));
			////printf("line:%d p->pbmap:%x\n",__LINE__,p->pbmap);
		}
		
		pk = ps;
		while((pe = strchr(pk,',')) != NULL){
			//printf("line %d pe %p   pc %p\n",__LINE__,pe,pc);
			
			if(++pe > pc)
				break;
			
			if((atoi(pe) != 0) && (atoi(pe) < 28)){
				//printf("line:%d %d \n",__LINE__,atoi(pe));
				p->pbmap |= (0x1U << atoi(pe));
				//printf("line:%d p->pbmap:0x%x\n",__LINE__,p->pbmap);
			}
			if((pd = strchr(pe,',')) != NULL)
				pk += (pd - pe)+1;
			else
				pk++;
		}
		
		if((pc = strchr(ps,':') ) != NULL){
			//printf("line %d \n",__LINE__);
			pc++;
			memcpy(p->desc,pc,((pt - pc) < 31)? (pt - pc):31);
			p->desc[31] = '\0';
		}else{
			return -1;
		}
		
		//printf("line:%d  p->pbmap:%x\n",__LINE__,p->pbmap);
		//printf("line:%d  p->desc:%s\n\n",__LINE__,p->desc);
		ps = pt + 1;
		p++;
	}

	if(ps == voice_config)//如果相等，则说明没有分号
		return -1;
	
	return 0;
}


