/******************************************************************************
*  Copyright (c) 2002 Linux UPnP Internet Gateway Device Project              *    
*  All rights reserved.                                                       *
*                                                                             *   
*  This file is part of The Linux UPnP Internet Gateway Device (IGD).         *
*                                                                             *
*  The Linux UPnP IGD is free software; you can redistribute it and/or modify *
*  it under the terms of the GNU General Public License as published by       *
*  the Free Software Foundation; either version 2 of the License, or          *
*  (at your option) any later version.                                        *
*                                                                             *    
*  The Linux UPnP IGD is distributed in the hope that it will be useful,      *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*  GNU General Public License for more details.                               *
*                                                                             *   
*  You should have received a copy of the GNU General Public License          * 
*  along with Foobar; if not, write to the Free Software                      *
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  *
*                                                                             *  
*                                                                             *  
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>		/* strcmp and friends */
#include <ctype.h>		/* isdigit and friends */
#include <stddef.h>		/* offsetof */
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#if __GLIBC__ >=2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>
#else
#include <asm/types.h>
#include <linux/if_ether.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#include "if_info.h"

unsigned long int crc_32_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

int get_sockfd()
{
	int sockfd = -1;
	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("user: socket creating failed");
		return (-1);
	}
	return sockfd;
}

/*TODO: maybe we can get ip address directly from nvram */
int getIFInfo(if_info_t *if_info)
{
	unsigned char *pt;
	struct ifreq ifr;
	struct sockaddr_in *saddr;
	int fd;
	int ret=0;
	if ((fd=get_sockfd())>=0)
	{
		strcpy(ifr.ifr_name, if_info->ifname);
		ifr.ifr_addr.sa_family = AF_INET;
		/* get ip address */
		if (ioctl(fd, SIOCGIFADDR, &ifr)==0){
			saddr = (struct sockaddr_in *)&ifr.ifr_addr;
			if_info->hexip = saddr->sin_addr.s_addr;
			strcpy(if_info->ipaddr,(char *)inet_ntoa(saddr->sin_addr));
			/* for hide on demand ip */
			if(strcmp(if_info->ipaddr,"10.64.64.64")==0)
				ret=-2;			
		}else
			ret=-1;
		/* get mac address */
		if (ioctl(fd, SIOCGIFHWADDR, &ifr)==0){
			pt=ifr.ifr_hwaddr.sa_data;
            memcpy(if_info->hexmac, ifr.ifr_hwaddr.sa_data, 6);
			sprintf(if_info->mac,"%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx"
					,*pt,*(pt+1),*(pt+2),*(pt+3),*(pt+4),*(pt+5));
			if_info->mac[17]='\0';
		}else 
			ret=-1;
		/* get netmask */
		if (ioctl(fd,SIOCGIFNETMASK , &ifr)==0){
			saddr = (struct sockaddr_in *)&ifr.ifr_addr;
			if_info->hexmask = saddr->sin_addr.s_addr;
			strcpy(if_info->mask,(char *)inet_ntoa(saddr->sin_addr));
		}else
			ret=-1;

		/* get bcask */
		if (ioctl(fd,SIOCGIFBRDADDR , &ifr)==0){
			saddr = (struct sockaddr_in *)&ifr.ifr_addr;
			strcpy(if_info->bcask,(char *)inet_ntoa(saddr->sin_addr));
		}else
			ret=-1;	
					
		/* get mtu */
		if (ioctl(fd,SIOCGIFMTU, &ifr)==0){
			if_info->mtu=ifr.ifr_mtu;	
		}else
			ret=-1;	
		close(fd);
		return ret;

	}
	return -1;
}


