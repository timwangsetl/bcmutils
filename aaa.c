#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "aaa.h"

/* check aaa server whether exist.
 * some time aaa server had dead with exception.
 */
int aaa_is_exist()
{
	FILE *fp;
	char buf[128] = {'\0'};
	int ret = -1;

	if (!(fp = fopen("/proc/net/unix", "r")))
		return -1;
	
	while (fgets(buf, sizeof(buf) -1, fp)) {
		if (strstr(buf, AAA_SOCK)) {
			ret = 0;
			break;
		}
	}

	fclose(fp);
	if (ret) {
		system("/usr/sbin/aaa > /dev/null 2>&1");
/* 		printf("aaa server restart...\n");
 */
/* 		sleep(1);
 */
	}

	return ret;
}

/* send message to aaa. return int */
int aaa_send_msg_bcm(struct cli_msg *msg)
{
	char aaa_client_sock[40] = { '\0' };
	int fd, ret;
	socklen_t len;
	struct sockaddr_un aaa_sock_addr, aaa_client_addr;
	struct reply reply;
	struct timeval tv = {
		.tv_sec = 150,
		.tv_usec = 0,
	};

	aaa_is_exist();
	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd == -1) {
/* 		perror("cannot create aaa_client fd");
 */
		return AAA_REPLY_FAIL;
	}

	sprintf(aaa_client_sock, "/tmp/aaa_client_sock.%d", getpid());
	unlink(aaa_client_sock);

	memset(&aaa_client_addr, 0, sizeof(aaa_client_addr));
	aaa_client_addr.sun_family = AF_UNIX;
	strncpy(aaa_client_addr.sun_path, aaa_client_sock, sizeof(aaa_client_addr.sun_path) - 1);

	ret = bind(fd, (struct sockaddr *)&aaa_client_addr, sizeof(aaa_client_addr));
	if (ret == -1) {
/* 		perror("bind error");
 */
		return AAA_REPLY_FAIL;
	}
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	memset(&aaa_sock_addr, 0, sizeof(aaa_sock_addr));
	aaa_sock_addr.sun_family = AF_UNIX;
	strncpy(aaa_sock_addr.sun_path, AAA_SOCK, sizeof(aaa_sock_addr) - 1);
	len = sizeof(aaa_sock_addr);

	ret = sendto(fd, msg, sizeof(*msg), 0, (struct sockaddr *)&aaa_sock_addr, len);
	if (ret == -1) {
		return AAA_REPLY_FAIL;
	}

	memset(&reply, '\0', sizeof(reply));
	ret = recvfrom(fd, &reply, sizeof(reply), 0, (struct sockaddr *)&aaa_sock_addr, &len);
	if (ret <= 0) {
		reply.identify = AAA_REPLY_INT;
		reply.code = AAA_REPLY_FAIL;
	}

	close(fd);
	unlink(aaa_client_sock);

	switch (reply.identify) {
	case AAA_REPLY_INT:
		return reply.code;
	case AAA_REPLY_STR:
	default:
		/* unuse */
		return AAA_REPLY_FAIL;
	}
}

/* send message to aaa. return struct * */
struct reply *aaa_send_msg_struct_bcm(struct cli_msg *msg)
{
	char aaa_client_sock[40] = {'\0'};
	int fd, ret;
	socklen_t len;
	struct sockaddr_un aaa_sock_addr, aaa_client_addr;
	static struct reply reply;
	struct timeval tv = {
		.tv_sec = 150,
		.tv_usec = 0,
	};

	aaa_is_exist();
	memset(&reply, '\0', sizeof(reply));
	reply.identify = AAA_REPLY_INT;
	reply.code = AAA_REPLY_FAIL;

	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd == -1) {
/* 		perror("cannot create aaa_client fd");
 */
		return &reply;
	}

	sprintf(aaa_client_sock, "/tmp/aaa_client_sock.%d", getpid());
	unlink(aaa_client_sock);

	memset(&aaa_client_addr, 0, sizeof(aaa_client_addr));
	aaa_client_addr.sun_family = AF_UNIX;
	strncpy(aaa_client_addr.sun_path, aaa_client_sock, sizeof(aaa_client_addr.sun_path) - 1);

	ret = bind(fd, (struct sockaddr *)&aaa_client_addr, sizeof(aaa_client_addr));
	if (ret == -1) {
/* 		perror("bind error");
 */
		return &reply;
	}
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	memset(&aaa_sock_addr, 0, sizeof(aaa_sock_addr));
	aaa_sock_addr.sun_family = AF_UNIX;
	strncpy(aaa_sock_addr.sun_path, AAA_SOCK, sizeof(aaa_sock_addr) - 1);
	len = sizeof(aaa_sock_addr);

