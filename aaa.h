#ifndef __BCM_AAA_H
#define __BCM_AAA_H

#include <stdint.h>
#include <time.h>

#define AAA_SOCK "/var/run/aaa/aaa.sock"

/* action */
#define AAA_LOCAL                        (1 << 0)
#define AAA_LINE                         (1 << 1)
#define AAA_ENABLE                       (1 << 2)
#define AAA_LOGIN                        (1 << 3)
#define AAA_LEVEL                        (1 << 4)
#define AAA_NO                           (1 << 5)
#define AAA_UNENCRYPTED                  (1 << 6)
#define AAA_ENCRYPTED                    (1 << 7)
#define AAA_PASSWD                       (1 << 8)
#define AAA_SECRET                       (1 << 9)

#define AAA_AUTHOR                       (1 << 10)
#define AAA_EXEC                         (1 << 11)
#define AAA_CMD                          (1 << 12)
#define AAA_ACCT                         (1 << 13)
#define AAA_START                        (1 << 14)
#define AAA_STOP                         (1 << 15)
#define AAA_SET                          (1 << 16)
#define AAA_CHECK                        (1 << 17)
#define AAA_SERVICE                      (1 << 18)
#define AAA_CONSOLE                      (1 << 19)
#define AAA_AUTH_REQUEST                 (1 << 20)
#define AAA_LOCKOUT                      (1 << 21)
#define AAA_CLEAR                        (1 << 22)
#define AAA_WEB                          (1 << 23)
#define AAA_SSH                          (1 << 24)
#define AAA_TELNET                       (1 << 25)

#define AAA_VTY                          (AAA_TELNET | AAA_SSH)

/* LOGIN */
#define AAA_LOGIN_SET                    (AAA_LOGIN | AAA_SET)
#define AAA_LOGIN_CHECK                  (AAA_LOGIN | AAA_CHECK)

#define AAA_LOGIN_SET_LEVEL              (AAA_LOGIN_SET | AAA_LEVEL)
#define AAA_LOGIN_CHECK_LINE             (AAA_LOGIN_CHECK | AAA_LINE)
#define AAA_LOGIN_CHECK_LOCAL            (AAA_LOGIN_CHECK | AAA_LOCAL)
#define AAA_LOGIN_SET_UNENCRYPTED        (AAA_LOGIN_SET | AAA_UNENCRYPTED)
#define AAA_LOGIN_SET_ENCRYPTED          (AAA_LOGIN_SET | AAA_ENCRYPTED)
#define AAA_LOGIN_LINE                   (AAA_LOGIN | AAA_LINE)
#define AAA_LOGIN_LOCAL                  (AAA_LOGIN | AAA_LOCAL)
#define AAA_LOGIN_NO                     (AAA_LOGIN_SET | AAA_NO)

/* ENABLE */
#define AAA_ENABLE_SET                   (AAA_ENABLE | AAA_SET)
#define AAA_ENABLE_CHECK                 (AAA_ENABLE | AAA_CHECK)

#define AAA_ENABLE_LINE                  (AAA_ENABLE | AAA_LINE)
#define AAA_ENABLE_LOCAL                 (AAA_ENABLE | AAA_LOCAL)

#define AAA_ENCRYPTED_PASSWD             (AAA_ENCRYPTED | AAA_PASSWD)
#define AAA_ENCRYPTED_SECRET             (AAA_ENCRYPTED | AAA_SECRET)

#define AAA_UNENCRYPTED_PASSWD           (AAA_UNENCRYPTED | AAA_PASSWD)
#define AAA_UNENCRYPTED_SECRET           (AAA_UNENCRYPTED | AAA_SECRET)

#define AAA_ENABLE_PASSWD                (AAA_ENABLE_SET | AAA_PASSWD)
#define AAA_ENABLE_SECRET                (AAA_ENABLE_SET | AAA_SECRET)

#define AAA_ENABLE_ENCRYPTED_PASSWD      (AAA_ENCRYPTED_PASSWD | AAA_ENABLE_SET)
#define AAA_ENABLE_UNENCRYPTED_PASSWD    (AAA_UNENCRYPTED_PASSWD | AAA_ENABLE_SET)

#define AAA_ENABLE_ENCRYPTED_SECRET      (AAA_ENCRYPTED_SECRET | AAA_ENABLE_SET)
#define AAA_ENABLE_UNENCRYPTED_SECRET    (AAA_UNENCRYPTED_SECRET | AAA_ENABLE_SET)

#define AAA_ENABLE_PASSWD_NO             (AAA_NO | AAA_ENABLE_PASSWD)
#define AAA_ENABLE_SECRET_NO             (AAA_NO | AAA_ENABLE_SECRET)

/* AUTHOR */
#define AAA_AUTHOR_EXEC                  (AAA_AUTHOR | AAA_EXEC)
#define AAA_AUTHOR_CMD                   (AAA_AUTHOR | AAA_CMD)
#define AAA_AUTHOR_CMD_CHECK             (AAA_AUTHOR_CMD | AAA_CHECK) 

/* ACCOUNT */
#define AAA_ACCT_EXEC_START              (AAA_ACCT | AAA_EXEC | AAA_START)
#define AAA_ACCT_EXEC_STOP               (AAA_ACCT | AAA_EXEC | AAA_STOP)
#define AAA_ACCT_CMD                     (AAA_ACCT | AAA_CMD)
#define AAA_ACCT_CMD_START               (AAA_ACCT | AAA_CMD | AAA_START)
#define AAA_ACCT_CMD_STOP                (AAA_ACCT | AAA_CMD | AAA_STOP)

