#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bcmutils.h"

int cli_rate_storm_get(unsigned char *addr);

#include <stdarg.h>
int COMMAND(const char *format, ...)
{
    char buf[1024]="";
    va_list arg;
    FILE *fp;
    va_start(arg, format);
    vsnprintf(buf,1024, format, arg);
    va_end(arg);
    fp=fopen("/tmp/cmd_agent","w");
    if(fp==NULL)
        return 1;
    fwrite(buf,1024,1,fp);
    fclose(fp);
    usleep(1);
    return 0;
}

/*
 *  Function : check_vlan_include
 *  Purpose:
 *     check the port include port range or not
 *  Parameters:
 *     vlanid    - VLAN ID
 *     vlan_str  - VLAN range format: 1,2-3 (max VLAN_MAX_NUM)
 *  Returns:
 *     1 - The vlan id include in the vlan range
 *     0 - The vlan id out of the vlan range
 *
 *  Author  : xichen
 *  Date    :2011/2/14 (Valentine's Day ^_^)
 */
int check_vlan_include(int vlanid, char *vlan_str)
{
    char port[256][8], *p, *p1;
    int i = 0, j, min = 0, max = 0;

    memset(&port, '\0', 256*8);

	p1 = vlan_str;
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
    	if((p1 = strchr(port[j], '-')) != NULL) {
    		max = atoi(p1+1);
			if(vlanid < min) {
				return 0;
			}else if(vlanid <= max) {
				return 1;
			}
    	}else if(vlanid == min) {
			return 1;
    	}
    }

    return 0;
}

/*
 *  Function : cli_nvram_safe_get
 *  Purpose:
 *     get nvram param, and initialize it when value is NULL
 *  Parameters:
 *     type   - Config Type (CLI_NVRAM_TYPE)
 *     param  - Param String
 *  Returns:
 *     address  -  string buff address
 *
 *  Author  : xichen
 *  Date    :2011/2/14 (Valentine's Day ^_^)
 */
char *cli_nvram_safe_get(int type, char *param)
{
	int portid, i;
	char buff[512], tmp[32];
	char *value_str = NULL;

    memset(tmp, '\0', 32);
    memset(buff, '\0', 512);

	value_str = nvram_safe_get(param);
	if(0 == strlen(value_str)) {
		memset(buff, 0, sizeof(buff));
		switch(type){
			case CLI_ALL_ONE:
				for(portid = 1; portid <= PNUM; portid++)
					strcat(buff, "1");
				break;

			case CLI_AGG_ALL_ONE:
				for(portid = 1; portid <= TNUM; portid++)
					strcat(buff, "1");
				break;
				
			case CLI_ALL_ZERO:
				for(portid = 1; portid <= PNUM; portid++)
					strcat(buff, "0");
				break;
			
			case CLI_AGG_ALL_ZERO:
				for(portid = 1; portid <= TNUM; portid++)
					strcat(buff, "0");
				break;

			case CLI_DUPLEX_ALL_AUTO:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "%d",PORT_DUPLEX_AUTO);
					strcat(buff, tmp);
				}
				break;

			case CLI_SPEED_ALL_AUTO:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "%d",PORT_SPEED_AUTO);
					strcat(buff, tmp);
				}
				break;

			case CLI_ALL_DES:
				for(portid = 1; portid <= PNUM; portid++)
					strcat(buff,";");
				break;

			case CLI_COMMA:
				for(portid = 1; portid <= PNUM; portid++)
					strcat(buff, ",");
				break;
			
			case CLI_AGG_COMMA:
				for(portid = 1; portid <= TNUM; portid++)
					strcat(buff, ",");
				break;

			case CLI_COMMA_ONE:
				for(portid = 1; portid <= PNUM; portid++) {
					if(1 == portid)
						strcat(buff, "1");
					else
						strcat(buff, ",1");
				}
				break;

			case CLI_COMMA_ZERO:
				for(portid = 1; portid <= PNUM; portid++) {
					if(1 == portid)
						strcat(buff, "0");
					else
						strcat(buff, ",0");
				}
				break;

			case CLI_PORT_NAME:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "port%d,",portid);
					strcat(buff, tmp);
				}
				break;

			case CLI_VLAN_FORWARD:
				sprintf(buff, "0:1:Default:1-%d:1-%d;", PNUM, PNUM);
				break;

			case CLI_RATE_ALL:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "%d,,;", portid);
					strcat(buff, tmp);
				}
				break;

			case CLI_RC_NUM:
				for(portid = 1; portid <= PNUM; portid++) {
					if(1 == portid)
						strcat(buff, "00");
					else
						strcat(buff, ",00");
				}
				strcat(buff, ";");
				for(portid = 1; portid <= PNUM; portid++) {
					if(1 == portid)
						strcat(buff, "00");
					else
						strcat(buff, ",00");
				}
				break;
				
			case CLI_RATE:
				for(portid = 1; portid <= PNUM; portid++) 
					strcat(buff, ",");
				break;

			case CLI_RSTP_CONFIG:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "0,128,2,0,0,0,0,0;");/*shanming.ren 2011-9-19*/
					strcat(buff, tmp);
				}
				break;
			
			case CLI_AGG_RSTP_CONFIG:
				for(portid = 1; portid <= TNUM; portid++) {
					sprintf(tmp, "0,128,2,0,0,0,0,0;");/*yaohui.jiang 2012-04-26,add for aggregation's rstp config*/
					strcat(buff, tmp);
				}
				break;
				
			case CLI_MSTP_PORT_CP_CONFIG:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "0,128;");/* exp: "0,128;..." */
					strcat(buff, tmp);
				}
				break;
				
			case CLI_MSTP_PORT_CONFIG:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "2,0,1,0,0,0,0,0;"); /* exp: "2,0,1,0,0;..." */
					strcat(buff, tmp);
				}
				break;

			case CLI_MAC_ADVANCED:
				for(portid = 1; portid <= PNUM; portid++) {
				    /*
					if(1 == portid)
						strcat(buff, "8191");
					else
						strcat(buff, ",8191");
						*/
					strcat(buff, "8191,");
				}
				break;

			case CLI_DOT1X_CONFIG:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "1,0,4096;");
					strcat(buff, tmp);
				}
				break;

			case CLI_PROTECT_CONFIG:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "%d,0;",portid);
					strcat(buff, tmp);
				}
				break;
				
			case CLI_AGG_PROTECT_CONFIG:
				for(portid = 1; portid <= TNUM; portid++) {
					sprintf(tmp, "%d,0;",portid);
					strcat(buff, tmp);
				}
				break;

            case CLI_DSCP_CONFIG:
				for(i = 0; i < 64; i++){
                    sprintf(tmp, "%d",i>>3);
					strcat(buff, tmp);
				}
				break;
			case CLI_COS_CONFIG:
				for(i=0; i<8; i++){
					sprintf(tmp, "%d",i);
					strcat(buff, tmp);
				}
				break;
			case CLI_TRUNK_VLAN:
				for(portid = 1; portid <= PNUM; portid++) {
					sprintf(tmp, "%d:;",portid);
					strcat(buff, tmp);
				}
				break;
			
			case CLI_PORT_ACL:
			case CLI_PORT_POLICY:
				for(portid = 1; portid <= PNUM; portid++) {
					strcat(buff, ",");
				}
				break;
			case CLI_CPU_ACL:
				strcat(buff, ",");
				break;