DEBUG("[%s:%d] sizeof(*msg) %d %d", __FUNCTION__, __LINE__, sizeof(*msg), sizeof(struct cli_msg));
	ret = sendto(fd, msg, sizeof(struct cli_msg), 0, (struct sockaddr *)&aaa_sock_addr, len);
	if (ret == -1) {
		perror("sendto error1111111");
		return &reply;
	}
DEBUG("[%s:%d] ret %d", __FUNCTION__, __LINE__, ret);

	ret = recvfrom(fd, &reply, sizeof(reply), 0, (struct sockaddr *)&aaa_sock_addr, &len);

	if (ret <= 0) {
		reply.identify = AAA_REPLY_INT;
		reply.code = AAA_REPLY_FAIL;
	}

	close(fd);
	unlink(aaa_client_sock);

	return &reply;
}

struct aaa_line_info *get_aaa_line_info()
{
	struct aaa_line_info* line_info;
	char *console, *vty;
    int i = 0, level;

	line_info = (struct aaa_line_info *) malloc(sizeof(*line_info));
	if (line_info == NULL)
		return NULL;

	memset(line_info, '\0', sizeof(*line_info));
	char *nvram = nvram_safe_get("aaa_line_info");


    if (*nvram == '\0') {
        free(nvram);
 	    line_info->console.id = 0;
		line_info->console.param.absolute_timeout = 30;
		line_info->console.param.execute_timeout = 300;
        for (i = 0; i < MAX_VTY; i++) {
		    line_info->vty[i].id = i;   
			line_info->vty[i].param.absolute_timeout = 30;
			line_info->vty[i].param.execute_timeout = 300;
		}     
        return line_info;
    }

/* 
 * aaa line info in nvram
 * format:
 * aaa_line_info = con@0|<authen_name>,<author_exec>,<author_command>,<author_command_level>,
 *                 <acct_exec>,<acct_connect>,<acct_command>,<acct_command_level>;
 * 				   vty@1|,,,,,,,;2|,,,,,,,;3|,,,,,,,;    ... 16|,,,,,,,;  
 */

	console = strstr(nvram, "con@");
	if (console == NULL)
		goto out;
	strsep(&console, "@");
	line_info->console.id = atoi(strsep(&console, "|"));
	strcpy(line_info->console.authen.login,  strsep(&console, ","));
	strcpy(line_info->console.author.exec,  strsep(&console, ","));
	
	for (level = 0; level < 15; level++)
		strcpy(line_info->console.author.command[level],  strsep(&console, ","));
	
	strcpy(line_info->console.acct.exec,  strsep(&console, ","));
	strcpy(line_info->console.acct.connect,  strsep(&console, ","));
	
	for (level = 0; level < 15; level++)
		strcpy(line_info->console.acct.command[level],  strsep(&console, ","));

	line_info->console.param.absolute_timeout = atoi(strsep(&console, ","));
	line_info->console.param.execute_timeout = atoi(strsep(&console, ";"));
	vty = strstr(console, "vty@");
	if (vty == NULL)
		goto out;
 
	strsep(&vty, "@");
    for (i = 0; i < MAX_VTY; i++) {
		line_info->vty[i].id = atoi(strsep(&vty, "|"));
		strcpy(line_info->vty[i].authen.login,  strsep(&vty, ","));
		strcpy(line_info->vty[i].author.exec,  strsep(&vty, ","));
		for (level = 0; level < 15; level++)
			strcpy(line_info->vty[i].author.command[level],  strsep(&vty, ","));

		strcpy(line_info->vty[i].acct.exec,  strsep(&vty, ","));
		strcpy(line_info->vty[i].acct.connect,  strsep(&vty, ","));
		for (level = 0; level < 15; level++)
			strcpy(line_info->vty[i].acct.command[level],  strsep(&vty, ","));		

		line_info->vty[i].param.absolute_timeout = atoi(strsep(&vty, ","));
		line_info->vty[i].param.execute_timeout = atoi(strsep(&vty, ";"));
	}
	free(nvram);

	return line_info;

out:
	free(nvram);
	free(line_info);
	return NULL;
}

