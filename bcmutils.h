#ifndef _BCMUTILS_H_
#define _BCMUTILS_H_

#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <asm/param.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <string.h>
#include <stdarg.h>

#include "kputils.h"

#define DEVICE_FILE_NAME "/dev/linux-uk-proxy"

typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned long	uint32;
typedef __u64			uint64;
//typedef __u64			uint64_t;

/* added by xichen */
#define CLI_SUCCESS 0
#define CLI_FAILED  1
/* added by qianfeng */
#define BRC_CON (0x01<<3)
#define UNC_CON (0x01<<5)
#define UB_CON ((0x01<<5)|(0x01<<3))

/* Class of Service policies. */
#define BCM_COSQ_NONE                       0x0        		/* Pass through. */
#define BCM_COSQ_STRICT                     0x01       		/* Weights are ignored. */
#define BCM_COSQ_ROUND_ROBIN                0x02       		/* Weights are ignored. */
#define BCM_COSQ_WEIGHTED_ROUND_ROBIN       0x03       
#define BCM_COSQ_WEIGHTED_FAIR_QUEUING      0x04       
#define BCM_COSQ_DEFICIT_ROUND_ROBIN        0x05       		/* XGS3 only. */
#define BCM_COSQ_BOUNDED_DELAY              0x06       		/* Strata only. */
#define BCM_COSQ_SP                         BCM_COSQ_STRICT /* Strict priority. */
#define BCM_COSQ_EF                         0x07       		/* Expedited forwarding. */
#define BCM_COSQ_AF                         0x08       		/* Assured forwarding. */
#define BCM_COSQ_SP_GLOBAL                  0x09       
#define BCM_COSQ_BE                         0x0A       		/* Best effort. */
#define BCM_COSQ_CALENDAR                   0x0B       		/* Calendar based*/
/* end */

#define BCM_COSQ_DISCARD_ENABLE             0x0001     
#define   bcmVlanShared  3					/* Shared vs. Independent VLAN Learning. */
#define   bcmVlanSharedID 4 			   /* Shared Learning VLAN ID. */



#define FNUM  (PNUM-GNUM)
#define TNUM  6					//gujiajie add for port-aggregator
#define PORT_NUM (PNUM+TNUM)    // PORT_NUM = PNUM+TNUM  OR PORT_NUM = FNUM+GNUM+TNUM
#define MSTI_NUM 16

typedef enum PORT_CONFIG{
	PORT_SPEED_AUTO=0,
	PORT_SPEED_10,
	PORT_SPEED_100,
	PORT_SPEED_1000,
	PORT_DUPLEX_AUTO,
	PORT_DUPLEX_FULL,
	PORT_DUPLEX_HALF,
	PORT_AUTO_AUTO = (1<<PORT_SPEED_AUTO)| (1<<PORT_DUPLEX_AUTO),  //auto/auto
	PORT_AUTO_FULL = (1<<PORT_SPEED_AUTO)| (1<<PORT_DUPLEX_FULL),  //auto/full
	PORT_AUTO_HALF = (1<<PORT_SPEED_AUTO)| (1<<PORT_DUPLEX_HALF),  //auto/half
	PORT_10_AUTO   = (1<<PORT_SPEED_10)  | (1<<PORT_DUPLEX_AUTO),  //10/auto
	PORT_10_FULL   = (1<<PORT_SPEED_10)  | (1<<PORT_DUPLEX_FULL),  //10/full
	PORT_10_HALF   = (1<<PORT_SPEED_10)  | (1<<PORT_DUPLEX_HALF),  //10/half
	PORT_100_AUTO  = (1<<PORT_SPEED_100) | (1<<PORT_DUPLEX_AUTO),  //100/auto
	PORT_100_FULL  = (1<<PORT_SPEED_100) | (1<<PORT_DUPLEX_FULL),  //100/full
	PORT_100_HALF  = (1<<PORT_SPEED_100) | (1<<PORT_DUPLEX_HALF),  //100/half	
}PORT_CONFIG;

static int phy[64] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};
static int phy2port[64] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};

/***********************************************************************/
/*
 * CLI param type
 */
typedef enum INTERFACE_CONF{
	INTF_DHCP=0,
	INTF_SUPERVLAN, 
	INTF_ARP_PROXY,
}INTF_CONF;

/***********************************************************************/
/**************************  NVRAM Config  *****************************/
/***********************************************************************/
/*
 * CLI param type
 */
typedef enum CLI_CONF{
	CLI_VLAN_FOWD,
	CLI_VLAN_PORT,
	CLI_AGG_VLAN_PORT,
	CLI_SOURCE_BINDING,
	CLI_RSTP_PORT,
	CLI_SNMP_USER,
	CLI_ACCESS_USER,
	CLI_DOT1X_CONF,
	CLI_TRUNK_LIST,
	CLI_STATIC_ARP,
	CLI_RATE_STORM,
	CLI_IPV6_ROUTE,  		//wuchunli
	CLI_AGGR_RATE_STORM,	/*add by jiangyaohui 20120427*/
	CLI_AGG_RSTP_PORT,		/*add by jiangyaohui 20120507*/
	CLI_MSTP_PORT, 
	CLI_MSTP_PORT_COST_PRIO, 
	CLI_RING_INFO,
	CLI_POE_CONF,
	CLI_POE_NAMES,
}CLI_CONF;

/*
 * nvram param type
 */