#ifdef POE_SI3482 
			case CLI_POE_PORT:
				for(portid = 1; portid <= PoE_NUM; portid++) {
//#if (PORTPOWER_MAX==46)
					sprintf(tmp, "%d,%d,%d;", 1, PORTPOWER_MAX, 0);
//#else
//					sprintf(tmp, "%d,%d,%d;", 1, PORTPOWER_MAX, 0);
//#endif
					strcat(buff, tmp);
				}
				break;									
			case CLI_POE_DESP:
				for(portid = 1; portid <= PoE_NUM; portid++) {
					strcat(buff, "port");
					if(portid<PNUM)
						sprintf(tmp, "%d,", portid);
					else
						sprintf(tmp, "%d", portid);	
					strcat(buff, tmp);
				}
				break;
#endif
									
			default:
				break;
		}
		scfgmgr_set(param, buff);
		free(value_str);
		value_str = nvram_safe_get(param);
	}
	return value_str;
}

static void cli_poe_nvram_prase(unsigned char *addr)
{
	char *p;
	int portid;
	char *poe_port_conf = NULL;

	/* poe config struct */
	struct poe_conf *c_poe = NULL;
	poe_nvram_conf *p_poe = NULL;

	p_poe = (poe_nvram_conf *)addr;
	c_poe = p_poe->poe_port;

	poe_port_conf = cli_nvram_safe_get(CLI_POE_PORT, "poe_port_conf");
//printf("cli_poe_nvram_prase poe_port_conf=%s ----------\n",poe_port_conf);
	
	p = poe_port_conf;
	for(portid = 1; portid<=PoE_NUM; portid++) {
#ifdef POE_SI3482
		c_poe->enable = atoi(p);
		p = strchr(p, ',');
		p++;

		c_poe->max_power = atoi(p);
		p = strchr(p, ',');
		p++;

		c_poe->priority = atoi(p);
		p = strchr(p, ';');
		p++;
#else
		c_poe->enable = atoi(p);
		p = strchr(p, ',');
		p++;

		c_poe->max_power = atoi(p);
		p = strchr(p, ',');
		p++;

		c_poe->mode = atoi(p);
		p = strchr(p, ',');
		p++;

		c_poe->priority = atoi(p);
		p = strchr(p, ';');
		p++;
#endif
		c_poe++;

	}

	free(poe_port_conf);

	return;
}

#ifdef POE_SI3482
static void cli_poe_desp_nvram_prase(unsigned char *addr){
	char *p_start, *p_end;
	int portid;
	char *poe_port_names = NULL;

	/* poe config struct */
	struct poe_desp *c_poe = NULL;
	struct poe_desp_s *p_poe = NULL;

	p_poe = (struct poe_desp_s *)addr;
	c_poe = p_poe->poe_names;
	
	memset((void *)c_poe, NULL, PoE_NUM);
	
	poe_port_names = cli_nvram_safe_get(CLI_POE_DESP, "port_desps");

	p_start = p_end = poe_port_names;
	
	for(portid = 1; portid<=PoE_NUM; portid++) {
		char *temp;
		char *found;
		p_end = strchr(p_start,',');
		if(p_end == 0){
			int remain_len = strlen(poe_port_names)-(p_start-poe_port_names)+1;
			temp = (char * )malloc(sizeof(char)*remain_len);
			found = temp;
			memset(temp, '\0',  sizeof(char)*remain_len);
			while(*p_start!='\0'){
			 *temp++ = *p_start++;
			}
			c_poe->port_desp = found;
			break;
		}
		temp = (char * )malloc(sizeof(char)*(p_end-p_start+1));
		memset(temp, '\0', sizeof(char)*(p_end-p_start+1));
		found = temp;
		while(p_start<=p_end){
          *temp++ = *p_start++;
        }
		*--temp = '\0';
		c_poe->port_desp = found;
		c_poe++;
	}

	free(poe_port_names);

	return;
}