int set_aaa_line_info(struct aaa_line_info *info)
{
	char buf[4096] = {'\0'};
	int i = 0;
 
	/* console */
	snprintf(buf, sizeof(buf), "con@%d|%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
			     		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
			           "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d;",
			info->console.id, info->console.authen.login, 
			info->console.author.exec, info->console.author.command[0],
			info->console.author.command[1], info->console.author.command[2],
			info->console.author.command[3], info->console.author.command[4],
			info->console.author.command[5], info->console.author.command[6],
			info->console.author.command[7], info->console.author.command[8],
			info->console.author.command[9], info->console.author.command[10],
			info->console.author.command[11], info->console.author.command[12],
			info->console.author.command[13], info->console.author.command[14],			
			info->console.acct.exec, 
			info->console.acct.connect, info->console.acct.command[0],
			info->console.acct.command[1], info->console.acct.command[2],
			info->console.acct.command[3], info->console.acct.command[4],
			info->console.acct.command[5], info->console.acct.command[6],
			info->console.acct.command[7], info->console.acct.command[8],
			info->console.acct.command[9], info->console.acct.command[10],
			info->console.acct.command[11], info->console.acct.command[12],
			info->console.acct.command[13], info->console.acct.command[14],
			info->console.param.absolute_timeout,
			info->console.param.execute_timeout
			);
    
	/* vty */
	strcat(buf, "vty@");
	for (i = 0; i < MAX_VTY; i++) {
		snprintf(buf, sizeof(buf), "%s%d|%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
			     		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,"
			           "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d;", buf,
				info->vty[i].id, info->vty[i].authen.login, 
				info->vty[i].author.exec, info->vty[i].author.command[0],
				info->vty[i].author.command[1], info->vty[i].author.command[2],
				info->vty[i].author.command[3], info->vty[i].author.command[4],
				info->vty[i].author.command[5], info->vty[i].author.command[6],
				info->vty[i].author.command[7], info->vty[i].author.command[8],
				info->vty[i].author.command[9], info->vty[i].author.command[10],
				info->vty[i].author.command[11], info->vty[i].author.command[12],
				info->vty[i].author.command[13], info->vty[i].author.command[14],
				info->vty[i].acct.exec, 
				info->vty[i].acct.connect, info->vty[i].acct.command[0],
				info->vty[i].acct.command[1], info->vty[i].acct.command[2],
				info->vty[i].acct.command[3], info->vty[i].acct.command[4],
				info->vty[i].acct.command[5], info->vty[i].acct.command[6],
				info->vty[i].acct.command[7], info->vty[i].acct.command[8],
				info->vty[i].acct.command[9], info->vty[i].acct.command[10],
				info->vty[i].acct.command[11], info->vty[i].acct.command[12],
				info->vty[i].acct.command[13], info->vty[i].acct.command[14],
				info->vty[i].param.absolute_timeout,
				info->vty[i].param.execute_timeout
				);
	}

	if (strlen(buf) > 3072)
		return -1;

	nvram_set("aaa_line_info", buf);
	return 0;
}

struct aaa_line_vty_info *get_aaa_line_vty_info(void)
{
	struct aaa_line_vty_info *info = NULL;
	FILE *fp;
	int line_cnt = 0;
	char buf[128];
	
	info = (struct aaa_line_vty_info *)malloc(MAX_VTY * sizeof(struct aaa_line_vty_info));
	memset(info, '\0', sizeof(*info) * MAX_VTY);
	
	if (NULL == info)
		return NULL;
	
	if ((fp = fopen("/tmp/telnet_sessions", "r")) != NULL) {
    	while (fgets(buf, 128, fp)!= NULL) {
			sscanf(buf, "%[^,],%d,%[^,],%d,%d;", 
			info[line_cnt].remote_ip, &info[line_cnt].remote_port, 
			info[line_cnt].local_ip, &info[line_cnt].local_port, &info[line_cnt].line_id);
			line_cnt++;
    	}
		fclose(fp);
	}
			
	if ((fp = fopen("/tmp/ssh_sessions", "r")) != NULL) {
    	while (fgets(buf, 128, fp)!= NULL) {
			sscanf(buf, "%[^,],%d,%d,%d;",
			info[line_cnt].remote_ip, &info[line_cnt].remote_port, &info[line_cnt].pid, &info[line_cnt].line_id);
			info[line_cnt].local_port = 22;
			line_cnt++;
    	}
		fclose(fp);
	}
	
	return info;
}