typedef enum CLI_NVRAM_TYPE{
	CLI_ALL_ONE,         /* exp: "111111111" */
	CLI_ALL_ZERO,        /* exp: "000000000" */
	CLI_COMMA,			 /* exp: ",,,,...." */
	CLI_COMMA_ONE,       /* exp: "1,1,1,1,1" */
	CLI_COMMA_ZERO,      /* exp: "0,0,0,0,0" */
	CLI_PORT_NAME,       /* exp: "port1,port2..." */
	CLI_VLAN_FORWARD,    /* exp: "0:1:Default:1-N:1-N;" */
	CLI_RATE_ALL,        /* exp: "1,,;2,,;3,,;..." */
	CLI_RC_NUM,          /* exp: "00,00,...;00,00" */
	CLI_RATE,			 /* exp: ",,,,...." */	
	CLI_RSTP_CONFIG,     /* exp: "0,0,0,1,1;..." */
	CLI_MSTP_PORT_CP_CONFIG, /* exp: "0,128;..." */
	CLI_MSTP_PORT_CONFIG,/* exp: "2,0,1,0,0;..." */
	CLI_MAC_ADVANCED,    /* exp: "8191,8191,..." */
	CLI_DOT1X_CONFIG,    /* exp: "1,0,4096;..." */
	CLI_PROTECT_CONFIG,  /* exp: "1,0;2,0;3,0;..." */
	CLI_COS_CONFIG,
	CLI_DSCP_CONFIG,     /* exp: "1111111111..." */
	CLI_TRUNK_VLAN,      /* exp: "1:;2:;3:;..." */
	CLI_ALL_DES,		 /* exp: ",,,,,,..." */
	CLI_PORT_ACL,        /* exp: "acl1,acl2,acl3..." */
	CLI_PORT_POLICY,
	CLI_CPU_ACL,         /* exp: "in,out" */
	CLI_SPEED_ALL_AUTO,  /* exp: "000000000" */
	CLI_DUPLEX_ALL_AUTO, /* exp: "444444444" */
    CLI_ERR_DISABLE,     /* exp: "1111111111..." */
    CLI_ERR_RECOVER,     /* exp: "1111111111..." */
    CLI_AGG_RSTP_CONFIG,	/* exp: "0,0,0,1,1,1,1;..." */
    CLI_AGG_PROTECT_CONFIG,	/* exp: "1,0;2,0;3,0;..." */
	CLI_POE_PORT,        /* exp: "0,30000,1;.." */
	CLI_POE_DESP,
    CLI_AGG_ALL_ONE,	/* exp: "111111" */
    CLI_AGG_ALL_ZERO,	/* exp: "000000" */
    CLI_AGG_COMMA,		/* exp: ",,,,," */
}CLI_NVRAM_TYPE;

typedef enum _shr_port_stp_e {
    _SHR_PORT_STP_DISABLE,
    _SHR_PORT_STP_BLOCK,
    _SHR_PORT_STP_LISTEN,
    _SHR_PORT_STP_LEARN,
    _SHR_PORT_STP_FORWARD,
    _SHR_PORT_STP_COUNT    /* last, please */
} _shr_port_stp_t;

#define BCM_PORT_STP_DISABLE    _SHR_PORT_STP_DISABLE
#define BCM_PORT_STP_BLOCK      _SHR_PORT_STP_BLOCK
#define BCM_PORT_STP_LISTEN     _SHR_PORT_STP_LISTEN
#define BCM_PORT_STP_BLOCKTX    _SHR_PORT_STP_LEARN
#define BCM_PORT_STP_LEARN      _SHR_PORT_STP_LEARN
#define BCM_PORT_STP_FORWARD    _SHR_PORT_STP_FORWARD
#define BCM_PORT_STP_COUNT      _SHR_PORT_STP_COUNT

#define BPDU_FWD       _SHR_PORT_STP_DISABLE
#define BPDU_RING       _SHR_PORT_STP_BLOCK
#define BPDU_NOTE       _SHR_PORT_STP_LISTEN

/*by liujh*/
#define cli_debug_p(fmt,arg...)		printf("[%s %d]: "fmt,__FUNCTION__,__LINE__,##arg)
#define BCM_53344_L2
/* end */

/* The max port string len, exp: "1, 3-4" */
#define PORT_STR_LEN  (PNUM*3)

/***********************************************************************/
/**************************  VLAN Config  ******************************/
/***********************************************************************/
/*
 * Just define for vlan config struct
 */
#define CLI_DEFAULT_VLAN 1
#define MAX_WORD_LEN 32
#define VLAN_ALLOW_UNTAG_LEN ((128+12)*PNUM)
#define VLAN_MAX_NUM  4094
#define VLAN_RANGE_MAX_LEN 4096
#define VLAN_MAX_CNT  4096

/* The max vlan struct len with one vlan */
#define VLAN_STRUCT_LEN  (46+PNUM*6)

typedef struct vlan_entry_t{
	int  vlanid;
    char name[32];
    uint64_t  forward;
    uint64_t  untag;
}VLAN_ENTRY;

typedef struct ipmc_entry_t{
	int  vlanid;
	int  version;
	int  query;
	int  timeout;
}IPMC_ENTRY;
typedef struct port_based_vlan_t{
    uint32  pbmap;
	uint8 desc[32];
}PORT_BASED_V;

typedef enum CLI_TRUNK{
	CLI_TRUNK_ALLOWED,
	CLI_TRUNK_UNTAGGED,
}CLI_TRUNK;

/*
 *    struct cli_vlan_info_conf
 *
 *    1.vlanid --- Vlan ID (1 - VLAN_MAX_NUM)
 *    2.name --- Vlan name (VLAN_NAME_LEN)
 *    3.forward --- Vlan forward port range
 *    4.untag --- Vlan untag port range
 *    5.next --- Next Vlan struct
 */
typedef struct cli_vlan_info_conf_t{
        int  vlanid;
    char name[36];
    uint64_t  forward;
    uint64_t  untag;
    struct cli_vlan_info_conf_t *next;
}cli_vlan_info_conf;

typedef struct cli_vlan_conf_t{
	int  vlan_count;
	cli_vlan_info_conf *cur_vlan_info;
}cli_vlan_conf;

typedef struct vlan_port_conf_t{
    int  pvid;
    int  mode;
    char allow[256]; // > 28*3
    char untag[256];
}PORT_CONF_ENTRY;

typedef struct vlan_interface_conf_t{
    int  vlanid;
    int  iptype;
    char ipaddr[18];
    int  netmask;
}vlan_interface_conf;
typedef struct vlan_interface_confv6_t{
    int  vlanid;
    char ipaddr[64];
}vlan_interface_v6conf;
/*
 *    struct cli_port_conf
 *
 *    1.portid --- port ID (1 - PNUM)
 *    2.pvid --- PVID
 *    3.mode --- TYPE (Access, Trunk)
 *    4.allow --- Trunk port vlan allowed range (1 - VLAN_MAX_NUM)
 *    5.untag --- Trunk port vlan untagged range (1 - VLAN_MAX_NUM)
 */
