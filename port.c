#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>

#include "bcmutils.h"


int set_port_disable(int skfd, int i)
{
    return 0;      
}

int set_port_fiber_mode(int skfd, int i)
{
    return 0;
}

int set_fiber_manual(int skfd, int port, int status)
{
    return 0;     	
}

int set_extral_phy_sfp_manual(int skfd, int gphy, int status)
{ 
    return 0;    	
}

/*
 * get port enable/disable status added by xichen 101117
 * modified by eagles for 5331x on 2014/11/16
 */
int get_port_disable(int port)
{
    int val = 0;
    char * enable = nvram_safe_get("port_enable");
    
    if(strlen(enable) < PNUM)
        val = 1;
    else 
    {
        if(*(enable+phy[port]-1) =='1')
            val = 1;
        else
            val = 0;    
    }  
    
    free(enable);
    return  val;           
}

int bcm_get_all_txrx_average_rate_bps(char *data)
{
    void *shm = NULL;
    struct shared_use_st *shared;
    int shmid,running=1;
    int i=0,len=0;
    char * buf = data;
    if(NULL == data)
        return -1;

    shmid = shmget((key_t)SHM_KEY_FLOW_INTERVAL,sizeof(struct shared_use_st),0666|IPC_CREAT);
    if(shmid == -1){
        printf("shmget failed\n");
        exit(0);
    }
    shm = shmat(shmid,0,0);
    if(shm == (void*)-1){
        printf("shmat failed\n");
        exit(0);
    }
    shared = (struct shared_use_st*)shm;
    while(running){
        if(shared->writing){
            sleep(1);
        }else{
            shared->reading = 1;
            for(i = 0; i < PNUM; i++)
            {
                len +=sprintf(buf,"%d,%llX,%llX;",i+1,shared->byte_tx[i],shared->byte_rx[i]);
                buf= data+len;
            }
            //strncpy(shared->data,data,len);
            shared->reading = 0;
            running =0;
        }
    }

    if(shmdt(shm)==-1){
        printf("shmdt failed\n");
        exit(0);
    }
    
    return 0;
}
#if 0
int bcm_get_port_txrx_average_rate_bps_old(int port, uint64_t *tx, uint64_t *rx)
{
    char flow_interval[4096] = {0}, *p1 =NULL, *p2 =NULL, tmp[48] = {0};
    int port_id = 0;
    if(port <= 0)
        return -1;
    void *shm = NULL;
    struct shared_use_st *shared;
    int shmid,running=1;

    shmid = shmget((key_t)SHM_KEY_FLOW_INTERVAL,sizeof(struct shared_use_st),0666|IPC_CREAT);
    if(shmid == -1){
        printf("shmget failed\n");
        exit(0);
    }
    shm = shmat(shmid,0,0);
    if(shm == (void*)-1){
        printf("shmat failed\n");
        exit(0);
    }
    shared = (struct shared_use_st*)shm;
    while(running){
        if(shared->writing){
            sleep(1);
        }else{
            shared->reading = 1;
            strcpy(flow_interval,shared->data);
            shared->reading = 0;
            running =0;
        }
    }
/////
    
    p1 = flow_interval;
    while((p2=strchr(p1, ';'))!=NULL){
        memset(tmp,'\0',sizeof(tmp));
        port_id = 0;
        memcpy(tmp,p1,p2-p1);
        sscanf(tmp,"%d,%llx,%llx",&port_id,tx,rx);
        if(port_id == port)
           break;

        p1 = p2 + 1;
    }
    if(shmdt(shm)==-1){
        printf("shmdt failed\n");
        exit(0);
    }
    
    return 0;
}
#endif
int bcm_get_port_txrx_average_rate_bps(int port, uint64_t *tx, uint64_t *rx)
{

    void *shm = NULL;
    struct shared_use_st *shared;
    int shmid,running=1;
    if(port <= 0)
        return -1;
        
    shmid = shmget((key_t)SHM_KEY_FLOW_INTERVAL,sizeof(struct shared_use_st),0666|IPC_CREAT);
    if(shmid == -1){
        printf("shmget failed\n");
        exit(0);
    }
    shm = shmat(shmid,0,0);
    if(shm == (void*)-1){
        printf("shmat failed\n");
        exit(0);
    }
    shared = (struct shared_use_st*)shm;
    while(running){
        if(shared->writing){
            sleep(1);
        }else{
            shared->reading = 1;
            *tx = shared->byte_tx[port-1];
            *rx = shared->byte_rx[port-1];
            shared->reading = 0;
            running =0;
        }
    }

    if(shmdt(shm)==-1){
        printf("shmdt failed\n");
        exit(0);
    }
    
    return 0;
}


