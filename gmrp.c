#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include "bcmutils.h"

int hex2mac(char *name, uint64_t *macaddr)
{
	int len;
	uint64_t val = 0x00ULL;
	
	for(len = 6; len > 0; name++, len--)
        val = (val<<8) + (uint64_t)*name;
    
    *macaddr = val;    
	return 0;
}

//802.1Q CONTROL 4 REGISTER (PAGE 34H/ADDR 03H)
int set_gmrp_enable(int skfd, int enable)
{
    return 0;
}

//TABLE MEMORY RESET CONTROL REGISTER (PAGE 03H/ADDR 01H)
int reset_gmrp_table(int skfd, uint64_t mac)
{
    return 0;
}

int add_hex_multimac_with_vlan(int skfd, int vlan_id, int index, char mac, uint64_t bitmaps)
{
	return 0;    
}

int del_hex_multimac_with_vlan(int skfd, int vlan_id, int index, char mac)
{
	return 0;      
}