typedef struct cli_port_conf_t{
	int  portid;
    int  pvid;
    char mode;
    char *allow;
    char *untag;
}cli_port_conf;

static cli_vlan_conf cur_vlan_conf;
static cli_port_conf cur_port_conf[PNUM];
static cli_port_conf cur_agg_port_conf[TNUM];

/***********************************************************************/
/**************************  DHCP Snooping  ****************************/
/***********************************************************************/
/* The max source struct len */
#define SOURCE_STRUCT_LEN  46

/*
 *    struct cli_source_info_conf
 *
 *    1.index --- Entry Index
 *    2.mac_addr --- MAC Address
 *    3.ip_addr --- IP Address
 *    4.port --- Port ID
 *    5.vlan --- Vlan ID
 *    6.type --- TYPE (Static, Dynamic)
 *    7.next --- Next Source struct
 */
typedef struct cli_source_info_conf_t{
	int index;
	unsigned char mac_addr[6];
	struct in_addr ip_addr;
	int port;
	int vlan;
	int type;
	struct cli_source_info_conf_t *next;
}cli_source_info_conf;

typedef struct cli_source_conf_t{
	int  source_count;
	cli_source_info_conf *cur_source_info;
}cli_source_conf;

static cli_source_conf cur_source_conf;

/***********************************************************************/
/**************************  RSTP Config  ******************************/
/***********************************************************************/
typedef enum RSTP_CONFIG{
	CLI_PORT_ENABLE,
	CLI_PORT_COST,
	CLI_PORT_PRIORITY,
	CLI_PORT_P2P,
	CLI_PORT_EDGE,
	CLI_PORT_BPDU_GUARD,
	CLI_PORT_BPDU_FILTER,
	CLI_PORT_GUARD,
}RSTP_CONFIG;

/*
 *    struct cli_rstp_conf
 *
 *    1.pathcost --- port path cost
 *    2.priority --- port priority
 *    3.p2p --- port point to point
 *    4.edge --- edge port
 *    5.status --- port rule
 */
typedef struct cli_rstp_conf_t{
	int  pathcost;
    int  priority;
    int  p2p;
    int  edge;
	int  status;

	/*shanming.ren 2011-9-16 begin*/
	int  bpdu_guard;
	int  bpdu_filter;
	int  guard;
	/*shanming.ren 2011-9-16 end*/
}cli_rstp_conf;

/***********************************************************************/
/**************************  MSTP Config  ******************************/
/***********************************************************************/

/* 
 *  	struct cli_mstp_port_cost_prio
 *  	
 *  	1.cost --- port path cost for msti
 *  	2.prio --- port priority for msti
 */
typedef struct cli_mstp_port_cost_prio_t{
	int cost;
	int prio;
}cli_mstp_port_cost_prio;

/*
 *  	struct cli_mstp_port_config
 *  
 *  	1.p2p --- port link type
 *  	2.adminedge --- port is admin edge or not
 *  	3.autoedge --- port is auto edge or not
 *  	4.restrrole --- Restrict port ability to take Root role
 *  	5.restrtcn --- Restrict port ability to propagate received TCNs
 *  	6.bpdufilter -- BPDU Filter
 *  	7.bpduguard -- BPDU Guard
 *  	8.rootguard -- Root Guard
 */
typedef struct cli_mstp_port_conf_t{
	int p2p;
	int adminedge;
	int autoedge;
	int restrrole;
	int restrtcn;
	int bpdufilter;
	int bpduguard;
	int rootguard;
}cli_mstp_port_conf;

/***********************************************************************/
/*************************  SNMP v3 Config  ****************************/
/***********************************************************************/
/* The max source struct len */
#define MAX_SNMP_USER 5

typedef enum CLI_SNMP{
	CLI_SNMP_RONLY,
	CLI_SNMP_RWRITE,
}CLI_SNMP;

/*
 *    struct cli_port_conf
 *
 *    1.portid --- port ID (1 - PNUM)
 *    2.pvid --- PVID
 *    3.mode --- TYPE (Access, Trunk)
 *    4.allow --- Trunk port vlan allowed range (1 - VLAN_MAX_NUM)
 *    5.untag --- Trunk port vlan untagged range (1 - VLAN_MAX_NUM)
 */
typedef struct cli_snmp_user_info_t{
	int  index;
    char name[36];
    char auth[8];
    char auth_passwd[36];
    char priv[8];
    char priv_passwd[36];
    int  mode;
}cli_snmp_user_info;

typedef struct cli_snmp_user_t{
	int  user_count;
	cli_snmp_user_info cur_snmp_user_info[MAX_SNMP_USER];
}cli_snmp_user;

static cli_snmp_user cur_snmp_user;

/***********************************************************************/
/*************************  user for access  ***************************/
/***********************************************************************/
/* The max source struct len */
#define MAX_ACCESS_USER 3

//typedef enum CLI_SNMP{
//	CLI_SNMP_RONLY,
//	CLI_SNMP_RWRITE,
//}CLI_SNMP;

/*
 *    struct cli_access_user_info
 *
 *    1.index --- user index
 *    2.name --- user name
 *    3.passwd --- password
 *    4.mode --- mode
 *
 */
typedef struct cli_access_user_info_t{
	int  index;
    char name[36];
    char passwd[36];
    int  mode;
}cli_access_user_info;

typedef struct cli_access_user_t{
	int  user_count;
	cli_access_user_info cur_access_user_info[MAX_ACCESS_USER];
}cli_access_user;

static cli_access_user cur_access_user;

/***********************************************************************/
/*************************  dot1x config  ******************************/
/***********************************************************************/

typedef enum CLI_DOT1X_MODE{
	CLI_DOT1X_FAUTH = 1,
	CLI_DOT1X_AUTO,
	CLI_DOT1X_FUNAUTH,
}CLI_DOT1X_MODE;

/*
 *    struct cli_dot1x_conf
 *
 *    1.auth_mode --- authentication mode
 *    2.ctrl_type --- auth control type
 *    3.max_num --- max auth user number
 *
 */
typedef struct cli_dot1x_conf_t{
	int auth_mode;
	int ctrl_type;
	int guest_id;
	int max_user;
}cli_dot1x_conf;

static cli_dot1x_conf cur_dot1x_conf[PNUM];