#include "socket_tools.h"
static void cli_poe_conf_store_to_nvram(unsigned char *addr){
	int portid;
	char poe_nvram_conf[512], tmp[32];
	struct poe_conf *c_poe = NULL;
	//poe_nvram_conf *p_poe = NULL;
	struct poe_nvram_conf_s *p_poe = NULL;

	p_poe = (struct poe_nvram_conf_s *)addr;
	c_poe = p_poe->poe_port;

	memset(poe_nvram_conf, '\0', 512);
	memset(tmp, '\0', 32);
	
	for(portid = 1; portid <= PoE_NUM; portid++) {
		sprintf(tmp, "%d,%d,%d;", c_poe->enable, c_poe->max_power, c_poe->priority);
		strcat(poe_nvram_conf, tmp);
		c_poe++;
	}

	scfgmgr_set("poe_port_conf", poe_nvram_conf);
	scfgmgr_commit();	
}


static void cli_poe_desp_store_to_nvram(unsigned char *addr){
	int portid;
	char poe_nvram_desp[1024], tmp[64];
	struct poe_desp *c_poe = NULL;
	struct poe_desp_s *p_poe = NULL;

	p_poe = (struct poe_desp_s *)addr;
	c_poe = p_poe->poe_names;

	memset(poe_nvram_desp, '\0', 1024);
	memset(tmp, '\0', 64);

	for(portid = 1; portid <= PoE_NUM; portid++) {
		if(portid < PoE_NUM)
			sprintf(tmp, "%s,", c_poe->port_desp);
		else
			sprintf(tmp, "%s", c_poe->port_desp);
		strcat(poe_nvram_desp, tmp);
		c_poe++;
	}

	scfgmgr_set("port_desps", poe_nvram_desp);
}
#endif


/*
 *  Function : cli_nvram_conf_get
 *  Purpose:
 *     nvram config get
 *  Parameters:
 *     type  - Config Type (CLI_CONF)
 *     addr  - Config Struct
 *  Returns:
 *     CLI_SUCCESS - Success
 *     CLI_FAILED  - Failure
 *
 *  Author  : xichen
 *  Date    :2011/2/14 (Valentine's Day ^_^)
 */
