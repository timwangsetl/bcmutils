#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_tools.h"
extern char *nvram_data;

char *remote_ip="127.0.0.1";
int  remote_port=0;

#define  SWAP_16(x) (((x<<8) & 0xFF00) | ((x>>8) & 0x00FF))
#define  SWAP_32(x) (((SWAP_16(x)<<16) & 0xFFFF0000) | ( SWAP_16(x>>16) & 0x0000FFFF))
int socket_read(scfgmgr_header **header,char **data,int infd)
{
	int len=0;
	int total=0;
	char *pt;
	*header=malloc(sizeof(scfgmgr_header));
	if(*header==NULL)
		return -1;
	if(read(infd,*header,sizeof(scfgmgr_header))<0){
		free(*header);
		return -1;
	}

#ifdef _BIG_ENDIAN_
	(*header)->magic=SWAP_32((*header)->magic);
	(*header)->len=SWAP_32((*header)->len);
	(*header)->cmd=SWAP_32((*header)->cmd);	
#endif
	if((*header)->magic != SCM_MAGIC ){
		free(*header);
		return -1;
	}
	if((*header)->len>0){
		if(!(*data=malloc((*header)->len))){
			free(*header);
		       	return -1;
		}
		pt=*data;
		while(total< (*header)->len){
			if((len=read(infd,pt,(*header)->len))<0){
				free(*header);
				free(*data);
				return -1;
			}
			total+=len;	
			pt+=len;
		}	
	}
	return 0;			
}


int socket_write(scfgmgr_header *header,char *data,int infd)
{
#ifdef _BIG_ENDIAN_
	header->magic=SWAP_32(header->magic);
	header->len=SWAP_32(header->len);
	header->cmd=SWAP_32(header->cmd);	
#endif

	if(write(infd,header,sizeof(scfgmgr_header))<0) return -1;

#ifdef _BIG_ENDIAN_
	header->len=SWAP_32(header->len);
#endif
	if(header->len > 0){
		if(write(infd,data,header->len)<0) return -1;
	}
	return 0;
}
#ifdef TEST
void socket_log(char *msg,int len)
{
	FILE *fd;
	fd=fopen(SOCKET_LOG,"w+");
	fwrite(msg,len,1,fd);
	fclose(fd);
}
#endif

int socket_connect()
{
	int sockfd;
	struct sockaddr_in cli;
		
	if((sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
		return -1;
	
	memset(&cli,0,sizeof(cli));
	cli.sin_family = AF_INET;
	cli.sin_port = htons((remote_port!=0)?:DEFAULT_REMOTE_PORT);
	cli.sin_addr.s_addr=inet_addr(remote_ip?:DEFAULT_REMOTE_IP);	
	if(connect(sockfd,(struct sockaddr *)&cli,sizeof(cli)))
		return -1;
	
	return sockfd;
}

int scfgmgr_connect(scfgmgr_header *shd,char *sdata,scfgmgr_header **rhd,char **rdata)
{
	int sockfd;
	if((sockfd=socket_connect())<0) return -1;
	if(socket_write(shd,sdata,sockfd)<0) return -1;
	if(socket_read(rhd,rdata,sockfd)<0) return -1;
	close(sockfd);
	return 0;	
}

int scfgmgr_cmd(int cmd,char **rdata)
{	
	int i=0;
	scfgmgr_header shd;
	scfgmgr_header *rhd;
	shd.magic=SCM_MAGIC;
	shd.cmd=cmd;
	shd.len=0;
	if(scfgmgr_connect(&shd,NULL,&rhd,rdata)<0) return -1;
	i=rhd->cmd;
	free(rhd);
	return (i==SCFG_ERR)?-1:0;	
}


int scfgmgr_set(char *name,char *data)
{	
	int i=0;
	char tmp[4500]="";
	scfgmgr_header shd;
	scfgmgr_header *rhd;
	shd.magic=SCM_MAGIC;
	shd.cmd=SCFG_SET;
	sprintf(tmp,"%s=%s",name,data);
	shd.len=strlen(tmp)+1;
	if(scfgmgr_connect(&shd,tmp,&rhd,NULL)<0) return -1;
	i=rhd->cmd;
	free(rhd);
	return (i==SCFG_ERR)?-1:0;	
}

int scfgmgr_sendfile(char *data,int len)
{	
	int i=0;
	scfgmgr_header shd;
	scfgmgr_header *rhd;
	shd.magic=SCM_MAGIC;
	shd.cmd=SCFG_RECEIVE;
	shd.len=len;
	if(scfgmgr_connect(&shd,data,&rhd,NULL)<0) return -1;
	i=rhd->cmd;
	free(rhd);
	return (i==SCFG_ERR)?-1:0;	
}


int scfgmgr_getall(char **rdata)
{	
	int i=0;
	scfgmgr_header shd;
	scfgmgr_header *rhd;
	shd.magic=SCM_MAGIC;
	shd.cmd=SCFG_GETALL;
	shd.len=0;
	if(scfgmgr_connect(&shd,NULL,&rhd,rdata)<0) return -1;	
	free(rhd);
	return (i==SCFG_ERR)?-1:0;	
}

int scfgmgr_get(char *data,char **rdata)
{	
	int i=0;
	scfgmgr_header shd;
	scfgmgr_header *rhd;
	shd.magic=SCM_MAGIC;
	shd.cmd=SCFG_GET;
	shd.len=strlen(data)+1;
	if(scfgmgr_connect(&shd,data,&rhd,rdata)<0) return -1;	
	free(rhd);
	return (i==SCFG_ERR)?-1:0;	
}

int scfgmgr_console(char *data,char **rdata)
{	
	int i=0;
	scfgmgr_header shd;
	scfgmgr_header *rhd;
	shd.magic=SCM_MAGIC;
	shd.cmd=SCFG_CONSOLE;
	shd.len=strlen(data)+1;
	if(scfgmgr_connect(&shd,data,&rhd,rdata)<0) return -1;	
	free(rhd);
	return (i==SCFG_ERR)?-1:0;	
}