/***********************************************************************/
/*************************  trunk config  ******************************/
/***********************************************************************/
/* aggregation group number */
#define CLI_TRUNK_GROUP  8

typedef enum CLI_TRUNK_MODE{
	CLI_STATIC = 1,
	CLI_STATIC_LACP,
	CLI_LACP,
}CLI_TRUNK_MODE;

/*
 *    struct cli_trunk_list
 *
 *    1.groupid --- Group ID (1 - CLI_TRUNK_GROUP)
 *    2.name --- Group name (VLAN_NAME_LEN)
 *    3.mode --- Trunk mode (static, static LACP, LACP)
 *    4.key --- Manage Key
 *    5.port_int --- Group member bitmap
 *    6.next --- Next Vlan struct
 *
 */
typedef struct cli_trunk_list_t{
	int group_no;
    char name[36];
    int mode;
    int key;
    uint64_t port_int;
}cli_trunk_list;

typedef struct cli_trunk_conf_t{
	int  balance_mode;
	int  group_count;
	cli_trunk_list cur_trunk_list[CLI_TRUNK_GROUP];
}cli_trunk_conf;

static cli_trunk_conf cur_trunk_conf;

/*struct cli_static_arp_conf*/
typedef struct cli_static_arp_list_t{
	char static_ip[20];
	unsigned char static_mac[20];
	int vlan;
	int port;
	struct cli_static_arp_list_t *next;
}cli_static_arp_list;

typedef struct cli_static_arp_conf_t{
	int arp_info_count;
	cli_static_arp_list *static_arp_list;
}cli_static_arp_conf;

static cli_static_arp_conf cur_static_arp_conf;

#define STATIC_ARP_LEN 37

/***********************************************************************/
/*************************  rate storm config  ******************************/
/***********************************************************************/

/*
 *    struct cli_trunk_list
 *
 *    1.group_no --- Group ID (1 - CLI_TRUNK_GROUP)
 *    2.name --- Group name (VLAN_NAME_LEN)
 *    3.mode --- Trunk mode (static, static LACP, LACP)
 *    4.key --- Manage Key
 *    5.port_int --- Group member bitmap
 *    6.next --- Next Vlan struct
 *
 */
typedef struct cli_rate_storm_t{
	int rate_igr;
    int rate_egr;
    int storm_bro;
    int storm_mul;
    int storm_uni;
}cli_rate_storm;

static cli_rate_storm cur_rate_storm[PNUM];

typedef struct cli_ring_conf_t{
    int  enable;
    int  ident[2];
    int  type;
    int  ports[4];
}cli_ring_conf;
static int rate_rc[48] = {0,1,2,4,8,16,29,30,31,32,33,34,35,36,37,47,57,67,77,87,97,107,117,127,140,153,165,178,190,203,215,228,240}; 
static char rate_str[48][16] = {"no limit","64K","128K","256K","512K","1M","2M","3M","4M","5M","6M","7M","8M","9M","10M","20M","30M","40M","50M","60M","70M","80M","90M","100M","200M","300M","400M","500M","600M","700M","800M","900M","1000M"}; 

/***********************************************************************/
/*************************  ipv6 route config ******************************/
/***********************************************************************/

#define MAX_IPV6_ROUTE 128

typedef struct cli_ipv6_route_list_t{
	char prefix[MAX_IPV6_ROUTE];
	char nexthop[MAX_IPV6_ROUTE];
	struct cli_ipv6_route_list_t *next;
}cli_ipv6_route_list;

typedef struct cli_ipv6_route_conf_t{
	int ipv6_route_count;
	cli_ipv6_route_list *ipv6_route_list;
}cli_ipv6_route_conf;

static cli_ipv6_route_conf cur_ipv6_route_conf;

/* ARP Inspection CFP start */
#define CFP_ARP_REDIRECT 71
#define CFP_DHCP_REQUEST 72
#define CFP_DHCP_REPLY 73
/* end */
/* CFP index */
#define CFP_DROP_TAG 77
#define CFP_DROP_ICMP 78
#define CFP_DROP_DHCP 79

#define CFP_BPDU_IMP_QOS 1000
#define CFP_ICMP_IMP_QOS 1001
#define CFP_HTTP_IMP_QOS 1002
#define CFP_LACP_IMP_QOS 1011
#define CFP_HWMAC_IMP_QOS 1012
#define CFP_ARP_IMP_QOS 1013
#define CFP_LOOP_IMP 1014
#define CFP_ALL_IMP_QOS 1015

//========acl start ================
#define SHOW_RUNNING_ACL       "/tmp/acl_cfg"        /* show running */
#define SHOW_RUNNING_POLICY    "/tmp/policy_cfg"     /* show running */

#define SHOW_ONE_IP_ACL        "/tmp/ip_acl_one"	/* show ip access-lists xx*/
#define SHOW_ALL_IP_ACL        "/tmp/ip_acl_all"    /* show ip access-lists*/

#define SHOW_ONE_IPV6_ACL        "/tmp/ipv6_acl_one"	  /* show ipv6 access-lists xx*/
#define SHOW_ALL_IPV6_ACL        "/tmp/ipv6_acl_all"    /* show ipv6 access-lists*/

#define SHOW_ONE_POLICY        "/tmp/policy_one"	/* show policy xx*/
#define SHOW_ALL_POLICY        "/tmp/policy_all"	/* show policy*/

#define PKTS_WITH_BCAT 1
#define PING_OF_DEATH1 2
#define PING_OF_DEATH2 3

#define DOS_SAME_IP 76
#define DOS_SAME_PORT 75
#define DOS_SYN_PORT 74

/* mac acl EID:1-127,  deny any: 128 */
#define CFP_ACL 1
#define MAC_ACL_NUM 127
//#define FIRST_MAC_ACL_ENTRY  CFP_ACL     //1
//#define LAST_MAC_ACL_ENTRY   (CFP_ACL+MAC_ACL_NUM-1)  //127
//#define DENY_MAC_ACL_ENTRY  (LAST_MAC_ACL_ENTRY+1)    //128