int cli_nvram_conf_get(int type, unsigned char *addr)
{
	int portid, index;
	char tmp[128];
	char *p, *p1, *p2, *p3;
	char *forward_vlan, *pvid, *link_type, *allow, *untag, *source, *rstp_config, *snmp_user;
	char *access_user, *dot1x_config, *trunk_list, *static_arp_info_list;
	forward_vlan = pvid = link_type = allow = untag = rstp_config = snmp_user = NULL;	
	access_user = dot1x_config = trunk_list = static_arp_info_list = NULL;

	/* VLAN config struct */
	cli_vlan_conf *c_vlan = NULL;
	cli_vlan_info_conf *p_vlan = NULL;
	cli_port_conf *c_port = NULL;
	cli_rstp_conf *r_rstp = NULL;
	
	/* DHCP Snooping config struct */
	cli_source_conf *c_source = NULL;
	cli_source_info_conf *p_source = NULL;

	/* SNMP User config struct */
	cli_snmp_user *s_snmp = NULL;
	cli_snmp_user_info *p_snmp = NULL;

	/* Access User config struct */
	cli_access_user *s_user = NULL;
	cli_access_user_info *p_user = NULL;

	/* Dot1x Config struct */
	cli_dot1x_conf *c_dot1x = NULL;

	/* Trunk config struct */
	cli_trunk_conf *c_trunk = NULL;
	cli_trunk_list *p_trunk = NULL;
	
	/*Static arp config struct */
	cli_static_arp_conf *static_arp = NULL;
	cli_static_arp_list *sta_arp_list = NULL;

	switch(type) {
		case CLI_VLAN_FOWD:
			c_vlan = (cli_vlan_conf *)addr;
			c_vlan->vlan_count = 0;

			p_vlan = c_vlan->cur_vlan_info;
			forward_vlan = cli_nvram_safe_get(CLI_VLAN_FORWARD, "vlan_fwd_config");

			p = forward_vlan;
			while((p3=strchr(p, ';')) != NULL)
			{
				if(NULL == p_vlan) {
					p_vlan = malloc(sizeof(cli_vlan_info_conf));
					if(NULL == p_vlan){
						free(forward_vlan);
						return CLI_FAILED;
					}
					c_vlan->cur_vlan_info = p_vlan;
				} else {
					p_vlan->next = malloc(sizeof(cli_vlan_info_conf));
					if(NULL == p_vlan->next){
						free(forward_vlan);
						return CLI_FAILED;
					}
					p_vlan = p_vlan->next;
				}

				memset(p_vlan, 0, sizeof(cli_vlan_info_conf));
				p_vlan->next = NULL;

				p1 = strchr(p, ':');
				p1++;
				p_vlan->vlanid = atoi(p1);
		
				p = ++p1;
				p1 = strchr(p, ':');
				p = ++p1;
				p2 = strchr(p, ':');
				
				memcpy(p_vlan->name, p1, p2-p1);
				
				p = p2;
				p1 = strchr(p, ':');
				p = ++p1;

				p2 = strchr(p, ':');
				memset(tmp, '\0', sizeof(tmp));
				memcpy(tmp, p1, p2-p1);
				
				str2bit(tmp, &(p_vlan->forward));
				p = p2;
					
				p1 = strchr(p, ':');
				p1++;
				memset(tmp, '\0', sizeof(tmp));
				memcpy(tmp, p1, p3-p1);
				str2bit(tmp, &(p_vlan->untag));
				
				p = p3 + 1;
				
				c_vlan->vlan_count++;

			}
			
			free(forward_vlan);
			break;

		case CLI_VLAN_PORT:
			c_port = (cli_port_conf *)addr;
			
			link_type = cli_nvram_safe_get(CLI_ALL_ONE, "vlan_link_type");
			pvid = cli_nvram_safe_get(CLI_COMMA_ONE, "pvid_config");
			allow = cli_nvram_safe_get(CLI_TRUNK_VLAN, "trunk_vlan_allowed");
			untag = cli_nvram_safe_get(CLI_TRUNK_VLAN, "trunk_vlan_untagged");
			
			p = pvid;
			p1 = allow;
			p2 = untag;
			for(portid = 1; portid<=PNUM; portid++) {
				c_port->portid = portid;

				c_port->mode = *(link_type+portid-1);

				c_port->pvid = atoi(p);
				p = strchr(p, ',');
				p++;
				
				p3 = strchr(p1, ':');
				p3++;
				
				p1 = strchr(p1, ';');

				c_port->allow = malloc(p1-p3+1);
				if(NULL == c_port->allow){
					free(link_type);
					free(pvid);
					free(allow);
					free(untag);
					return CLI_FAILED;
				}

				memset(c_port->allow, '\0', p1-p3+1);
				memcpy(c_port->allow, p3, p1-p3);

				p1++;
				p3 = strchr(p2, ':');
				p3++;
				p2 = strchr(p2, ';');

				c_port->untag = malloc(p2-p3+1);
				if(NULL == c_port->untag){
					free(link_type);
					free(pvid);
					free(allow);
					free(untag);
					return CLI_FAILED;
				}

				memset(c_port->untag, '\0', p2-p3+1);
				memcpy(c_port->untag, p3, p2-p3);
				
				p2++;
				c_port++;
			}
			
			free(link_type);
			free(pvid);
			free(allow);
			free(untag);
			break;

		case CLI_SOURCE_BINDING:
			c_source = (cli_source_conf *)addr;
			c_source->source_count = 0;

			p_source = c_source->cur_source_info;
			source = nvram_safe_get("source_binding");

			p = source;
			while((p3=strchr(p, ';')) != NULL)
			{
				if(NULL == p_source) {
					p_source = malloc(sizeof(cli_source_info_conf));
					if(NULL == p_source){
						free(source);
						return CLI_FAILED;
					}
					c_source->cur_source_info = p_source;
				} else {
					p_source->next = malloc(sizeof(cli_source_info_conf));
					if(NULL == p_source->next){
						free(source);
						return CLI_FAILED;
					}
					p_source = p_source->next;
				}

				memset(p_source, 0, sizeof(cli_source_info_conf));
				p_source->next = NULL;

				cli_str2mac(p, p_source->mac_addr);

				p1 = strchr(p, '|');
				p1++;
				p = strchr(p1, '|');

				memset(tmp, '\0', sizeof(tmp));
				memcpy(tmp, p1, p-p1);
				p++;

				inet_pton(AF_INET, tmp, (void *)&p_source->ip_addr.s_addr);
				p_source->port = atoi(p);

				p = strchr(p, '|');
				p++;
			
				p_source->vlan = atoi(p);

				p = strchr(p, '|');
				p++;
				
				p_source->type = atoi(p);

				p = p3 + 1;
				c_source->source_count++;
			}
			
			free(source);
			break;

		case CLI_RSTP_PORT:
			r_rstp = (cli_rstp_conf *)addr;

			rstp_config = cli_nvram_safe_get(CLI_RSTP_CONFIG, "rstp_config");
			p = rstp_config;

			for(portid = 1; portid<=PNUM; portid++) {
				if((p3=strchr(p, ';')) == NULL){
					free(rstp_config);
					return CLI_FAILED;
				}

				r_rstp->pathcost = atoi(p);
				p1 = strchr(p, ',');
				p = ++p1;

				r_rstp->priority = atoi(p);
				p1 = strchr(p, ',');
				p = ++p1;

				r_rstp->p2p = atoi(p);
				p1 = strchr(p, ',');
				p = ++p1;

				r_rstp->edge = atoi(p);
				p1 = strchr(p, ',');
//				p = ++p1;
				

				r_rstp->status = 0;//atoi(p);
				p = p3 + 1;
//                		p1 = strchr(p, ',');
//				p = ++p1;

                		r_rstp->bpdu_guard = 0;// atoi(p);
//                		p1 = strchr(p, ',');
//				p = ++p1;

                		r_rstp->bpdu_filter = 0;//atoi(p);
//                		p1 = strchr(p, ',');
//				p = ++p1;

                		r_rstp->guard = 0;//atoi(p);
//				p = p3 + 1;
				r_rstp++;
			}

			free(rstp_config);
			break;

		case CLI_SNMP_USER:
			s_snmp = (cli_snmp_user *)addr;
			p_snmp = s_snmp->cur_snmp_user_info;

			snmp_user = nvram_safe_get("snmp_user");
			p = snmp_user;

			for(index = 1; index<=MAX_SNMP_USER; index++) {
				if((p3=strchr(p, ';')) == NULL)
					break;

				p1 = strchr(p, ':');
				memcpy(p_snmp->name, p, p1-p);
				p1++;

				p = p1;
				p1 = strchr(p, ':');
				memcpy(p_snmp->auth, p, p1-p);
				p1++;

				p = p1;
				p1 = strchr(p, ':');
				memcpy(p_snmp->auth_passwd, p, p1-p);
				p1++;

				p = p1;
				p1 = strchr(p, ':');
				memcpy(p_snmp->priv, p, p1-p);
				p1++;

				p = p1;
				p1 = strchr(p, ':');
				memcpy(p_snmp->priv_passwd, p, p1-p);
				p1++;

				p = p1;
				if(CLI_SNMP_RONLY == atoi(p))
					p_snmp->mode = CLI_SNMP_RONLY;
				else
					p_snmp->mode = CLI_SNMP_RWRITE;

				p = p3 + 1;
				s_snmp->user_count++;
				p_snmp++;
			}

			free(snmp_user);
			break;

		case CLI_ACCESS_USER:
			s_user = (cli_access_user *)addr;
			p_user = s_user->cur_access_user_info;

			access_user = nvram_safe_get("user");
			p = access_user;

			for(index = 1; index<=MAX_ACCESS_USER; index++) {
				if((p3=strchr(p, ';')) == NULL)
					break;

				p1 = strchr(p, ':');
				memcpy(p_user->name, p, p1-p);
				p1++;

				p = p1;
				p1 = strchr(p, ':');
				memcpy(p_user->passwd, p, p1-p);
				p1++;


				p = p1;
				p_user->mode = atoi(p);

				p = p3 + 1;
				
				if( (strlen(p_user->passwd)>0)&&(strlen(p_user->name)>0) )
					s_user->user_count++;
				p_user++;
			}

			free(access_user);
			break;

		case CLI_DOT1X_CONF:
			c_dot1x = (cli_dot1x_conf *)addr;

			dot1x_config = nvram_safe_get("dot1x_config");
			p = dot1x_config;

			for(index = 1; index <= PNUM; index++) {
				if((p3=strchr(p, ';')) == NULL)
					break;

				c_dot1x->auth_mode = atoi(p);
				p1 = strchr(p, ',');
				p = ++p1;

				c_dot1x->ctrl_type = atoi(p);
				p1 = strchr(p, ',');
				p = ++p1;

				c_dot1x->max_user = atoi(p);

				p = p3 + 1;

				c_dot1x++;
			}

			free(dot1x_config);
			break;

		case CLI_TRUNK_LIST:
			c_trunk = (cli_trunk_conf *)addr;
			c_trunk->group_count = 0;

			p_trunk = c_trunk->cur_trunk_list;

			trunk_list = nvram_safe_get("trunk_list");
			p = trunk_list;

			for(index = 1; index<=CLI_TRUNK_GROUP; index++) {
				if((p3=strchr(p, ';')) == NULL)
					break;

				p_trunk->group_no = atoi(p+6);

				p1 = strchr(p, '|');
				memcpy(p_trunk->name, p, p1-p);
				p = ++p1;

				p_trunk->mode = atoi(p);
				p1 = strchr(p, '|');
				p = ++p1;

				p_trunk->key = atoi(p);
				p1 = strchr(p, '|');
				p = ++p1;

				memset(tmp, '\0', sizeof(tmp));
				memcpy(tmp, p, p3-p);

				str2bit(tmp, &(p_trunk->port_int));

				p = p3 + 1;

				p_trunk++;
				c_trunk->group_count++;
			}

			free(trunk_list);
			break;
		
		case CLI_STATIC_ARP:
			static_arp = (cli_static_arp_conf *)addr;
			static_arp->arp_info_count = 0;
			
			static_arp->static_arp_list = NULL;
			sta_arp_list = static_arp->static_arp_list;
			static_arp_info_list = nvram_safe_get("static_arp_list");
			p = static_arp_info_list;
			
			while((p3=strchr(p, ';')) != NULL)
			{	
				if(NULL == sta_arp_list) {
					sta_arp_list = malloc(sizeof(cli_static_arp_list));
					if(NULL == sta_arp_list){
						free(static_arp_info_list);
						return CLI_FAILED;
					}
					static_arp->static_arp_list = sta_arp_list;
				} else {
					sta_arp_list->next = malloc(sizeof(cli_static_arp_list));
					if(NULL == sta_arp_list->next){
						free(static_arp_info_list);
						return CLI_FAILED;
					}
					sta_arp_list = sta_arp_list->next;
				}
				/*format is like "1.1.1.1|00:00:00:00:00:00;"*/
				memset(sta_arp_list, 0, sizeof(cli_static_arp_list));
				sta_arp_list->next = NULL;
				
				p1 = strchr(p, '|');
				memcpy(sta_arp_list->static_ip,p,p1-p);
				p2 = strchr(p1, ';');
				memcpy(sta_arp_list->static_mac,p1+1,p2-p1-1);
				p = p3 + 1;
				
				static_arp->arp_info_count++;
			}
			
			free(static_arp_info_list);
			break;
		case CLI_RATE_STORM:
			cli_rate_storm_get(addr);
			break;
		case CLI_POE_CONF:
			cli_poe_nvram_prase(addr);
			break;		
#ifdef POE_SI3482
		case CLI_POE_NAMES:
			cli_poe_desp_nvram_prase(addr);
			break;
#endif			
		default:
			break;
	}

	return CLI_SUCCESS;
}

