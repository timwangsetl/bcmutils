#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

#if BCM_DEBUG
void DEBUG(char *format,...){
 	va_list args;
    FILE *fp;

    fp = fopen("/var/bcmuitls","a+");
    if(!fp){
        return;
    }
    va_start(args,format);
    vfprintf(fp,format,args);
    va_end(args);
    fprintf(fp,"\n");
    fflush(fp);
    fclose(fp);
}
#else
#include <stdarg.h>
void DEBUG(char *format,...){}
#endif


int SYSTEM(const char *format, ...)
{
#if 0
    FILE *pf,*pft;
#endif
    static char buf[4096]="";
    va_list arg;

    va_start(arg, format);
    vsnprintf(buf,4096, format, arg);
    va_end(arg);
#if 0
    pf = fopen(SHELL_LOG,"a");
    if ( pf )
    {
        pft = fopen("/proc/uptime","r");
	if ( pft )
        {
#define _BUF_TM_LEN 100
	    static char _buf_tm[_BUF_TM_LEN];
	    fgets(_buf_tm,_BUF_TM_LEN,pft);
	    fputs(_buf_tm,pf);
	    fclose(pft);
	}
        fputs(buf,pf);
        fputc('\n',pf);
        fclose(pf);
    }
#endif
#ifdef  DEBUG
    printf("%s\n",buf);
#endif    
    system(buf);
    usleep(1);
    return 0;
}

#define BUFSZ PIPE_BUF
void myPipe(char *command, char **output)
{
    FILE *fp;
    char buf[BUFSZ];
    int len;
  
    *output=malloc(1);
    strcpy(*output, "");
    if((fp=popen(command, "r"))==NULL)
        return;
    while((fgets(buf, BUFSZ, fp)) != NULL){
        len=strlen(*output)+strlen(buf);	  
        if((*output=realloc(*output, (sizeof(char) * (len+1))))==NULL)
            return;
        strcat(*output, buf);
    }
    pclose(fp);
}

int TRAPLOG(const char *format, ...)
{
    static char buf[256]="", trap[512];
    va_list arg;
    char * snmp = nvram_safe_get("snmp_enable");
    char * snmp_rcomm = nvram_safe_get("snmp_rcomm");
//    char * snmp_rwcomm = nvram_safe_get("snmp_rwcomm");
    char * snmp_gateway = nvram_safe_get("snmp_gateway");
printf("strlen(snmp_rcomm) %d\n",strlen(snmp_rcomm));
printf("strlen(snmp_gateway) %d\n",strlen(snmp_gateway));
    if(('1' != * snmp) || (0 == strlen(snmp_gateway)) || (0 == strlen(snmp_rcomm))){
		if(snmp)free(snmp);
		if(snmp_rcomm)free(snmp_rcomm);
		if(snmp_gateway)free(snmp_gateway);
        return -1;
	}
    va_start(arg, format);
    vsnprintf(buf, 256, format, arg);
    va_end(arg);
    
    memset("trap", '\0', sizeof(trap));    
    sprintf(trap, "/usr/sbin/snmptrap -v 1 -c %s %s %s", snmp_rcomm, snmp_gateway, buf);  
    printf("%s\n",trap);  
    system(trap);

	if(snmp)free(snmp);
	if(snmp_rcomm)free(snmp_rcomm);
	if(snmp_gateway)free(snmp_gateway);

	return 0;
}

int spi_rreg(int skfd, int page, int reg, uint8 *value, int size, int len){}

int spi_wreg(int skfd, int page, int reg, uint8 *value, int size, int SC){}