/* sip acl EID:129-255,  deny any: 256 */
#define SIP_ACL_NUM 63
//#define FIRST_SIP_ACL_ENTRY (DENY_MAC_ACL_ENTRY+1)    //129
//#define LAST_SIP_ACL_ENTRY (FIRST_SIP_ACL_ENTRY+SIP_ACL_NUM-1)  //255
//#define DENY_SIP_ACL_ENTRY  (LAST_SIP_ACL_ENTRY+1)    //256

/* ipv6 acl EID: 257-383, deny any: 384 */
#define SIPV6_ACL_NUM 63
//#define FIRST_SIPV6_ACL_ENTRY (DENY_SIP_ACL_ENTRY+1)  //257
//#define LAST_SIPV6_ACL_ENTRY (FIRST_SIPV6_ACL_ENTRY+SIPV6_ACL_NUM-1)  //383
//#define DENY_SIPV6_ACL_ENTRY  (LAST_SIPV6_ACL_ENTRY+1)    //384

/* eip acl EID: 385-511, deny any: 512 */
#define EIP_ACL_NUM 63
//#define FIRST_EIP_ACL_ENTRY 385
//#define LAST_EIP_ACL_ENTRY 511
//#define DENY_EIP_ACL_ENTRY 512

/*
#define CPU_ACL_NUM 128
#define FIRST_CPU_ACL_ENTRY 475
#define LAST_CPU_ACL_ENTRY 602
#define DENY_CPU_ACL_ENTRY 603
*/

#define POLICY_NUM 63
//#define FIRST_POLICY_ENTRY 475
//#define LAST_POLICY_ENTRY 602
//#define DENY_POLICY_ENTRY 800
/*目前ACL功能的entry id是自动生成的by xgs 2019.12.21
自动生成的id从1开始总共占用
MAC_ACL_NUM+1 SIP_ACL_NUM+1 EIP_ACL_NUM+1 SIPV6_ACL_NUM+1 POLICY_NUM+1
如果宏的值修改了，请同步修改linux-uk-proxy处的 kputils.h
中的MAC_ACL_MAX等宏*/



/* 配置文件字符串宏 by xgs 2019.12.25 */

#define NVRAM_STR_SNOOP_ENABLE     "snoop_enable"
#define NVRAM_STR_V6SNOOP_ENABLE     "dhcp6_snoop_enable"
#define NVRAM_STR_V6SNOOP_TRUST     "dhcp6_snoop_trust_port"
#define NVRAM_STR_HTTPS_ENABLE     "https_enable"
#define NVRAM_STR_L3_IPV6     "l3_ipv6"
#define NVRAM_STR_IPV6_GW     "ipv6_gw"
#define NVRAM_STR_FLOW_INTERVAL     "flow_interval"
#define NVRAM_STR_ALL_PORT_AVERAGE_RATE     "all_port_average_rate"
/*配置文件字符串宏 end*/

#define SHM_KEY_FLOW_INTERVAL   9111
struct shared_use_st
{
	int writing;// //1 正在写
    int reading;// 1正在读 
	uint64_t byte_rx[PNUM];
	uint64_t byte_tx[PNUM];
	uint32_t ucast_rx[PNUM];
	uint32_t mcast_rx[PNUM];
	uint32_t bcast_rx[PNUM];
	uint32_t ucast_tx[PNUM];
	uint32_t mcast_tx[PNUM];
	uint32_t bcast_tx[PNUM];
};


/* CFP ACCESS REGISTER (PAGE 20H/ADDR 00H–03H) */
#define OP_CODE_NO_OPERRATION  0x00
#define OP_CODE_READ           0x01
#define OP_CODE_WRITE          0x02
#define OP_CODE_MEM_FILL       0x03

#define MEM_SEL_NO_OPERATION   (0x00<<4)
#define MEM_SEL_TCAM           (0x01<<4)
#define MEM_SEL_ACTION         (0x02<<4)
#define MEM_SEL_RATE           (0x04<<4)
#define MEM_SEL_IN_BAND        (0x08<<4)
#define MEM_SEL_OUT_BAND       (0x10<<4)
#define MEM_SEL_ALL            (0x1F<<4)

#define RATE_MTR_EN            (0x01<<31)

#define FLOW_TO_VLAN_NUM 16

enum cfp_action {
    CHNG_DSCP_OB,
    CHNG_FWD_OB_1,
    CHNG_FWD_OB_2,
    CHNG_FWD_OB_3,
    CHNG_FLOW_1,
    CHNG_FLOW_2,
    CHNG_FLOW_3,
    CHNG_DSCP_IB,
    CHNG_PCP,
    CHNG_QOS,
    CHNG_QOS_IMP,
    CHNG_FWD_IB_1,
    CHNG_FWD_IB_2,
    CHNG_FWD_IB_3,
}action;

enum slice0_definitions {
    SLICE0_INGRESS_PORT_MAP,
    SLICE0_SP_TAGGED,
    SLICE0_CUSTOMER_TAGGED,
    SLICE0_L2_FRAME_FORMAT,
    SLICE0_L3_FRAME_FORMAT,
    SLICE0_L4_FRAME_FORMAT,
    SLICE0_VLAN_RANGE,
    SLICE0_L4_PORT_RANGE,
    SLICE0_MAC_DA,
    SLICE0_MAC_SA,
    SLICE0_BMAC_SA,
    SLICE0_SP_TAG,
    SLICE0_CUSTOMER_TAG,
    SLICE0_COS,
    SLICE0_ETHER_TYPE,
    SLICE0_IPV4_DA,
    SLICE0_IPV4_SA,
    SLICE0_IPV4_TOS,
    SLICE0_IPV4_PRECEDENCE,
    SLICE0_IPV4_DSCP,
    SLICE0_IPV4_PROTOCOL,
    SLICE0_IPv4_TTL_RANGE,
    SLICE0_SAME_IP_ADDRESS,
    SLICE0_L4_DESTINATION_PORT,
    SLICE0_L4_SOURCE_PORT,
    SLICE0_TCP_FLAG,
    SLICE0_SAME_L4_PORT,
    SLICE0_PORT_LESS_1024,
    SLICE0_UDF_A0,
    SLICE0_UDF_A1,
    /* IPV6 defination */
    SLICE0_IPV6_FLOW_ID,
    SLICE0_IPV6_SA,
    SLICE0_IPV6_TRAFFIC_CLASS_FIELD,
}slice0;