/*TODO: maybe we can get ip address directly from nvram */
int get_interface_ip(char * intf, int vlan, uint32 *ip)
{
	unsigned char *pt;
	struct ifreq ifr;
	struct sockaddr_in *saddr;
	int fd;
	int ret=0;
	
	if ((fd=get_sockfd())>=0)
	{
		sprintf(ifr.ifr_name, "%s.%d", intf, vlan);
		ifr.ifr_addr.sa_family = AF_INET;
		/* get ip address */
		if (ioctl(fd, SIOCGIFADDR, &ifr)==0){
			saddr = (struct sockaddr_in *)&ifr.ifr_addr;
			*ip = saddr->sin_addr.s_addr;
		}else
			ret=-1;
			
		close(fd);
		return ret;
	}
	
	return -1;
}

int find_exist_interface(char * intf, int vlan)
{
    char key[8], ipstr[8192];
    char *l3_ipv6 = nvram_safe_get(NVRAM_STR_L3_IPV6);
    char *l3_ip = nvram_safe_get("lan_ipaddr");
	
    memset(key, '\0', sizeof(key));
    sprintf(key, ";%d,", vlan);
    
    memset(ipstr, '\0', sizeof(ipstr));
    strcpy(ipstr, ";");
	strcat(ipstr, l3_ip);
    free(l3_ip);
	
	if(strstr(ipstr, key) != NULL)
	{
	    free(l3_ipv6);
	    return 0;
	}
	
    memset(ipstr, '\0', sizeof(ipstr));
    strcpy(ipstr, ";");
	strcat(ipstr, l3_ipv6);
    free(l3_ipv6);
	
	if(strstr(ipstr, key) != NULL)
	{
	    return 0;
	}
    
	return -1;
}

int set_interface_status(char * intf, int vlan, int up)
{
	unsigned char *pt;
	struct ifreq ifr;
	struct sockaddr_in *saddr;
	int skfd;
	int ret=0;
	
	if ((skfd=get_sockfd())>=0)
	{
	    if(*intf == 'e')
		    sprintf(ifr.ifr_name, "%s.%d", intf, vlan);
		else
		    sprintf(ifr.ifr_name, "%s%d", intf, vlan);
		        
		ifr.ifr_addr.sa_family = AF_INET;
	    if ( ioctl(skfd, SIOCGIFINDEX, &ifr) < 0 ) {
		    close(skfd);
			return -1;
	    }
	    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
             fprintf(stderr,"SIOCGIFFLAGS error: interface: %s\n", ifr.ifr_name);
		     close(skfd);
             return (-1);
        }
        
        if(1 == up)
            ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
        else
            ifr.ifr_flags &= ~IFF_UP;
                
        if (ioctl(skfd, SIOCSIFFLAGS, &ifr) < 0) {
             fprintf(stderr,"SIOCSIFFLAGS error: interface: %s\n", ifr.ifr_name);
		     close(skfd);
             return (-1);
        }
		close(skfd);
		return 0;
	}
	
	return -1;
}

unsigned long swap32(unsigned long i)
{
    i = (i << 16) | (i >> 16);

    return (i & 0xff00ffff) >> 8 | (i & 0xffff00ff) << 8;
}


unsigned long crc32(char *data, int length)
{
    unsigned long crc;
    
    crc = 0xFFFFFFFF;

    while( length-- > 0) {
        crc = ((crc>>8) & 0x00FFFFFF) ^ crc_32_table[ (crc^((char)*(data++))) & 0xFF ];
    }
	
	crc = crc^0xFFFFFFFF;
    return swap32(crc);
}

unsigned long mcrc32(char *data, int length)
{
	unsigned long crc, poly;
	long crcTable[256];
	int i, j;

    poly = 0x44332211UL;
    
	for (i=0; i<256; i++) {
		crc = i;
		for (j=8; j>0; j--) {
			if (crc&1) {
				crc = (crc >> 1) ^ poly;
			} else {
				crc >>= 1;
			}
		}
		crcTable[i] = crc;
	}
	crc = 0xFFFFFFFF;

	while( length-- > 0) {
		crc = ((crc>>8) & 0x00FFFFFF) ^ crcTable[ (crc^((char)*(data++))) & 0xFF ];
	}
	
	return crc^0xFFFFFFFF;
}


