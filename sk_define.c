#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include "sk_define.h"

int my_itoa(int val, char* buf)
{
    const unsigned int radix = 10;

    char* p;
    unsigned int a;        //every digit
    int len;
    char* b;            //start of the digit char
    char temp;
    unsigned int u;

    p = buf;

    if (val < 0)
    {
        *p++ = '-';
        val = 0 - val;
    }
    u = (unsigned int)val;

    b = p;

    do
    {
        a = u % radix;
        u /= radix;

        *p++ = a + '0';

    } while (u > 0);

    len = (int)(p - buf);

    *p-- = 0;

    //swap
    do
    {
        temp = *p;
        *p = *b;
        *b = temp;
        --p;
        ++b;

    } while (b < p);

    return len;
}

INT8* get_sk_file_name(const INT8 *path, char flgPid)
{
	char pidstr[10];
	INT16 pid;

	pid = getpid();
	INT8 *sk_path = (INT8*)malloc(sizeof(char)*100);
	
	if(sk_path == NULL)
	{
		return sk_path;
	}
	
	memset(sk_path, '\0', 100);
	memcpy(sk_path, path, 100-11);
	if(flgPid)
	{
		my_itoa(pid, pidstr);
		memcpy((char*)&(sk_path[strlen(sk_path)]), pidstr, 6);
	}
	
	memcpy((char*)&sk_path[strlen(sk_path)], ".sock", 6);
	//printf("socket file name:%s\n", sk_path);
	return sk_path;
}

INT32 init_sock_addr(struct sockaddr_un *sock_addr, const INT8 *sock_path, INT8 cFlgPid)
{
	INT8 *pcPath;

	pcPath = get_sk_file_name(sock_path, cFlgPid);
	if(pcPath == NULL)
	{
		return -1;
	}
	// configure socket's addr
    memset(sock_addr, 0, sizeof(struct sockaddr_un));
    (*sock_addr).sun_family = AF_UNIX;
    strncpy((*sock_addr).sun_path, pcPath, sizeof((*sock_addr).sun_path)-1);
	free(pcPath);
	return 0;
}

INT32 apps_creat_socket(INT32 *skfd, struct sockaddr_un *sock_addr, const INT8 *sock_path, INT8 cFlgPid)
{
	INT32 ret;
	INT8 *pcPath;

	pcPath = get_sk_file_name(sock_path, cFlgPid);
	if(pcPath == NULL)
	{
		perror("can't get socket file name.");
		return -1;
	}
    // create aaa_sock socket fd
	*skfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (*skfd == -1) 
	{
		perror("cannot create server fd.");
		return -1;
    }
    unlink(pcPath);
    if(init_sock_addr(sock_addr, sock_path, cFlgPid) == -1)
	{
		perror("init sock_addr failed");
		free(pcPath);
		return -1;
	}

    // bind skfd to sock_addr
    ret = bind(*skfd, (struct sockaddr*)sock_addr, sizeof(struct sockaddr_un));
    if (ret == -1) 
	{
		perror("can not bind sock_addr");
		free(pcPath);
		return -1;
    }
	free(pcPath);
	return 0;
}

INT32 creat_sk_server(INT32 *skfd, struct sockaddr_un *sock_addr, INT8 *server_path)
{
	/*creat socket for inter-process communication*/
    if(apps_creat_socket(skfd, sock_addr, server_path, 0) == -1)
	{
		return -1;
	}
	return 0;
}

INT32 creat_sk_client(INT32 *skfd, struct sockaddr_un *server_sock_addr, INT8 *server_path, struct sockaddr_un *client_sock_addr, INT8 *client_path, INT8 cFlgClientPid)
{
	struct timeval tv;
	int ret;

	/*creat socket for inter-process communication*/
    if(apps_creat_socket(skfd, client_sock_addr, client_path, cFlgClientPid) == -1)/*SOCK_PATH_CLIENT is socket file path*/
	{
		/*if creat socket failed, return -1*/
		perror("can't create ipc file");
		return -1;
	}
	
	tv.tv_sec = 3;
	tv.tv_usec = 0;
	ret = setsockopt(*skfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));/*set socket's timeout*/
	if (ret == -1) 
	{
		perror("setsockopt error.");
		return -1;
	}

	init_sock_addr(server_sock_addr, server_path, 0);/*initializate server socket address, which we will send data to server*/
	return 0;
}