int input_num(char *input, int force_hex, int size, uint8 *value)
{
    uint64_t val = 0x00ULL;
    int num = 10, out = 0;
    char *name = input;

    if(*(name+1) == 'x')
    {
        num = 16;
		name += 2;  
    }else if(1 == force_hex)
        num = 16;
    else
        num = 10;

    for(;*name != '\0';name++)
	{
	    if(1 == out)
	        break;
	        
		switch(*name)
		{
			case '0'...'9':
				val = val*num + (*name - '0');
				break;
				
			case 'a'...'f':
			    if(10 == num)
			    {   
			        out = 1; 
                    break;
			    }
			        
				val = val*num + (*name - 'a') + 10ULL;
				break;
				
			case 'A'...'F':	
			    if(10 == num)		    
			    {   
			        out = 1; 
                    break;
			    }
			        
				val = val*num + (*name - 'A') + 10ULL;
				break;
				
			case '-':	
			    continue;
				
			default:
			    out = 1;
			    break;
		}
	}    

    DEBUG("[%s:%d] val:0x%08x%08x\n", __FUNCTION__, __LINE__, (uint32)(val >> 32), (uint32)val);
    	
	if(1 == size)
	    * ((uint8 *)value) = (uint8) val;
	else if(2 == size)
	    * ((uint16 *)value) = (uint16) val;
	else if(4 == size)
	    * ((uint32 *)value) = (uint32) val;
	else if(8 == size)
	    * ((uint64 *)value) = (uint64) val;
	else
	{
	    printf("Unknown inout size, pls check\n");
	    return -1;
	}    
	    
    return 0;
}


int parse_input(char *input, char keywd, int size, uint8 *value)
{
    uint64_t val = 0x00ULL;
    int i = 0;
    char *name = input;

    for(;*name != '\0';name++, i++)
	{
        if(*name == keywd)
            val |= (0x01ULL<<phy[i]); 
	}    

    DEBUG("[%s:%d] val:0x%08x%08x\n", __FUNCTION__, __LINE__, (uint32)(val >> 32), (uint32)val);
	
	if(1 == size)
	    * ((uint8 *)value) = (uint8) val;
	else if(2 == size)
	    * ((uint16 *)value) = (uint16) val;
	else if(4 == size)
	    * ((uint32 *)value) = (uint32) val;
	else if(8 == size)
	    * ((uint64 *)value) = (uint64) val;
	else
	{
	    printf("Unknown inout size, pls check\n");
	    return -1;
	}    
	    
    return 0;
}


char * bit2str(uint64_t pmaps)
{
	char *portstr;
	int i, count = 0, tcount = 0, lastis = 0;
	//int number = 64;
	uint64_t maps = pmaps;

	portstr = malloc(256);
	if(NULL == portstr)
		return NULL;

	memset(portstr, '\0', 256);
    for(i = 0; i < PNUM; i++)
    {
        if(maps & (0x01ULL << phy[i+1]))
        {
            if(0 == lastis)
				sprintf(portstr,"%s%s%d", portstr, ((0 == count)?"":","), (i+1));
            else if(i == (PNUM-1))
			{
                sprintf(portstr,"%s%s%d", portstr, "-", (i+1)); 
				break;
			}
            lastis = 1;
            tcount++;
            count++; 
        }else
        {
            if((1 == lastis)&&(tcount > 1))
				sprintf(portstr,"%s%s%d", portstr, "-", i);
            lastis = 0;
            tcount = 0;
        }
    }

    return portstr;
}


char * bit2vlan(uint64_t pmaps)
{
	char *portstr;
	int i, count = 0, tcount = 0, lastis = 0;
	//int number = 64;
	uint64_t maps = pmaps;
	
	portstr = malloc(256);
	if(NULL == portstr)
		return NULL;

	memset(portstr, '\0', 256);
    for(i = 0; i < PNUM; i++)
    {
        if(maps & (0x01ULL << (i+1)))
        {
            if(0 == lastis)
				sprintf(portstr,"%s%s%d", portstr, ((0 == count)?"":","), (i+1));
            else if(i == (PNUM-1))
			{
                sprintf(portstr,"%s%s%d", portstr, "-", (i+1)); 
				break;
			}
            lastis = 1;
            tcount++;
            count++; 
        }else
        {
            if((1 == lastis)&&(tcount > 1))
				sprintf(portstr,"%s%s%d", portstr, "-", i);
            lastis = 0;
            tcount = 0;
        }
    }

    return portstr;
}

