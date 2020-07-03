#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "if_info.h"

//acl_type: mac acl-->1, sip acl-->2, eip acl-->3, ipv6 acl-->4 
int bcm_acl_create_all(int skfd, int acl_type, int *first_entry_id)
{
	uint32 data[8];
	
	data[0] = acl_type;
	data[1] = first_entry_id;
	
	if (ioctl(skfd, ACL_CREATE_ALL_ENTRY, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

//action: 0 is deny, 1 is permit
int bcm_mac_acl_set(int skfd, uint64 * phymaps, uint64 * src_mac, uint64 * dst_mac, int ether_type, int action, int *entry_id)
{
	uint32 data[8];
	
	data[0] = phymaps;
	data[1] = src_mac;
	data[2] = dst_mac;
	data[3] = ether_type;
	data[4] = action;
	data[5]	= entry_id;
	
	if (ioctl(skfd, MAC_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

int bcm_mac_acl_del_all(int skfd)
{
	uint32 data[8];	
	
	if (ioctl(skfd, MAC_ACL_DEL_ALL, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

/* group dump, mac: acl_type=1 */
int bcm_acl_dump(int skfd, int acl_type)
{
	uint32 data[8];	
	
	data[0] = acl_type;
	
	if (ioctl(skfd, ACL_DUMP, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

/* acl entry dump */
int bcm_acl_entry_dump(int skfd, int entry_id)
{
	uint32 data[8];	
	
	data[0] = entry_id;
	
	if (ioctl(skfd, ACL_ENTRY_DUMP, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

//action: 0 is deny, 1 is permit
int bcm_ip_std_acl_set(int skfd, uint64 * phymaps, uint32 src_ip, uint32 src_ip_mask, int action, int *entry_id)
{
	uint32 data[8];
	
	data[0] = phymaps;
	data[1] = src_ip;
	data[2] = src_ip_mask;
	data[3] = action;
	data[4]	= entry_id;
	
	if (ioctl(skfd, IP_STD_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

int bcm_ip6_std_acl_set(int skfd, uint64 * phymaps, uint8 *src_ip, uint32 src_ip_mask, int action, int *entry_id)
{
	uint32 data[9];
	memcpy(data, phymaps, sizeof(uint64));//copy data not address
	memcpy(&data[2], src_ip, 16*sizeof(char));

	data[6] = src_ip_mask;
	data[7] = action;
	data[8]	= entry_id;
	
	if (ioctl(skfd, IP6_STD_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}
int bcm_policy_ip6_std_acl_set(int skfd, uint64 * phymaps, uint8 *src_ip, uint32 src_ip_mask, uint64 * action, int *entry_id)
{
	uint32 data[9];

	memcpy(data, phymaps, sizeof(uint64));
	memcpy(&data[2], src_ip, 16*sizeof(uint8));

	data[6] = src_ip_mask;
	data[7] = action;
	data[8]	= entry_id;

	if (ioctl(skfd, POLICY_IP6_STD_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

/* Parameter format:
* protocol_and_mask: bit 15:0 is ipv4 protocol, 0-255, bit 31:16 is mask
* src_ip_and_mask: bit 31:0 is src ip, bit 63:32 is src ip mask
* dst_ip_and_mask: bit 31:0 is dst ip, bit 63:32 is dst ip mask
* port: bit 15:0 is src min port, bit 31:16 is src max port; bit 47:32 is dst min port, bit 63:48 is dst max port
* tos_and_mask:  bit 4:1 is tos, bit 7:5 is precedence, bit 15:8 is mask
* action: 0 is deny, 1 is permit
*/
int bcm_ip_ext_acl_set(int skfd, uint64 * phymaps, uint16 protocol_and_mask, uint64 *src_ip_and_mask, uint64 *dst_ip_and_mask, uint64 *port_range, uint16 tos_and_mask, int action, int *entry_id)
{
	uint32 data[8];
	
	data[0] = phymaps;
	data[1] = protocol_and_mask;
	data[2] = src_ip_and_mask;
	data[3] = dst_ip_and_mask;
	data[4]	= port_range;
	data[5] = tos_and_mask;
	data[6] = action;
	data[7] = entry_id;
	
	if (ioctl(skfd, IP_EXT_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

/*
* action: 	bit 4:0 is action type, (bit 4-->0: drop, bandwidth, vid, dscp, cos) 
*			bit 15:8 is cos
*			bit 23:16 is dscp
*			bit 39:24 is vid
*			bit 55:40 is bandwidth
*/
int bcm_policy_mac_acl_set(int skfd, uint64 * phymaps, uint64 * src_mac, uint64 * dst_mac, int ether_type, uint64 * action, int *entry_id)
{
	uint32 data[8];
	
	data[0] = phymaps;
	data[1] = src_mac;
	data[2] = dst_mac;
	data[3] = ether_type;
	data[4] = action;
	data[5]	= entry_id;
	
	if (ioctl(skfd, POLICY_MAC_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

/*
* action: 	bit 4:0 is action type, (bit 4-->0: drop, bandwidth, vid, dscp, cos) 
*			bit 15:8 is cos
*			bit 23:16 is dscp
*			bit 39:24 is vid
*			bit 55:40 is bandwidth
*/
int bcm_policy_ip_std_acl_set(int skfd, uint64 * phymaps, uint32 src_ip, uint32 src_ip_mask, uint64 * action, int *entry_id)
{
	uint32 data[8];
	
	data[0] = phymaps;
	data[1] = src_ip;
	data[2] = src_ip_mask;
	data[3] = action;
	data[4]	= entry_id;
	
	if (ioctl(skfd, POLICY_IP_STD_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

/* Parameter format:
* protocol_and_mask: bit 7:0 is ipv4 protocol, 0-255, bit 15:8 is mask
* src_ip_and_mask: bit 31:0 is src ip, bit 63:32 is src ip mask
* dst_ip_and_mask: bit 31:0 is dst ip, bit 63:32 is dst ip mask
* port: bit 15:0 is src min port, bit 31:16 is src max port; bit 47:32 is dst min port, bit 63:48 is dst max port
* tos_and_mask:  bit 4:1 is tos, bit 7:5 is precedence, bit 15:8 is mask
* action: 	bit 4:0 is action type, (bit 4-->0: drop, bandwidth, vid, dscp, cos) 
*			bit 15:8 is cos
*			bit 23:16 is dscp
*			bit 39:24 is vid
*			bit 55:40 is bandwidth
*/
int bcm_policy_ip_ext_acl_set(int skfd, uint64 * phymaps, uint16 protocol_and_mask, uint64 *src_ip_and_mask, uint64 *dst_ip_and_mask, uint64 *port_range, uint16 tos_and_mask, uint64 *action, int *entry_id)
{
	uint32 data[8];
	
	data[0] = phymaps;
	data[1] = protocol_and_mask;
	data[2] = src_ip_and_mask;
	data[3] = dst_ip_and_mask;
	data[4]	= port_range;
	data[5] = tos_and_mask;
	data[6] = action;
	data[7] = entry_id;
	
	if (ioctl(skfd, POLICY_IP_EXT_ACL_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

/*
* type:  0 means classify=dscp, 1 means classify=cos, 2 means classify=vlan, 3 classify=any
*
* action: 	bit 4:0 is action type, (bit 4-->0: drop, bandwidth, vid, dscp, cos) 
*			bit 15:8 is cos
*			bit 23:16 is dscp
*			bit 39:24 is vid
*			bit 55:40 is bandwidth
*/
int bcm_policy_other_set(int skfd, uint64 * phymaps, int type, uint16 value, uint64 * action, int *entry_id)
{
	uint32 data[8];
	
	data[0] = phymaps;
	data[1] = type;
	data[2] = value;
	data[3] = action;
	data[4]	= entry_id;
	
	if (ioctl(skfd, POLICY_OTHER_ADD, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
    return 0;  
}

int set_arp_rate_by_portid(int skfd, int portid, uint32 rate)
{    
//    uint32 entry_id, access;
//    uint64 val = 0x00ULL;
//    TCAMDATA tcam;
//    
//    entry_id = portid-1;
//    memset(&tcam, 0, sizeof(TCAMDATA));
//    access = MEM_SEL_ALL|OP_CODE_WRITE|RATE_MTR_EN;
//	access |= (entry_id<<9);
//    
//    set_tcam_slice0_data(SLICE0_INGRESS_PORT_MAP, 0x01ULL<<phy[portid], &tcam);
//    set_tcam_slice0_data(SLICE0_ETHER_TYPE, 0x0806ULL, &tcam);
//    
//    set_cfp_action_value(CHNG_FWD_OB_3, 60, &val);
//    
//	write_rate_meter_regs(skfd, rate);	
//	write_tcam_data_regs(skfd, &tcam);       
//	write_cfp_action_regs(skfd, val);
//	write_cfp_access_regs(skfd, access);
    
    return 0;
}

/*
*Function: set_arp_rate
*
*Purpose: set arp packet rate using slice 0 and entry id same with portid
*
*Parameters:
*			bitmaps: phy maps the rule applies to 
*			rate:	 arp rate 
*
*Return: 
*           -1 : rate is out of range
*           0  : normal
*/
int set_arp_rate_by_phymaps(int skfd, uint64_t bitmaps, uint32 rate)
{
    int i; 
    uint64_t bmaps = bitmaps;
                    
    for(i = 1; i <= PNUM; i++)
    {
        if((bmaps >> phy[i]) & (0x01ULL))
            set_arp_rate_by_portid(skfd, i, rate);           
    } 
    
    return 0;    
}

/*
*Function: disable_arp_rate_by_portid
*
*Purpose: disable arp packet rate using slice 0 and entry id same with portid
*
*Parameters:
*			portid: port id the rule applies to 
*			rate:	 arp rate 
*
*Return: 
*           -1 : rate is out of range
*           0  : normal
*/
int disable_arp_rate_by_portid(int skfd, int portid)
{
//	uint32 entry_id, access;
//	TCAMDATA tcam;
//	
//	entry_id = portid-1;
//		
//	memset(&tcam, 0, sizeof(TCAMDATA));		
//	access = MEM_SEL_ALL|OP_CODE_WRITE|RATE_MTR_EN;
//	access |= (entry_id<<9);
//	write_tcam_data_regs(skfd, &tcam);   
//	write_cfp_action_regs(skfd, 0x00ULL);
//	write_cfp_access_regs(skfd, access);

	return 0;
}