INT32 ipc_recv(INT32 skfd, IPC_SK *pstRx, struct sockaddr_un *src_sock_addr)
{
	size_t recvSize;
	socklen_t len;
	static int i=0;

	
	len = sizeof(struct sockaddr_un);
	//memset(&src_sock_addr, 0, len);
	memset(pstRx, 0, sizeof(IPC_SK));/*clear socket receive buffer*/

	/*receive data*/
	recvSize = recvfrom(skfd, pstRx, sizeof(IPC_SK), 0, (struct sockaddr*)src_sock_addr, &len);

	/*receive time out*/
	if(recvSize == -1)
	{
		printf("IPC recvive failed\n");
		return -1;
	}
	else
	{
		//printf("%s.%s.%d, skfd = %d, i = %d, recvSize=%d\n", __FILE__, __FUNCTION__, __LINE__, skfd, ++i, recvSize);
		#if 0
		printf("rx=head.cmd=%d,head.opt=%d,head.back=%d\ndata=", pstRx->stHead.enCmd, pstRx->stHead.cOpt, pstRx->stHead.cBack);
		int j;
		for(j = 0; j < sizeof(pstRx->acData); j++)
		{
			printf("%02x ", pstRx->acData[j]);
		}
		printf("\n");
		#endif
		/*Operation after receive data from server*/
		#if 0
		printf("%s.%s.%d\n", __FILE__, __FUNCTION__, __LINE__);
		printf("receive %d bytes\n", recvSize);
		printf("recvive sun_path = %s\n", (*src_sock_addr).sun_path);
		#endif
		return recvSize;
	}
}

INT32 ipc_send(INT32 skfd, IPC_SK *pstTx, struct sockaddr_un *des_sock_addr)
{
	size_t sendSize;
	socklen_t len;
	static int i=0;

	//printf("%s.%s.%d, skfd = %d, i = %d\n", __FILE__, __FUNCTION__, __LINE__, skfd, ++i);
	len = sizeof(struct sockaddr_un);
	/*send data to server*/
	sendSize = sendto(skfd, pstTx, sizeof(IPC_SK), 0, (struct sockaddr*)des_sock_addr, len);

	/*if send failed, it would return -1*/
	if (sendSize == -1) 
	{
		//perror("sendto error");
		return -1;
	}
	#if 1
	//printf("%s.%s.%d--sendSize=%d\n", __FILE__, __FUNCTION__, __LINE__, (int)sendSize);
	//printf("send %d bytes\n", sendSize);
	#endif
	return sendSize;
}
INT32 ipc_send_recv(INT32 skfd, IPC_SK *pstTx, IPC_SK *pstRx, struct sockaddr_un *pstSockAddr)
{
	if(ipc_send(skfd, pstTx, pstSockAddr) > 0)
	{
		//printf("%s:%s:%d--after ipc_socket send--disable\n", __FILE__, __FUNCTION__, __LINE__);/*shanming.ren 2012-1-4 9:18:45*/
		//printf("des_sock_addr=%s\n", stSkAddrEdr.sun_path);
		if(ipc_recv(skfd, pstRx, pstSockAddr) > 0)
		{
			//printf("%s:%s:%d--after ipc_socket recv--disable\n", __FILE__, __FUNCTION__, __LINE__);/*shanming.ren 2012-1-4 9:18:45*/
			return 0;
		}
	}
	return -1;
}
INT32 ipc_recv_send(INT32 skfd, IPC_SK *pstTx, IPC_SK *pstRx, struct sockaddr_un *pstSockAddr)
{
	if(ipc_recv(skfd, pstRx, pstSockAddr) > 0)
	{
		if(pstRx->stHead.enCmd == pstTx->stHead.enCmd)
		{
			if(ipc_send(skfd, pstTx, pstSockAddr) > 0)
			{
				return 0;
			}
			//printf("%s:%s:%d--after ipc_socket send--recover\n", __FILE__, __FUNCTION__, __LINE__);/*shanming.ren 2012-1-4 9:18:45*/
		}
	}
	return -1;
}