int str2bit(char *input, uint64_t *value)
{
    uint64_t val = 0x00ULL;
    char port[PNUM][8], *p, *p1 = input;
    int i = 0, j, k, min, max;

    memset(&port, '\0', PNUM*8);

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
                    
        for(k = min; k <= max; k++)
        {  
            if(k > 0)
                val |= 0x01ULL<<k;  //phy[0]=48 is IMP port  
        } 
    }
   
	* ((uint64 *)value) = (uint64) val;
	
    return 0;
}

int port2phy_64(uint64_t bmaps, uint64_t * nmaps)
{
	uint64_t value = 0x00ULL;
	int i;

	for(i = 0; i < PNUM; i++)
	{
		if(bmaps & (0x01ULL << i))
			value |= (0x01ULL << phy[i+1]);
	}	

	* ((uint64 *)nmaps) = (uint64) value;    
	return 0;
}

int str2phy_64(char * bmaps, uint64_t * nmaps)
{
	uint64_t value = 0x00ULL;
	int i, count = 0, tcount = 0, lastis = 0;
	
    for(i = 0; i < PNUM; i++)
    {
        if(*(bmaps+i) == '1')
            value |= (0x01ULL << phy[i+1]);
    }
    
	* ((uint64 *)nmaps) = (uint64) value;
    return 0;
}

int phy2port_64(uint64_t bmaps, uint64_t * nmaps)
{
	uint64_t value = 0X00ULL;
	int i;

    for(i = 1; i <= PNUM; i++)
    {
        if(bmaps & (0x01ULL << phy[i]))
            value |= (0x01ULL << i);
    }

	* ((uint64 *)nmaps) = (uint64) value;
    return 0;
}

int pmap2port(uint64_t bmaps, int * nmaps)
{
	int i;

    for(i = 1; i <= PNUM; i++){
        if(bmaps & (0x01ULL << i))
            return *nmaps = i;
    }

    return -1;
}


/*
 *  Function : cli_str2mac
 *  Purpose:
 *     convert mac address string to 16 hex
 *  Parameters:
 *     name  - mac address string
 *     val   - return value with 16 hex
 *  Returns:
 *
 *  Author  : xichen
 *  Date    :2011/2/14 (Valentine's Day ^_^)
 */
void cli_str2mac(char *name, unsigned char *val)
{
	int len;
	
	for(len = 6; len > 0; name += 2, len--, val++)
	{
		switch(*name)
		{
			case '0'...'9':
				*val = (*name - '0')*16;
				break;
			case 'a'...'f':
				*val = ((*name - 'a') + 10)*16;
				break;
			case 'A'...'F':
				*val = ((*name - 'A') + 10)*16;
				break;
			default:
				break;
		}
		switch(*(name+1))
		{
			case '0'...'9':
				*val = *val + (*(name+1) - '0');
				break;
			case 'a'...'f':
				*val = *val + (*(name+1) - 'a') + 10;
				break;
			case 'A'...'F':
				*val = *val + (*(name+1) - 'A') + 10;
				break;
			default:
				break;
		}
	}

	return;
}

int str2vlan(char *input, int *value, int *total)
{
    char port[1024][16], *p, *p1 = input;
    int i = 0, j, k, min, max;

    memset(port, '\0', 1024*16);

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
                    
        for(k = min; k <= max; k++)
        {  
            *(value+k) = 1; 
            *(total+k) = 1;    
        } 
    }
   
    return 0;
}