#define LOADER_SIZE	0x0080000

int getSWInfo(MAC_INFO *sw_info)
{
    FILE *fp;
    MAC_INFO info;
    
    memset(&info, '\0', sizeof(MAC_INFO));
    
    fp = fopen("/dev/mtdblock0", "r");
    fseek(fp, LOADER_SIZE-160, SEEK_SET); //0x80 is pid
    fread(&info, sizeof(MAC_INFO), 1, fp);
    fclose(fp);   
    
    if(info.crc != mcrc32(&info.mac, 8))
        info.mac = 0x001122334455ULL;
  	    
    memcpy(sw_info, &info, sizeof(MAC_INFO));
	       
	return 0; 
}

int inet6_parse(char *name, size_t len, struct sockaddr_in6 *sin6)
{
	if(sin6->sin6_family != AF_INET6) return (-1);
	if(inet_ntop(AF_INET6, &sin6->sin6_addr, name, len) != NULL)	return (0);
	
	return -1;
}

if_ipv6_t *get_ipv6_addr(void) 
{ 
	FILE *f;
	char addr6[40], devname[20];
	struct sockaddr_in6 sap;
	int plen, scope, dad_status, if_idx;
	char addr6p[8][5];
	
	char buff[128] = {'\0'};
	if_ipv6_t *if_ipv6_ptr = NULL;
	if_ipv6_t *if_ipv6 = NULL;

	if ((f = fopen("/proc/net/if_inet6", "r")) != NULL) {
		while (fscanf
			   (f, "%4s%4s%4s%4s%4s%4s%4s%4s %02x %02x %02x %02x %20s\n",
				addr6p[0], addr6p[1], addr6p[2], addr6p[3], addr6p[4],
				addr6p[5], addr6p[6], addr6p[7], &if_idx, &plen, &scope,
				&dad_status, devname) != EOF) {

				if((if_ipv6_ptr = (if_ipv6_t *)malloc(sizeof(if_ipv6_t)))  == NULL)
				{
					fclose(f);
					free_ipv6_addr(if_ipv6);
					return NULL;
				}
				memset(if_ipv6_ptr, '\0', sizeof(if_ipv6_t));

				sprintf(addr6, "%s:%s:%s:%s:%s:%s:%s:%s",
						addr6p[0], addr6p[1], addr6p[2], addr6p[3],
						addr6p[4], addr6p[5], addr6p[6], addr6p[7]);
				inet_pton(AF_INET6, addr6, &sap.sin6_addr);
				sap.sin6_family = AF_INET6;
				if(inet6_parse(buff, sizeof(buff), &sap) != 0)
				{
					fclose(f);
					free_ipv6_addr(if_ipv6);
					return NULL;
				}
				memcpy(if_ipv6_ptr->addr6, buff, sizeof(buff));
				memcpy(if_ipv6_ptr->devname, devname, sizeof(devname));
				if_ipv6_ptr->plen = plen;
				
				if_ipv6_ptr->scope_type = scope & IPV6_ADDR_SCOPE_MASK;
				switch (if_ipv6_ptr->scope_type) {
					case 0:
						memcpy(if_ipv6_ptr->scope, "Global", 32);
						break;
					case IPV6_ADDR_LINKLOCAL:
						memcpy(if_ipv6_ptr->scope, "LinkLocal", 32);
						break;
					case IPV6_ADDR_SITELOCAL:
						memcpy(if_ipv6_ptr->scope, "Site", 32);
						break;
					case IPV6_ADDR_COMPATv4:
						memcpy(if_ipv6_ptr->scope, "Compat", 32);
						break;
					case IPV6_ADDR_LOOPBACK:
						memcpy(if_ipv6_ptr->scope, "Host", 32);
						break;
					default:
						memcpy(if_ipv6_ptr->scope, "Unknown", 32);
						break;
				}
				
				if_ipv6_ptr->dad_status = dad_status;
				if_ipv6_ptr->if_idx = if_idx;

				if_ipv6_ptr->next = if_ipv6;
				if_ipv6 = if_ipv6_ptr;
		}
		fclose(f);
	}

	return if_ipv6;
} 