enum slice2_definitions {
    SLICE2_INGRESS_PORT_MAP,
    SLICE2_SP_TAGGED,
    SLICE2_CUSTOMER_TAGGED,
    SLICE2_L2_FRAME_FORMAT,
    SLICE2_L3_FRAME_FORMAT,
    SLICE2_L4_FRAME_FORMAT,
    SLICE2_VLAN_RANGE,
    SLICE2_L4_PORT_RANGE,
    SLICE2_UDF_C0,
    SLICE2_UDF_C1,
}slice2;

enum {
	UDF_TAG=0,
	UDF_ETHER_TYPE,
	UDF_IP_HEADER,
	UDF_START_OF_PKT	
}udf;

typedef struct tcam_data {
    uint64_t data0;
    uint64_t data1;
    uint64_t data2;
    uint64_t data3;
    uint64_t data4;
    uint64_t data5;
    uint64_t mask0;
    uint64_t mask1;
    uint64_t mask2;
    uint64_t mask3;
    uint64_t mask4;
    uint64_t mask5;
}TCAMDATA;


#define ACL_NAME_LEN 20

#define BCM_PORT_FORCE_VLAN_ENABLE          0x01       /* Enable force VLAN on  this port. */
#define BCM_PORT_FORCE_VLAN_UNTAG           0x02       /* Egress untagged when*/
#define BCM_PORT_FORCE_VLAN_PROMISCUOUS_PORT 0x04       /* Promiscuous port type. */
#define BCM_PORT_FORCE_VLAN_ISOLATED_PORT   0x08       /* isolated port type. */
#define BCM_PORT_FORCE_VLAN_COMMUNITY_PORT  0x0c
typedef struct acl_value {
    int       flag;
    uint64_t  value;
    struct    acl_value * next;
}ACL_VALUE;

typedef struct acl_entry {
    int val_num;
    struct acl_value * acl_value;
    struct acl_entry * next;
}ACL_ENTRY;

typedef struct acl_list {
    char      name[ACL_NAME_LEN+1];
    int       entry_num;
    uint64_t  portmaps;
    struct    acl_entry * acl_entry;
    struct    acl_list * next;    
}ACL_LIST;

//=========acl end ==================

typedef enum RATE_STORM{
	RATE_INGRESS,
	RATE_EGRESS,
	STORM_BROADCAST,
	STORM_MULTICAST,
	STORM_UNICAST,
}RATE_STORM;

#define ARL_OP_SEARCH_START            			0x80
#define ARL_OP_SEARCH_READ            			0x01
#define ARL_OP_SEARCH_WRITE            			0x00

#define ENABLE              0x01
#define DISABLE             0x00

#define SPIDEV_MAJOR	153
#define DEVICE_NAME     "spidev"

/*
 * added by xichen 110429 for snmp get l2 table
 */
typedef struct arl_table_t {
	unsigned char mac_address[6];
	int  port;
	int  vlan;
	int  type;
	struct arl_table_t *next;
}arl_table;


#if 1
#define PoE_WHOLE_POWER 256000
#define PoE_DEF_PORT_POWER 32000
#else 
	/* p8 */
#define PoE_WHOLE_POWER 120000
#define PoE_DEF_PORT_POWER 15000
#endif

#define PoE_ENABLE  1
#define PoE_DISABLE 0

typedef enum PoE_MODE{
	PoE_SIGNAL = 0,
	PoE_SPARE,
}PoE_MODE;

typedef enum PoE_PRIORITY{
	PoE_CRITICAL = 0,
	PoE_HIGH,
	PoE_LOW,
}PoE_PRIORITY;

/*
 *    struct poe_conf
 *
 *    1.mode --- poe port mode
 *    2.max_wat --- poe max wattage
 *
 */
#ifdef POE_SI3482
typedef struct poe_desp{
	char *port_desp;
}poe_desp_t;

typedef struct poe_desp_s{
	struct poe_desp poe_names[PoE_NUM];
}poe_desp_s_t;

typedef struct poe_conf {
	int enable;
	int max_power;
	int priority;
}poe_conf_t;

typedef struct poe_nvram_conf_s {
	struct poe_conf poe_port[PoE_NUM];
}poe_nvram_conf;
#else
#if 0
typedef struct poe_conf {
	int enable;
	int max_power;
	int mode;
	int priority;
}poe_conf_t;

typedef struct poe_nvram_conf_s {
	struct poe_conf poe_port[PoE_NUM*3];
	struct poe_conf poe_trunk[TNUM];
}poe_nvram_conf;
#endif
#endif

/* 
 * added by xichen 101115 for port MIB
 */
#define TxOctets 10
#define TxBroadcastPkts 12
#define TxMulticastPkts 13
#define TxUnicastPkts 14
#define TxCollisions 15
#define TxDropPkts 16
#define TxSingleCollision 17
#define TxMultipleCollision 18
#define TxDeferredTransmit 19
#define TxLateCollision 20
#define TxExcessiveCollision 21
#define TxPausePkts 22
#define TxFrameInDisc 23
#define TXPkts64Octets 24
#define TXPkts65to127Octets 25
#define TXPkts128to255Octets 26
#define TXPkts256to511Octets 27
#define TXPkts512to1023Octets 28
#define TXPkts1024to1522Octets 29
#define TXPkts1519to2047Octets 30
#define TXPkts2048to4095Octets 31
#define TXPkts4096to9216Octets 32

#define RxOctets 50
#define RxAlignmentErrors 51
#define RxFCSErrors 52
#define RxUnicastPkts 53
#define RxMulticastPkts 54
#define RxBroadcastPkts 55
#define RXSymbolError 56
#define RxDropPkts 57
#define RxOversizePkts 58
#define RxUndersizePkts 59
#define RxFragments 60
#define RxJabbers 61
#define Pkts64Octets 62
#define Pkts65to127Octets 63
#define Pkts128to255Octets 64
#define Pkts256to511Octets 65
#define Pkts512to1023Octets 66
#define Pkts1024to1522Octets 67
#define Pkts1519to2047Octets 68
#define Pkts2048to4095Octets 69
#define Pkts4096to9216Octets 70
#define RxGoodOctets 71
#define RxPausePkts 72
#define RxExcessSizeDisc 73
#define XeDetect         74