int cli_str2bitmap(char *str, uint64_t *value)
{
	char port_num[256] = {'\0'};
	char *comma = NULL, *hyphen = NULL, *prompt = str;
	uint64_t bmap = 0x0ULL, t_bmap = 0x0ULL;
	char *flag = NULL;
	char *comman = NULL;
	
	if(prompt == NULL)
		return -1;
	
	comma = hyphen = prompt;
	while(prompt != NULL)
	{
		t_bmap = 0x0ULL;
		memset(port_num, '\0', sizeof(port_num));
		if((flag = strpbrk(prompt, "fgptFGPT")) != NULL)
		{
			if((comma = strchr(prompt, ',')) != NULL)
			{
				memcpy(port_num, prompt, (comma-prompt));
			
				if(strncasecmp(port_num, "f0/", 3) == 0)
				{
					/* f0/P,N-M */
					str2bit(port_num+3, &t_bmap);
				}
				else if(strncasecmp(port_num, "g0/", 3) == 0)
				{
					/* g0/P,N-M */
#if (XPORT==1)
					str2bit(port_num+3, &t_bmap);
DEBUG("[%s:%d] port_str %s port_int:0x%08x%08x\n", __FUNCTION__, __LINE__, port_num, (uint32)(t_bmap >> 32), (uint32)t_bmap);	
#else					
					str2bit(port_num+3, &t_bmap);
					t_bmap <<= (phy[FNUM+1]-phy[1]); 
DEBUG("[%s:%d] port_str %s port_int:0x%08x%08x\n", __FUNCTION__, __LINE__, port_num, (uint32)(t_bmap >> 32), (uint32)t_bmap);	
#endif					
				}
				else if(strncasecmp(port_num, "t0/", 3) == 0)
				{
					str2bit(port_num+3, &t_bmap);
					t_bmap <<= phy[GNUM]; 
				}
				else
					printf("Unknow port type: %s!!\n", port_num);
			
				prompt = comma + 1;
			}
			else
			{
				memcpy(port_num, prompt, strlen(prompt));
				if(strncasecmp(port_num, "f0/", 3) == 0)
				{
					/* f0/P,N-M */
					str2bit(port_num+3, &t_bmap);
				}
				else if(strncasecmp(port_num, "g0/", 3) == 0)
				{
					/* g0/P,N-M */
#if (XPORT==1)
					str2bit(port_num+3, &t_bmap);
DEBUG("[%s:%d] port_str %s port_int:0x%08x%08x\n", __FUNCTION__, __LINE__, port_num, (uint32)(t_bmap >> 32), (uint32)t_bmap);	
#else					
					str2bit(port_num+3, &t_bmap);
DEBUG("[%s:%d] port_str %s port_int:0x%08x%08x\n", __FUNCTION__, __LINE__, port_num, (uint32)(t_bmap >> 32), (uint32)t_bmap);	
					t_bmap <<= (phy[FNUM+1]-phy[1]); 
#endif					
				}
				else if(strncasecmp(port_num, "t0/", 3) == 0)
				{
					str2bit(port_num+3, &t_bmap);
					t_bmap <<= phy[GNUM]; 
				}
				else if(strncasecmp(port_num, "p", 1) == 0)
				{
					/* pn */
					str2bit(port_num+1, &t_bmap);
				}
				else
					printf("Unknow port type: %s!!\n", port_num);
			
				prompt = NULL;
			}
		}
		else
		{
			memcpy(port_num, prompt, strlen(prompt));
			if((comman = strpbrk(port_num, ",-")) != NULL)
			{
				if(strpbrk(port_num,"fgptFGPT"))
				{
					str2bit(port_num, &t_bmap);
					prompt = comman + 1;
				}
				else
				{
					str2bit(port_num, &t_bmap);
					prompt = NULL;
				
				}
			}
			else
			{
				str2bit(port_num, &t_bmap);
				prompt = NULL;
			}
	
		}
		
		bmap |= t_bmap;
	}

	* ((uint64_t *)value) = (uint64_t) bmap;    

	return 0;
}