/*
 *  Function : cli_nvram_conf_set
 *  Purpose:
 *     nvram config set
 *  Parameters:
 *     type  - Config Type (CLI_CONF)
 *     addr  - Config Struct
 *  Returns:
 *     CLI_SUCCESS - Success
 *     CLI_FAILED  - Failure
 *
 *  Author  : xichen
 *  Date    :2011/2/14 (Valentine's Day ^_^)
 */
int cli_nvram_conf_set(int type, unsigned char *addr)
{
	int portid, index;
	char tmp[128+12];/* why the len is "128+12", be careful! */
	char buff[256];
	char *result, *tmp_str;
	result = tmp_str = NULL;

	/* VLAN config struct */
	cli_vlan_conf *c_vlan = NULL;
	cli_vlan_info_conf *p_vlan = NULL;
	cli_port_conf *c_port = NULL;
	char pvid[4*PNUM+PNUM];
	char link_type[PNUM+1];
	char allow[VLAN_ALLOW_UNTAG_LEN];
	char untag[VLAN_ALLOW_UNTAG_LEN];

	/* RSTP config struct */
	cli_rstp_conf *r_rstp = NULL;
	char rstp_config[32*PNUM];

	/* DHCP Snooping config struct */
	cli_source_conf *c_source = NULL;
	cli_source_info_conf *p_source = NULL;

	/* SNMP User config struct */
	cli_snmp_user *s_snmp = NULL;
	cli_snmp_user_info *p_snmp = NULL;
	char snmp_user[130*MAX_SNMP_USER];

	/* Access User config struct */
	cli_access_user *s_user = NULL;
	cli_access_user_info *p_user = NULL;

	/* Dot1x Config struct */
	cli_dot1x_conf *c_dot1x = NULL;
	char dot1x_config[16*PNUM];

	/* Trunk config struct */
	cli_trunk_conf *c_trunk = NULL;
	cli_trunk_list *p_trunk = NULL;
	
	/*Static arp config struct */
	cli_static_arp_conf *static_arp = NULL;
	cli_static_arp_list *sta_arp_list = NULL;

	switch(type) {
		case CLI_VLAN_FOWD:
			index = 0;
			c_vlan = (cli_vlan_conf *)addr;
			p_vlan = c_vlan->cur_vlan_info;
			
			/* the format is like "0:1:Default:1-8:1-8;" */
			result = malloc(VLAN_STRUCT_LEN*(c_vlan->vlan_count));
			if(NULL == result)
				return CLI_FAILED;
			
			/* initialize result buff */
			sprintf(result, "");
			while(NULL != p_vlan) {
				sprintf(tmp, "%d:%d:%s:", index, p_vlan->vlanid, p_vlan->name);
				strcat(result, tmp);

				tmp_str = bit2str(p_vlan->forward);
				sprintf(tmp, "%s:", tmp_str);
				strcat(result, tmp);
				free(tmp_str);

				tmp_str = bit2str(p_vlan->untag);
				sprintf(tmp, "%s;", tmp_str);
				strcat(result, tmp);
				free(tmp_str);

				p_vlan = p_vlan->next;
				index++;
			}
			scfgmgr_set("vlan_fwd_config", result);
			free(result);
			break;

		case CLI_VLAN_PORT:
			c_port = (cli_port_conf *)addr;

			memset(tmp, '\0', sizeof(tmp));
			memset(link_type, '\0', sizeof(link_type));
			memset(pvid, '\0', sizeof(pvid));
			memset(allow, '\0', sizeof(allow));
			memset(untag, '\0', sizeof(untag));
			
			for(portid = 1; portid <= PNUM; portid++) {

				link_type[portid-1] = c_port->mode;

				if(1 == portid)
					sprintf(tmp, "%d", c_port->pvid);
				else
					sprintf(tmp, ",%d", c_port->pvid);
				
				strcat(pvid, tmp);

				sprintf(tmp, "%d:%s;", c_port->portid, c_port->allow);
				strcat(allow, tmp);
				
				sprintf(tmp, "%d:%s;", c_port->portid, c_port->untag);
				strcat(untag, tmp);
				
				c_port++;
			}

			scfgmgr_set("vlan_link_type", link_type);
			scfgmgr_set("pvid_config", pvid);
			scfgmgr_set("trunk_vlan_allowed", allow);
			scfgmgr_set("trunk_vlan_untagged", untag);
                        system("/usr/sbin/rc vlan start");
                      
			break;

		case CLI_SOURCE_BINDING:
			index = 0;
			c_source = (cli_source_conf *)addr;
			p_source = c_source->cur_source_info;
			
			/* the format is like "000000000001|192.168.1.100|1|4094|1;" */
			result = malloc(SOURCE_STRUCT_LEN*(c_source->source_count));
			if(NULL == result)
				return CLI_FAILED;
			
			/* initialize result buff */
			sprintf(result, "");
			while(NULL != p_source) {
				sprintf(tmp, "%02x%02x%02x%02x%02x%02x", p_source->mac_addr[0], p_source->mac_addr[1], p_source->mac_addr[2], p_source->mac_addr[3], p_source->mac_addr[4], p_source->mac_addr[5]);
				strcat(result, tmp);
				
				sprintf(tmp, "|%s|%d|%d|%d;", inet_ntoa(p_source->ip_addr), p_source->port, p_source->vlan, p_source->type);
				strcat(result, tmp);

				p_source = p_source->next;
				index++;
			}
			scfgmgr_set("source_binding", result);
			free(result);
			break;

		case CLI_RSTP_PORT:
			r_rstp = (cli_rstp_conf *)addr;

			memset(rstp_config, '\0', sizeof(rstp_config));
			
			for(portid = 1; portid <= PNUM; portid++) {
				sprintf(tmp, "%d,%d,%d,%d,%d,%d,%d,%d;", r_rstp->pathcost, r_rstp->priority, r_rstp->p2p, r_rstp->edge, r_rstp->status, r_rstp->bpdu_guard, r_rstp->bpdu_filter, r_rstp->guard);
				strcat(rstp_config, tmp);
				r_rstp++;
			}

			scfgmgr_set("rstp_config", rstp_config);
			break;

		case CLI_SNMP_USER:
			s_snmp = (cli_snmp_user *)addr;
			p_snmp = s_snmp->cur_snmp_user_info;

			memset(snmp_user, '\0', sizeof(snmp_user));

			for(index = 1; index <= s_snmp->user_count; index++) {
				sprintf(tmp, "%s:%s:%s:%s:%s:%d;", p_snmp->name, p_snmp->auth, p_snmp->auth_passwd, p_snmp->priv, p_snmp->priv_passwd, p_snmp->mode);
				strcat(snmp_user, tmp);

				p_snmp++;
			}

			scfgmgr_set("snmp_user", snmp_user);
			break;

		case CLI_ACCESS_USER:
			s_user = (cli_access_user *)addr;
			p_user = s_user->cur_access_user_info;

			memset(buff, '\0', sizeof(buff));

			//for(index = 1; index <= s_user->user_count; index++) {
			for(index = 1; index <= MAX_ACCESS_USER; index++) {
				sprintf(tmp, "%s:%s:%d;", p_user->name, p_user->passwd, p_user->mode);
				strcat(buff, tmp);

				p_user++;
			}

			scfgmgr_set("user", buff);
			break;

		case CLI_DOT1X_CONF:
			c_dot1x = (cli_dot1x_conf *)addr;

			memset(dot1x_config, '\0', sizeof(dot1x_config));
			
			for(portid = 1; portid <= PNUM; portid++) {
				sprintf(tmp, "%d,%d,%d;", c_dot1x->auth_mode, c_dot1x->ctrl_type, c_dot1x->max_user);
				strcat(dot1x_config, tmp);

				c_dot1x++;
			}

			scfgmgr_set("dot1x_config", dot1x_config);
			break;

		case CLI_TRUNK_LIST:
			c_trunk = (cli_trunk_conf *)addr;
			p_trunk = c_trunk->cur_trunk_list;
			
			/* the format is like "Trunk-1|1|1000|1,3-4;" */
			result = malloc(PORT_STR_LEN*(c_trunk->group_count));
			if(NULL == result)
				return CLI_FAILED;
			
			/* initialize result buff */
			sprintf(result, "");
			for(index = 1; index <= c_trunk->group_count; index++) {
				sprintf(tmp, "%s|%d|%d|", p_trunk->name, p_trunk->mode, p_trunk->key);
				strcat(result, tmp);

				tmp_str = bit2str(p_trunk->port_int);
				sprintf(tmp, "%s;", tmp_str);
				strcat(result, tmp);
				free(tmp_str);

				p_trunk++;
			}
			scfgmgr_set("trunk_list", result);
			free(result);
			break;
			
		case CLI_STATIC_ARP:
			static_arp = (cli_static_arp_conf *)addr;
			sta_arp_list = static_arp->static_arp_list;
			
			/*the format is like "1.1.1.1|00:00:00:00:00:00;"*/
			result = malloc(STATIC_ARP_LEN*(static_arp->arp_info_count));
			if(NULL == result)
				return CLI_FAILED;
			
			/* initialize result buff */
			sprintf(result, "");
			while(sta_arp_list != NULL){
				sprintf(tmp, "%s|%s;", sta_arp_list->static_ip, sta_arp_list->static_mac);
				strcat(result, tmp);
				sta_arp_list = sta_arp_list->next;
			}
			
			scfgmgr_set("static_arp_list", result);

			free(result);
			break;
#ifdef POE_SI3482
		case CLI_POE_CONF:
			cli_poe_conf_store_to_nvram(addr);
			break;
		case CLI_POE_NAMES:
			cli_poe_desp_store_to_nvram(addr);
			break;
#endif			
		default:
			break;
	}

	return CLI_SUCCESS;
}


