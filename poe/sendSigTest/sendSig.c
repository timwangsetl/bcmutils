#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define   SIG_PAUSE_POE_BG		SIGRTMIN+2   
#define   SIG_CONTINUE_POE_BG	SIGRTMIN+3  
#define   SIG_PAUSE_POE_ECHO	SIGRTMIN+4   
#define   SIG_CONTINUE_POE_ECHO	SIGRTMIN+5 

int find_pid_by_name( char* ProcName, int* pidFound)
{
        DIR             *dir;
        struct dirent   *d;
        int             pid, i, j;
        char            *s;
        int pnlen;
	int foundpid[128];

        i = 0;
        foundpid[0] = 0;
        pnlen = strlen(ProcName);

        /* Open the /proc directory. */
        dir = opendir("/proc");
        if (!dir)
        {
                printf("cannot open /proc");
                return -1;
        }

        /* Walk through the directory. */
        while ((d = readdir(dir)) != NULL) {

                char exe [PATH_MAX+1];
                char path[PATH_MAX+1];
                int len;
                int namelen;

                /* See if this is a process */
                if ((pid = atoi(d->d_name)) == 0)       continue;

                snprintf(exe, sizeof(exe), "/proc/%s/exe", d->d_name);
                if ((len = readlink(exe, path, PATH_MAX)) < 0)
                        continue;
                path[len] = '\0';

                /* Find ProcName */
                s = strrchr(path, '/');
                if(s == NULL) continue;
                s++;

                /* we don't need small name len */
                namelen = strlen(s);
                if(namelen < pnlen)     continue;

                if(!strncmp(ProcName, s, pnlen)) {
                        /* to avoid subname like search proc tao but proc taolinke matched */
                        if(s[pnlen] == ' ' || s[pnlen] == '\0') {
                                foundpid[i] = pid;
                                i++;
                        }
                }
        }

        foundpid[i] = 0;
        closedir(dir);

	*pidFound=0;
	for(i=0; foundpid[i] != 0; i++);
	for(j=0; j<i; j++){
           *pidFound += foundpid[j]*((int)pow(10,(i-1)));
        }
	
        return  0;

}


void HandlePauseSig(int signo){
	printf("pause echo %d sig received\n", signo);
}

void HandleContinueSig(int signo){
	printf("continue sig %d sig received\n", signo);
}

int main(int argc, char *argv[])
{
        int j, i, rv, pid_t;
        if ( argc != 2 )
        {
			fprintf(stdout,"Usage %s procname\n",argv[0]);
			return 0;
        }

		if(signal(SIG_PAUSE_POE_ECHO, HandlePauseSig)==SIG_ERR){
			perror("SIG_PAUSE_POE_BG");
			exit(0);
		}
		if(signal(SIG_CONTINUE_POE_ECHO, HandleContinueSig)==SIG_ERR){
			perror("SIG_CONTINUE_POE_BG");
			exit(0);
		}

        rv = find_pid_by_name( argv[1],&pid_t);
        if(!rv) {
			printf("pid=%d\n", pid_t);
			
			while(1){
				printf("Enter p to Pause, Enter c to Continue:");
				if(getchar()=='p'){
					kill(pid_t, SIG_PAUSE_POE_BG);
				}else if(getchar()=='c'){
					kill(pid_t, SIG_CONTINUE_POE_BG);
				}
			}
	}

    return 0;
}