void free_ipv6_addr(if_ipv6_t *if_ipv6)
{
	if_ipv6_t *if_ip6_ptr = if_ipv6, *next_if_ipv6 = NULL;
	
	while(if_ip6_ptr != NULL)
	{
		next_if_ipv6 = if_ip6_ptr->next;
		free(if_ip6_ptr);
		if_ip6_ptr = next_if_ipv6;
	}

	return;
}


int is_in_same_subnet(char *ipaddr, char *subnet) 
{ 
    int i, rv, mask;
    struct in_addr addr;
    char *p, lanip[18];
    uint32 ip, sub, netmask = 0;

    memset(lanip, '\0', sizeof(lanip));
    if(NULL == (p = strchr(subnet, '/')))
    {    
        netmask = 0xffffff;
        strcpy(lanip, subnet);
    }    
    else
    {
        memcpy(lanip, subnet, p - subnet);
        mask = atoi(p+1);
        for(i = 0; i < mask; i++) 
            netmask |= 1<<(31-i);          
    } 
    
	ip = inet_addr(ipaddr);
	sub = inet_addr(lanip);
	rv = ((ip&ntohl(netmask)) == (sub&ntohl(netmask)))?1:0;
  
	return rv;
}

//for different ip/mask
int isin_same_subnet(char *ipaddr, char *subnet) 
{ 
    int i, rv, mask, mask1;
    struct in_addr addr;
    char *p, lanip[18], lanip1[18];
    unsigned long ip, sub, netmask = 0, netmask1 = 0;

    memset(lanip, '\0', sizeof(lanip));
    if(NULL == (p = strchr(subnet, '/')))
    {    
        netmask = 0xffffff;
        strcpy(lanip, subnet);
    }    
    else
    {
        memcpy(lanip, subnet, p - subnet);
        mask = atoi(p+1);   
        for(i = 0; i < mask; i++) 
            netmask |= 1<<(31-i);        
    } 
    
    memset(lanip1, '\0', sizeof(lanip1));
    if(NULL == (p = strchr(ipaddr, '/')))
    {    
        netmask1 = 0xffffff;
        strcpy(lanip1, ipaddr);
    }    
    else
    {
        memcpy(lanip1, ipaddr, p - ipaddr);
        mask1 = atoi(p+1);
        for(i = 0; i < mask1; i++) 
            netmask1 |= 1<<(31-i);         
    } 
    
    if(netmask != netmask1)
        return 0;
        
	ip = inet_addr(lanip);
	sub = inet_addr(lanip1);
	rv = ((ip&ntohl(netmask)) == (sub&ntohl(netmask)))?1:0;
  
	return rv;
}

char *get_broadcast_addr(char *lan_ipaddr) 
{ 
    int i, mask;
    char lanip[18];
    char *p, * lan_bipaddr;
    unsigned long int ipaddr, netmask = 0, bipaddr;
    struct in_addr addr;

    memset(lanip, '\0', 18);
    if(NULL == (p = strchr(lan_ipaddr, '/')))
    {    
        mask = 24;
        strcpy(lanip, lan_ipaddr);
    }    
    else
    {
        memcpy(lanip, lan_ipaddr, p - lan_ipaddr);
        mask = atoi(p+1);
    }
    
    for(i = 0; i < mask; i++) 
        netmask |= 1<<(31-i);    
   
    
	ipaddr = inet_addr(lanip);
	bipaddr = (ipaddr & ntohl(netmask)) | ntohl(~netmask);
	addr.s_addr = bipaddr;
	lan_bipaddr = inet_ntoa(addr);

	return lan_bipaddr;
} 