int bcm_get_all_txrx_average_rate_pps(char *data)
{
    void *shm = NULL;
    struct shared_use_st *shared;
    int shmid,running=1;
    int i=0,len=0;
    char * buf = data;
    if(NULL == data)
        return -1;


    shmid = shmget((key_t)SHM_KEY_FLOW_INTERVAL,sizeof(struct shared_use_st),0666|IPC_CREAT);
    if(shmid == -1){
        printf("shmget failed\n");
        exit(0);
    }
    shm = shmat(shmid,0,0);
    if(shm == (void*)-1){
        printf("shmat failed\n");
        exit(0);
    }
    shared = (struct shared_use_st*)shm;
    while(running){
        if(shared->writing){
            sleep(1);
        }else{
            shared->reading = 1;
            for(i = 0; i < PNUM; i++)
            {
                len +=sprintf(buf,"%lx,%lx,%lx,%lx,%lx,%lx;",shared->ucast_tx[i],shared->mcast_tx[i],shared->bcast_tx[i],\
                        shared->ucast_rx[i],shared->mcast_rx[i],shared->bcast_rx[i]);
                buf= data+len;
            }
            //strncpy(shared->data,data,len);
            shared->reading = 0;
            running =0;
        }
    }

    if(shmdt(shm)==-1){
        printf("shmdt failed\n");
        exit(0);
    }
    
    return 0;
}

int bcm_get_port_txrx_average_rate_pps(int port, uint32_t *ucast_tx, uint32_t *ucast_rx, uint32_t *mcast_tx, uint32_t *mcast_rx, uint32_t *bcast_tx, uint32_t *bcast_rx)
{
    char flow_interval[4096] = {0}, *p1 =NULL, *p2 =NULL, tmp[48] = {0};
    int port_id = 0;
    if(port <= 0)
        return -1;
    void *shm = NULL;
    struct shared_use_st *shared;
    int shmid,running=1;

    shmid = shmget((key_t)SHM_KEY_FLOW_INTERVAL,sizeof(struct shared_use_st),0666|IPC_CREAT);
    if(shmid == -1){
        printf("shmget failed\n");
        exit(0);
    }
    shm = shmat(shmid,0,0);
    if(shm == (void*)-1){
        printf("shmat failed\n");
        exit(0);
    }
    shared = (struct shared_use_st*)shm;
    while(running){
        if(shared->writing){
            sleep(1);
        }else{
             shared->reading = 1;
            *ucast_tx=shared->ucast_tx[port-1];
            *mcast_tx=shared->mcast_tx[port-1];
            *bcast_tx=shared->bcast_tx[port-1];
            *ucast_rx=shared->ucast_rx[port-1];
            *mcast_rx=shared->mcast_rx[port-1];
            *bcast_rx=shared->bcast_rx[port-1];
            shared->reading = 0;
            running =0;
        }
    }

    if(shmdt(shm)==-1){
        printf("shmdt failed\n");
        exit(0);
    }
    
    return 0;
}

/*
 * added by xichen 101115 for port MIB status
 */