/* add by zwhao 20101022 */
#define INT_FE_PORT_NUM    24
#define INT_GE_PORT_NUM    28
//#define EXT_FE_PORT_NUM    
//#define EXT_GE_PORT_NUM    
#define IMP_PORT           48

static cli_rate_storm cur_rate_storm[PNUM];

#define IOCTL_SPI_READ _IOR(SPIDEV_MAJOR, 0, char *)
#define IOCTL_SPI_WRITE _IOR(SPIDEV_MAJOR, 1, char *)
	
#define	PROMPT	"...................."

//void DEBUG(char *format,...);
int input_num(char *input, int force_hex, int size, uint8 *value);
int parse_input(char *input, char keywd, int size, uint8 *value);

int spi_rreg(int skfd, int page, int reg, uint8 *value, int size, int len);

int spi_wreg(int skfd, int page, int reg, uint8 *value, int size, int len);

int get_port_summary(uint64 * link);

int get_fiber_bits(void);

uint64_t get_link_summary(int skfd, uint64_t fiber, int *Ge_status, int Ge_num);

int port2phy(int pmap);

int clear_mac_by_port(int skfd, int port);

int add_mii_unicast_mac_without_vlan(uint64_t macaddr);

int add_802_1x_mac_without_vlan(uint64_t macaddr, int phy);

int del_static_mac_with_vlan(uint64_t mac);

int del_802_1x_static_unicast_mac(int skfd, int id);

int reset_external_phy(int skfd);

int set_port_disable(int skfd, int i);

int get_port_disable(int port);

int bcm_get_all_txrx_average_rate_bps(char *data);

int bcm_get_port_txrx_average_rate_bps(int port, uint64_t *tx, uint64_t *rx);

int bcm_get_all_txrx_average_rate_pps(char *data);

int bcm_get_port_txrx_average_rate_pps(int port, uint32_t *ucast_tx, uint32_t *ucast_rx, uint32_t *mcast_tx, uint32_t *mcast_rx, uint32_t *bcast_tx, uint32_t *bcast_rx);


int get_port_txrx_status(int skfd, int port, int status, uint32 *high_data, uint32 *low_data);

int get_port_link_speed(uint64_t *val64);

int get_port_link_duplex(uint64_t *m_duplex);

int get_port_link_status(uint64_t *link);

int set_port_flow_control(int skfd, int i, int enable);

int set_fastethernet_port_speed(int skfd, int port, int flag);

int set_gigaethernet_port_speed(int skfd, int port, int flag);

int set_qos_enable_cli(int skfd, int enable);
int set_qos_pause_enable(int skfd, int enable);

int set_qos_schedule_mode(int skfd, int schedule);
int set_qos_8021p_enable(int skfd, int enable);

int set_qos_port_priority(int skfd, int i, int priority);

int set_qos_tos_dscp_enable(int skfd, int flag);

int set_qos_8021p_priority(int skfd, int tag, int priority);

int set_qos_dscp_priority(int skfd, int tag, int priority);

int set_qos_tos_priority(int skfd, int tag, int priority);

int set_qos_mac_priority(int skfd, uint64_t mac, int enable, int priority, int port, int vid);

//int set_port_rate_ingress_enable(int skfd, int i, int enable);

//int set_port_rate_egress_enable(int skfd, int i, int enable);

//int set_port_rate_ingress(int skfd, int i, int count);

//int set_port_rate_egress(int skfd, int i, int count);

int set_port_control_type(int skfd,int type);

//int set_port_storm_ingress_enable(int skfd,int enable);

//int set_port_storm_type(int skfd, int type,int count);

//int set_port_storm_rate(int skfd, int i, int count);
int get_rstp_stp_block(int port,uint8 *link);

int set_rstp_stp_block(int skfd, int port);

int set_rstp_stp_listen(int skfd, int port);

int set_rstp_stp_learn(int skfd, int port);

int set_rstp_stp_forword(int skfd, int port);

int set_rstp_stp_disable(int skfd, int port);

int set_rstp_stp_nostp(int skfd, int port);

int SetBPDUtoDefault(int skfd);

int SetBPDUtoLinkup(int skfd);

int SetBPDUtoForward(int skfd);

int set_vlan_entry(int skfd, uint16 id, uint64_t fmaps, uint64_t umaps);

int set_default_vlan(int skfd);

int set_vlan_8021q_enable(int skfd, int enable);

int set_vlan_default_id(int skfd, int port, int val);

int set_preserve_ingress_non1q(int skfd, int enable);

int set_Drop_Non_1Q_Frame(int skfd, uint64_t bitmaps);

int set_trunk_enable(int skfd, int enable);

int set_trunking_type(int skfd, int entry, int config, int enable);

int set_mirror_capture_enable(int skfd, int enable, int dst, uint64_t *in, uint64_t *out);
int set_bcm_mirror_vlan(int skfd, int enable, int session, int dst, int vlan);


int get_mirror_capture_enable(int skfd);

int set_mirror_capture_port(int skfd, int port);
int get_mirror_capture_port(int skfd);

int set_mirror_ingress(int skfd, uint64_t *phymaps);
int get_mirror_ingress(int skfd, uint64_t *phymaps);

int set_mirror_egress(int skfd,  uint64_t *phymaps);
int get_mirror_egress(int skfd,  uint64_t *phymaps);

int set_mirror_ingress_div(int skfd, int enable);

int set_mirror_egress_div(int skfd, int enable);

int set_mirror_div(int skfd, uint16 val, int type);

int set_multicast_lookup_fail(int skfd, int imgp_enable, int pmap);

int set_igmp_snoop_enable(int skfd, int imgp_enable);

int set_port_auth_dynamic_mode_in_arl(int skfd, int port, int enable);

uint64_t str2mac(char *name);
uint64_t str2mac_blackhole(char *name);

int pmap_port2phy(int bits);

int add_multimac_with_vlan(uint64_t mac, int index);

int add_fwdtable_in_switch(int index, uint64_t fwdmap);

int del_fwdtable_in_switch(int index);
int add_static_mac(int skfd, uint64_t macaddr, int port, int vid);