char *get_netmask_str(char *lan_ipaddr) 
{ 
    int i, mask;
    char lanip[18];
    char *p, * lan_bipaddr;
    unsigned long int ipaddr, netmask = 0, bipaddr;
    struct in_addr addr;

    memset(lanip, '\0', 18);
    if(NULL == (p = strchr(lan_ipaddr, '/')))
    {    
        netmask = 0xffffff;
        strcpy(lanip, lan_ipaddr);
    }    
    else
    {
        memcpy(lanip, lan_ipaddr, p - lan_ipaddr);
        mask = atoi(p+1);
   
        for(i = 0; i < mask; i++) 
            netmask |= 1<<(31-i);    
    }   
    
	addr.s_addr = ntohl(netmask);
	lan_bipaddr = inet_ntoa(addr);

	return lan_bipaddr;
} 

char *get_submask_addr(char *lan_ipaddr) 
{ 
    int i, mask;
    char lanip[24];
    char *p, * lan_subnet;
    unsigned long int ipaddr, netmask = 0, subnet;
    struct in_addr addr;

    memset(lanip, '\0', 18);
    if(NULL == (p = strchr(lan_ipaddr, '/')))
    {    
        netmask = 0xffffff;
        strcpy(lanip, lan_ipaddr);
    }    
    else
    {
        memcpy(lanip, lan_ipaddr, p - lan_ipaddr);
        mask = atoi(p+1);
        for(i = 0; i < mask; i++) 
            netmask |= 1<<(31-i);        
    }

	ipaddr = inet_addr(lanip);
	subnet = ipaddr & ntohl(netmask);
    addr.s_addr = subnet;
	lan_subnet = inet_ntoa(addr);		
 
	return lan_subnet;
} 

int get_mask_addr(char *lan_ipaddr) 
{ 
    int mask;
    char *p;

    if(NULL == (p = strchr(lan_ipaddr, '/')))
    {    
        mask = 24;
    }    
    else
    { 
        mask = atoi(p+1);     
    }
    
	return mask;
} 

uint32 get_netmask_addr(char *lan_ipaddr) 
{ 
    int i, mask;
    char *p;
    uint32 netmask = 0;

    if(NULL == (p = strchr(lan_ipaddr, '/')))
    {    
        netmask = 0xffffff;
    }    
    else
    {
        mask = atoi(p+1);
        for(i = 0; i < mask; i++) 
            netmask |= 1<<(31-i);        
    }	
 
	return netmask;
} 

uint32 get_ipv4_addr(char *ipaddr) 
{ 
    struct in_addr addr;
    char *p, lanip[18];
    uint32 ip;

    memset(lanip, '\0', sizeof(lanip));
    if(NULL == (p = strchr(ipaddr, '/')))
    {    
        strcpy(lanip, ipaddr);
    }    
    else
    {
        memcpy(lanip, ipaddr, p - ipaddr);
    } 
    
	ip = inet_addr(lanip);
	return ip;
}

int get_mask_subnet(char *lan_subnet) 
{ 
    int i, mask = 0;
    unsigned long int netmask;

    netmask = inet_addr(lan_subnet);
    
    for(i = 0; i < 32; i++)
    {
        if((1<<i) & netmask)
            mask++;
    }
    
	return mask;
} 

int check_file(char *name)
{
    FILE *fp;
	int len=0, count = 0;

    while(count < 20)
    {
        if(access(name, F_OK)!=0)
        {
            usleep(50000);
            count++;
        }
        else
        {
            fp=fopen(name,"r");
            if( fp != NULL)
            {
            	fseek(fp,0,SEEK_END);
            	len=ftell(fp);
                fseek(fp,0,SEEK_SET);
                fclose(fp);
                if(len < 128)
                {
                    usleep(50000);
                    count++;
                }else
                    break;
            }else
            {
                usleep(50000);
                count++;
            }
        }    
    }

	return 0;
}