/* whether need to authentication */
#define AAA_AUTH_REQUEST_CONSOLE        (AAA_AUTH_REQUEST | AAA_CONSOLE)
#define AAA_AUTH_REQUEST_VTY            (AAA_AUTH_REQUEST | AAA_VTY)
#define AAA_AUTH_REQUEST_TELNET         (AAA_AUTH_REQUEST | AAA_TELNET)
/* get ssh login information , contain username, level.
 * and, need to change the aid of ssh's sta_info in aaa. 
 */
#define AAA_AUTH_REQUEST_SSH            (AAA_AUTH_REQUEST | AAA_SSH)

/* define for cli command `service encryption` */
#define AAA_SERVICE_ENCRYPTED            (AAA_SERVICE | AAA_ENCRYPTED)

/* aaa local */
#define AAA_LOCAL_LOCKOUT                (AAA_LOCAL | AAA_LOCKOUT)
#define AAA_CLEAR_LOCAL_LOCKOUT          (AAA_CLEAR | AAA_LOCAL | AAA_LOCKOUT)


#ifndef MAXSIZE
#define MAXSIZE 33
#endif

struct cli_msg {
	char user[MAXSIZE];
	char password[MAXSIZE];
	char cmd[256];      /* command, use for authorization or accounting */
	uint8_t level;		/*  level = 0 ~ 15 */
	uint32_t type;		/*  type, distingush action for aaa parse function */

	uint32_t nas_port;	/* nas port, equal line id */
	uint32_t nas_port_id;	/* nas port id, equal login line type + line id */
	uint32_t nas_port_type;	/* nas port type, eg. virtual */
	uint32_t service_type;	/* service type, eg. Exec-user,  */

	uint32_t acct_authentic;	/* acct authentic, is accout server type, eg. radius, tacacs+ */

	time_t acct_session_start;
	time_t acct_session_end;
	uint32_t acct_session_id;	/* equal line id */
	uint32_t acct_status_type;	/* acct status, equal start or stop */

#define DISC_CAUSE_EXIT           1	/* when user command `exit` to exit system */
#define DISC_CAUSE_UNUSUAL        2	/* when unusual happens */
	uint32_t disc_cause;	/* for tacacs+ ,  discription exit reason. */
	char ip[40];
} __attribute__ ((packed));

struct reply {
#define AAA_REPLY_INT                     0
#define AAA_REPLY_STR                     1
	int identify;
	union {
#define AAA_REPLY_SUCCESS                 0
#define AAA_REPLY_FAIL                    1
#define AAA_REPLY_LOCK                    2
		uint32_t code;
		char buf[64];
	};
};


#define	MAX_VTY	     16	
#ifndef MAX_ARGV_LEN
#define MAX_ARGV_LEN 64
#endif

enum {
    LINE_CONSOLE,
    LINE_VTY,  
};

struct aaa_authen {
    char login[MAX_ARGV_LEN];  /* login authentication name */
};

struct aaa_author {
    char exec[MAX_ARGV_LEN];     /* authorization exec */
    char command[15][MAX_ARGV_LEN];  /* authorization command */
};

struct aaa_acct {
    char exec[MAX_ARGV_LEN];     /* accouting exec */
    char connect[MAX_ARGV_LEN];     /* accouting connect */
    char command[15][MAX_ARGV_LEN];  /* accouting command */
};

struct line_parameter {
	int absolute_timeout;
	int execute_timeout;
};

struct aaa_line_entry {
    int id;                     /* line id */
    struct aaa_authen authen;
    struct aaa_author author;
    struct aaa_acct acct;
    struct line_parameter param;
};


struct aaa_line_info {
    struct aaa_line_entry console;
    struct aaa_line_entry vty[MAX_VTY];   
};

enum service_type {
	EXEC_USER,
	CONFIG,
};

enum acct_status_type {
	ACCT_START = 1, 
	ACCT_STOP,
};

enum acct_authentic {
	RADIUS = 1, 
	TACACS,
};

enum nas_port_type {
	CONSOLE = 4,
	VIRTUAL = 5,
};


/* log aaa status information for authentication or accouting */
struct aaa_sta_info {
	char *user_name; 				/* login user name */
	char *user_passwd;				/* login user password */
	int level;					/* point user level, value from 0 to 15 */
	uint32_t nas_port;				/* nas port, equal line id */
	uint32_t nas_port_id;			/* nas port id, equal login line type + line id */
	uint32_t nas_port_type;			/* nas port type, eg. virtual */
	uint32_t service_type;			/* service type, eg. Exec-user,  */

	uint32_t acct_authentic;		/* acct authentic, is accout server type, eg. radius, tacacs+ */
	uint32_t acct_session_id;		/* equal line id */
	uint32_t acct_status_type;		/* acct status, equal start or stop */

	time_t acct_start; 

	int remote_type;               /* log remote login type. eg. telnet, ssh */
	char remote_ip[128];           /* log remote ip address */
	int pid;
};


/* telnet and ssh infomation packet */
struct aaa_line_vty_info {
	char remote_ip[64];
	uint32_t remote_port;
	char local_ip[64];
	uint32_t local_port;
	uint32_t line_id;
	int pid;
};

extern struct aaa_sta_info sta_info;

int aaa_is_exist();
/* send message to aaa. return int */
int aaa_send_msg_bcm(struct cli_msg *msg);
/* send message to aaa. return struct */
struct reply *aaa_send_msg_struct_bcm(struct cli_msg *msg);

struct aaa_line_info *get_aaa_line_info();
int set_aaa_line_info(struct aaa_line_info *info);
struct aaa_line_vty_info *get_aaa_line_vty_info(void);

#endif /* __BCM_AAA_H */
