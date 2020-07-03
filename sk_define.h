#ifndef SOCKET_DEFINE_H
#define SOCKET_DEFINE_H

#include <sys/un.h>

#define IPC_SOCKET_BUF_MAX 1024
#define IPC_MAX_SIZE (1024*1024)
#define IPC_DATA_SIZE 1024

// define send and recv buf size
#define APP_SOCK_BUFSIZE 256
typedef int INT32;
typedef char INT8;
typedef unsigned short UINT16;
typedef short INT16;
typedef long long INT64;
#define SWITCH_DEBUG
#ifndef SWITCH_DEBUG
// define unix domain socket path
#define SOCK_PATH_SERVER "/home/renshanming/server"
#define SOCK_PATH_CLIENT "/home/renshanming/client"
#define SOCK_PATH_CLIENT1 "/home/renshanming/client1"
#define SOCK_PATH_CLIENT2 "/home/renshanming/client2"
#else
#define SOCK_PATH_EDR "/tmp/errdisable"
#define SOCK_PATH_LOOPBACK "/tmp/loopback"
#define SOCK_PATH_RSTP "/tmp/rstp"
#define SOCK_PATH_LACP "/tmp/lacp"
#define SOCK_PATH_CONSOLE "/tmp/console"
#define SOCK_PATH_CGI "/tmp/cgi"
#define SOCK_PATH_MSTP "/tmp/mstp"
#define SOCK_PATH_MSTPCTL "/tmp/mstpctl"
#define SOCK_PATH_SERVER "/tmp/server"
#define SOCK_PATH_CLIENT "/tmp/client"
#define SOCK_PATH_CLIENT1 "/tmp/client1"
#define SOCK_PATH_CLIENT2 "/tmp/client2"
#endif

#define IPC_PATH_LOOPBACK "/tmp/loopback.sock"
#define IPC_PATH_RSTP "/tmp/rstp.sock"
#define IPC_PATH_LACP "/tmp/lacp.sock"
#define IPC_SK_BACK 1
#define IPC_SK_NOBACK 0

typedef enum
{
    IPC_CMD_GET,
    IPC_CMD_SET,
	IPC_CMD_EDR_DISABLE,
	IPC_CMD_EDR_RECOVER,
	IPC_CMD_GET_LACP,
	/* FOR MSTP */
	IPC_CMD_MSTP_SHOW,
	IPC_CMD_MSTP_SHOW_CFG,
	IPC_CMD_MSTP_SHOW_MSTI,
	IPC_CMD_MSTP_PRINT_INFO,
	IPC_CMD_MSTP_CONFID,
	IPC_CMD_MSTP_VID2MSTID,
	IPC_CMD_MSTP_MAXAGE,
	IPC_CMD_MSTP_FDELAY,
	IPC_CMD_MSTP_MAXHOPS,
	IPC_CMD_MSTP_MODE,
	IPC_CMD_MSTP_TXHOLDCOUNT,
	IPC_CMD_MSTP_READVLAN,
	IPC_CMD_MSTP_READMIRROR,
	IPC_CMD_MSTP_HELLO,
	IPC_CMD_MSTP_CREATE_TREE,
	IPC_CMD_MSTP_DELETE_TREE,
	IPC_CMD_MSTP_TREEPRIO,
	IPC_CMD_MSTP_PATHCOST,
	IPC_CMD_MSTP_ADMINEDGE,
	IPC_CMD_MSTP_AUTOEDGE,
	IPC_CMD_MSTP_P2P,
	IPC_CMD_MSTP_RESTRROLE,
	IPC_CMD_MSTP_RESTRTCN,
	IPC_CMD_MSTP_MCHECK,
	IPC_CMD_MSTP_TREEPORTPRIO,
	IPC_CMD_MSTP_TREEPORTCOST,
	IPC_CMD_MSTP_DEBUG,
	IPC_CMD_MSTP_VID2FID,
	IPC_CMD_MSTP_FID2MSTID,
	IPC_CMD_MSTP_BPDUFILTER_GLOBAL,
	IPC_CMD_MSTP_BPDUFILTER,
	IPC_CMD_MSTP_BPDUGUARD,
	IPC_CMD_MSTP_ROOTGUARD,
}IPC_CMD;/*socket type define*/

typedef struct
{
	//INT32 dataSize;
	IPC_CMD enCmd;/*socket type*/
    INT8 cOpt;
	INT8 cBack;
}IPC_SK_HEAD;

typedef struct
{
	IPC_SK_HEAD stHead;
	INT8 acData[IPC_DATA_SIZE];
}IPC_SK;
INT32 init_sock_addr(struct sockaddr_un *sock_addr, const INT8 *sock_path, INT8 cFlgPid);
INT32 creat_sk_server(INT32 *skfd, struct sockaddr_un *sock_addr, INT8 *server_path);
INT32 creat_sk_client(INT32 *skfd, struct sockaddr_un *server_sock_addr, INT8 *server_path, struct sockaddr_un *client_sock_addr, INT8 *client_path, INT8 cFlgClientPid);
INT32 ipc_recv(INT32 skfd, IPC_SK *pstRx, struct sockaddr_un *src_sock_addr);
INT32 ipc_send(INT32 skfd, IPC_SK *pstTx, struct sockaddr_un *des_sock_addr);
INT32 ipc_send_recv(INT32 skfd, IPC_SK *pstTx, IPC_SK *pstRx, struct sockaddr_un *pstSockAddr);
INT32 ipc_recv_send(INT32 skfd, IPC_SK *pstTx, IPC_SK *pstRx, struct sockaddr_un *pstSockAddr);
INT8* get_sk_file_name(const INT8 *path, char flgPid);
#endif