//GLOBAL TRUNK CONTROL REGISTER (PAGE 32H/ADDR 00H)
int set_trunk_enable(int skfd, int enable)
{
    uint8 trunk;

    spi_rreg(skfd, 0x31, 0x00, (uint8*)&trunk, 1, 1);

    trunk &= ~0x03;
    trunk |= (enable & 0x03);

    spi_wreg(skfd, 0x31, 0x00, (uint8*)&trunk, 1, 1);
    
	DEBUG("[%s:%d] set trunking setting: type: 0x%02x", __FUNCTION__, __LINE__, (enable & 0x03));
    return 0;
}

//GLOBAL TRUNK CONTROL REGISTER (PAGE 32H/ADDR 00H)
int set_trunking_type(int skfd, int entry, int config, int enable)
{
    int i;
    uint64 trunking = 0;

    if((entry >= 0) && (entry < 2))
    {   
        if(1 == enable)
        {
            for(i = 0; ((i < 64) && (phy[i]>=0)); i++)
            {
                if((config & (1<<i))&&(phy[i]>0))
                    trunking |= (1<<phy[i]);
            }
            trunking |= (1<<27);
        } 

        spi_wreg(skfd, 0x31, 0x10+entry*8, (uint8*)&trunking, 8, 8);
        
    	DEBUG("[%s:%d] set trunking config: entry: 0x%02x, config: 0x%02x, enable: 0x%02x", __FUNCTION__, __LINE__, entry, config, enable);   
    }
    return 0;
}