/*
 *  Function : cli_nvram_conf_free
 *  Purpose:
 *     nvram config free
 *  Parameters:
 *     type  - Config Type (CLI_CONF)
 *     addr  - Config Struct
 *  Returns:
 *     CLI_SUCCESS - Success
 *     CLI_FAILED  - Failure
 *
 *  Author  : xichen
 *  Date    :2011/2/14 (Valentine's Day ^_^)
 */
int cli_nvram_conf_free(int type, unsigned char *addr)
{
	int portid;

	/* VLAN config struct */
	cli_vlan_conf *c_vlan = NULL;
	cli_vlan_info_conf *p_vlan = NULL;
	cli_vlan_info_conf *t_vlan = NULL;
	cli_port_conf *c_port = NULL;

	/*Arp config struct*/
	cli_static_arp_conf *static_arp = NULL;
	cli_static_arp_list *p_sta_arp_list = NULL;
	cli_static_arp_list *t_sta_arp_list = NULL;

	/* DHCP Snooping config struct */
	cli_source_conf *c_source = NULL;
	cli_source_info_conf *p_source = NULL;
	cli_source_info_conf *t_source = NULL;

	switch(type) {
		case CLI_VLAN_FOWD:
			c_vlan = (cli_vlan_conf *)addr;
			p_vlan = c_vlan->cur_vlan_info;
			t_vlan = c_vlan->cur_vlan_info;

			while(NULL != p_vlan) {
				t_vlan = p_vlan;
				p_vlan = p_vlan->next;
				free(t_vlan);
			}
			break;

		case CLI_VLAN_PORT:
			c_port = (cli_port_conf *)addr;
			for(portid = 1; portid <= PNUM; portid++) {
				if(c_port->allow)
					free(c_port->allow);
				if(c_port->untag)
					free(c_port->untag);
				c_port++;
			}
			break;

		case CLI_SOURCE_BINDING:
			c_source = (cli_source_conf *)addr;
			p_source = c_source->cur_source_info;
			t_source = c_source->cur_source_info;

			while(NULL != p_source) {
				t_source = p_source;
				p_source = p_source->next;
				free(t_source);
			}
			break;
			
		case CLI_STATIC_ARP:
			static_arp = (cli_vlan_conf *)addr;
			p_sta_arp_list = static_arp->static_arp_list;
			t_sta_arp_list = static_arp->static_arp_list;

			while(NULL != p_sta_arp_list) {
				t_sta_arp_list = p_sta_arp_list;
				p_sta_arp_list = p_sta_arp_list->next;
				free(t_sta_arp_list);
			}
			break;

		case CLI_RSTP_PORT:
			/* do nothing */
			break;

		case CLI_SNMP_USER:
			/* do nothing */
			break;

		case CLI_ACCESS_USER:
			/* do nothing */
			break;

		case CLI_DOT1X_CONF:
			/* do nothing */
			break;

		case CLI_TRUNK_LIST:
			/* do nothing */
			break;

		case CLI_POE_CONF:
			/* do nothing */
			break;
#ifdef POE_SI3482
		case CLI_POE_NAMES:
		{
			struct poe_desp_s *poe_desp_p = NULL;	
			poe_desp_p = (struct poe_desp_s *)addr;
			for(portid = 1; portid <= PoE_NUM; portid++) {
				if(poe_desp_p->poe_names[portid-1].port_desp!=NULL){
					free(poe_desp_p->poe_names[portid-1].port_desp);
				}
			}
		}
		break;
#endif
		default:
			break;
	}

	return CLI_SUCCESS;
}