int get_port_txrx_status(int skfd, int port, int status, uint32 *high_data, uint32 *low_data)
{
    uint64_t val_64;
    int i = port, page_addr = 0;

    if((i >= 1) && (i <= PNUM))
    {
		switch(status) {
			/*
		 	* port tx status
			*/
			case TxOctets:
				/* 00h每07h 64 TxOctets */
				bcm_get_port_statistics64(skfd, 0, phy[i],  TxOctets, &val_64);
//				spi_rreg(skfd, page_addr, 0x00, (uint8*)&val_64, 8, 8);
				*high_data = (uint32)(val_64 >> 32);
				*low_data = (uint32)val_64;
				break;
			case TxDropPkts:	
				/* 08h每0Bh 32 TxDropPkts*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxDropPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x08, (uint8*)low_data, 4, 4);
				break;
			case TxPausePkts:	
				/* 0Ch每0Fh 32 TxPausePkts*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxPausePkts, low_data);
//				spi_rreg(skfd, page_addr, 0x0C, (uint8*)low_data, 4, 4);
				break;
			case TxBroadcastPkts:
				/* 10h每13h 32 TxBroadcastPkts*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxBroadcastPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x10, (uint8*)low_data, 4, 4);
				break;
			case TxMulticastPkts:
				/* 14h每17h 32 TxMulticastPkts */
				bcm_get_port_statistics(skfd, 0, phy[i],  TxMulticastPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x14, (uint8*)low_data, 4, 4);
				break;   			
			case TxUnicastPkts:
				/* 18h每1Bh 32 TxUnicastPkts */
				bcm_get_port_statistics(skfd, 0, phy[i],  TxUnicastPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x18, (uint8*)low_data, 4, 4);
				break;
			case TxCollisions:
				/* 1Ch每1Fh 32 TxCollisions*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxCollisions, low_data);
//				spi_rreg(skfd, page_addr, 0x1C, (uint8*)low_data, 4, 4);
				break;
			case TxSingleCollision:
				/* 20h每23h 32 TxSingleCollision*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxSingleCollision, low_data);
//				spi_rreg(skfd, page_addr, 0x20, (uint8*)low_data, 4, 4);
				break;
			case TxMultipleCollision:
				/* 24h每27h 32 TxMultipleCollision*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxMultipleCollision, low_data);
//				spi_rreg(skfd, page_addr, 0x24, (uint8*)low_data, 4, 4);
				break;
			case TxDeferredTransmit:
				/* 28h每2Bh 32 TxDeferredTransmit*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxDeferredTransmit, low_data);
//				spi_rreg(skfd,page_addr, 0x28, (uint8*)low_data, 4, 4);
				break;
			case TxLateCollision:
				/* 2Ch每2Fh 32 TxLateCollision*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxLateCollision, low_data);
//				spi_rreg(skfd, page_addr, 0x2C, (uint8*)low_data, 4, 4);
				break;
			case TxExcessiveCollision:
				/* 2Ch每2Fh 32 TxExcessiveCollision*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxExcessiveCollision, low_data);
//				spi_rreg(skfd, page_addr, 0x30, (uint8*)low_data, 4, 4);
				break;
			case TxFrameInDisc:
				/* 34h每37h 32 TxFrameInDisc*/
				bcm_get_port_statistics(skfd, 0, phy[i],  TxFrameInDisc, low_data);
//				spi_rreg(skfd, page_addr, 0x34, (uint8*)low_data, 4, 4);
				break;
	
			/*
			* port rx status
			*/
			case RxOctets:
				/* 50h每57h  64 RxOctets */
				bcm_get_port_statistics64(skfd, 0, phy[i],  RxOctets, &val_64);
//				spi_rreg(skfd, page_addr, 0x50, (uint8*)&val_64, 8, 8);
    			*high_data = (uint32)(val_64 >> 32);
    			*low_data = (uint32)val_64;				
    			break;
     		case RxUndersizePkts:
        		/* 58h每5Bh  32 RxUndersizePkts*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxUndersizePkts, low_data);
//				spi_rreg(skfd, page_addr, 0x58, (uint8*)low_data, 4, 4);
    			break;
    		case RxPausePkts:
    			/* 5Ch每5Fhh 32 RxPausePkts*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxPausePkts, low_data);
//				spi_rreg(skfd, page_addr, 0x5C, (uint8*)low_data, 4, 4);
    			break;
    		case Pkts64Octets:
        		/* 60h每63h 32 Pkts64Octets*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  Pkts64Octets, low_data);
//				spi_rreg(skfd, page_addr, 0x60, (uint8*)low_data, 4, 4);
    			break;
     		case Pkts65to127Octets:
        		/* 64h每67 32 Pkts65to127Octets*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  Pkts65to127Octets, low_data);
//				spi_rreg(skfd, page_addr, 0x64, (uint8*)low_data, 4, 4);
    			break;
     		case Pkts128to255Octets:
        		/* 68h每6Bh 32 Pkts128to255Octets*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  Pkts128to255Octets, low_data);
//				spi_rreg(skfd, page_addr, 0x68, (uint8*)low_data, 4, 4);
    			break;
     		case Pkts256to511Octets:
        		/* 6Ch每6Fh 32 Pkts256to511Octets*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  Pkts256to511Octets, low_data);
//				spi_rreg(skfd, page_addr, 0x6C, (uint8*)low_data, 4, 4);
    			break;
     		case Pkts512to1023Octets:
        		/* 70h每73h 32 Pkts512to1023Octets*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  Pkts512to1023Octets, low_data);
//				spi_rreg(skfd, page_addr, 0x70, (uint8*)low_data, 4, 4);
    			break;
     		case Pkts1024to1522Octets:
        		/* 74h每77h  32 Pkts1024to1522Octets*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  Pkts1024to1522Octets, low_data);
//				spi_rreg(skfd, page_addr, 0x74, (uint8*)low_data, 4, 4);
    			break;
    		case RxOversizePkts:
        		/* 78h每7Bh 32 RxOversizePkts*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxOversizePkts, low_data);
//				spi_rreg(skfd, page_addr, 0x78, (uint8*)low_data, 4, 4);
    			break;
    		case RxJabbers:
       			/* 7Ch每7Fh  32 RxJabbers*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxJabbers, low_data);
//				spi_rreg(skfd, page_addr, 0x7C, (uint8*)low_data, 4, 4);
    			break;
    		case RxAlignmentErrors:
    			/* 80h每83h 32 RxAlignmentErrors*/
    			bcm_get_port_statistics(skfd, 0, phy[i],  RxAlignmentErrors, low_data);
//				spi_rreg(skfd, page_addr, 0x80, (uint8*)low_data, 4, 4);
    			break;
    		case RxFCSErrors:
        		/* 84h每87h  32 RxFCSErrors*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxFCSErrors, low_data);
//				spi_rreg(skfd, page_addr, 0x84, (uint8*)low_data, 4, 4);
    			break;
    		case RxGoodOctets:
        		/* 88h每8Fh 64 RxGoodOctets*/
        		bcm_get_port_statistics64(skfd, 0, phy[i],  RxGoodOctets, &val_64);
//				spi_rreg(skfd, page_addr, 0x88, (uint8*)&val_64, 8, 8);
    			*high_data = (uint32)(val_64 >> 32);
    			*low_data = (uint32)val_64;				
    			break;
    		case RxDropPkts:
        		/* 90h每93h  32 RxDropPkts */
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxDropPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x90, (uint8*)low_data, 4, 4);
    			break;
    		case RxUnicastPkts:
        		/* 94h每97h 32 RxUnicastPkts */
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxUnicastPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x94, (uint8*)low_data, 4, 4);
    			break;
    		case RxMulticastPkts:
        		/* 98h每9Bh 32 RxMulticastPkts*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxMulticastPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x98, (uint8*)low_data, 4, 4);    				
    			break;
    		case RxBroadcastPkts:
        		/* 9Ch每9Fh 32 RxBroadcastPkts*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxBroadcastPkts, low_data);
//				spi_rreg(skfd, page_addr, 0x9C, (uint8*)low_data, 4, 4);     				
    			break;
    		case RxFragments:
        		/* A4h每A7h  32 RxFragments*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxFragments, low_data);
//				spi_rreg(skfd, page_addr, 0xA4, (uint8*)low_data, 4, 4);     				
    			break;
    		case RxExcessSizeDisc:
        		/* C0h每C3h 32 RxExcessSizeDisc*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RxExcessSizeDisc, low_data);
//				spi_rreg(skfd, page_addr, 0xC0, (uint8*)low_data, 4, 4); 
                *low_data = 0;    				
    			break;
    		case RXSymbolError:
        		/* ACh每AFh  32 RXSymbolError*/
        		bcm_get_port_statistics(skfd, 0, phy[i],  RXSymbolError, low_data);
//				spi_rreg(skfd, page_addr, 0xAC, (uint8*)low_data, 4, 4);
    			break;
			default:
				*high_data = 0;
				*low_data = 0;
				break;
    	}
    } else {
		*high_data = 0;
		*low_data = 0;
		return -1;
    }

    return 0;      
}


