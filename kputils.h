#ifndef _KPUTILS_H_
#define _KPUTILS_H_

#define InUcast         0
#define InMulticast     1
#define InBroadcast     2
#define InError         3
#define OutUcast        4
#define OutMulticast    5
#define OutBroadcast    6
#define OutError        7

//in bcmutils.h
//#define TxOctets        10
//#define TxBroadcastPkts 12
//#define TxMulticastPkts 13
//#define TxUnicastPkts 14
//#define TxCollisions 15
//#define TxDropPkts 16
//#define TxSingleCollision 17
//#define TxMultipleCollision 18
//#define TxDeferredTransmit 19
//#define TxLateCollision 20
//#define TxExcessiveCollision 21
//#define TxPausePkts 22
//#define TxFrameInDisc 23
//
//#define RxOctets 50
//#define RxAlignmentErrors 51
//#define RxFCSErrors 52
//#define RxUnicastPkts 53
//#define RxMulticastPkts 54
//#define RxBroadcastPkts 55
//#define RXSymbolError 56
//#define RxDropPkts 57
//#define RxOversizePkts 58
//#define RxUndersizePkts 59
//#define RxFragments 60
//#define RxJabbers 61
//#define Pkts64Octets 62
//#define Pkts65to127Octets 63
//#define Pkts128to255Octets 64
//#define Pkts256to511Octets 65
//#define Pkts512to1023Octets 66
//#define Pkts1024to1522Octets 67
//#define RxGoodOctets 68
//#define RxPausePkts 69
//#define RxExcessSizeDisc 70

/* port status get */
#define PORT_SPEED_GET 	        0
#define PORT_LINK_GET 	        1
#define PORT_DUPLEX_GET 		102


/* port Statistics get */
#define PORT_Statis_GET 	    3
/* all mac list */ 
#define MAC_LIST_GET 		    4

/* port status set */
#define PORT_STAT_GET           5
#define PORT_TYPE_SET 		    6
#define PORT_STAT_SET           7

///* trunk */
#define TRUNK_SET 		        8
#define TRUNK_DEL 	            9

/* vlan entry, pvid*/
#define VLAN_PVID_GET 		    10
#define VLAN_PVID_SET 		    11
#define VLAN_SET 		        12
#define VLAN_DEL 		        13
#define VLAN_GET	            14
#define VLAN_DEL_ALL	        15

/* mac deal: get, add, del */
#define MAC_STATIC_ADD 	        16
#define MAC_STATIC_DEL	        17
#define MAC_STATIC_GET          18
#define MAC_DEL_BY_PORT	        19


///* rstp */
#define SET_PORT_STP_STATUS     20
#define GET_PORT_STP_STATUS	    21
#define PORT_BPDU_ENABLE	    22
#define BPDU_MAC_MODIFY		    23
#define PORT_TX_ENABLE	        24
#define GET_SWITCH_LINK	        25

/* rate control */
#define RATE_INGRESS_GET        28
#define RATE_INGRESS_SET        29

/* storm */
#define STORM_GET               30
#define STORM_SET               31

/* QOS */
#define QOS_PORT_PRORITY        32
#define QOS_ENABLE_SET          33
/*÷ÿ∏¥∂®“Â
#define COSQ_SCHED_SET          34
#define COSQ_SCHED_GET          35 
#define COSQ_MAPPING_SET        36
#define COSQ_MAPPING_GET        37
#define COSQ_INIT               38*/
#define	QOS_DROP_PROFILE		34


/* advanced mac */
#define MAC_LIMIT               40
#define PORT_AUTH_SET           41
#define PORT_AUTH_GET           42
#define MAC_AUTH_ADD            43
#define MAC_AUTH_DEL            44

#define GET_LINK	            45
#define PORT_DUMP               46
#define MIRROR_SET              47
#define MULMAC_LIST_GET         48
#define MULMAC_ADD_PORT         49
#define MULMAC_ADD_PMAP         50
#define MULMAC_DEL_ALL          51
#define MULMAC_DEL_ADDR         52
#define IGMP_SNOOP_SET          53
#define TRUNK_SET_PORT          54
#define PORT_PHY_RESET          55
#define LED_INIT                56
#define MAC_LIST_DEBUG_GET      57
/* QOS */
#define COSQ_SCHED_SET          61
#define COSQ_SCHED_GET          62 
#define COSQ_MAPPING_SET        63
#define COSQ_MAPPING_GET        64
#define COSQ_INIT               65
#define PORT_COSQ               66

#define I2C_READ                71
#define I2C_WRITE               72
#define CCTV_MODE               73
#define AGE_TIME                74
#define RATE_EGRESS_GET         75
#define RATE_EGRESS_SET         76
#define CLEAR_CNT               77
/* storm control */
#define STORM_BCAST             78
#define STORM_MCAST             79
#define STORM_UCAST             80

#define TRUNK_DEL_ALL           85

#define RTC_READ                90
#define RTC_WRITE               91
#define TEMPERATURE             92



// acl
#define MAC_ACL_ADD             93 
#define MAC_ACL_DEL_ALL         94
#define ACL_DUMP                95
#define IP_STD_ACL_ADD          96
#define ACL_COUNTER_GET         97
#define ACL_CREATE_ALL_ENTRY    98
#define IP_EXT_ACL_ADD          99
#define ACL_ENTRY_DUMP          100
#define POLICY_MAC_ACL_ADD      101
#define POLICY_IP_STD_ACL_ADD   103
#define POLICY_IP_EXT_ACL_ADD   104
#define POLICY_OTHER_ADD        105
#define IP6_STD_ACL_ADD         106
#define POLICY_IP6_STD_ACL_ADD  107