int del_static_mac(int skfd, uint64_t macaddr,int vid);
int set_port_learn_disable(int skfd, uint64_t learn_maps);
int set_trunk_start(int skfd, int enable);
int set_trunk_mode(int skfd, int mode);
int set_trunk_group(int skfd, int group, int enable, uint64_t port_map);
int set_trunk_port(int skfd, int group, int enable, int port);
void static_lacp_port_status(int skfd, char *link_status, char *real_speed);
/* add by zwhao */
int set_storm_broadcast_config(int skfd,int portid, int enable,int ing_rc_drop_en, int rate);

int set_storm_muticast_config(int skfd,int portid, int enable,int ing_rc_drop_en, int rate);

int set_rate_ingress_uincast_config(int skfd, int port, int enable,int ing_rc_drop_en, int rate);

int set_rate_egress_uincast_config(int skfd, int port, int enable,int ing_rc_drop_en, int rate);

int set_IMP_port_rate_limited(int skfd, int enable);
int set_age_time(int skfd, uint32 time);

int bcm_l2_learn_limit_by_port(int port, int max ,int enflag);
int set_eap_mode(int skfd, uint32 port, uint32 eap_mode);

int cli_get_l2_mac_list(int cli_type, int cli_port, int cli_vid, uint64_t cli_mac, uint64_t cli_multi);

/* added by xichen 110429 for snmp bridge-mib get */
int cli_get_l2_mac_list_new(arl_table *cur_arl_table);

int set_cfp_enable(int skfd, uint64_t bitmaps, int enable);
int set_arp_rate_by_phymaps(int skfd, uint64_t bitmaps, uint32 rate);
//int set_arp_rate_by_portid(int skfd, int portid, uint32 rate);

int set_qos_imp_all(int skfd, int index , int qos);
int set_qos_imp_by_dmac(int skfd, int index , int qos, uint64_t dmac);

int test_cfp(int skfd);
int write_cfp_vid_range_checker_regs(int skfd, int id, int vid_upper, int vid_lower);
int clear_counters(int skfd);
uint64_t cli_get_pmap_by_index(int skfd, uint16 index);

int TRAPLOG(const char *format, ...);

int bcm_port_ability_get(int skfd, int unit, int port, int *enable, int *auge, int *speed, int *duplex, int *flow);
int bcm_port_ability_set(int skfd, int unit, int port, int enable, int auge, int speed, int duplex, int flow);

int bcm_trunk_disable(int skfd, int unit, int tid);
int bcm_trunk_remove_all(int skfd);
int bcm_trunk_set(int skfd, int unit, int tid, uint64 * maps);

int bcm_port_pvid_get(int skfd, int unit, int port, int *pvid);
int bcm_port_pvid_set(int skfd, int unit, int port, int pvid);
int bcm_get_vlan_entry(int skfd, int unit, int vid, uint64_t *mem, uint64_t *untag);
int bcm_set_vlan_entry(int skfd, int unit, int vid, uint64_t *mem, uint64_t *untag);
int bcm_del_vlan_entry(int skfd, int unit, int vid);
int bcm_delete_all_vlan(int skfd, int unit);
int bcm_get_pvid_from_entry(char *pvid_entry, int *p);
int bcm_get_entry_number(char *fwd_entry);
int bcm_vlanfwd_entry(char *fwd_entry, VLAN_ENTRY *fwd);

int bcm_get_all_mac(int skfd);
int bcm_l2_addr_add(int skfd, int unit, int vlan, int port, uint64 *l2addr);
int bcm_l2_addr_del(int skfd, int unit, int vlan, uint64 *l2addr);
int bcm_l2_addr_delete_by_port(int skfd, int unit, int port);

int bcm_get_rstp_stp(int skfd, int unit, int port, int * status);
int bcm_set_rstp_stp(int skfd, int unit, int port, int status);
int bcm_set_bpdu_mac(int skfd, int unit, int which);
int get_port_stp_status(int port, char *ring);

int bcm_rate_set(int skfd, int pps, int flags); 
int bcm_set_broadcast_data(int skfd, int port, int pps);
int bcm_set_multicast_data(int skfd, int port, int pps);

int bcm_rate_get(int skfd, int *pps, int *flags);

int bcm_rate_bandwidth_set(int skfd, int port, uint32 kbits_sec, uint32 kbits_burst); 
int bcm_rate_bandwidth_get(int skfd, int port, uint32 *kbits_sec, uint32 *kbits_burst);

int bcm_auth_mode_set(int skfd, int port, uint32 mode); 
int bcm_auth_mode_get(int skfd, int port, uint32 *mode);
int bcm_l3_intf_create(int skfd, int vlan);
int bcm_l3_intf_delete(int skfd, int vlan);
int bcm_l3_intf_dump(int skfd);
int bcm_l3_host_delete_all(int skfd);
int bcm_l3_host_delete(int skfd, uint32 ip);
int bcm_l3_host_create(int skfd, int dir, int port, int vlan, uint32 ip, uint64 *l2addr, int pri);
int bcm_l3_host_find(int skfd, uint32 ip, int *vlan, int *port, uint64 *l2addr, int *priority);
int bcm_l3v6_host_create(int skfd, int port, int vlan, uint8 *ipv6, uint64 *l2addr, int untag);
int bcm_l3v6_host_delete(int skfd, int port, int vlan, uint8 *ipv6, uint64 *l2addr);
int bcm_l3v6_host_find(int skfd, uint8 *ipv6, int *port, uint64 *l2addr);
int bcm_l3v6_host_init(int skfd);
int bcm_port_protect_set(int skfd, uint64 *pmap);
//ynn 20170810 add
int bcm_get_port_statistics(int skfd, int unit, int port, int type, uint32 *number);
int bcm_port_statistics_clear(int skfd, int unit, int port);
int bcm_register_read(int skfd, uint32 reg,uint64 *value);
int bcm_register_write(int skfd, uint32 reg,uint64 value);
int find_l3_interface(char * name);
int vlan_control_set(int skfd, int type,int arg);
int vlan_control_set(int skfd, int type,int arg);
int pvlan_port_type_set(int skfd, int mode,int portType,int vlan,uint64_t bmaps);
int bcm_port_egress_set(int skfd, int port,uint32 portmap);
int bcm_dhcpsnoop_trustport_set(int skfd, int trustports);
int bcm_dhcpv6_trustport_set(int skfd, int trustports);

#endif