int bcm_get_port_speed(int skfd, int unit, int port, int *speed)
{ 
	uint32 data[8];
	
    data[0] = unit;
    data[1] = port;
    data[2] = (int *) speed;
       
	if (ioctl(skfd, PORT_SPEED_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	//DEBUG("[%s:%d] get port speed: %d", __FUNCTION__, __LINE__, *(int *)speed);	
	return 0;
}


int bcm_get_port_statistics(int skfd, int unit, int port, int type, uint32 *number)
{ 
	uint32 data[8];
	
    data[0] = unit;
    data[1] = port;
    data[2] = type;
    data[3] = (uint32 *) number;
       
	if (ioctl(skfd, PORT_Statis_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	//DEBUG("[%s:%d] get port statistics: %d", __FUNCTION__, __LINE__, *(uint32 *)number);	
	
	return 0;
}
//ynn 20170807 add
int bcm_port_statistics_clear(int skfd, int unit, int port)
{ 
	uint32 data[8];
	
    data[0] = unit;
    data[1] = port;
       
	if (ioctl(skfd, PORT_Statis_CLEAR, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
        //DEBUG("clear [%u]%u statistics\n",port,PORT_Statis_CLEAR);	
	return 0;
}

int bcm_get_port_statistics64(int skfd, int unit, int port, int type, uint64 *number)
{ 
	uint32 data[8];
	
    data[0] = unit;
    data[1] = port;
    data[2] = type;
    data[3] = (uint64 *) number;
       
	if (ioctl(skfd, PORT_Statis_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
//	DEBUG("[%s:%d] get port statistics: 0x%08x%08x", __FUNCTION__, __LINE__, (uint32)((*number) >> 32), (uint32)(*number));	
	return 0;
}

int bcm_clear_all_counters()
{
    int skfd;
	uint32 data[8];
	
	if((skfd = open(DEVICE_FILE_NAME, 0)) < 0) 
        return -1;
	
	if (ioctl(skfd, CLEAR_CNT, &data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
        close(skfd);
		return -1;    
	}

    close(skfd);
    return 0;    
}

/*
 * get port real link duplex added by qianfeng 101215
 */
int get_port_link_duplex(uint64_t *m_duplex)
{
}

int bcm_get_port_duplex(int skfd, int unit, int port, int *duplex)
{ 
	uint32 data[8];
	
    data[0] = unit;
    data[1] = port;
    data[2] = duplex;
    
	if (ioctl(skfd, PORT_DUPLEX_GET, data) < 0) {
		DEBUG("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	//DEBUG("[%s:%d] get port duplex: %d", __FUNCTION__, __LINE__, *duplex);
	return 0;
}

int set_fastethernet_port_speed(int skfd, int port, int flag){}
int set_gigaethernet_port_speed(int skfd, int port, int flag){}
int set_port_flow_control(int skfd, int i, int enable){}

int bcm_port_ability_get(int skfd, int unit, int port, int *enable, int *auge, int *speed, int *duplex, int *flow)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = port;
    data[2] = enable;
    data[3] = auge;
    data[4] = speed;
    data[5] = duplex;
    data[6] = flow;

	if (ioctl(skfd, PORT_STAT_GET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	//DEBUG("[%s:%d] get port duplex: %d", __FUNCTION__, __LINE__, *duplex);
	
	return 0;
}

int bcm_port_ability_set(int skfd, int unit, int port, int enable, int auge, int speed, int duplex, int flow)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = unit;
    data[1] = port;
    data[2] = enable;
    data[3] = auge;
    data[4] = speed;
    data[5] = duplex;
    data[6] = flow;

	if (ioctl(skfd, PORT_STAT_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_reset_external_phy(int skfd, int port)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = 0;
    data[1] = port;

	if (ioctl(skfd, PORT_PHY_RESET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	
	return 0;
}

int bcm_port_info_get(int skfd, int type, int unit, int pmaps)
{
	uint32 data[8];

	memset(&data, '\0', sizeof(uint32) * 8);
    data[0] = type;
    data[1] = unit;
    data[2] = pmaps;

	if (ioctl(skfd, PORT_DUMP, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_rx_show(int skfd, int unit)
{
//	uint32 data[8];
//
//	memset(&data, '\0', sizeof(uint32) * 8);
//    data[0] = unit;
//
//	if (ioctl(skfd, RX_DUMP, &data) < 0) {
//		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
//		return -1;    
//	}
	return 0;
}

//port_speed=0000000000000000000000000000
//port_duplex=4444444444444444444444444444
//port_flow=1111111111111111111111111111
//port_enable=1111111111111111111111111111
int nvram_port_seepd_get(char *conf, int port, int *autoge, int *speed)
{
    if(strlen(conf) != PNUM)
    {  
        *autoge = 1;
        *speed = 0;  
    }else
    {             
        switch(*(conf +(port-1)))
        {
            case '0':
                *autoge = 1;
                *speed = 0;  
            	break;
            	
            case '1':
                *autoge = 0;
                *speed = 1;  
                break;
            	
            case '2':
                *autoge = 0;
                *speed = 2; 
                break;
            	
            case '3':
                *autoge = 0;
                *speed = 3; 
                break;
            	
            case '4':
                *autoge = 0;
                *speed = 4;  
                
            default:
                *autoge = 1;
                *speed = 0;  
                break;
        } 
    }
    
    return 0;
}

int nvram_port_duplex_get(char *conf, int port, int *duplex)
{ 
    if(strlen(conf) != PNUM)
    {    
        *duplex = 4;
        return 0;
    }     
  
    *duplex = *(conf +(port-1)) - '0';
    return 0;
}

int nvram_port_enable_get(char *conf, int port, int *enable)
{ 
    if(strlen(conf) != PNUM)
    {    
        *enable = 1;
        return 0;
    }     
  
    *enable = *(conf +(port-1)) == '0'?0:1;
    return 0;
}

int nvram_port_flow_get(char *conf, int port, int *flow)
{
    if(strlen(conf) != PNUM)
    {    
        *flow = 0;
        return 0;
    }     
  
    *flow = *(conf +(port-1)) == '1'?1:0;

    return 0;
}

int bcm_port_mtu_set(int skfd, int port, int mtu)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = mtu;

	if (ioctl(skfd, PORT_MTU_SET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_port_mtu_get(int skfd, int port, int *mtu)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = port;
    data[1] = mtu;

	if (ioctl(skfd, PORT_MTU_GET, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

int bcm_port_protect_set(int skfd, uint64 *pmap)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = pmap;

	if (ioctl(skfd, PORT_PROTECTED, data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