int check_ipv6_same_subnet(char *lan_ipv6addr, char *lan_ipv6gateway)
{
	struct in6_addr s6_ip;
	struct in6_addr s6_gw;
	int i, value, value1;
	char tmp[64];
	char *str, *str1;

	if(strlen(lan_ipv6gateway) == 0)
	    return -1;
	    
	if(strlen(lan_ipv6addr) == 0)
	    return -1;
	    
	if((str = strchr(lan_ipv6addr, '/')) == NULL)
	    return -1;
	    
	if((str1 = strchr(lan_ipv6gateway, '/')) == NULL)
	    return -1;
	
	value = atoi(str+1);
	value1 = atoi(str+1);  
//	fprintf(stderr, "prefix %d <---> %d\n", value, value1);
	if(value != value1)   
	    return -1; 
	    
	memset(tmp, '\0', sizeof(tmp));
	memcpy(tmp, lan_ipv6addr, str-lan_ipv6addr);
	if(1 != inet_pton(AF_INET6, tmp, (void *)&s6_ip)) {
		fprintf(stderr, "Invalid s6_ip IPv6 address\n");
		return -1;
	}
	
	memset(tmp, '\0', sizeof(tmp));
	memcpy(tmp, lan_ipv6gateway, str1-lan_ipv6gateway);
	if(1 != inet_pton(AF_INET6, tmp, (void *)&s6_gw)) {
		fprintf(stderr, "Invalid s6_gw IPv6 address\n");
		return -1;
	}
	
//	fprintf(stderr, "[%s:%d] ip %08x:%08x:%08x:%08x gw %08x:%08x:%08x:%08x\n", __FUNCTION__, __LINE__,
//	    s6_ip.s6_addr32[0], s6_ip.s6_addr32[1], s6_ip.s6_addr32[2], s6_ip.s6_addr32[3],
//	    s6_gw.s6_addr32[0], s6_gw.s6_addr32[1], s6_gw.s6_addr32[2], s6_gw.s6_addr32[3]);
	
	if(value >= 96) {
		if( (s6_ip.s6_addr32[0] >> (value-96)) != (s6_gw.s6_addr32[0] >> (value-96)) ) {
//    		fprintf(stderr, "[%s:%d] not in the same subnet\n", __FUNCTION__, __LINE__);
    		return -1;
		}
	} else if(value >= 64) {
		if( (s6_ip.s6_addr32[0] != s6_gw.s6_addr32[0]) || ((s6_ip.s6_addr32[1] >> (value-64)) != (s6_gw.s6_addr32[1] >> (value-64))) ) {
//    		fprintf(stderr, "[%s:%d] not in the same subnet\n", __FUNCTION__, __LINE__);
    		return -1;
		}
	} else if(value >= 32) {
		if( (s6_ip.s6_addr32[0] != s6_gw.s6_addr32[0]) || (s6_ip.s6_addr32[1] != s6_gw.s6_addr32[1]) || ((s6_ip.s6_addr32[2] >> (value-32)) != (s6_gw.s6_addr32[2] >> (value-32))) ) {
//    		fprintf(stderr, "[%s:%d] not in the same subnet\n", __FUNCTION__, __LINE__);
    		return -1;
		}
	} else {
		if( (s6_ip.s6_addr32[0] != s6_gw.s6_addr32[0]) || (s6_ip.s6_addr32[1] != s6_gw.s6_addr32[1]) || (s6_ip.s6_addr32[2] != s6_gw.s6_addr32[2]) || ((s6_ip.s6_addr32[3] >> (value)) != (s6_gw.s6_addr32[3] >> (value))) ) {
//    		fprintf(stderr, "[%s:%d] not in the same subnet\n", __FUNCTION__, __LINE__);
    		return -1;
		}
	}

	return 0;
}