//PROTECTED PORT SELECT REGISTER (PAGE 00H/ADDR 30H-33H)
int set_protected_port(int skfd, uint64_t bmaps)
{
#if 1
    uint64_t protect = bmaps;            
    spi_wreg(skfd, 0x00, 0x40, (uint8*)&protect, 8, 8);
    
	DEBUG("[%s:%d] set protect portmaps: 0x%08x%08x", __FUNCTION__, __LINE__, (uint32)(protect>>32), (uint32)(protect));
#else
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = bmaps;

	if (ioctl(skfd, PORT_PROTECTED, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

#endif	
    return 0;
}
int pvlan_port_type_set(int skfd, int mode,int portType,int vlan,uint64_t bmaps)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = vlan;
	data[1] = mode;
	data[2] = bmaps;
	data[3] = portType;

	if (ioctl(skfd, PVLAN_PORT_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int bcm_port_egress_set(int skfd, int port,uint32 portmap)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
	data[1] = portmap;

	if (ioctl(skfd, BCM_PORT_EGRESS_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int bcm_port_egress_get(int skfd, int port,uint32 *portmap)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
	data[1] = portmap;

	if (ioctl(skfd, BCM_PORT_EGRESS_GET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int bcm_port_ingress_filter_set(int skfd, int port,int enable)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
	data[1] = enable;

	if (ioctl(skfd, BCM_PORT_INGRESS_FILTER_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int vlan_translate_set(int skfd, int port,int oldvlan,int newvlan)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
	data[1] = oldvlan;
	data[2] = newvlan;

	if (ioctl(skfd, VLAN_TRANLATE_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int vlan_control_set(int skfd, int type,int arg)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = type;
	data[1] = arg;
	
	if (ioctl(skfd, VLAN_CONTROL_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}
int vlan_control_get(int skfd, int type,int *arg)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
	data[0] = type;
	data[1] = arg;


	if (ioctl(skfd, VLAN_CONTROL_GET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}

int vlan_control_vlan_set(int skfd, int vid,int arg)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = vid;
	data[1] = arg;
	
	if (ioctl(skfd, VLAN_CONTROL_VLAN_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}
int vlan_control_vlan_get(int skfd, int vid,int *arg)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
	data[0] = vid;
	data[1] = arg;


	if (ioctl(skfd, VLAN_CONTROL_VLAN_GET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}


int bcm_register_read(int skfd, uint32 reg,uint64 *value)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = reg;
	data[1] = value;

	if (ioctl(skfd, BCM_REGISTER_READ, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}
int bcm_register_write(int skfd, uint32 reg,uint64 value)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = reg;
	data[1] = value>>32;
	data[2] = value & 0xffffffff;

	if (ioctl(skfd, BCM_REGISTER_WRITE, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}

    return 0;
}


//NEW CONTROL REGISTER (PAGE 00H/ADDR 03H)
//MULTICAST LOOKUP FAIL FORWARD MAP REGISTER (PAGE 00H/ADDR 2CH-2FH)
int set_multicast_lookup_fail(int skfd, int imgp_enable, int pmap)
{
    int i;
    uint8 new_control;
	uint32 portmaps = 0;
	
	if(0 == imgp_enable)
	{
	    spi_rreg(skfd, 0x00, 0x03, (uint8 *)&new_control, 1, 1);
	    new_control &= (~0x80);
	    spi_wreg(skfd, 0x00, 0x03, (uint8 *)&new_control, 1, 1);
	    
	    portmaps = 0;
	    spi_wreg(skfd, 0x00, 0x2c, (char*)&portmaps, 4, 4); 
	}else
	{
	    spi_rreg(skfd, 0x00, 0x03, (uint8 *)&new_control, 1, 1);
	    new_control |= 0x80;
	    spi_wreg(skfd, 0x00, 0x03, (uint8 *)&new_control, 1, 1);        

      	/* CHIP 0 PORT-BASED AGING CONTROL REGISTER (PAGE 43H/ADDR 08H-0BH) */ 	
      	for(i = 0; ((i < 32) && (phy[i]>=0)); i++)
        {     
            if(pmap & (1<<i))
                portmaps |= (1<<phy[i]);
      	}
  	
	    spi_wreg(skfd, 0x00, 0x2c, (char*)&portmaps, 4, 4); 	    
	}	

    DEBUG("[%s:%d] new_control: 0x%02x portmaps: 0x%08x", __FUNCTION__, __LINE__, new_control, portmaps);
    return 0;
}

//GLOBAL MANAGEMENT CONFIGURATION REGISTER (PAGE 03H/ADDR 00H)
int set_igmp_snoop_enable(int skfd, int imgp_enable)
{
    uint8 value;
	
	spi_rreg(skfd, 0x03, 0x00, &value, 1, 1);

    if(1 == imgp_enable)
        value |= (3<<3); //Enable IGMP IP layer Snooping
    else
        value &= ~(3<<3);    
	
	spi_wreg(skfd, 0x03, 0x00, &value, 1, 1);	

    DEBUG("[%s:%d] GLOBAL Setting: 0x%02x, snoop: %s", __FUNCTION__, __LINE__, value, (1 == imgp_enable)?"Enable":"Disable");
    return 0;
}

int set_port_auth_dynamic_mode_in_arl(int skfd, int port, int enable)
{    
    int i = port, en = enable;
    uint8 val = 0;
    uint16 val_tmp = 0x0;
    if(phy[i] < 0)
        return -1;
    
    spi_rreg(skfd, 0x40, phy[i]-24, (uint8*)&val, 1, 1); 
        
    if(1 == en)
    {
        val &= 0xF0;
        val |= 0x02;
        val_tmp = 0x0;
    }
    else {
        val &= 0xF0;
        val_tmp = 0x1fff;
    }
            
    //PORT EAP CONFIGURATION REGISTER (PAGE 40H/ADDR 00H每1AH)
    spi_wreg(skfd, 0x40, phy[i]-24, (uint8*)&val, 1, 1);
    
    //added by xichen for 802.1x max limit learn num to 0	
    spi_wreg(skfd, 0x41, 0x20+2*phy[port], (uint8*)&val_tmp, 2, 2);
    	
    DEBUG("[%s:%d] Set Port %d Author %s", __FUNCTION__, __LINE__, port, (1 == enable)?"Enable":"Disable");
    return 0;
}

int pmap_port2phy(int bits)
{
    int i, portmaps = 0;
    
    for(i = 0; ((i < 32) && (phy[i]>=0)); i++)
    {     
        if(bits & (1<<i))
            portmaps |= (1<<phy[i]);
    }
  	
  	return 	portmaps;
}

/*
//MAC ADDRESS SECURITY CONTROL REGISTER 3 (PAGE 41H/ADDR 10H每13H)
int set_port_rev_auth_pkts(int skfd, int port, int enable)
{
    int i = port;
    uint64_t val;
        
    //MAC ADDRESS SECURITY CONTROL REGISTER 2 (PAGE 42H/ADDR 10H-17H)
    spi_rreg(skfd, 0x42, 0x10, (char*)&val, 8, 8);
    
    if(1 == enable)
        val |= 0x1ULL<<phy[i];
    else
        val &= ~(0x1ULL<<phy[i]);
    
    spi_wreg(skfd, 0x42, 0x10, (char*)&val, 8, 8);
    
    DEBUG("[%s:%d] Set Port %d phy %d Rev Auth %s!", __FUNCTION__, __LINE__, port, phy[i], (1 == enable)?"Enable":"Disable");
    return 0;
}
*/

int set_SW_check_auth_type(int skfd, int enable)
{
    uint16 val;
    
    //EAP GLOBAL CONFIGURATION REGISTER (PAGE 40H/ADDR 10H-11H)
    spi_rreg(skfd, 0x41, 0x10, (char*)&val, 2, 2);
    
    if(1 == enable)
        val |= 0x05;
    else
        val &= ~(0x05);
    
    spi_wreg(skfd, 0x41, 0x10, (char*)&val, 2, 2);
    
    DEBUG("[%s:%d] Set Switch check Auth type %s!", __FUNCTION__, __LINE__, (1 == enable)?"Enable":"Disable");
    return 0;
}

int set_port_learn_disable(int skfd, uint64_t learn_maps)
{
    uint64_t maps = learn_maps;
    int i;
    uint8 val = 0;
    
    for(i = 1; i <= PNUM; i++)
    {
        if((maps & (0x01ULL << phy[i])) != 0x00ULL)
        {
            // PORT EAP CONFIGURATION REGISTER (PAGE 40H/ADDR 00H每1AH)
            spi_rreg(skfd, 0x40, phy[i]-24, (uint8*)&val, 1, 1);            
            val |= (0x01 << 5);            
            spi_wreg(skfd, 0x40, phy[i]-24, (uint8*)&val, 1, 1);
        }
        else
        {
            // PORT EAP CONFIGURATION REGISTER (PAGE 40H/ADDR 00H每1AH)
            spi_rreg(skfd, 0x40, phy[i]-24, (uint8*)&val, 1, 1);            
            val &= ~(0x01 << 5);            
            spi_wreg(skfd, 0x40, phy[i]-24, (uint8*)&val, 1, 1);
        }
    }
    
    DEBUG("[%s:%d] Set learn_maps 0x%08x%08x!", __FUNCTION__, __LINE__, (uint32)(maps >> 32), (uint32)maps);
    return 0;
}

/*
 * cli reboot command should reset switch chip first before system shutdown 
 * added by xichen 110301
 */
int cli_switch_reset(void)
{
    int skfd;
	uint8 value;
		
	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
		return -1;

    //CHIP RESET CONTROL REGISTER (PAGE 03H/ADDR 7CH)
    value = 0x01;     
	spi_wreg(skfd, 0x03, 0x7C, (uint8 *)&value, 1, -1);
	
	usleep(50000);
	spi_rreg(skfd, 0x03, 0x7C, (uint8 *)&value, 1, 1);
	while(value & 0x01)
	{	
    	usleep(50000);
    	spi_rreg(skfd, 0x03, 0x7C, (uint8 *)&value, 1, 1);
	}
	
	reset_external_phy(skfd);
				
	close(skfd);
    return 0;
}

/*
*Function:clear ports statistical information  Add by gujiajie 
*/
int clear_counters(int skfd)
{
	uint8 val8;
		
	val8 = 0x01;	
	spi_wreg(skfd, 0x03, 0x01, (uint8*)&val8, 1, 1);

	return 0;
}