#define SFP_READ                110

#define L3_HOST_FIND_PORT       111
#define L3_HOST_DELETE_ALL 	    112
#define L3_ROUTE_SET 	        113
#define L3_ROUTE_DEL 	        114
#define L3_ROUTE_GET 	        115
#define L3_ROUTE_DUMP 	        116
#define L3_ROUTE_DELETE_ALL 	117

#define GET_TYPE                120

//trunk info
#define TRUNK_GET_PORT          121


//l3 ipmc
#define IPMC_ENABLE             150
#define IPMC_ADD                151
#define IPMC_DEL                152
#define IPMC_DEL_ALL            153  
#define IPMC_EGRESS_ADD         154
#define IPMC_EGRESS_DEL         155
#define IPMC_EGRESS_DEL_ALL     156  
#define IPMC_EGRESS_PORT        157  
#define IPMC_ZEGRESS_ADD         158
#define IPMC_ZEGRESS_DEL         159 
#define IPMC_ZEGRESS_PORT        160
#define IPMC_DUMP                161

//QoS DSCP
#define DSCP_MAPPING_SET        170

#define PORT_MTU_SET            180
#define PORT_MTU_GET            181

#define MIRROR_SET_VLAN         190
#define MIRROR_SESSION_DEL      191


#define MSTP_ENABLE             200
#define MSTP_PORT_SET           201
#define MSTP_PORT_GET           202
#define MSTP_ADD_VLAN           203
#define MSTP_REMOVE_VLAN        204

#define VLAN_GET_ALL            220

#define VLAN_DTAG_TPID          230
#define VLAN_DTAG_SET           231
#define VLAN_DTAG_DELETE        232
#define VLAN_DTAG_MODE          234
#define DTAG_DUMP_MODE          235
#define DTAG_DUMP_PORT          236
#define DTAG_DUMP_ENTRY         237

#define MAC_VLAN_ADD            238
#define MAC_VLAN_DELETE         239
#define MAC_VOICE_VLAN          240

#define PROTOCOL_VLAN_ADD       242
#define PROTOCOL_VLAN_DELETE    243
#define L3V6_HOST_CREATE	    250
#define L3V6_HOST_DUMP	        251
#define L3V6_HOST_DELETE	    252
#define L3V6_HOST_DELETE_ALL 	253
#define L3V6_HOST_FIND 	        254
#define L3V6_HOST_INIT 	        255


#define PORT_PROTECTED 	        260
#define PORT_Statis_CLEAR	    262
#define POE_READ                270
#define POE_WRITE               271
#define POE_RESET               272
#define POE_INIT                273

#define DHCP_SNOOP_ENABLE       280
#define ARP_SNOOP_ENABLE        281
#define IPADDR_GUARD_INIT       282
#define IPADDR_POLICE_SET       283
#define IPADDR_GUARD_SET        284
#define IPADDR_GUARD_CLEAR      285

#define DHCPv6_SNOOP_ENABLE     286
#define DHCPv6_SNOOP_TRUST      287
#define DHCP_SNOOP_TRUST        288
#define FAST_MAC_DEL_BY_PORT    290
#define FAST_MAC_STATIC_ADD 	291
#define FAST_MAC_STATIC_DEL	    292

//erps
#define ERPS_BPDU_LEVEL_SET     350
#define ERPS_STG_CREATE         351
#define ERPS_STG_DESTROY        352
#define ERPS_STG_STATE_SET      353
#define ERPS_STG_STATE_GET      354
#define ERPS_STG_ADD_VLAN       355
#define ERPS_STG_REMOVE_VLAN    356
#define BCM_STG_INIT            357   


//QinQ
#define QINQ_TPID_SET           360
#define QINQ_TPID_GET           361
#define QINQ_ENABLE             362

//PVLAN
#define PVLAN_PORT_SET          370
#define PVLAN_PORT_GET          371
#define PVLAN_DISABLE           372

#define VLAN_TRANLATE_SET       375
#define VLAN_TRANLATE_DELETE    376

//bcm control
#define BCM_REGISTER_READ       380
#define BCM_REGISTER_WRITE      381
#define VLAN_CONTROL_SET        382
#define VLAN_CONTROL_GET        383
#define VLAN_CONTROL_VLAN_GET   384 
#define VLAN_CONTROL_VLAN_SET   385 

#define BCM_PORT_EGRESS_SET     386 
#define BCM_PORT_EGRESS_GET     387 

#define BCM_PORT_INGRESS_FILTER_SET  388 
#define BCM_PORT_INGRESS_FILTER_GET  389

#define BCM_MAC_BLACKHOLE_SET	 390
#define BCM_MAC_BLACKHOLE_GET	 391
#define AGE_TIME_GET			 392

enum {
	L3_INTF_CREATE = 393,
	L3_INTF_DUMP,
	L3_HOST_CREATE,   
	L3_HOST_DUMP,  
	L3_HOST_DELETE,  
	L3_INTF_DELETE	, 
	L3_INTF_DELETE_ALL, 
	L3_ARP_SET, 
	L3_ARP_GET, 
	L3_HOST_FIND,
	PORT_BASED_VLAN_SET,
	PORT_BASED_VLAN_DEL,
};
#endif