int cli_rate_storm_get(unsigned char *addr)
{
	char *rate_ingress, *rate_egress, *storm_bro, *storm_mul, *storm_uni;
	int portid;
	cli_rate_storm *c_rate = NULL;
	char *p, *p1, *p2, *p3, *p4;
	c_rate = (cli_rate_storm *)addr;

	rate_ingress = cli_nvram_safe_get(CLI_COMMA_ZERO, "rate_ingress");
	rate_egress = cli_nvram_safe_get(CLI_COMMA_ZERO, "rate_egress");
	storm_bro = cli_nvram_safe_get(CLI_COMMA_ZERO, "storm_bro");
	storm_mul = cli_nvram_safe_get(CLI_COMMA_ZERO, "storm_mul");
	storm_uni = cli_nvram_safe_get(CLI_COMMA_ZERO, "storm_uni");
	p = rate_ingress;
	p1 = rate_egress;
	p2 = storm_bro;
	p3 = storm_mul;
	p4 = storm_uni;
	
	for(portid = 1; portid<=PNUM; portid++) {
		c_rate->rate_igr = atoi(p);
		c_rate->rate_egr = atoi(p1);
		c_rate->storm_bro = atoi(p2);
		c_rate->storm_mul = atoi(p3);
		c_rate->storm_uni = atoi(p4);

		p = strchr(p, ',');p++;
		p1 = strchr(p1, ',');p1++;
		p2 = strchr(p2, ',');p2++;
		p3 = strchr(p3, ',');p3++;
		p4 = strchr(p4, ',');p4++;

		c_rate++;
	}

	free(rate_ingress);
	free(rate_egress);
	free(storm_bro);
	free(storm_mul);
	free(storm_uni);
	return 0;
}


//l3_ip=1:0,192.168.1.1/24,2000::1:2345:6789:abcd/64;2:0,192.168.2.1/24,;4:0,,2001:db8:85a3:8a3:1319:8a2e:370:7344/64;
int cli_interface_info_get(char *info, int *vid, int *type, char *ipv4, char *ipv6)
{
    int vlan, itype;
    char *p1, *p2, *p3;
    
    vlan = atoi(info);
    
    p1 = strchr(info, ':')+1;
    itype = atoi(p1);
    
    p2 = strchr(p1, ',')+1;
    p3 = strchr(p2, ',');
    memcpy(ipv4, p2, p3-p2);
    strcpy(ipv6, p3+1);
    *vid = vlan;
    *type = itype;
    
    return 0;
}
