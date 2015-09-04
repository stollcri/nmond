/**
 * nmond.h -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *
 * 
 * nmond -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *  https://github.com/stollcri/nmond
 *  forked from (near complete rewrite):
 *   lmon.c -- Curses based Performance Monitor for Linux
 *   Developer: Nigel Griffiths.
 *   (lmon15g.c dated 2015-07-13)
 *
 * 
 * Copyright (c) 2009-2015, Nigel Griffiths
 * Copyright (c) 2015, Christopher Stoll
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of nmond nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "nmond.h"
#include "nmond_clean.c"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <pwd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fstab.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <net/if.h>

#include "mntent.h"

#include "sysinfo.h"
#include "uicli.h"
#include "uicurses.h"

#define RAW(member)      (long)((long)(p->cpuN[i].member)   - (long)(q->cpuN[i].member))
#define RAWTOTAL(member) (long)((long)(p->cpu_total.member) - (long)(q->cpu_total.member))
#define FLIP(variable) if(variable) variable=0; else variable=1;
#define GETVM(variable) p->vm.variable = get_vm_value(__STRING(variable) );

#ifdef MALLOC_DEBUG
#define MALLOC(argument) mymalloc(argument,__LINE__)
#define FREE(argument) myfree(argument,__LINE__)
#define REALLOC(argument1,argument2) myrealloc(argument1,argument2,__LINE__)
void *mymalloc(int size, int line)
{
	void * ptr;
	ptr= malloc(size);
	fprintf(stderr,"0x%x = malloc(%d) at line=%d\n",ptr,size,line);
	return ptr;
}
void myfree(void *ptr,int line)
{
	fprintf(stderr,"free(0x%x) at line=%d\n",ptr,line);
	free(ptr);
}
void *myrealloc(void *oldptr, int size, int line)
{
	void * ptr;
	ptr= realloc(oldptr,size);
	fprintf(stderr,"0x%x = realloc(0x%x, %d) at line=%d\n",ptr,oldptr,size,line);
	return ptr;
}
#else
#define MALLOC(argument) malloc(argument)
#define FREE(argument) free(argument)
#define REALLOC(argument1,argument2) realloc(argument1,argument2)
#endif /* MALLOC_DEBUG */

/* Windows moved here so they can be cleared when the screen mode changes */
WINDOW *padwelcome = NULL;
WINDOW *padtop = NULL;
WINDOW *padmem = NULL;
WINDOW *padlarge = NULL;
WINDOW *padpage = NULL;
WINDOW *padkstat = NULL;
WINDOW *padnet = NULL;
WINDOW *padneterr = NULL;
WINDOW *padnfs = NULL;
WINDOW *padsys = NULL;
WINDOW *padsmp = NULL;
WINDOW *padlong = NULL;
WINDOW *paddisk = NULL;
WINDOW *paddg = NULL;
WINDOW *padmap = NULL;
WINDOW *padjfs = NULL;
WINDOW *padverb = NULL;
WINDOW *padhelp = NULL;


/* for Disk Busy rain style output covering 100's of diskss on one screen */
const char disk_busy_map_ch[] =
"_____.....----------++++++++++oooooooooo0000000000OOOOOOOOOO8888888888XXXXXXXXXX##########@@@@@@@@@@*";
/*"00000555551111111111222222222233333333334444444444555555555566666666667777777777888888888899999999991"*/

char *month[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
	"JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

/* Cut of everything after the first space in callback
 * Delete any '&' just before the space
 */
char *check_call_string (char* callback, const char* name)
{
	char * tmp_ptr = callback;
	
	if (strlen(callback) > 256) {
		fprintf(stderr,"ERROR nmon: ignoring %s - too long\n", name);
		return (char *) NULL;
	}
	
	for( ; *tmp_ptr != '\0' && *tmp_ptr != ' ' && *tmp_ptr != '&'; ++tmp_ptr )
		;
	
	*tmp_ptr = '\0';
	
	if( tmp_ptr == callback )
		return (char *)NULL;
	else
		return callback;
}


void proc_init()
{
	proc[P_CPUINFO].filename = "/Users/stollcri/Documents/code/c/nmond/dbg/cpuinfo"; //"/proc/cpuinfo";
	proc[P_STAT].filename    = "/Users/stollcri/Documents/code/c/nmond/dbg/stat"; //"/proc/stat";
	proc[P_VERSION].filename = "/Users/stollcri/Documents/code/c/nmond/dbg/version"; //"/proc/version";
	proc[P_MEMINFO].filename = "/Users/stollcri/Documents/code/c/nmond/dbg/meminfo"; //"/proc/meminfo";
	proc[P_UPTIME].filename  = "/Users/stollcri/Documents/code/c/nmond/dbg/uptime"; //"/proc/uptime";
	proc[P_LOADAVG].filename = "/Users/stollcri/Documents/code/c/nmond/dbg/loadavg"; //"/proc/loadavg";
	proc[P_NFS].filename     = "/Users/stollcri/Documents/code/c/nmond/dbg/net/rpc/nfs"; //"/proc/net/rpc/nfs";
	proc[P_NFSD].filename    = "/Users/stollcri/Documents/code/c/nmond/dbg/net/rpc/nfsd"; //"/proc/net/rpc/nfsd";
	proc[P_VMSTAT].filename	 = "/Users/stollcri/Documents/code/c/nmond/dbg/vmstat"; //"/proc/vmstat";
}

void proc_read(int num)
{
	int i;
	int size;
	int found;
	char buf[1024];
	
	if(proc[num].read_this_interval == 1 )
		return;
	
	if(proc[num].fp == 0) {
		if( (proc[num].fp = fopen(proc[num].filename,"r")) == NULL) {
			sprintf(buf, "failed to open file %s", proc[num].filename);
			proc[num].fp = 0;
			return;
		}
	}
	rewind(proc[num].fp);
	
	if(proc[num].size == 0) {
		/* first time so allocate  initial now */
		proc[num].buf = MALLOC(512);
		proc[num].size = 512;
	}
	
	for(i=0;i<4096;i++) {   /* MAGIC COOKIE WARNING  POWER8 default install can have 2655 processes */
		size = fread(proc[num].buf, 1, proc[num].size-1, proc[num].fp);
		if(size < proc[num].size -1)
			break;
		proc[num].size +=512;
		proc[num].buf = REALLOC(proc[num].buf,proc[num].size);
		rewind(proc[num].fp);
	}
	
	proc[num].buf[size]=0;
	proc[num].lines=0;
	proc[num].line[0]=&proc[num].buf[0];
	if(num == P_VERSION) {
		found=0;
		for(i=0;i<size;i++) { /* remove some weird stuff found the hard way in various Linux versions and device drivers */
			/* place ") (" on two lines */
			if( found== 0 &&
			   proc[num].buf[i]   == ')' &&
			   proc[num].buf[i+1] == ' ' &&
			   proc[num].buf[i+2] == '(' ) {
				proc[num].buf[i+1] = '\n';
				found=1;
			} else {
				/* place ") #" on two lines */
				if( proc[num].buf[i]   == ')' &&
				   proc[num].buf[i+1] == ' ' &&
				   proc[num].buf[i+2] == '#' ) {
					proc[num].buf[i+1] = '\n';
				}
				/* place "#1" on two lines */
				if(
				   proc[num].buf[i]   == '#' &&
				   proc[num].buf[i+2] == '1' ) {
					proc[num].buf[i] = '\n';
				}
			}
		}
	}
	for(i=0;i<size;i++) {
		/* replace Tab characters with space */
		if(proc[num].buf[i] == '\t')	{
			proc[num].buf[i]= ' ';
		}
		else if(proc[num].buf[i] == '\n') {
			/* replace newline characters with null */
			proc[num].lines++;
			proc[num].buf[i] = '\0';
			proc[num].line[proc[num].lines] = &proc[num].buf[i+1];
		}
		if(proc[num].lines==PROC_MAXLINES-1)
			break;
	}
	/* Set flag so we do not re-read the data even if called multiple times in same interval */
	proc[num].read_this_interval = 1;
}

int isroot = 0;

int debug =0;
time_t  timer;			/* used to work out the hour/min/second */

/* Counts of resources */
int	cpus = 1;  	/* number of CPUs in system (lets hope its more than zero!) */
int   cores          = 0;
int   siblings       = 0;
int   processorchips = 0;
int   hyperthreads   = 0;
char *vendor_ptr = "-";
char *model_ptr  = "-";
char *mhz_ptr    = "00000000000";
char *bogo_ptr   = "-";
int	networks = 0;  	/* number of networks in system  */
int	partitions = 0;  	/* number of partitions in system  */
int	partitions_short = 0;  	/* partitions file data short form (i.e. data missing) */
int	disks    = 0;  	/* number of disks in system  */
int	seconds  = -1; 	/* pause interval */
char	hostname[256];
char	run_name[256];
int	run_name_set = 0;
char	fullhostname[256];
int	loop;

#define DPL 150 /* Disks per line for file output to ensure it
does not overflow the spreadsheet input line max */

int disks_per_line = DPL;

#define NEWDISKGROUP(disk) ( (disk) % disks_per_line == 0)

/* Mode of output variables */
int	show_aaa     = 1;
int	show_para    = 1;
int	show_headings= 1;
int	show_cpu     = 0;
int	show_smp     = 0;
int	show_longterm= 0;
int	show_disk    = 0;
#define SHOW_DISK_NONE  0
#define SHOW_DISK_STATS 1
#define SHOW_DISK_GRAPH 2
int	show_diskmap = 0;
int	show_memory  = 0;
int	show_large   = 0;
int	show_kernel  = 0;
int	show_nfs     = 0;
int	show_net     = 0;
int	show_neterror= 0;
int	show_help    = 0;
int	show_top     = 0;
int	show_topmode = 1;
#define ARGS_NONE 0
#define ARGS_ONLY 1
int	show_args    = 0;
int	show_all     = 1;	/* 1=all procs& disk 0=only if 1% or more busy */
int	show_verbose = 0;
int	show_jfs     = 0;
int	flash_on     = 0;
int	first_huge   = 1;
int	first_steal  = 1;
long	huge_peak    = 0;
int	welcome      = 1;
int	dotline      = 0;
int	show_lpar    = 0;
int	show_vm    = 0;
int	show_dgroup  = 0; /* disk groups */
int	auto_dgroup  = 0; /* disk groups defined via -g auto */
int	disk_only_mode  = 0; /* disk stats shows disks only if user used -g auto */
int     dgroup_loaded = 0; /* 0 = no, 1=needed, 2=loaded */
int	show_raw    = 0;

double ignore_procdisk_threshold = 0.1;
double ignore_io_threshold      = 0.1;
/* Curses support */
#define COLOUR if(colour) /* Only use this for single line colour curses calls */
int	colour = 1;	/* 1 = using colour curses and
				 0 = using black and white curses  (see -b flag) */
#define MVPRINTW(row,col,string) {move((row),(col)); \
attron(A_STANDOUT); \
printw(string); \
attroff(A_STANDOUT); }
FILE *fp;	/* filepointer for spreadsheet output */


char *timestamp(int loop, time_t eon)
{
	static char string[64];
	sprintf(string,"T%04d",loop);
	return string;
}
#define LOOP timestamp(loop,timer)


void args_output(int pid, int loop, char *progname)
{
	FILE *pop;
	int i,j,n;
	char tmpstr[CMDLEN];
	static int arg_first_time = 1;
	
	if(pid == 0)
		return; /* ignore init */
	for(i=0;i<ARGSMAX-1;i++ ) {   /* clear data out */
		if(arglist[i].pid == pid){
			return;
		}
		if(arglist[i].pid == 0) /* got to empty slot */
			break;
	}
	sprintf(tmpstr,"ps -p %d -o args 2>/dev/null", pid);
	pop = popen(tmpstr, "r");
	if(pop == NULL) {
		return;
	} else {
		if(fgets(tmpstr, CMDLEN, pop) == NULL) { /* throw away header */
			pclose(pop);
			return;
		}
		tmpstr[0]=0;
		if(fgets(tmpstr, CMDLEN, pop) == NULL) {
			pclose(pop);
			return;
		}
		tmpstr[strlen(tmpstr)-1]=0;
		if(tmpstr[strlen(tmpstr)-1]== ' ')
			tmpstr[strlen(tmpstr)-1]=0;
		arglist[i].pid = pid;
		if(arg_first_time) {
			fprintf(fp,"UARG,+Time,PID,ProgName,FullCommand\n");
			arg_first_time = 0;
		}
		n=strlen(tmpstr);
		for(i=0;i<n;i++) {
			/*strip out stuff that confused Excel i.e. starting with maths symbol*/
			if(tmpstr[i] == ',' &&
			   ((tmpstr[i+1] == '-') || tmpstr[i+1] == '+')  )
				tmpstr[i+1] = '_';
			/*strip out double spaces */
			if(tmpstr[i] == ' ' && tmpstr[i+1] == ' ') {
				for(j=0;j<n-i;j++)
					tmpstr[i+j]=tmpstr[i+j+1];
				i--; /* rescan to remove triple space etc */
			}
		}
		
		fprintf(fp,"UARG,%s,%07d,%s,%s\n",LOOP,pid,progname,tmpstr);
		pclose(pop);
		return;
	}
}

void args_load()
{
	FILE *pop;
	int i;
	char tmpstr[CMDLEN];
	
	for(i=0;i<ARGSMAX;i++ ) {   /* clear data out */
		if(arglist[i].pid == -1)
			break;
		if(arglist[i].pid != 0){
			arglist[i].pid = -1;
			free(arglist[i].args);
		}
	}
	pop = popen("ps -eo pid,args 2>/dev/null", "r");
	if(pop == NULL) {
		return;
	} else {
		if(fgets(tmpstr, CMDLEN, pop) == NULL) { /* throw away header */
			pclose(pop);
			return;
		}
		for(i=0;i<ARGSMAX;i++ ) {
			tmpstr[0]=0;
			if(fgets(tmpstr, CMDLEN, pop) == NULL) {
				pclose(pop);
				return;
			}
			tmpstr[strlen(tmpstr)-1]=0;
			if(tmpstr[strlen(tmpstr)-1]== ' ')
				tmpstr[strlen(tmpstr)-1]=0;
			arglist[i].pid = atoi(tmpstr);
			arglist[i].args = MALLOC(strlen(tmpstr));
			strcpy(arglist[i].args,&tmpstr[6]);
		}
		pclose(pop);
	}
}

char *args_lookup(int pid, char *progname)
{
	int i;
	for(i=0;i<ARGSMAX;i++) {
		if(arglist[i].pid == pid)
			return arglist[i].args;
		if(arglist[i].pid == -1)
			return progname;
	}
	return progname;
}
/* end args mode stuff here */

void   linux_bbbp(char *name, char *cmd, char *err)
{
	int   i;
	int   len;
#define STRLEN 4096
	char   str[STRLEN];
	FILE * pop;
	static int   lineno = 0;
	
	pop = popen(cmd, "r");
	if (pop == NULL) {
		fprintf(fp, "BBBP,%03d,%s failed to run %s\n", lineno++, cmd, err);
	} else {
		fprintf(fp, "BBBP,%03d,%s\n", lineno++, name);
		for (i = 0; i < 2048 && (fgets(str, STRLEN, pop) != NULL); i++) { /* 2048=sanity check only */
			len = strlen(str);
			if(len>STRLEN) len=STRLEN;
			if (str[len-1] == '\n') /*strip off the newline */
				str[len-1] = 0;
			/* fix lsconf style output so it does not confuse spread sheets */
			if(str[0] == '+') str[0]='p';
			if(str[0] == '*') str[0]='m';
			if(str[0] == '-') str[0]='n';
			if(str[0] == '/') str[0]='d';
			if(str[0] == '=') str[0]='e';
			fprintf(fp, "BBBP,%03d,%s,\"%s\"\n", lineno++, name, str);
		}
		pclose(pop);
	}
}

#define WARNING "needs root permission or file not present"

/* Global name of programme for printing it */
char	*progname;





char *nfs_v2_names[NFS_V2_NAMES_COUNT] = {
	"null", "getattr", "setattr", "root", "lookup", "readlink",
	"read", "wrcache", "write", "create", "remove", "rename",
	"link", "symlink", "mkdir", "rmdir", "readdir", "fsstat"};


char *nfs_v3_names[22] ={
	"null", "getattr", "setattr", "lookup", "access", "readlink",
	"read", "write", "create", "mkdir", "symlink", "mknod",
	"remove", "rmdir", "rename", "link", "readdir", "readdirplus",
	"fsstat", "fsinfo", "pathconf", "commit"};


int nfs_v4s_names_count=NFS_V4S_NAMES_COUNT;
char *nfs_v4s_names[NFS_V4S_NAMES_COUNT] = {  /* get these names from nfsstat as they are NOT documented */
	"op0-unused",   "op1-unused",   "op2-future",   "access",       "close",        "commit",       /* 1 - 6 */
	"create",       "delegpurge",   "delegreturn",  "getattr",      "getfh",        "link",         /* 7 - 12 */
	"lock",         "lockt",        "locku",        "lookup",       "lookup_root",  "nverify",      /* 13 - 18 */
	"open",         "openattr",     "open_conf",    "open_dgrd",    "putfh",        "putpubfh",     /* 19 - 24 */
	"putrootfh",    "read",         "readdir",      "readlink",     "remove",       "rename",       /* 25 - 30 */
	"renew",        "restorefh",    "savefh",       "secinfo",      "setattr",      "setcltid",     /* 31 - 36 */
	"setcltidconf", "verify",       "write",        "rellockowner", "bc_ctl",	"blind_conn",	/* 37 - 42 */
	"exchange_id",	"create_ses",	"destroy_ses",	"free_statid",	"getdirdelag",	"getdevinfo",	/* 43 - 48 */
	"getdevlist",	"layoutcommit",	"layoutget",	"layoutreturn",	"secunfononam",	"sequence",	/* 49 - 54 */
	"set_ssv",	"test_stateid",	"want_deleg",	"destory_clid",	"reclaim_comp",	"stat60",	/* 55 - 60 */
	"stat61",	"stat62",	"stat63",	"stat64",	"stat65",	"stat66",	/* 61 - 66 */
	"stat67",	"stat68",	"stat69",	"stat70",	"stat71",	"stat72"	/* 67 - 72 */
};


int nfs_v4c_names_count=NFS_V4C_NAMES_COUNT;
char *nfs_v4c_names[NFS_V4C_NAMES_COUNT] = {  /* get these names from nfsstat as they are NOT documented */
	"null",         "read",         "write",        "commit",       "open",         "open_conf",    /* 1 - 6 */
	"open_noat",    "open_dgrd",    "close",        "setattr",      "fsinfo",       "renew",        /* 7 - 12 */
	"setclntid",    "confirm",      "lock",         "lockt",        "locku",        "access",       /* 13 - 18 */
	"getattr",      "lookup",       "lookup_root",  "remove",       "rename",       "link",         /* 19 - 24 */
	"symlink",      "create",       "pathconf",     "statfs",       "readlink",     "readdir",      /* 25 - 30 */
	"server_caps",  "delegreturn",  "getacl",       "setacl",       "fs_locations", "rel_lkowner",  /* 31 - 36 */
	"secinfo",      "exchange_id",  "create_ses",   "destroy_ses",  "sequence",     "get_lease_t",  /* 37 - 42 */
	"reclaim_comp", "layoutget",    "getdevinfo",   "layoutcommit", "layoutreturn", "getdevlist",	/* 43 - 48 */
	"stat49", 	"stat50",       "stat51",       "stat52",       "start53", 	"stat54",	/* 49 - 54 */
	"stat55", 	"stat56",       "stat57",       "stat58",       "start59", 	"stat60"	/* 55 - 60 */
};


int nfs_v2c_found=0;
int nfs_v2s_found=0;
int nfs_v3c_found=0;
int nfs_v3s_found=0;
int nfs_v4c_found=0;
int nfs_v4s_found=0;
int nfs_clear=0;




int diskmax = DISKMIN;




long long get_vm_value( char *s)	{
	int currline;
	int currchar;
	long long result = -1;
	char *check;
	int len;
	int found;
	
	for(currline=0; currline<proc[P_VMSTAT].lines; currline++) {
		len = strlen(s);
		for(currchar=0,found=1; currchar<len; currchar++) {
			if( proc[P_VMSTAT].line[currline][currchar] == 0 ||
			   s[currchar] != proc[P_VMSTAT].line[currline][currchar]) {
				found=0;
				break;
			}
		}
		if(found && proc[P_VMSTAT].line[currline][currchar] == ' ')	{
			result = strtoll(&proc[P_VMSTAT].line[currline][currchar+1],&check,10);
			if( *check == proc[P_VMSTAT].line[currline][currchar+1])	{
				fprintf(stderr,"%s has an unexpected format: >%s<\n", proc[P_VMSTAT].filename, proc[P_VMSTAT].line[currline]);
				return -1;
			}
			return result;
		}
	}
	return -1;
}



int read_vmstat()
{
	proc_read(P_VMSTAT);
	if( proc[P_VMSTAT].read_this_interval == 0 || proc[P_VMSTAT].lines == 0)
		return(-1);
	
	/* Note: if the variable requested is not found in /proc/vmstat then it is set to -1 */
	GETVM(nr_dirty);
	GETVM(nr_writeback);
	GETVM(nr_unstable);
	GETVM(nr_page_table_pages);
	GETVM(nr_mapped);
	GETVM(nr_slab);
	GETVM(pgpgin);
	GETVM(pgpgout);
	GETVM(pswpin);
	GETVM(pswpout);
	GETVM(pgalloc_high);
	GETVM(pgalloc_normal);
	GETVM(pgalloc_dma);
	GETVM(pgfree);
	GETVM(pgactivate);
	GETVM(pgdeactivate);
	GETVM(pgfault);
	GETVM(pgmajfault);
	GETVM(pgrefill_high);
	GETVM(pgrefill_normal);
	GETVM(pgrefill_dma);
	GETVM(pgsteal_high);
	GETVM(pgsteal_normal);
	GETVM(pgsteal_dma);
	GETVM(pgscan_kswapd_high);
	GETVM(pgscan_kswapd_normal);
	GETVM(pgscan_kswapd_dma);
	GETVM(pgscan_direct_high);
	GETVM(pgscan_direct_normal);
	GETVM(pgscan_direct_dma);
	GETVM(pginodesteal);
	GETVM(slabs_scanned);
	GETVM(kswapd_steal);
	GETVM(kswapd_inodesteal);
	GETVM(pageoutrun);
	GETVM(allocstall);
	GETVM(pgrotated);
	return 1;
}


/* These macro simplify the access to the Main data structure */
#define DKDELTA(member) ( (q->dk[i].member > p->dk[i].member) ? 0 : (p->dk[i].member - q->dk[i].member))
#define SIDELTA(member) ( (q->si.member > p->si.member)       ? 0 : (p->si.member - q->si.member))

#define IFNAME 64

#define TIMEDELTA(member,index1,index2) ((p->procs[index1].member) - (q->procs[index2].member))
#define COUNTDELTA(member) ( (q->procs[topper[j].other].member > p->procs[i].member) ? 0 : (p->procs[i].member  - q->procs[topper[j].other].member) )

#define TIMED(member) ((double)(p->procs[i].member.tv_sec))

double *cpu_peak; /* ptr to array  - 1 for each cpu - 0 = average for machine */
double *disk_busy_peak;
double *disk_rate_peak;
double net_read_peak[NETMAX];
double net_write_peak[NETMAX];
int aiorunning;
int aiorunning_max = 0;
int aiocount;
int aiocount_max = 0;
float aiotime;
float aiotime_max =0.0;

char *dskgrp(int i)
{
	static char error_string[] = { "Too-Many-Disks" };
	static char *string[16] = {"",   "1",  "2",  "3",
		"4",  "5",  "6",  "7",
		"8",  "9",  "10", "11",
		"12", "13", "14", "15"};
	
	i = (int)((float)i/(float)disks_per_line);
	if(0 <= i && i <= 15 )
		return string[i];
	return error_string;
}

/* command checking against a list */

#define CMDMAX 64

char *cmdlist[CMDMAX];
int cmdfound = 0;

int cmdcheck(char *cmd)
{
	int i;
#ifdef CMDDEBUG
	fprintf(stderr,"cmdfound=%d\n",cmdfound);
	for(i=0;i<cmdfound;i++)
		fprintf(stderr,"cmdlist[%d]=\"%s\"\n",i,cmdlist[i]);
#endif /* CMDDEBUG */
	for(i=0;i<cmdfound;i++) {
		if(strlen(cmdlist[i]) == 0)
			continue;
		if( !strncmp(cmdlist[i],cmd,strlen(cmdlist[i])) )
			return 1;
	}
	return 0;
}

/* Convert secs + micro secs to a double */
double	doubletime(void)
{
	
	gettimeofday(&p->tv, 0);
	return((double)p->tv.tv_sec + p->tv.tv_usec * 1.0e-6);
}

/*
 * Get Intel processor details (modified to use sysctl)
 *  stollcri, 2015-08-22
 */
void get_intel_spec() {
	struct syshw thissys = getsyshwinfo();

	vendor_ptr = thissys.cpubrand;
	model_ptr = thissys.machine;
	mhz_ptr = (char *)malloc(sizeof(int));
	sprintf(mhz_ptr, "%d", (thissys.cpufrequency / 1000000));
	
	cores = thissys.physicalcpucount;
	siblings = thissys.logicalcpucount;
	hyperthreads=thissys.hyperthreads;
	cpus = thissys.cpucount;

	bogo_ptr = 0; // TODO: remove all references to this made up number
	processorchips = 1; // TODO: should not always be 1, find actual value
}

int stat8 = 0; /* used to determine the number of variables on a line */


void proc_nfs()
{
	int i;
	int j;
	int len;
	int lineno;
	
	/* sample /proc/net/rpc/nfs
	 net 0 0 0 0
	 rpc 70137 0 0
	 proc2 18 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
	 proc3 22 0 27364 0 32 828 22 40668 0 1 0 0 0 0 0 0 0 0 1212 6 2 1 0
	 proc4 35 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
	 */
	if(proc[P_NFS].fp != 0) {
		for(lineno=0;lineno<proc[P_NFS].lines;lineno++) {
			if(!strncmp("proc2 ",proc[P_NFS].line[lineno],6)) {
				/* client version 2 line readers "proc2 18 num num etc" */
				len=strlen(proc[P_NFS].line[lineno]);
				for(j=0,i=8;i<len && j<NFS_V2_NAMES_COUNT;i++) {
					if(proc[P_NFS].line[lineno][i] == ' ') {
						p->nfs.v2c[j] =atol(&proc[P_NFS].line[lineno][i+1]);
						nfs_v2c_found=1;
						j++;
					}
				}
			}
			if(!strncmp("proc3 ",proc[P_NFS].line[lineno],6)) {
				/* client version 3 line readers "proc3 22 num num etc" */
				len=strlen(proc[P_NFS].line[lineno]);
				for(j=0,i=8;i<len && j<NFS_V3_NAMES_COUNT;i++) {
					if(proc[P_NFS].line[lineno][i] == ' ') {
						p->nfs.v3c[j] =atol(&proc[P_NFS].line[lineno][i+1]);
						nfs_v3c_found=1;
						j++;
					}
				}
			}
			if(!strncmp("proc4 ",proc[P_NFS].line[lineno],6)) {
				/* client version 4 line readers "proc4 35 num num etc" */
				nfs_v4c_names_count = atoi(&proc[P_NFS].line[lineno][6]);
				len=strlen(proc[P_NFS].line[lineno]);
				for(j=0,i=8; i<len && j<nfs_v4c_names_count; i++) {
					if(proc[P_NFS].line[lineno][i] == ' ') {
						p->nfs.v4c[j] =atol(&proc[P_NFS].line[lineno][i+1]);
						nfs_v4c_found=1;
						j++;
					}
				}
			}
		}
	}
	/* sample /proc/net/rpc/nfsd
	 rc 0 0 0
	 fh 0 0 0 0 0
	 io 0 0
	 th 4 0 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000 0.000
	 ra 32 0 0 0 0 0 0 0 0 0 0 0
	 net 0 0 0 0
	 rpc 0 0 0 0 0
	 proc2 18 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
	 proc3 22 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
	 proc4 2 0 0
	 proc4ops 40 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
	 */
	if(proc[P_NFSD].fp != 0) {
		for(lineno=0;lineno<proc[P_NFSD].lines;lineno++) {
			if(!strncmp("proc2 ",proc[P_NFSD].line[lineno],6)) {
				/* server version 2 line readers "proc2 18 num num etc" */
				len=strlen(proc[P_NFSD].line[lineno]);
				for(j=0,i=8;i<len && j<NFS_V2_NAMES_COUNT;i++) {
					if(proc[P_NFSD].line[lineno][i] == ' ') {
						p->nfs.v2s[j] =atol(&proc[P_NFSD].line[lineno][i+1]);
						nfs_v2s_found=1;
						j++;
					}
				}
			}
			if(!strncmp("proc3 ",proc[P_NFSD].line[lineno],6)) {
				/* server version 3 line readers "proc3 22 num num etc" */
				len=strlen(proc[P_NFSD].line[lineno]);
				for(j=0,i=8;i<len && j<NFS_V2_NAMES_COUNT;i++) {
					if(proc[P_NFSD].line[lineno][i] == ' ') {
						p->nfs.v3s[j] =atol(&proc[P_NFSD].line[lineno][i+1]);
						nfs_v3s_found=1;
						j++;
					}
				}
			}
			if(!strncmp("proc4ops ",proc[P_NFSD].line[lineno],9)) {
				/* server version 4 line readers "proc4ops 40 num num etc"
				 NOTE: the "ops" hence starting in column 9 */
				nfs_v4s_names_count = atol(&proc[P_NFSD].line[lineno][9]);
				len=strlen(proc[P_NFSD].line[lineno]);
				for(j=0,i=11; i<len && j<nfs_v4s_names_count; i++) {
					if(proc[P_NFSD].line[lineno][i] == ' ') {
						p->nfs.v4s[j] =atol(&proc[P_NFSD].line[lineno][i+1]);
						nfs_v4s_found=1;
						j++;
					}
				}
			}
		}
	}
}

void proc_kernel()
{
	int i;
	p->cpu_total.uptime=0.0;
	p->cpu_total.idletime=0.0;
	p->cpu_total.uptime=atof(proc[P_UPTIME].line[0]);
	for(i=0;i<strlen(proc[P_UPTIME].line[0]);i++) {
		if(proc[P_UPTIME].line[0][i] == ' ') {
			p->cpu_total.idletime=atof(&proc[P_UPTIME].line[0][i+1]);
			break;
		}
	}
	
	sscanf(&proc[P_LOADAVG].line[0][0], "%f %f %f",
		   &p->cpu_total.mins1,
		   &p->cpu_total.mins5,
		   &p->cpu_total.mins15);
	
}

char *proc_find_sb(char * p)
{
	for(; *p != 0;p++)
		if(*p == ' ' && *(p+1) == '(')
			return p;
	return 0;
}

#define DISK_MODE_IO 1
#define DISK_MODE_DISKSTATS 2
#define DISK_MODE_PARTITIONS 3

int disk_mode = 0;

void proc_disk_io(double elapsed)
{
	int diskline;
	int i;
	int ret;
	char *str;
	int fudged_busy;
	
	disks = 0;
	for(diskline=0;diskline<proc[P_STAT].lines;diskline++) {
		if(strncmp("disk_io", proc[P_STAT].line[diskline],7) == 0)
			break;
	}
	for(i=8;i<strlen(proc[P_STAT].line[diskline]);i++) {
		if( proc[P_STAT].line[diskline][i] == ':')
			disks++;
	}
	
	str=&proc[P_STAT].line[diskline][0];
	for(i=0;i<disks;i++) {
		str=proc_find_sb(str);
		if(str == 0)
			break;
		ret = sscanf(str, " (%d,%d):(%ld,%ld,%ld,%ld,%ld",
					 &p->dk[i].dk_major,
					 &p->dk[i].dk_minor,
					 &p->dk[i].dk_noinfo,
					 &p->dk[i].dk_reads,
					 &p->dk[i].dk_rkb,
					 &p->dk[i].dk_writes,
					 &p->dk[i].dk_wkb);
		if(ret != 7)
			exit(7);
		p->dk[i].dk_xfers = p->dk[i].dk_noinfo;
		/* blocks  are 512 bytes*/
		p->dk[i].dk_rkb = p->dk[i].dk_rkb/2;
		p->dk[i].dk_wkb = p->dk[i].dk_wkb/2;
		
		p->dk[i].dk_bsize = (p->dk[i].dk_rkb+p->dk[i].dk_wkb)/p->dk[i].dk_xfers*1024;
		
		/* assume a disk does 200 op per second */
		fudged_busy = (p->dk[i].dk_reads + p->dk[i].dk_writes)/2;
		if(fudged_busy > 100*elapsed)
			p->dk[i].dk_time += 100*elapsed;
		p->dk[i].dk_time = fudged_busy;
		
		sprintf(p->dk[i].dk_name,"dev-%d-%d",p->dk[i].dk_major,p->dk[i].dk_minor);
		/*	fprintf(stderr,"disk=%d name=\"%s\" major=%d minor=%d\n", i,p->dk[i].dk_name, p->dk[i].dk_major,p->dk[i].dk_minor); */
		str++;
	}
}

void proc_diskstats(double elapsed)
{
	static FILE *fp = (FILE *)-1;
	char buf[1024];
	int i;
	int ret;
	
	if( fp == (FILE *)-1) {
		if( (fp = fopen("/Users/stollcri/Documents/code/c/nmond/dbg/diskstats","r")) == NULL) {
			/* DEBUG if( (fp = fopen("diskstats","r")) == NULL) { */
			//error("failed to open - /Users/stollcri/Documents/code/c/nmond/dbg/diskstats");
			disks=0;
			return;
		}
	}
	/*
	 2    0 fd0 1 0 2 13491 0 0 0 0 0 13491 13491
	 3    0 hda 41159 53633 1102978 620181 39342 67538 857108 4042631 0 289150 4668250
	 3    1 hda1 58209 58218 0 0
	 3    2 hda2 148 4794 10 20
	 3    3 hda3 65 520 0 0
	 3    4 hda4 35943 1036092 107136 857088
	 22    0 hdc 167 5394 22308 32250 0 0 0 0 0 22671 32250 <-- USB !!
	 8    0 sda 990 2325 4764 6860 9 3 12 417 0 6003 7277
	 8    1 sda1 3264 4356 12 12
	 */
	for(i=0;i<DISKMAX;) {
		if(fgets(buf,1024,fp) == NULL)
			break;
		/* zero the data ready for reading */
		p->dk[i].dk_major =
		p->dk[i].dk_minor =
		p->dk[i].dk_name[0] =
		p->dk[i].dk_reads =
		p->dk[i].dk_rmerge =
		p->dk[i].dk_rkb =
		p->dk[i].dk_rmsec =
		p->dk[i].dk_writes =
		p->dk[i].dk_wmerge =
		p->dk[i].dk_wkb =
		p->dk[i].dk_wmsec =
		p->dk[i].dk_inflight =
		p->dk[i].dk_time =
		p->dk[i].dk_backlog =0;
		
		ret = sscanf(&buf[0], "%d %d %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
					 &p->dk[i].dk_major,
					 &p->dk[i].dk_minor,
					 &p->dk[i].dk_name[0],
					 &p->dk[i].dk_reads,
					 &p->dk[i].dk_rmerge,
					 &p->dk[i].dk_rkb,
					 &p->dk[i].dk_rmsec,
					 &p->dk[i].dk_writes,
					 &p->dk[i].dk_wmerge,
					 &p->dk[i].dk_wkb,
					 &p->dk[i].dk_wmsec,
					 &p->dk[i].dk_inflight,
					 &p->dk[i].dk_time,
					 &p->dk[i].dk_backlog );
		if(ret == 7) { /* shuffle the data around due to missing columns for partitions */
			p->dk[i].dk_partition = 1;
			p->dk[i].dk_wkb = p->dk[i].dk_rmsec;
			p->dk[i].dk_writes = p->dk[i].dk_rkb;
			p->dk[i].dk_rkb = p->dk[i].dk_rmerge;
			p->dk[i].dk_rmsec=0;
			p->dk[i].dk_rmerge=0;
			
		}
		else if(ret == 14) p->dk[i].dk_partition = 0;
		else fprintf(stderr,"disk sscanf wanted 14 but returned=%d line=%s\n",
					 ret,buf);
		
		p->dk[i].dk_rkb /= 2; /* sectors = 512 bytes */
		p->dk[i].dk_wkb /= 2;
		p->dk[i].dk_xfers = p->dk[i].dk_reads + p->dk[i].dk_writes;
		if(p->dk[i].dk_xfers == 0)
			p->dk[i].dk_bsize = 0;
		else
			p->dk[i].dk_bsize = ((p->dk[i].dk_rkb+p->dk[i].dk_wkb)/p->dk[i].dk_xfers)*1024;
		
		p->dk[i].dk_time /= 10.0; /* in milli-seconds to make it upto 100%, 1000/100 = 10 */
		
		if( p->dk[i].dk_xfers > 0)
			i++;
	}
	rewind(fp);
	disks = i;
}

void strip_spaces(char *s)
{
	char *p;
	int spaced=1;
	
	p=s;
	for(p=s;*p!=0;p++) {
		if(*p == ':')
			*p=' ';
		if(*p != ' ') {
			*s=*p;
			s++;
			spaced=0;
		} else if(spaced) {
			/* do no thing as this is second space */
		} else {
			*s=*p;
			s++;
			spaced=1;
		}
		
	}
	*s = 0;
}

void proc_partitions(double elapsed)
{
	static FILE *fp = (FILE *)-1;
	char buf[1024];
	int i = 0;
	int ret;
	
	if( fp == (FILE *)-1) {
		if( (fp = fopen("/Users/stollcri/Documents/code/c/nmond/dbg/partitions","r")) == NULL) {
			//error("failed to open - /Users/stollcri/Documents/code/c/nmond/dbg/partitions");
			partitions=0;
			return;
		}
	}
	if(fgets(buf,1024,fp) == NULL) goto end; /* throw away the header lines */
	if(fgets(buf,1024,fp) == NULL) goto end;
	/*
	 major minor  #blocks  name     rio rmerge rsect ruse wio wmerge wsect wuse running use aveq
	 
	 33     0    1052352 hde 2855 15 2890 4760 0 0 0 0 -4 7902400 11345292
	 33     1    1050304 hde1 2850 0 2850 3930 0 0 0 0 0 3930 3930
	 3     0   39070080 hda 9287 19942 226517 90620 8434 25707 235554 425790 -12 7954830 33997658
	 3     1   31744408 hda1 651 90 5297 2030 0 0 0 0 0 2030 2030
	 3     2    6138720 hda2 7808 19561 218922 79430 7299 20529 222872 241980 0 59950 321410
	 3     3     771120 hda3 13 41 168 80 0 0 0 0 0 80 80
	 3     4          1 hda4 0 0 0 0 0 0 0 0 0 0 0
	 3     5     408208 hda5 812 241 2106 9040 1135 5178 12682 183810 0 11230 192850
	 */
	for(i=0;i<DISKMAX;i++) {
		if(fgets(buf,1024,fp) == NULL)
			break;
		strip_spaces(buf);
		ret = sscanf(&buf[0], "%d %d %lu %s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
					 &p->dk[i].dk_major,
					 &p->dk[i].dk_minor,
					 &p->dk[i].dk_blocks,
					 (char *)&p->dk[i].dk_name,
					 &p->dk[i].dk_reads,
					 &p->dk[i].dk_rmerge,
					 &p->dk[i].dk_rkb,
					 &p->dk[i].dk_rmsec,
					 &p->dk[i].dk_writes,
					 &p->dk[i].dk_wmerge,
					 &p->dk[i].dk_wkb,
					 &p->dk[i].dk_wmsec,
					 &p->dk[i].dk_inflight,
					 &p->dk[i].dk_use,
					 &p->dk[i].dk_aveq
					 );
		p->dk[i].dk_rkb /= 2; /* sectors = 512 bytes */
		p->dk[i].dk_wkb /= 2;
		p->dk[i].dk_xfers = p->dk[i].dk_rkb + p->dk[i].dk_wkb;
		if(p->dk[i].dk_xfers == 0)
			p->dk[i].dk_bsize = 0;
		else
			p->dk[i].dk_bsize = (p->dk[i].dk_rkb+p->dk[i].dk_wkb)/p->dk[i].dk_xfers*1024;
		
		p->dk[i].dk_time /= 10.0; /* in milli-seconds to make it upto 100%, 1000/100 = 10 */
		
		if(ret != 15) {
#ifdef DEBUG
			if(debug)fprintf(stderr,"sscanf wanted 15 returned = %d line=%s\n", ret,buf);
#endif /*DEBUG*/
			partitions_short = 1;
		} else partitions_short = 0;
	}
end:
	rewind(fp);
	disks = i;
}

void proc_disk(double elapsed)
{
	struct stat buf;
	int ret;
	if(disk_mode == 0) {
		ret = stat("/Users/stollcri/Documents/code/c/nmond/dbg/diskstats", &buf);
		if(ret == 0) {
			disk_mode=DISK_MODE_DISKSTATS;
		} else {
			ret = stat("/Users/stollcri/Documents/code/c/nmond/dbg/partitions", &buf);
			if(ret == 0) {
				disk_mode=DISK_MODE_PARTITIONS;
			} else {
				disk_mode=DISK_MODE_IO;
			}
		}
	}
	switch(disk_mode){
		case DISK_MODE_IO: 		proc_disk_io(elapsed);   break;
		case DISK_MODE_DISKSTATS: 	proc_diskstats(elapsed); break;
		case DISK_MODE_PARTITIONS: 	proc_partitions(elapsed); break;
	}
}
#undef isdigit
#define isdigit(ch) ( ( '0' <= (ch)  &&  (ch) >= '9')? 0: 1 )

long proc_mem_search( char *s)
{
	int i;
	int j;
	int len;
	len=strlen(s);
	for(i=0;i<proc[P_MEMINFO].lines;i++ ) {
		if( !strncmp(s, proc[P_MEMINFO].line[i],len) ) {
			for(j=len;
				!isdigit(proc[P_MEMINFO].line[i][j]) &&
				proc[P_MEMINFO].line[i][j] != 0;
				j++)
			/* do nothing */ ;
			return atol( &proc[P_MEMINFO].line[i][j]);
		}
	}
	return -1;
}

void proc_mem()
{
	if( proc[P_MEMINFO].read_this_interval == 0)
		proc_read(P_MEMINFO);
	
	p->mem.memtotal   = proc_mem_search("MemTotal");
	p->mem.memfree    = proc_mem_search("MemFree");
	p->mem.memshared  = proc_mem_search("MemShared");
	p->mem.buffers    = proc_mem_search("Buffers");
	p->mem.cached     = proc_mem_search("Cached");
	p->mem.swapcached = proc_mem_search("SwapCached");
	p->mem.active     = proc_mem_search("Active");
	p->mem.inactive   = proc_mem_search("Inactive");
	p->mem.hightotal  = proc_mem_search("HighTotal");
	p->mem.highfree   = proc_mem_search("HighFree");
	p->mem.lowtotal   = proc_mem_search("LowTotal");
	p->mem.lowfree    = proc_mem_search("LowFree");
	p->mem.swaptotal  = proc_mem_search("SwapTotal");
	p->mem.swapfree   = proc_mem_search("SwapFree");
#ifdef LARGEMEM
	p->mem.dirty         = proc_mem_search("Dirty");
	p->mem.writeback     = proc_mem_search("Writeback");
	p->mem.mapped        = proc_mem_search("Mapped");
	p->mem.slab          = proc_mem_search("Slab");
	p->mem.committed_as  = proc_mem_search("Committed_AS");
	p->mem.pagetables    = proc_mem_search("PageTables");
	p->mem.hugetotal     = proc_mem_search("HugePages_Total");
	p->mem.hugefree      = proc_mem_search("HugePages_Free");
	p->mem.hugesize      = proc_mem_search("Hugepagesize");
#else
	p->mem.bigfree       = proc_mem_search("BigFree");
#endif /*LARGEMEM*/
}



int next_cpu_snap = 0;
int cpu_snap_all = 0;



int snap_average()
{
	int i;
	int end;
	int total = 0;
	
	if(cpu_snap_all)
		end = MAX_SNAPS;
	else
		end = next_cpu_snap;
	
	for(i=0;i<end;i++) {
		total = total + cpu_snap[i].user + cpu_snap[i].kernel;
	}
	return (total / end) ;
}

void snap_clear()
{
	int i;
	for(i=0;i<MAX_SNAPS;i++) {
		cpu_snap[i].user = 0;
		cpu_snap[i].kernel = 0;
		cpu_snap[i].iowait = 0;
		cpu_snap[i].idle = 0;
		cpu_snap[i].steal= 0;
	}
	next_cpu_snap=0;
	cpu_snap_all=0;
}


void init_pairs()
{
	COLOUR init_pair((short)0,(short)7,(short)0); /* White */
	COLOUR init_pair((short)1,(short)1,(short)0); /* Red */
	COLOUR init_pair((short)2,(short)2,(short)0); /* Green */
	COLOUR init_pair((short)3,(short)3,(short)0); /* Yellow */
	COLOUR init_pair((short)4,(short)4,(short)0); /* Blue */
	COLOUR init_pair((short)5,(short)5,(short)0); /* Magenta */
	COLOUR init_pair((short)6,(short)6,(short)0); /* Cyan */
	COLOUR init_pair((short)7,(short)7,(short)0); /* White */
	COLOUR init_pair((short)8,(short)0,(short)1); /* Red background, red text */
	COLOUR init_pair((short)9,(short)0,(short)2); /* Green background, green text */
	COLOUR init_pair((short)10,(short)0,(short)4); /* Blue background, blue text */
	COLOUR init_pair((short)11,(short)0,(short)3); /* Yellow background, yellow text */
	COLOUR init_pair((short)12,(short)0,(short)6); /* Cyan background, cyan text */
}


/* only place the q=previous and p=currect pointers are modified */
void switcher(void)
{
	static int	which = 1;
	int i;
	
	if (which) {
		p = &database[0];
		q = &database[1];
		which = 0;
	} else {
		p = &database[1];
		q = &database[0];
		which = 1;
	}
	if(flash_on)
		flash_on = 0;
	else
		flash_on = 1;
	
	// Reset flags so /proc/... is re-read in next interval 
	for(i=0;i<P_NUMBER;i++) {
		proc[i].read_this_interval = 0;
	}
}


/* Lookup the right string */
char	*status(int n)
{
	switch (n) {
		case 0:
			return "Run  ";
		default:
			return "Sleep";
	}
}

/* Lookup the right process state string */
char	*get_state( char n)
{
	static char	duff[64];
	switch (n) {
		case 'R': return "Running  ";
		case 'S': return "Sleeping ";
		case 'D': return "DiskSleep";
		case 'Z': return "Zombie   ";
		case 'T': return "Traced   ";
		case 'W': return "Paging   ";
		default:
			sprintf(duff, "%d", n);
			return duff;
	}
}

#ifdef GETUSER
/* Convert User id (UID) to a name with caching for speed
 * getpwuid() should be NFS/yellow pages safe
 */
char	*getuser(uid_t uid)
{
#define NAMESIZE 16
	struct user_info {
		uid_t uid;
		char	name[NAMESIZE];
	};
	static struct user_info *u = NULL;
	static int	used = 0;
	int	i;
	struct passwd *pw;
	
	i = 0;
	if (u != NULL) {
		for (i = 0; i < used; i++) {
			if (u[i].uid == uid) {
				return u[i].name;
			}
		}
		u = (struct user_info *)REALLOC(u, (sizeof(struct user_info ) * (i + 1)));
	} else
		u = (struct user_info *)MALLOC(sizeof(struct user_info ));
	used++;
	
	/* failed to find a match so add it */
	u[i].uid = uid;
	pw = getpwuid(uid);
	
	if (pw != NULL)
		strncpy(u[i].name, pw->pw_name, NAMESIZE);
	else
		sprintf(u[i].name, "unknown%d",uid);
	return u[i].name;
}
#endif /* GETUSER */

/* User Defined Disk Groups */

char   *save_word(char *in, char *out)
{
	int   len;
	int   i;
	len = strlen(in);
	out[0] = 0;
	for (i = 0; i < len; i++) {
		if ( isalnum(in[i]) || in[i] == '_' || in[i] == '-' || in[i] == '/' ) {
			out[i] = in[i];
			out[i+1] = 0;
		} else
			break;
	}
	for (; i < len; i++)
		if (isalnum(in[i]))
			return &in[i];
	return &in[i];
}

#define DGROUPS 64
#define DGROUPITEMS 512

char   *dgroup_filename;
char   *dgroup_name[DGROUPS];
int   *dgroup_data;
int   dgroup_disks[DGROUPS];
int   dgroup_total_disks = 0;
int   dgroup_total_groups;

void load_dgroup(struct dsk_stat *dk)
{
	FILE * gp;
	char   line[4096];
	char   name[1024];
	int   i, j;
	char   *nextp;
	
	if (dgroup_loaded == 2)
		return;
	dgroup_data = MALLOC(sizeof(int)*DGROUPS * DGROUPITEMS);
	for (i = 0; i < DGROUPS; i++)
		for (j = 0; j < DGROUPITEMS; j++)
			dgroup_data[i*DGROUPITEMS+j] = -1;
	
	gp = fopen(dgroup_filename, "r");
	
	if (gp == NULL) {
		perror("opening disk group file");
		fprintf(stderr,"ERROR: failed to open %s\n", dgroup_filename);
		exit(9);
	}
	
	for (dgroup_total_groups = 0;
		 fgets(line, 4096-1, gp) != NULL && dgroup_total_groups < DGROUPS;
		 dgroup_total_groups++) {
		/* ignore lines starting with # */
		if(line[0] == '#' ) { /* was a comment line */
			/* Decrement dgroup_total_groups by 1 to correct index for next loop */
			--dgroup_total_groups;
			continue;
		}
		/* save the name */
		nextp = save_word(line, name);
		if(strlen(name) == 0) { /* was a blank line */
			fprintf(stderr,"ERROR nmon:ignoring odd line in diskgroup file \"%s\"\n",line);
			/* Decrement dgroup_total_groups by 1 to correct index for next loop */
			--dgroup_total_groups;
			continue;
		}
		/* Added +1 to be able to correctly store the terminating \0 character */
		dgroup_name[dgroup_total_groups] = MALLOC(strlen(name)+1);
		strcpy(dgroup_name[dgroup_total_groups], name);
		
		/* save the hdisks */
		for (i = 0; i < DGROUPITEMS && *nextp != 0; i++) {
			nextp = save_word(nextp, name);
			for (j = 0; j < disks; j++) {
				if ( strcmp(dk[j].dk_name, name) == 0 ) {
					/*DEBUG printf("DGadd group=%s,name=%s,disk=%s,dgroup_total_groups=%d,dgroup_total_disks=%d,j=%d,i=%d,index=%d.\n",
					 dgroup_name[dgroup_total_groups],
					 name, dk[j].dk_name, dgroup_total_groups, dgroup_total_disks, j, i,dgroup_total_groups*DGROUPITEMS+i);
					 */
					dgroup_data[dgroup_total_groups*DGROUPITEMS+i] = j;
					dgroup_disks[dgroup_total_groups]++;
					dgroup_total_disks++;
					break;
				}
			}
			if (j == disks)
				fprintf(stderr,"ERROR nmon:diskgroup file - failed to find disk=%s for group=%s disks known=%d\n",
						name, dgroup_name[dgroup_total_groups],disks);
		}
	}
	fclose(gp);
	dgroup_loaded = 2;
}


void list_dgroup(struct dsk_stat *dk)
{
	int   i, j, k, n;
	int   first = 1;
	
	/* DEBUG for (n = 0, i = 0; i < dgroup_total_groups; i++) {
	 fprintf(fp, "CCCG,%03d,%s", n++, dgroup_name[i]);
	 for (j = 0; j < dgroup_disks[i]; j++) {
	 if (dgroup_data[i*DGROUPITEMS+j] != -1) {
	 fprintf(fp, ",%d=%d", j, dgroup_data[i*DGROUPITEMS+j]);
	 }
	 }
	 fprintf(fp, "\n");
	 }
	 */
	if( !show_dgroup) return;
	
	for (n = 0, i = 0; i < dgroup_total_groups; i++) {
		if (first) {
			fprintf(fp, "BBBG,%03d,User Defined Disk Groups Name,Disks\n", n++);
			first = 0;
		}
		fprintf(fp, "BBBG,%03d,%s", n++, dgroup_name[i]);
		for (k = 0, j = 0; j < dgroup_disks[i]; j++) {
			if (dgroup_data[i*DGROUPITEMS+j] != -1) {
				fprintf(fp, ",%s", dk[dgroup_data[i*DGROUPITEMS+j]].dk_name);
				k++;
			}
			/* add extra line if we have lots to stop spreadsheet line width problems */
			if (k == 128) {
				fprintf(fp, "\nBBBG,%03d,%s continued", n++, dgroup_name[i]);
			}
		}
		fprintf(fp, "\n");
	}
	fprintf(fp, "DGBUSY,Disk Group Busy %s", hostname);
	for (i = 0; i < DGROUPS; i++) {
		if (dgroup_name[i] != 0)
			fprintf(fp, ",%s", dgroup_name[i]);
	}
	fprintf(fp, "\n");
	fprintf(fp, "DGREAD,Disk Group Read KB/s %s", hostname);
	for (i = 0; i < DGROUPS; i++) {
		if (dgroup_name[i] != 0)
			fprintf(fp, ",%s", dgroup_name[i]);
	}
	fprintf(fp, "\n");
	fprintf(fp, "DGWRITE,Disk Group Write KB/s %s", hostname);
	for (i = 0; i < DGROUPS; i++) {
		if (dgroup_name[i] != 0)
			fprintf(fp, ",%s", dgroup_name[i]);
	}
	fprintf(fp, "\n");
	fprintf(fp, "DGSIZE,Disk Group Block Size KB %s", hostname);
	for (i = 0; i < DGROUPS; i++) {
		if (dgroup_name[i] != 0)
			fprintf(fp, ",%s", dgroup_name[i]);
	}
	fprintf(fp, "\n");
	fprintf(fp, "DGXFER,Disk Group Transfers/s %s", hostname);
	for (i = 0; i < DGROUPS; i++) {
		if (dgroup_name[i] != 0)
			fprintf(fp, ",%s", dgroup_name[i]);
	}
	fprintf(fp, "\n");
}

int is_dgroup_name(char *name)
{
	int i;
	for (i = 0; i < DGROUPS; i++) {
		if(dgroup_name[i] == (char *)0 )
			return 0;
		if (strncmp(name,dgroup_name[i],strlen(name)) == 0)
			return 1;
	}
	return 0;
}


int jfses =0;
void jfs_load(int load)
{
	int i;
	struct stat stat_buffer;
	FILE * mfp; /* FILE pointer for mtab file*/
	struct mntent *mp; /* mnt point stats */
	static int jfs_loaded = 0;
	
	if(load==LOAD) {
		if(jfs_loaded == 0) {
			mfp = setmntent("/etc/mtab","r");
			for(i=0; i<JFSMAX && (mp = getmntent(mfp) ) != NULL; i++) {
				strncpy(jfs[i].device, mp->mnt_fsname,JFSNAMELEN);
				strncpy(jfs[i].name,mp->mnt_dir,JFSNAMELEN);
				strncpy(jfs[i].type, mp->mnt_type,JFSTYPELEN);
				mp->mnt_fsname[JFSNAMELEN-1]=0;
				mp->mnt_dir[JFSNAMELEN-1]=0;
				mp->mnt_type[JFSTYPELEN-1]=0;
			}
			endfsent();
			jfs_loaded = 1;
			jfses=i;
		}
		
		/* 1st or later time - just reopen the mount points */
		for(i=0;i<JFSMAX && jfs[i].name[0] !=0;i++) {
			if(stat(jfs[i].name, &stat_buffer) != -1 ) {
				
				jfs[i].fd = open(jfs[i].name, O_RDONLY);
				if(jfs[i].fd != -1 )
					jfs[i].mounted = 1;
				else
					jfs[i].mounted = 0;
			}
			else jfs[i].mounted = 0;
		}
	} else { /* this is an unload request */
		if(jfs_loaded)
			for(i=0;i<JFSMAX && jfs[i].name[0] != 0;i++) {
				if(jfs[i].mounted)
					close(jfs[i].fd);
				jfs[i].fd=0;
			}
		else
		/* do nothing */ ;
	}
}

/* Routine used by qsort to order the processes by CPU usage */
int	cpu_compare(const void *a, const void *b)
{
	return (int)(((struct topper *)b)->time - ((struct topper *)a)->time);
}

int	size_compare(const void *a, const void *b)
{
	return (int)((((struct topper *)b)->size - ((struct topper *)a)->size));
}

int	disk_compare(const void *a, const void *b)
{
	return (int)((((struct topper *)b)->io - ((struct topper *)a)->io));
}



/* checkinput is the subroutine to handle user input */
// int checkinput(void)
// {
// 	static int use_env = 1;
// 	char	buf[1024];
// 	int	bytes;
// 	int	chars;
// 	int	i;
// 	char *p;
	
// 	ioctl(fileno(stdin), FIONREAD, &bytes);
	
// 	if (bytes > 0 || use_env) {
// 		if(use_env) {
// 			use_env = 0;
// 			p=getenv("NMON");
// 			if(p!=0){
// 				strcpy(buf,p);
// 				chars = strlen(buf);
// 			}
// 			else chars = 0;
// 		}
// 		else
// 			chars = read(fileno(stdin), buf, bytes);
// 		if (chars > 0) {
// 			welcome = 0;
// 			for (i = 0; i < chars; i++) {
// 				switch (buf[i]) {
// 					case 'x':
// 					case 'q':
// 						nocbreak();
// 						endwin();
// 						exit(0);
						
// 					case '6':
// 					case '7':
// 					case '8':
// 					case '9':
// 						dotline = buf[i] - '0';
// 						break;
// 					case '+':
// 						seconds = seconds * 2;
// 						break;
// 					case '-':
// 						seconds = seconds / 2;
// 						if (seconds < 1)
// 							seconds = 1;
// 						break;
// 					case '.':
// 						if (show_all)
// 							show_all = 0;
// 						else {
// 							show_all = 1;
// 							show_disk = SHOW_DISK_STATS;
// 							show_top = 1;
// 							show_topmode =3;
// 						}
// 						wclear(paddisk);
// 						break;
// 					case '?':
// 					case 'h':
// 					case 'H':
// 						if (show_help)
// 							show_help = 0;
// 						else {
// 							show_help = 1;
// 							show_verbose = 0;
// 						}
// 						wclear(padhelp);
// 						break;
// 					case 'b':
// 					case 'B':
// 						FLIP(colour);
// 						clear();
// 						break;
// 					case 'Z':
// 						FLIP(show_raw);
// 						show_smp=1;
// 						wclear(padsmp);
// 						break;
// 					case 'l':
// 						FLIP (show_longterm);
// 						wclear(padlong);
// 						//cpulongitter = 0;
// 						break;
// 					case 'V':
// 						FLIP(show_vm);
// 						wclear(padpage);
// 						break;
// 					case 'j':
// 					case 'J':
// 						FLIP(show_jfs);
// 						jfs_load(show_jfs);
// 						wclear(padjfs);
// 						break;
// 					case 'k':
// 					case 'K':
// 						FLIP(show_kernel);
// 						wclear(padkstat);
// 						break;
// 					case 'm':
// 					case 'M':
// 						FLIP(show_memory);
// 						wclear(padmem);
// 						break;
// 					case 'L':
// 						FLIP(show_large);
// 						wclear(padlarge);
// 						break;
// 					case 'D':
// 						switch (show_disk) {
// 							case SHOW_DISK_NONE:
// 								show_disk = SHOW_DISK_STATS;
// 								break;
// 							case SHOW_DISK_STATS:
// 								show_disk = SHOW_DISK_NONE;
// 								break;
// 							case SHOW_DISK_GRAPH:
// 								show_disk = SHOW_DISK_STATS;
// 								break;
// 						}
// 						wclear(paddisk);
// 						break;
// 					case 'd':
// 						switch (show_disk) {
// 							case SHOW_DISK_NONE:
// 								show_disk = SHOW_DISK_GRAPH;
// 								break;
// 							case SHOW_DISK_STATS:
// 								show_disk = SHOW_DISK_GRAPH;
// 								break;
// 							case SHOW_DISK_GRAPH:
// 								show_disk = 0;
// 								break;
// 						}
// 						wclear(paddisk);
// 						break;
// 					case 'o':
// 					case 'O':
// 						FLIP(show_diskmap);
// 						wclear(padmap);
// 						break;
// 					case 'n':
// 						if (show_net) {
// 							show_net = 0;
// 							show_neterror = 0;
// 						} else {
// 							show_net = 1;
// 							show_neterror = 3;
// 						}
// 						wclear(padnet);
// 						break;
// 					case 'N':
// 						if(show_nfs == 0)
// 							show_nfs = 1;
// 						else if(show_nfs == 1)
// 							show_nfs = 2;
// 						else if(show_nfs == 2)
// 							show_nfs = 3;
// 						else if(show_nfs == 3)
// 							show_nfs = 0;
// 						nfs_clear=1;
// 						wclear(padnfs);
// 						break;
// 					case 'c':
// 					case 'C':
// 						FLIP(show_smp);
// 						wclear(padsmp);
// 						break;
// 					case 'r':
// 					case 'R':
// 						FLIP(show_cpu);
// 						wclear(padsys);
// 						break;
// 					case 't':
// 						show_topmode = 3; /* Fall Through */
// 					case 'T':
// 						FLIP(show_top);
// 						wclear(padtop);
// 						break;
// 					case 'v':
// 						FLIP(show_verbose);
// 						wclear(padverb);
// 						break;
// 					case 'u':
// 						if (show_args == ARGS_NONE) {
// 							args_load();
// 							show_args = ARGS_ONLY;
// 							show_top = 1;
// 							if( show_topmode != 3 &&
// 							   show_topmode != 4 &&
// 							   show_topmode != 5 )
// 								show_topmode = 3;
// 						} else
// 							show_args = ARGS_NONE;
// 						wclear(padtop);
// 						break;
// 					case '1':
// 						show_topmode = 1;
// 						show_top = 1;
// 						wclear(padtop);
// 						break;
// 						/*
// 						 case '2':
// 						 show_topmode = 2;
// 						 show_top = 1;
// 						 clear();
// 						 break;
// 						 */
// 					case '3':
// 						show_topmode = 3;
// 						show_top = 1;
// 						wclear(padtop);
// 						break;
// 					case '4':
// 						show_topmode = 4;
// 						show_top = 1;
// 						wclear(padtop);
// 						break;
// 					case '5':
// 						if(isroot) {
// 							show_topmode = 5;
// 							show_top = 1;
// 							wclear(padtop);
// 						}
// 						break;
// 					case '0':
// 						for(i=0;i<(cpus+1);i++)
// 							cpu_peak[i]=0;
// 						for(i=0;i<networks;i++) {
// 							net_read_peak[i]=0.0;
// 							net_write_peak[i]=0.0;
// 						}
// 						for(i=0;i<disks;i++) {
// 							disk_busy_peak[i]=0.0;
// 							disk_rate_peak[i]=0.0;
// 						}
// 						snap_clear();
// 						aiocount_max = 0;
// 						aiotime_max = 0.0;
// 						aiorunning_max = 0;
// 						huge_peak = 0;
// 						break;
// 					case ' ':
// 						clear();
// 						break;
// 					case 'G':
// 						if(auto_dgroup) {
// 							FLIP(disk_only_mode);
// 							clear();
// 						}
// 						break;
// 					case 'g':
// 						FLIP(show_dgroup);
// 						wclear(paddg);
// 						break;
						
// 					default: return 0;
// 				}
// 			}
// 			return 1;
// 		}
// 	}
// 	return 0;
// }

void proc_net()
{
	static FILE *fp = (FILE *)-1;
	char buf[1024];
	int i=0;
	int ret;
	unsigned long junk;
	
	if( fp == (FILE *)-1) {
		if( (fp = fopen("/Users/stollcri/Documents/code/c/nmond/dbg/net/dev","r")) == NULL) {
			//error("failed to open - /Users/stollcri/Documents/code/c/nmond/dbg/net/dev");
			networks=0;
			return;
		}
	}
	if(fgets(buf,1024,fp) == NULL) goto end; /* throw away the header lines */
	if(fgets(buf,1024,fp) == NULL) goto end; /* throw away the header lines */
	/*
	 Inter-|   Receive                                                |  Transmit
	 face |bytes    packets errs drop fifo frame compressed multicast|bytes    packets errs drop fifo colls carrier compressed
	 lo:    1956      30    0    0    0     0          0         0     1956      30    0    0    0     0       0          0
	 eth0:       0       0    0    0    0     0          0         0   458718       0  781    0    0     0     781          0
	 sit0:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
	 eth1:       0       0    0    0    0     0          0         0        0       0    0    0    0     0       0          0
	 */
	for(i=0;i<NETMAX;i++) {
		if(fgets(buf,1024,fp) == NULL)
			break;
		strip_spaces(buf);
		/* 1   2   3    4   5   6   7   8   9   10   11   12  13  14  15  16 */
		ret = sscanf(&buf[0], "%s %llu %llu %lu %lu %lu %lu %lu %lu %llu %llu %lu %lu %lu %lu %lu",
					 (char *)&p->ifnets[i].if_name,
					 &p->ifnets[i].if_ibytes,
					 &p->ifnets[i].if_ipackets,
					 &p->ifnets[i].if_ierrs,
					 &p->ifnets[i].if_idrop,
					 &p->ifnets[i].if_ififo,
					 &p->ifnets[i].if_iframe,
					 &junk,
					 &junk,
					 &p->ifnets[i].if_obytes,
					 &p->ifnets[i].if_opackets,
					 &p->ifnets[i].if_oerrs,
					 &p->ifnets[i].if_odrop,
					 &p->ifnets[i].if_ofifo,
					 &p->ifnets[i].if_ocolls,
					 &p->ifnets[i].if_ocarrier
					 );
		if(ret != 16)
			fprintf(stderr,"sscanf wanted 16 returned = %d line=%s\n", ret, (char *)buf);
	}
end:
	rewind(fp);
	networks = i;
}


int proc_procsinfo(int pid, int index)
{
	FILE *fp;
	char filename[64];
	char buf[1024*4];
	int size=0;
	int ret=0;
	int count=0;
	int i;
	
	sprintf(filename,"/Users/stollcri/Documents/code/c/nmond/dbg/%d/stat",pid);
	if( (fp = fopen(filename,"r")) == NULL) {
		sprintf(buf,"failed to open file %s",filename);
		//error(buf);
		return 0;
	}
	size = fread(buf, 1, 1024-1, fp);
	fclose(fp);
	if(size == -1) {
#ifdef DEBUG
		fprintf(stderr,"procsinfo read returned = %d assuming process stopped pid=%d\n", ret,pid);
#endif /*DEBUG*/
		return 0;
	}
	ret = sscanf(buf, "%d (%s)",
				 &p->procs[index].pi_pid,
				 &p->procs[index].pi_comm[0]);
	if(ret != 2) {
		fprintf(stderr,"procsinfo sscanf returned = %d line=%s\n", ret,buf);
		return 0;
	}
	p->procs[index].pi_comm[strlen(p->procs[index].pi_comm)-1] = 0;
	
	for(count=0; count<size;count++)	/* now look for ") " as dumb Infiniban driver includes "()" */
		if(buf[count] == ')' && buf[count+1] == ' ' ) break;
	
	if(count == size) {
#ifdef DEBUG
		fprintf(stderr,"procsinfo failed to find end of command buf=%s\n", buf);
#endif /*DEBUG*/
		return 0;
	}
	count++; count++;
	
	ret = sscanf(&buf[count],
				 "%c %d %d %d %d %d %lu %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %lu %lu %ld %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %d %d %lu %lu %llu",
				 &p->procs[index].pi_state,
				 &p->procs[index].pi_ppid,
				 &p->procs[index].pi_pgrp,
				 &p->procs[index].pi_session,
				 &p->procs[index].pi_tty_nr,
				 &p->procs[index].pi_tty_pgrp,
				 &p->procs[index].pi_flags,
				 &p->procs[index].pi_minflt,
				 &p->procs[index].pi_cmin_flt,
				 &p->procs[index].pi_majflt,
				 &p->procs[index].pi_cmaj_flt,
				 &p->procs[index].pi_utime,
				 &p->procs[index].pi_stime,
				 &p->procs[index].pi_cutime,
				 &p->procs[index].pi_cstime,
				 &p->procs[index].pi_pri,
				 &p->procs[index].pi_nice,
				 &p->procs[index].pi_num_threads,
				 &p->procs[index].pi_it_real_value,
				 &p->procs[index].pi_start_time,
				 &p->procs[index].pi_vsize,
				 &p->procs[index].pi_rss,
				 &p->procs[index].pi_rlim_cur,
				 &p->procs[index].pi_start_code,
				 &p->procs[index].pi_end_code,
				 &p->procs[index].pi_start_stack,
				 &p->procs[index].pi_esp,
				 &p->procs[index].pi_eip,
				 &p->procs[index].pi_pending_signal,
				 &p->procs[index].pi_blocked_sig,
				 &p->procs[index].pi_sigign,
				 &p->procs[index].pi_sigcatch,
				 &p->procs[index].pi_wchan,
				 &p->procs[index].pi_nswap,
				 &p->procs[index].pi_cnswap,
				 &p->procs[index].pi_exit_signal,
				 &p->procs[index].pi_cpu,
				 &p->procs[index].pi_rt_priority,
				 &p->procs[index].pi_policy,
				 &p->procs[index].pi_delayacct_blkio_ticks
				 
				 );

		if(ret != 40) {
			fprintf(stderr,"procsinfo2 sscanf wanted 40 returned = %d pid=%d line=%s\n", ret,pid,buf);
			return 0;
		}
		
		sprintf(filename,"/Users/stollcri/Documents/code/c/nmond/dbg/%d/statm",pid);
		if( (fp = fopen(filename,"r")) == NULL) {
			sprintf(buf,"failed to open file %s",filename);
			//error(buf);
			return 0;
		}
		size = fread(buf, 1, 1024*4-1, fp);
		fclose(fp); /* close it even if the read failed, the file could have been removed
					 between open & read i.e. the device driver does not behave like a file */
		if(size == -1) {
			sprintf(buf,"failed to read file %s",filename);
			//error(buf);
			return 0;
		}
		
		ret = sscanf(&buf[0], "%lu %lu %lu %lu %lu %lu %lu",
					 &p->procs[index].statm_size,
					 &p->procs[index].statm_resident,
					 &p->procs[index].statm_share,
					 &p->procs[index].statm_trs,
					 &p->procs[index].statm_lrs,
					 &p->procs[index].statm_drs,
					 &p->procs[index].statm_dt
					 );
		if(ret != 7) {
			fprintf(stderr,"sscanf wanted 7 returned = %d line=%s\n", ret,buf);
			return 0;
		}
		if(isroot) {
			p->procs[index].read_io = 0;
			p->procs[index].write_io = 0;
			sprintf(filename,"/Users/stollcri/Documents/code/c/nmond/dbg/%d/io",pid);
			if( (fp = fopen(filename,"r")) != NULL) {
				for(i=0;i<6;i++) {
					if(fgets(buf,1024,fp) == NULL) {
						break;
					}
					if(strncmp("read_bytes:",  buf, 11) == 0 )
						sscanf(&buf[12], "%lld", &p->procs[index].read_io);
					if(strncmp("write_bytes:", buf, 12) == 0 )
						sscanf(&buf[13], "%lld", &p->procs[index].write_io);
				}
			}
			
			if (fp != NULL)
				fclose(fp);
		}
		return 1;
	}
	
	int isnumbers(char *s)
	{
		while(*s != 0) {
			if( *s < '0' || *s > '9')
				return 0;
			s++;
		}
		return 1;
	}
	
	int getprocs(int details)
	{
		struct dirent *dent;
		DIR *procdir;
		int count =0;
		
		if((char *)(procdir = opendir("/Users/stollcri/Documents/code/c/nmond/dbg")) == NULL) {
			printf("opendir(/Users/stollcri/Documents/code/c/nmond/dbg) failed");
			return 0;
		}
		while( (char *)(dent = readdir(procdir)) != NULL ) {
			if(dent->d_type == 4) { /* is this a directlory */
				/* mainframes report 0 = unknown every time !!!!  */
				/*
				 printf("inode=%d type=%d name=%s\n",
				 dent->d_ino,
				 dent->d_type,
				 dent->d_name);
				 */
				if(isnumbers(dent->d_name)) {
					/*			printf("%s pid\n",dent->d_name); */
					if(details) {
						count=count+proc_procsinfo(atoi(dent->d_name),count);
					} else {
						count++;
					}
				}
				/*
				 else
				 printf("NOT numbers\n");
				 */
			}
		}
		closedir(procdir);
		return count;
	}

	
	int main(int argc, char **argv)
	{
		//
		// DEBUG -- stollcri
		// 

		setinterupthandlers();

		struct nmondsettings settings = NMONDSETTINGS_INIT;
		struct uiwinsets winsets = UIWINSETS_INIT;
		winsets.welcome.visible = true;
		getenvars(&winsets, &settings);

		struct syshw thishw = getsyshwinfo();
		struct syskern thiskern = getsyskerninfo();
		struct sysres thisres = SYSRES_INIT;
		getsysresinfo(&thisres);
		int cpulongitter = 0;
		size_t processcount = 0;
		struct sysproc thisproc = getsysprocinfoall(processcount);

		// getsysresinfo(&thisres);
		// exit(15);



		int secs;
		double cpu_scaled_user;
		double cpu_scaled_sys;
		double cpu_scaled_wait;
		double cpu_scaled_idle;
		double cpu_scaled_steal;
		int	n=0;			/* reusable counters */
		int	i=0;
		int	j=0;
		int	k=0;
		int	ret=0;
		int	max_sorted;
		int	skipped;
		int	x = 0;			/* curses row */
		int	y = 0;			/* curses column */
		double	elapsed;		/* actual seconds between screen updates */
		double	cpu_busy;
		double	ftmp;
		int	top_first_time =1;
		int	disk_first_time =1;
		int	nfs_first_time =1;
		int	vm_first_time =1;
		int bbbr_line=0;
		int	smp_first_time =1;
		int	proc_first_time =1;
		int	first_key_pressed = 0;
		pid_t childpid = -1;
		int ralfmode = 0;
		char	pgrp[32];
		struct tm *tim; /* used to work out the hour/min/second */
		float	total_busy;	/* general totals */
		float	total_rbytes;	/* general totals */
		float	total_wbytes;
		float	total_xfers;
		double top_disk_busy = 0.0;
		char *top_disk_name = "";
		int disk_mb;
		double disk_total;
		double disk_busy;
		double disk_read;
		double disk_size;
		double disk_write;
		double disk_xfers;
		double total_disk_read;
		double total_disk_write;
		double total_disk_xfers;
		double readers;
		double writers;
		
		/* for popen on oslevel */
		char str[512];
		char * str_p;
		int varperftmp = 0;
		char *formatstring;
		char user_filename[512];
		char user_filename_set = 0;
		struct statfs statfs_buffer;
		float fs_size;
		float fs_bsize;
		float fs_free;
		float fs_size_used;
		char cmdstr[256];
		int updays, uphours, upmins;
		float v2c_total;
		float v2s_total;
		float v3c_total;
		float v3s_total;
		float v4c_total;
		float v4s_total;
		int errors=0;
		
		unsigned long  pagesize = 1024*4; /* Default page size is 4 KB but newer servers compiled with 64 KB pages */
		float average;
		struct timeval nmon_tv; /* below is used to workout the nmon run, accumalate it and the
								 allow for in in the sleep time  to reduce time drift */
		double nmon_start_time = 0.0;
		double nmon_end_time = 0.0;
		double nmon_run_time = -1.0;
		int seconds_over = 0;
		
#define MAXROWS 256
#define MAXCOLS 150 /* changed to allow maximum column widths */
#define BANNER(pad,string) {mvwhline(pad, 0, 0, ACS_HLINE,COLS-2); \
wmove(pad,0,0); \
wattron(pad,A_STANDOUT); \
wprintw(pad," "); \
wprintw(pad,string); \
wprintw(pad," "); \
wattroff(pad,A_STANDOUT); }
		
#define DISPLAY(pad,rows) { \
if(x+2+(rows)>LINES)\
pnoutrefresh(pad, 0,0,x,1,LINES-2, COLS-2); \
else \
pnoutrefresh(pad, 0,0,x,1,x+rows+1,COLS-2); \
x=x+(rows);     \
if(x+4>LINES) { \
mvwprintw(stdscr,LINES-1, 10, MSG_WRN_NOT_SHOWN); \
}               \
}
		
		/* check the user supplied options */
		progname = argv[0];
		for (i=(int)strlen(progname)-1;i>0;i--)
			if(progname[i] == '/') {
				progname = &progname[i+1];
			}
		
		
		for(i=0; i<CMDMAX;i++) {
			sprintf(cmdstr,"NMONCMD%d",i);
			cmdlist[i] = getenv(cmdstr);
			if(cmdlist[i] != 0)
				cmdfound = i+1;
		}
		/* Setup long and short Hostname */
		gethostname(hostname, sizeof(hostname));
		strcpy(fullhostname, hostname);
		for (i = 0; i < sizeof(hostname); i++)
			if (hostname[i] == '.')
				hostname[i] = 0;
		if(run_name_set == 0)
			strcpy(run_name,hostname);
		
		if( getuid() == 0)
			isroot=1;
		
		/* Check the version of OS */
		if(sysconf(_SC_PAGESIZE) > 1024*4) /* Check if we have the large 64 KB memory page sizes compiled in to the kernel */
			pagesize = sysconf(_SC_PAGESIZE);
		proc_init();
		
		while ( -1 != (i = getopt(argc, argv, "?Rhs:bc:Dd:fF:r:tTxXzeEl:qpC:Vg:Nm:I:Z" ))) {
			switch (i) {
				case '?':
					uiclhint();
					exit(0);
				case 'h':
					uiclhelp(VERSION);
					exit(0);
				case 's':
					seconds = atoi(optarg);
					break;
				case 'p':
					ralfmode = 1;
					break;
				case 'b':
					colour = 0;
					break;
				case 'c':
					// maxloops = atoi(optarg);
					break;
				case 'N':
					show_nfs = 1;
					break;
				case 'm':
					if(chdir(optarg) == -1) {
						perror(MSG_ERR_CD_FAILED);
						printf(MSG_ERR_FAILEDDIR, optarg);
						exit(993);
					}
					break;
				case 'I':
					ignore_procdisk_threshold = atof(optarg);
					break;
				case 'd':
					diskmax = atoi(optarg);
					if(diskmax < DISKMIN) {
						printf(MSG_WRN_OVER_MIND, diskmax, DISKMIN);
						diskmax = DISKMIN;
					}
					break;
				// case 'R':
				// 	go_background(288, 300);
				// 	show_aaa = 0;
				// 	show_para = 0;
				// 	show_headings = 0;
				// 	break;
				case 'r': strcpy(run_name,optarg);
					run_name_set++;
					break;
				// case 'F': /* background mode with user supplied filename */
					// strcpy(user_filename,optarg);
					// user_filename_set++;
					// go_background(288, 300);
					// break;
				// case 'f': /* background mode i.e. for spread sheet output */
					// go_background(288, 300);
					// break;
				case 'T':
					show_args = ARGS_ONLY; /* drop through */
				case 't':
					show_top     = 1; /* put top process output in spreadsheet mode */
					show_topmode = 3;
					break;
				// case 'z': /* background mode for 1 day output to /var/perf/tmp */
					// varperftmp++;
					// go_background(4*24, 15*60);
					// break;
				// case 'x': /* background mode for 1 day capacity planning */
					// go_background(4*24, 15*60);
					// show_top =1;
					// show_topmode = 3;
					// break;
				// case 'X': /* background mode for 1 hour capacity planning */
					// go_background(120, 30);
					// show_top =1;
					// show_topmode = 3;
					// break;
				case 'Z':
					show_raw=1;
					break;
				case 'l':
					disks_per_line = atoi(optarg);
					if(disks_per_line < 3 || disks_per_line >250) disks_per_line = 100;
					break;
				case 'C': /* commandlist argument */
					cmdlist[0] = MALLOC(strlen(optarg)+1); /* create buffer */
					strcpy(cmdlist[0],optarg);
					if(cmdlist[0][0]!= 0)
						cmdfound=1;
					for(i=0,j=1;cmdlist[0][i] != 0;i++) {
						if(cmdlist[0][i] == ':') {
							cmdlist[0][i] = 0;
							cmdlist[j] = &cmdlist[0][i+1];
							j++;
							cmdfound=j;
							if(j >= CMDMAX) break;
						}
					}
					break;
				case 'V': /* nmon version */
					printf(MSG_VERSION, VERSION);
					exit(0);
					break;
				case 'g': /* disk groups */
					show_dgroup = 1;
					dgroup_loaded = 1;
					dgroup_filename = optarg;
					if( strncmp("auto",dgroup_filename,5) == 0) {
						auto_dgroup++;
						printf(MSG_INF_GEN_DISKG);
						ret = system("lsblk --nodeps --output NAME,TYPE --raw | grep disk | awk 'BEGIN {printf \"# This file created by: nmon -g auto\\n# It is an automatically generated disk-group file which excluses disk paritions\\n\" } { printf \"%s %s\\n\", $1, $1 }' >auto");
						if(ret != 0 ) {
							printf(MSG_INF_AUTOF_CMD,
								   "lsblk --nodeps --output NAME,TYPE --raw | grep disk | awk '{ printf \"%s %s\\n\", $1, $1 }' >auto");
							printf(MSG_INF_AUTOFSTAT, ret );
						}
					}
					break;
			}
		}
		/* Set parameters if not set by above */
		// if (maxloops == -1)
		// 	maxloops = 9999999;
		if (seconds  == -1)
			seconds = 2;
			show_dgroup = 0;
		
		/* To get the pointers setup */
		switcher();
		
		/* Initialise the time stamps for the first loop */
		p->time = doubletime();
		q->time = doubletime();
				

		get_intel_spec();

		proc_read(P_STAT);
		proc_read(P_UPTIME);
		proc_read(P_LOADAVG);
		proc_kernel();
		memcpy(&q->cpu_total, &p->cpu_total, sizeof(struct cpu_stat));
		
		p->dk = MALLOC(sizeof(struct dsk_stat) * diskmax+1);
		q->dk = MALLOC(sizeof(struct dsk_stat) * diskmax+1);
		disk_busy_peak = MALLOC(sizeof(double) * diskmax);
		disk_rate_peak = MALLOC(sizeof(double) * diskmax);
		for(i=0;i<diskmax;i++) {
			disk_busy_peak[i]=0.0;
			disk_rate_peak[i]=0.0;
		}
		
		cpu_peak = MALLOC(sizeof(double) * (CPUMAX + 1)); /* MAGIC */
		for(i=0;i < cpus+1;i++)
			cpu_peak[i]=0.0;
		
		n = getprocs(0);
		p->procs = MALLOC(sizeof(struct procsinfo ) * n  +8);
		q->procs = MALLOC(sizeof(struct procsinfo ) * n  +8);
		p->nprocs = q->nprocs = n;
		
		/* Initialise the top processes table */
		topper = MALLOC(sizeof(struct topper ) * topper_size); /* round up */
		
		/* Get Disk Stats. */
		proc_disk(0.0);
		memcpy(q->dk, p->dk, sizeof(struct dsk_stat) * disks);
		
		/* load dgroup - if required */
		if (dgroup_loaded == 1) {
			load_dgroup(p->dk);
		}
		
		/* Get Network Stats. */
		proc_net();
		memcpy(q->ifnets, p->ifnets, sizeof(struct net_stat) * networks);
		for(i=0;i<networks;i++) {
			net_read_peak[i]=0.0;
			net_write_peak[i]=0.0;
		}
		

		/* Set the pointer ready for the next round */
		switcher();
				
		initscr();
		cbreak();
		move(0, 0);
		refresh();
		COLOUR colour = has_colors();
		COLOUR start_color();
		COLOUR init_pairs();
		clear();
		padwelcome = newpad(24,MAXCOLS);
		padmap = newpad(24,MAXCOLS);
		padhelp = newpad(24,MAXCOLS);
		padmem = newpad(20,MAXCOLS);
		padlarge = newpad(20,MAXCOLS);
		padpage = newpad(20,MAXCOLS);
		padsys = newpad(20,MAXCOLS);
		padsmp = newpad(MAXROWS,MAXCOLS);
		padlong = newpad(MAXROWS,MAXCOLS);
		padnet = newpad(MAXROWS,MAXCOLS);
		padneterr = newpad(MAXROWS,MAXCOLS);
		paddisk = newpad(MAXROWS,MAXCOLS);
		paddg = newpad(MAXROWS,MAXCOLS);
		padjfs = newpad(MAXROWS,MAXCOLS);
		padkstat = newpad(5,MAXCOLS);
		padverb = newpad(8,MAXCOLS);
		padnfs = newpad(25,MAXCOLS);
		padtop = newpad(MAXROWS,MAXCOLS*2);

		/* To get the pointers setup */
		/* Was already done earlier, DONT'T switch back here to the old pointer! - switcher(); */
		/*checkinput();*/
		clear();
		fflush(NULL);
		
		/* Main loop of the code */
		for(loop=1; ; loop++) {
			// stollcri, 2015
			thishw = getsyshwinfo();
			thiskern = getsyskerninfo();
			getsysresinfo(&thisres);
			processcount = 0;
			thisproc = getsysprocinfoall(processcount);

			/* Save the time and work out how long we were actually asleep
			 * Do this as early as possible and close to reading the CPU statistics in /proc/stat
			 */
			p->time = doubletime();
			elapsed = p->time - q->time;
			timer = time(0);
			tim = localtime(&timer);
			
			
			// if(loop <= 3)  This stops the nmon causing the cpu peak at startup 
			// 	for(i=0;i < cpus+1;i++)
			// 		cpu_peak[i]=0.0;
			
			/* Reset the cursor position to top left */
			y = x = 0;
			
			uiheader(&x, colour, flash_on, hostname, elapsed, timer);
			
			if(winsets.welcome.visible) {
				uiwelcome(&padwelcome, &x, COLS, LINES, colour, thishw);
			}

			if (winsets.help.visible) {
				uihelp(&padhelp, &x, COLS, LINES);
			}

			if (show_verbose) {
				uiverbose(&padverb, &x, COLS, LINES);
			}

			if (show_cpu) {
				uisys(&padsys, &x, COLS, LINES, thiskern);
			}
			
			if (winsets.cpulong.visible) {
				uicpulong(&padlong, &x, COLS, LINES, &cpulongitter, colour, thisres);
			}
			
			if (winsets.cpu.visible) {
					uicpu(&padsmp, &x, COLS, LINES, colour, thisres, show_raw);
			}

			if (show_memory) {
				proc_read(P_MEMINFO);
				proc_mem();
				BANNER(padmem,"Memory Stats");
				mvwprintw(padmem,1, 1, "               RAM     High      Low     Swap    Page Size=%d KB",pagesize/1024);
				mvwprintw(padmem,2, 1, "Total MB    %8.1f %8.1f %8.1f %8.1f ",
						  p->mem.memtotal/1024.0,
						  p->mem.hightotal/1024.0,
						  p->mem.lowtotal/1024.0,
						  p->mem.swaptotal/1024.0);
				mvwprintw(padmem,3, 1, "Free  MB    %8.1f %8.1f %8.1f %8.1f ",
						  p->mem.memfree/1024.0,
						  p->mem.highfree/1024.0,
						  p->mem.lowfree/1024.0,
						  p->mem.swapfree/1024.0);
				mvwprintw(padmem,4, 1, "Free Percent %7.1f%% %7.1f%% %7.1f%% %7.1f%% ",
						  p->mem.memfree  == 0 ? 0.0 : 100.0*(float)p->mem.memfree/(float)p->mem.memtotal,
						  p->mem.highfree == 0 ? 0.0 : 100.0*(float)p->mem.highfree/(float)p->mem.hightotal,
						  p->mem.lowfree  == 0 ? 0.0 : 100.0*(float)p->mem.lowfree/(float)p->mem.lowtotal,
						  p->mem.swapfree == 0 ? 0.0 : 100.0*(float)p->mem.swapfree/(float)p->mem.swaptotal);
				
				
				mvwprintw(padmem,5, 1, "            MB                  MB                  MB");
#ifdef LARGEMEM
				mvwprintw(padmem,6, 1, "                     Cached=%8.1f     Active=%8.1f",
						  p->mem.cached/1024.0,
						  p->mem.active/1024.0);
#else
				mvwprintw(padmem,6, 1, " Shared=%8.1f     Cached=%8.1f     Active=%8.1f",
						  p->mem.memshared/1024.0,
						  p->mem.cached/1024.0,
						  p->mem.active/1024.0);
				mvwprintw(padmem,5, 68, "MB");
				mvwprintw(padmem,6, 55, "bigfree=%8.1f",
						  p->mem.bigfree/1024);
#endif /*LARGEMEM*/
				mvwprintw(padmem,7, 1, "Buffers=%8.1f Swapcached=%8.1f  Inactive =%8.1f",
						  p->mem.buffers/1024.0,
						  p->mem.swapcached/1024.0,
						  p->mem.inactive/1024.0);
				
				mvwprintw(padmem,8, 1, "Dirty  =%8.1f Writeback =%8.1f  Mapped   =%8.1f",
						  p->mem.dirty/1024.0,
						  p->mem.writeback/1024.0,
						  p->mem.mapped/1024.0);
				mvwprintw(padmem,9, 1, "Slab   =%8.1f Commit_AS =%8.1f PageTables=%8.1f",
						  p->mem.slab/1024.0,
						  p->mem.committed_as/1024.0,
						  p->mem.pagetables/1024.0);

				DISPLAY(padmem,10);
			}
			if (show_large) {
				proc_read(P_MEMINFO);
				proc_mem();
				BANNER(padlarge,"Large (Huge) Page Stats");
				if(p->mem.hugetotal > 0) {
					if(p->mem.hugetotal - p->mem.hugefree > huge_peak)
						huge_peak = p->mem.hugetotal - p->mem.hugefree;
					mvwprintw(padlarge,1, 1, "Total Pages=%7ld   100.0%%   Huge Page Size =%ld KB",    p->mem.hugetotal, p->mem.hugesize);
					mvwprintw(padlarge,2, 1, "Used  Pages=%7ld   %5.1f%%   Used Pages Peak=%-8ld",
							  (long)(p->mem.hugetotal - p->mem.hugefree),
							  (p->mem.hugetotal - p->mem.hugefree)/(float)p->mem.hugetotal*100.0,
							  huge_peak);
					mvwprintw(padlarge,3, 1, "Free  Pages=%7ld   %5.1f%%",    p->mem.hugefree, p->mem.hugefree/(float)p->mem.hugetotal*100.0);
				} else {
					mvwprintw(padlarge,1, 1, " There are no Huge Pages");
					mvwprintw(padlarge,2, 1, " - see /proc/meminfo");
				}
				DISPLAY(padlarge,4);
			}
			if (show_vm) {
#define VMDELTA(variable) (p->vm.variable - q->vm.variable)
#define VMCOUNT(variable) (p->vm.variable                 )
				ret = read_vmstat();
				BANNER(padpage,"Virtual-Memory");
				if(ret < 0 ) {
					mvwprintw(padpage,2, 2, "Virtual Memory stats not supported with this kernel");
					mvwprintw(padpage,3, 2, "/proc/vmstat only seems to appear in 2.6 onwards");
					
				} else {
					if(vm_first_time) {
						mvwprintw(padpage,2, 2, "Please wait - collecting data");
						vm_first_time=0;
					} else {
						mvwprintw(padpage,1, 0, "nr_dirty    =%9lld pgpgin      =%8lld",
								  VMCOUNT(nr_dirty),
								  VMDELTA(pgpgin));
						mvwprintw(padpage,2, 0, "nr_writeback=%9lld pgpgout     =%8lld",
								  VMCOUNT(nr_writeback),
								  VMDELTA(pgpgout));
						mvwprintw(padpage,3, 0, "nr_unstable =%9lld pgpswpin    =%8lld",
								  VMCOUNT(nr_unstable),
								  VMDELTA(pswpin));
						mvwprintw(padpage,4, 0, "nr_table_pgs=%9lld pgpswpout   =%8lld",
								  VMCOUNT(nr_page_table_pages),
								  VMDELTA(pswpout));
						mvwprintw(padpage,5, 0, "nr_mapped   =%9lld pgfree      =%8lld",
								  VMCOUNT(nr_mapped),
								  VMDELTA(pgfree));
						mvwprintw(padpage,6, 0, "nr_slab     =%9lld pgactivate  =%8lld",
								  VMCOUNT(nr_slab),
								  VMDELTA(pgactivate));
						mvwprintw(padpage,7, 0, "                       pgdeactivate=%8lld",
								  VMDELTA(pgdeactivate));
						mvwprintw(padpage,8, 0, "allocstall  =%9lld pgfault     =%8lld  kswapd_steal     =%7lld",
								  VMDELTA(allocstall),
								  VMDELTA(pgfault),
								  VMDELTA(kswapd_steal));
						mvwprintw(padpage,9, 0, "pageoutrun  =%9lld pgmajfault  =%8lld  kswapd_inodesteal=%7lld",
								  VMDELTA(pageoutrun),
								  VMDELTA(pgmajfault),
								  VMDELTA(kswapd_inodesteal));
						mvwprintw(padpage,10, 0,"slabs_scanned=%8lld pgrotated   =%8lld  pginodesteal     =%7lld",
								  VMDELTA(slabs_scanned),
								  VMDELTA(pgrotated),
								  VMDELTA(pginodesteal));
						
						
						
						mvwprintw(padpage,1, 46, "              High Normal    DMA");
						mvwprintw(padpage,2, 46, "alloc      %7lld%7lld%7lld",
								  VMDELTA(pgalloc_high),
								  VMDELTA(pgalloc_normal),
								  VMDELTA(pgalloc_dma));
						mvwprintw(padpage,3, 46, "refill     %7lld%7lld%7lld",
								  VMDELTA(pgrefill_high),
								  VMDELTA(pgrefill_normal),
								  VMDELTA(pgrefill_dma));
						mvwprintw(padpage,4, 46, "steal      %7lld%7lld%7lld",
								  VMDELTA(pgsteal_high),
								  VMDELTA(pgsteal_normal),
								  VMDELTA(pgsteal_dma));
						mvwprintw(padpage,5, 46, "scan_kswapd%7lld%7lld%7lld",
								  VMDELTA(pgscan_kswapd_high),
								  VMDELTA(pgscan_kswapd_normal),
								  VMDELTA(pgscan_kswapd_dma));
						mvwprintw(padpage,6, 46, "scan_direct%7lld%7lld%7lld",
								  VMDELTA(pgscan_direct_high),
								  VMDELTA(pgscan_direct_normal),
								  VMDELTA(pgscan_direct_dma));
					}
				}
				DISPLAY(padpage,11);
			}
			if (show_kernel) {
				proc_read(P_UPTIME);
				proc_read(P_LOADAVG);
				proc_kernel();
				BANNER(padkstat,"Kernel Stats");
				mvwprintw(padkstat,1, 1, "RunQueue       %8lld   Load Average    CPU use since boot time",
						  p->cpu_total.running);
				updays=p->cpu_total.uptime/60/60/24;
				uphours=(p->cpu_total.uptime-updays*60*60*24)/60/60;
				upmins=(p->cpu_total.uptime-updays*60*60*24-uphours*60*60)/60;
				mvwprintw(padkstat,2, 1, "ContextSwitch  %8.1f    1 mins %5.2f    Uptime Days=%3d Hours=%2d Mins=%2d",
						  (float)(p->cpu_total.ctxt - q->cpu_total.ctxt)/elapsed,
						  (float)p->cpu_total.mins1,
						  updays, uphours, upmins);
				updays=p->cpu_total.idletime/60/60/24;
				uphours=(p->cpu_total.idletime-updays*60*60*24)/60/60;
				upmins=(p->cpu_total.idletime-updays*60*60*24-uphours*60*60)/60;
				mvwprintw(padkstat,3, 1, "Forks          %8.1f    5 mins %5.2f    Idle   Days=%3d Hours=%2d Mins=%2d",
						  (float)(p->cpu_total.procs - q->cpu_total.procs)/elapsed,
						  (float)p->cpu_total.mins5,
						  updays, uphours, upmins);
				
				mvwprintw(padkstat,4, 1, "Interrupts     %8.1f   15 mins %5.2f",
						  (float)(p->cpu_total.intr - q->cpu_total.intr)/elapsed,
						  (float)p->cpu_total.mins15);
				average = (p->cpu_total.uptime - p->cpu_total.idletime)/ p->cpu_total.uptime *100.0;
				if( average > 0.0)
					mvwprintw(padkstat,4, 46, "Average CPU use=%6.2f%%", average);
				else
					mvwprintw(padkstat,4, 46, "Uptime has overflowed");
				DISPLAY(padkstat,5);
			}
			
			if (show_nfs) {
				proc_read(P_NFS);
				proc_read(P_NFSD);
				proc_nfs();
				
				if(nfs_first_time) {
					memcpy(&q->nfs,&p->nfs,sizeof(struct nfs_stat));
					nfs_first_time=0;
				}
				if(nfs_clear) {
					nfs_clear=0;
					for(i=0;i<25;i++)
						mvwprintw(padnfs,i, 0, "                                                                                ");
				}
				BANNER(padnfs,"Network Filesystem (NFS) I/O Operations per second");
				if(show_nfs == 1) {
					if(nfs_v2c_found || nfs_v2s_found) 
						mvwprintw(padnfs,1, 0, " Version 2        Client   Server");
					else
						mvwprintw(padnfs,1, 0, " Version 2 not active");
					
					if(nfs_v3c_found || nfs_v3s_found)
						mvwprintw(padnfs,1, 41, "Version 3      Client   Server");
					else
						mvwprintw(padnfs,1, 41, " Version 3 not active");
				} 
				if(show_nfs == 2 ) {
					if(nfs_v4c_found) 
						mvwprintw(padnfs,1, 0, " Version 4 Client (%d Stats found)", nfs_v4c_names_count);
					else
						mvwprintw(padnfs,1, 0, " Version 4 Client side not active");
				} 
				if(show_nfs == 3 ) {
					if(nfs_v4s_found)
						mvwprintw(padnfs,1, 0, " Version 4 Server (%d Stats found)", nfs_v4s_names_count);
					else
						mvwprintw(padnfs,1, 0, " Version 4 Server side not active");
				}
#define NFS_TOTAL(member) (double)(p->member)
#define NFS_DELTA(member) (((double)(p->member - q->member)/elapsed))
				v2c_total =0;
				v2s_total =0;
				v3c_total =0;
				v3s_total =0;
				v4c_total =0;
				v4s_total =0;
				if(nfs_v3c_found || nfs_v3s_found) {
					for(i=0;i<18;i++) {	/* NFS V2 Client & Server */
						if(show_nfs == 1) 
							mvwprintw(padnfs,2+i,  3, "%12s %8.1f %8.1f",
									  nfs_v2_names[i],
									  NFS_DELTA(nfs.v2c[i]),
									  NFS_DELTA(nfs.v2s[i]));
						v2c_total +=NFS_DELTA(nfs.v2c[i]);
						v2s_total +=NFS_DELTA(nfs.v2s[i]);
					}
				}
				if(nfs_v3c_found || nfs_v3s_found) {
					for(i=0;i<22;i++) {	/* NFS V3 Client & Server */
						if(show_nfs == 1)
							mvwprintw(padnfs,2+i, 41, "%12s %8.1f %8.1f",
									  nfs_v3_names[i],
									  NFS_DELTA(nfs.v3c[i]),
									  NFS_DELTA(nfs.v3s[i]));
						v3c_total +=NFS_DELTA(nfs.v3c[i]);
						v3s_total +=NFS_DELTA(nfs.v3s[i]);
					}
				}
				
				if(nfs_v4c_found) {
					for(i=0;i<18;i++) {	/* NFS V4 Client */
						if(show_nfs == 2) {
							mvwprintw(padnfs,2+i, 0, "%12s%7.1f",
									  nfs_v4c_names[i],
									  NFS_DELTA(nfs.v4c[i]));
						}
						v4c_total +=NFS_DELTA(nfs.v4c[i]);
					}
					for(i=18;i<35;i++) {	/* NFS V4 Client */
						if(show_nfs == 2) {
							mvwprintw(padnfs,2+i-18, 20, "%12s%7.1f",
									  nfs_v4c_names[i],
									  NFS_DELTA(nfs.v4c[i]));
						}
						v4c_total +=NFS_DELTA(nfs.v4c[i]);
					}
				}
				
				if(nfs_v4s_found) {
					for(i=0;i<18;i++) {	/* NFS V4 Server */
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i, 0, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
					for(i=18;i<36;i++) {	/* NFS V4 Server */
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i-18, 19, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
					for(i=36;i<54 && i<nfs_v4s_names_count;i++) {	/* NFS V4 Server */
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i-36, 39, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
					for(i=54;i<=70 && i<nfs_v4s_names_count;i++) {	/* NFS V4 Server */
						if(show_nfs == 3) {
							mvwprintw(padnfs,2+i-54, 59, "%12s%7.1f",
									  nfs_v4s_names[i],
									  NFS_DELTA(nfs.v4s[i]));
						}
						v4s_total +=NFS_DELTA(nfs.v4s[i]);
					}
				}
				mvwprintw(padnfs,2+18,  1, "--NFS-Totals->---Client----Server--");
				/* if(nfs_v2c_found || nfs_v2s_found) */
				mvwprintw(padnfs,2+19,  1, "NFSv2 Totals->%9.1f %9.1f", v2c_total,v2s_total);
				/* if(nfs_v3c_found || nfs_v3s_found)*/
				mvwprintw(padnfs,2+20,  1, "NFSv3 Totals->%9.1f %9.1f", v3c_total,v3s_total);
				/* if(nfs_v4c_found || nfs_v4s_found)*/
				mvwprintw(padnfs,2+21,  1, "NFSv4 Totals->%9.1f %9.1f", v4c_total,v4s_total);
				
				DISPLAY(padnfs,24);
			}
			if (show_net) {
				BANNER(padnet,"Network I/O");
				mvwprintw(padnet,1, 0, "I/F Name Recv=KB/s Trans=KB/s packin packout insize outsize Peak->Recv Trans");
				proc_net();
				for (i = 0; i < networks; i++) {
					
#define IFDELTA(member) ((float)( (q->ifnets[i].member > p->ifnets[i].member) ? 0 : (p->ifnets[i].member - q->ifnets[i].member)/elapsed) )
#define IFDELTA_ZERO(member1,member2) ((IFDELTA(member1) == 0) || (IFDELTA(member2)== 0)? 0.0 : IFDELTA(member1)/IFDELTA(member2) )
					
					if(net_read_peak[i] < IFDELTA(if_ibytes) / 1024.0)
						net_read_peak[i] = IFDELTA(if_ibytes) / 1024.0;
					if(net_write_peak[i] < IFDELTA(if_obytes) / 1024.0)
						net_write_peak[i] = IFDELTA(if_obytes) / 1024.0;
					
					mvwprintw(padnet,2 + i, 0, "%8s %7.1f %7.1f    %6.1f   %6.1f  %6.1f %6.1f    %7.1f %7.1f   ",
									&p->ifnets[i].if_name[0],
									IFDELTA(if_ibytes) / 1024.0,   
									IFDELTA(if_obytes) / 1024.0, 
									IFDELTA(if_ipackets), 
									IFDELTA(if_opackets),
									IFDELTA_ZERO(if_ibytes, if_ipackets),
									IFDELTA_ZERO(if_obytes, if_opackets),
									net_read_peak[i],
									net_write_peak[i]
									);
				}
				DISPLAY(padnet,networks + 2);
			}
			errors=0;
			for (i = 0; i < networks; i++) {
				errors += p->ifnets[i].if_ierrs - q->ifnets[i].if_ierrs
				+ p->ifnets[i].if_oerrs - q->ifnets[i].if_oerrs
				+ p->ifnets[i].if_ocolls - q->ifnets[i].if_ocolls;
			}
			if(errors) show_neterror=3;
			if(show_neterror) {
				BANNER(padneterr,"Network Error Counters");
				mvwprintw(padneterr,1, 0, "I/F Name iErrors iDrop iOverrun iFrame oErrors   oDrop oOverrun oCarrier oColls ");
				for (i = 0; i < networks; i++) {
					mvwprintw(padneterr,2 + i, 0, "%8s %7lu %7lu %7lu %7lu %7lu %7lu %7lu %7lu %7lu",
									&p->ifnets[i].if_name[0],
									p->ifnets[i].if_ierrs,   
									p->ifnets[i].if_idrop,   
									p->ifnets[i].if_ififo,   
									p->ifnets[i].if_iframe,   
									p->ifnets[i].if_oerrs,   
									p->ifnets[i].if_odrop,   
									p->ifnets[i].if_ofifo,   
									p->ifnets[i].if_ocarrier,   
									p->ifnets[i].if_ocolls);   
					
				}
				DISPLAY(padneterr,networks + 2);
				if(show_neterror > 0) show_neterror--;
			}
#ifdef JFS
			if (show_jfs) {
				BANNER(padjfs,"Filesystems");
				mvwprintw(padjfs,1, 0, "Filesystem            SizeMB  FreeMB  Use%% Type     MountPoint");
				
				for (k = 0; k < jfses; k++) {
					fs_size=0;
					fs_bsize=0;
					fs_free=0;
					fs_size_used=100.0;
					if(jfs[k].mounted) {
						if(!strncmp(jfs[k].name,"/Users/stollcri/Documents/code/c/nmond/dbg/",6)       /* sub directorys have to be fake too */
						   || !strncmp(jfs[k].name,"/sys/",5)
						   || !strncmp(jfs[k].name,"/dev/",5)
						   || !strncmp(jfs[k].name,"/proc",6) /* one more than the string to ensure the NULL */
						   || !strncmp(jfs[k].name,"/sys",5)
						   || !strncmp(jfs[k].name,"/dev",5)
						   || !strncmp(jfs[k].name,"/rpc_pipe",10)
						   ) { /* /proc gives invalid/insane values */
							mvwprintw(padjfs,2+k, 0, "%-14s", jfs[k].name);
							mvwprintw(padjfs,2+k, 27, "-");
							mvwprintw(padjfs,2+k, 35, "-");
							mvwprintw(padjfs,2+k, 41, "-");
							mvwprintw(padjfs,2+k, 43, "%-8s not a real filesystem",jfs[k].type);
						} else {
							statfs_buffer.f_blocks=0;
							if((ret=fstatfs( jfs[k].fd, &statfs_buffer)) != -1) {
								if(statfs_buffer.f_blocks != 0) {
									/* older Linux seemed to always report in 4KB blocks but
									 newer Linux release use the f_bsize block sizes but
									 the man statfs docs the field as the natural I/O size so
									 the blocks reported here are ambigous in size */
									if(statfs_buffer.f_bsize == 0) 
										fs_bsize = 4.0 * 1024.0;
									else
										fs_bsize = statfs_buffer.f_bsize;
									/* convert blocks to MB */
									fs_size = (float)statfs_buffer.f_blocks * fs_bsize/1024.0/1024.0;
									
									/* find the best size info available f_bavail is like df reports
									 otherwise use f_bsize (this includes inode blocks) */
									if(statfs_buffer.f_bavail == 0) 
										fs_free = (float)statfs_buffer.f_bfree  * fs_bsize/1024.0/1024.0;
									else
										fs_free = (float)statfs_buffer.f_bavail  * fs_bsize/1024.0/1024.0;
									
									/* this is a percentage */
									fs_size_used = (fs_size - (float)statfs_buffer.f_bfree  * fs_bsize/1024.0/1024.0)/fs_size * 100.0;
									/* try to get the same number as df using kludge */
									/*fs_size_used += 1.0; */
									if (fs_size_used >100.0)
										fs_size_used = 100.0;
									
									if( (i=strlen(jfs[k].device)) <20)
										str_p=&jfs[k].device[0];
									else {
										str_p=&jfs[k].device[i-20];
									}
									mvwprintw(padjfs,2+k, 0, "%-20s %7.0f %7.0f %4.0f%% %-8s %s",
											  str_p,
											  fs_size,
											  fs_free,
											  ceil(fs_size_used),
											  jfs[k].type,
											  jfs[k].name
											  );
									
								} else {
									mvwprintw(padjfs,2+k, 0, "%s", jfs[k].name);
									mvwprintw(padjfs,2+k, 43, "%-8s size=zero blocks!", jfs[k].type);
								}
							}
							else {
								mvwprintw(padjfs,2+k, 0, "%s", jfs[k].name);
								mvwprintw(padjfs,2+k, 43, "%-8s statfs failed", jfs[k].type);
							}
						}
					} else {
						mvwprintw(padjfs,2+k, 0, "%-14s", jfs[k].name);
						mvwprintw(padjfs,2+k, 43, "%-8s not mounted",jfs[k].type);
					}
				}
				DISPLAY(padjfs,2 + jfses);
			}
			
#endif /* JFS */
			
			if (show_disk || show_verbose || show_diskmap || show_dgroup) {
				proc_read(P_STAT);
				proc_disk(elapsed);
			}
			if (show_diskmap) {
				BANNER(padmap,"Disk %%Busy Map");
				mvwprintw(padmap,0, 18,"Key: @=90 #=80 X=70 8=60 O=50 0=40 o=30 +=20 -=10 .=5 _=0%%");
				mvwprintw(padmap,1, 0,"             Disk No.  1         2         3         4         5         6   ");
				if(disk_first_time) {
					disk_first_time=0;
					mvwprintw(padmap,2, 0,"Please wait - collecting disk data");
				} else {
					mvwprintw(padmap,2, 0,"Disks=%-4d   0123456789012345678901234567890123456789012345678901234567890123", disks);
					mvwprintw(padmap,3, 0,"disk 0 to 63 ");
					for (i = 0; i < disks; i++) {
						disk_busy = DKDELTA(dk_time) / elapsed;
						disk_read = DKDELTA(dk_rkb) / elapsed;
						disk_write = DKDELTA(dk_wkb) / elapsed;
						/* ensure boundaries */
						if (disk_busy <  0)
							disk_busy=0;
						else
							if (disk_busy > 99) disk_busy=99;
						
#define MAPWRAP 64
						mvwprintw(padmap,3 + (int)(i/MAPWRAP), 13+ (i%MAPWRAP), "%c",disk_busy_map_ch[(int)disk_busy]);
					}
				}
				DISPLAY(padmap,4 + disks/MAPWRAP);
			}

			if (show_disk) {
				if(show_disk) {
					BANNER(paddisk,"Disk I/O");
					switch(disk_mode) {
						case DISK_MODE_PARTITIONS: mvwprintw(paddisk, 0, 12, "/Users/stollcri/Documents/code/c/nmond/dbg/partitions");break;
						case DISK_MODE_DISKSTATS:  mvwprintw(paddisk, 0, 12, "/Users/stollcri/Documents/code/c/nmond/dbg/diskstats");break;
						case DISK_MODE_IO:         mvwprintw(paddisk, 0, 12, "/Users/stollcri/Documents/code/c/nmond/dbg/stat+disk_io");break;
					}
					mvwprintw(paddisk,0, 31, "mostly in KB/s");
					mvwprintw(paddisk,0, 50, "Warning:contains duplicates");
					switch (show_disk) {
						case SHOW_DISK_STATS: 
							mvwprintw(paddisk,1, 0, "DiskName Busy    Read    Write       Xfers   Size  Peak%%  Peak-RW    InFlight ");
							break;
						case SHOW_DISK_GRAPH: 
							mvwprintw(paddisk,1, 0, "DiskName Busy  ");
							COLOUR wattrset(paddisk,COLOR_PAIR(6));
							mvwprintw(paddisk,1, 15, "Read ");
							COLOUR wattrset(paddisk,COLOR_PAIR(3));
							mvwprintw(paddisk,1, 20, "Write");
							COLOUR wattrset(paddisk,COLOR_PAIR(0));
							mvwprintw(paddisk,1, 25, "KB|0          |25         |50          |75       100|");
							break;
					}
				}
				if(disk_first_time) { 
					disk_first_time=0;
					mvwprintw(paddisk,2, 0, "Please wait - collecting disk data");
				} else {
					total_disk_read  = 0.0;
					total_disk_write = 0.0;
					total_disk_xfers = 0.0;
					disk_mb = 0;
					for (i = 0,k=0; i < disks; i++) {
						disk_read = DKDELTA(dk_rkb) / elapsed;
						disk_write = DKDELTA(dk_wkb) / elapsed;
						if((show_disk == SHOW_DISK_GRAPH) && (disk_read > 9999.9 || disk_write > 9999.9)) {
							disk_mb=1;
							COLOUR wattrset(paddisk, COLOR_PAIR(1));
							mvwprintw(paddisk,1, 25, "MB");
							COLOUR wattrset(paddisk, COLOR_PAIR(0));
							break;
						}
					}
					for (i = 0,k=0; i < disks; i++) {
						if(disk_only_mode && is_dgroup_name(p->dk[i].dk_name) == 0)
							continue;
						
						/*
						 if(p->dk[i].dk_name[0] == 'h')
						 continue;
						 */
						disk_busy = DKDELTA(dk_time) / elapsed;
						disk_read = DKDELTA(dk_rkb) / elapsed;
						disk_write = DKDELTA(dk_wkb) / elapsed;
						disk_xfers = DKDELTA(dk_xfers);
						
						total_disk_read  +=disk_read;
						total_disk_write +=disk_write;
						total_disk_xfers +=disk_xfers;
						
						if(disk_busy_peak[i] < disk_busy)
							disk_busy_peak[i] = disk_busy;
						if(disk_rate_peak[i] < (disk_read+disk_write))
							disk_rate_peak[i] = disk_read+disk_write;
						if(!show_all && disk_busy < 1)
							continue;
						
						if(strlen(p->dk[i].dk_name) > 8)
							str_p = &p->dk[i].dk_name[strlen(p->dk[i].dk_name) -8];
						else
							str_p = &p->dk[i].dk_name[0];
						
						if(show_disk == SHOW_DISK_STATS) {
							/* output disks stats */
							mvwprintw(paddisk,2 + k, 0, "%-8s %3.0f%% %8.1f %8.1fKB/s %6.1f %5.1fKB  %3.0f%% %9.1fKB/s %3d",
									  str_p, 
									  disk_busy,
									  disk_read,
									  disk_write,
									  disk_xfers / elapsed,
									  disk_xfers == 0.0 ? 0.0 : 
									  (DKDELTA(dk_rkb) + DKDELTA(dk_wkb) ) / disk_xfers,
									  disk_busy_peak[i],
									  disk_rate_peak[i],
									  p->dk[i].dk_inflight);
							k++;
						}
						if(show_disk == SHOW_DISK_GRAPH) {
							/* output disk bar graphs */
							
							
							if(disk_mb) mvwprintw(paddisk,2 + k, 0, "%-8s %3.0f%% %6.1f %6.1f",
												  str_p, 
												  disk_busy,
												  disk_read/1024.0,
												  disk_write/1024.0);
							else mvwprintw(paddisk,2 + k, 0, "%-8s %3.0f%% %6.1f %6.1f",
										   str_p, 
										   disk_busy,
										   disk_read,
										   disk_write);
							mvwprintw(paddisk,2 + k, 27, "|                                                  ");
							wmove(paddisk,2 + k, 28);
							if(disk_busy >100) disk_busy=100;
							if( disk_busy > 0.0 && (disk_write+disk_read) > 0.1) {
								/* 50 columns in the disk graph area so divide % by two */
								readers = disk_busy*disk_read/(disk_write+disk_read)/2;
								writers = disk_busy*disk_write/(disk_write+disk_read)/2;
								if(readers + writers > 50) {
									readers=0;
									writers=0;
								}
								/* don't go beyond row 78 i.e. j = 28 + 50 */
								for (j = 0; j < readers && j<50; j++) {
									COLOUR wattrset(paddisk,COLOR_PAIR(12));
									wprintw(paddisk,"R");
									COLOUR wattrset(paddisk,COLOR_PAIR(0));
								}
								for (; j < readers + writers && j<50; j++) {
									COLOUR wattrset(paddisk,COLOR_PAIR(11));
									wprintw(paddisk,"W");
									COLOUR wattrset(paddisk,COLOR_PAIR(0));
								}
								for (j = disk_busy; j < 50; j++)
									wprintw(paddisk," ");
							} else {
								for (j = 0; j < 50; j++)
									wprintw(paddisk," ");
								if(p->dk[i].dk_time == 0.0) 
									mvwprintw(paddisk,2 + k, 27, "| disk busy not available");
							}
							if(disk_busy_peak[i] >100)
								disk_busy_peak[i]=100;
							
							mvwprintw(paddisk,2 + i, 77, "|");
							/* check rounding has not got the peak ">" over the 100% */
							j = 28+(int)(disk_busy_peak[i]/2);
							if(j>77)
								j=77;
							mvwprintw(paddisk,2 + i, j, ">");
							k++;
						}
					}
					mvwprintw(paddisk,2 + k, 0, "Totals Read-MB/s=%-8.1f Writes-MB/s=%-8.1f Transfers/sec=%-8.1f",
							  total_disk_read  / 1024.0,
							  total_disk_write / 1024.0,
							  total_disk_xfers / elapsed);
					
				}
				DISPLAY(paddisk,3 + k);
			}
			if (show_dgroup) {
				BANNER(paddg,"Disk-Group-I/O");
				if (dgroup_loaded != 2 || dgroup_total_disks == 0) {
					mvwprintw(paddg, 1, 1, "No Disk Groups found use -g groupfile when starting nmon");
					n = 0;
				} else if (disk_first_time) {
					disk_first_time=0;
					mvwprintw(paddg, 1, 1, "Please wait - collecting disk data");
				} else {
					mvwprintw(paddg, 1, 1, "Name          Disks AvgBusy Read-KB/s|Write  TotalMB/s   xfers/s BlockSizeKB");
					total_busy   = 0.0;
					total_rbytes = 0.0;
					total_wbytes = 0.0;
					total_xfers  = 0.0;
					for(k = n = 0; k < dgroup_total_groups; k++) {
						/*
						 if (dgroup_name[k] == 0 )
						 continue;
						 */
						disk_busy   = 0.0;
						disk_read = 0.0;
						disk_write = 0.0;
						disk_xfers  = 0.0;
						for (j = 0; j < dgroup_disks[k]; j++) {
							i = dgroup_data[k*DGROUPITEMS+j];
							if (i != -1) {
								disk_busy   += DKDELTA(dk_time) / elapsed;
								/*
								 disk_read += DKDELTA(dk_reads) * p->dk[i].dk_bsize / 1024.0 /elapsed;
								 disk_write += DKDELTA(dk_writes) * p->dk[i].dk_bsize / 1024.0 /elapsed;
								 */
								disk_read += DKDELTA(dk_rkb) /elapsed;
								disk_write += DKDELTA(dk_wkb) /elapsed;
								disk_xfers  += DKDELTA(dk_xfers) /elapsed;
							}
						}
						if (dgroup_disks[k] == 0)
							disk_busy = 0.0;
						else
							disk_busy = disk_busy / dgroup_disks[k];
						total_busy += disk_busy;
						total_rbytes += disk_read;
						total_wbytes += disk_write;
						total_xfers  += disk_xfers;
						/*						if (!show_all && (disk_read < 1.0 && disk_write < 1.0))
						 continue;
						 */
						if ((disk_read + disk_write) == 0 || disk_xfers == 0)
							disk_size = 0.0;
						else
							disk_size = ((float)disk_read + (float)disk_write) / (float)disk_xfers;
						mvwprintw(paddg, n + 2, 1, "%-14s   %3d %5.1f%% %9.1f|%-9.1f %6.1f %9.1f %6.1f ",
								  dgroup_name[k], 
								  dgroup_disks[k],
								  disk_busy,
								  disk_read,
								  disk_write,
								  (disk_read + disk_write) / 1024, /* in MB */
								  disk_xfers,
								  disk_size
								  );
						n++;
					}
					mvwprintw(paddg, n + 2, 1, "Groups=%2d TOTALS %3d %5.1f%% %9.1f|%-9.1f %6.1f %9.1f",
							  n,
							  dgroup_total_disks,
							  total_busy / dgroup_total_disks,
							  total_rbytes,
							  total_wbytes,
							  (((double)total_rbytes + (double)total_wbytes)) / 1024, /* in MB */
							  total_xfers
							  );
				}
				DISPLAY(paddg, 3 + dgroup_total_groups);
			}	/* 		if ((show_dgroup || (!cursed && dgroup_loaded)))  */
			
			if (show_top) {
				wmove(padtop,1, 1);
				wclrtobot(padtop);
				/* Get the details of the running processes */
				skipped = 0;
				n = getprocs(0);
				if (n > p->nprocs) {
					n = n +128; /* allow for growth in the number of processes in the mean time */
					p->procs = REALLOC(p->procs, sizeof(struct procsinfo ) * (n+1) ); /* add one to avoid overrun */
					p->nprocs = n;
				}
				
				n = getprocs(1);
				
				if (topper_size < n) {
					topper = REALLOC(topper, sizeof(struct topper ) * (n+1) ); /* add one to avoid overrun */
					topper_size = n;
				}
				/* Sort the processes by CPU utilisation */
				for ( i = 0, max_sorted = 0; i < n; i++) {
					/* move forward in the previous array to find a match*/
					for(j=0;j < q->nprocs;j++) {
						if (p->procs[i].pi_pid == q->procs[j].pi_pid) { /* found a match */
							topper[max_sorted].index = i;
							topper[max_sorted].other = j;
							topper[max_sorted].time =  TIMEDELTA(pi_utime,i,j) + 
							TIMEDELTA(pi_stime,i,j);
							topper[max_sorted].size =  p->procs[i].statm_resident;
							if(isroot)
								topper[max_sorted].io =  COUNTDELTA(read_io) + COUNTDELTA(write_io);
							
							max_sorted++;
							break;
						}
					}
				}
				switch(show_topmode) {
					default:
					case 3: qsort((void *) & topper[0], max_sorted, sizeof(struct topper ), &cpu_compare );
						break;
					case 4: qsort((void *) & topper[0], max_sorted, sizeof(struct topper ), &size_compare );
						break;
					case 5: qsort((void *) & topper[0], max_sorted, sizeof(struct topper ), &disk_compare );
						break;
				}
				BANNER(padtop,"Top Processes");
				if(isroot) {
					mvwprintw(padtop,0, 15, "Procs=%d mode=%d (1=Basic, 3=Perf 4=Size 5=I/O)", n, show_topmode);
				} else {
					mvwprintw(padtop,0, 15, "Procs=%d mode=%d (1=Basic, 3=Perf 4=Size 5=(root-only))", n, show_topmode);
				}
				if(top_first_time) {
					top_first_time = 0;
					mvwprintw(padtop,1, 1, "Please wait - information being collected");
				} else {
					switch (show_topmode) {
						case 1:
							mvwprintw(padtop,1, 1, "  PID      PPID  Pgrp Nice Prior Status    proc-Flag Command");
							for (j = 0; j < max_sorted; j++) {
								i = topper[j].index;
								if (p->procs[i].pi_pgrp == p->procs[i].pi_pid)
									strcpy(pgrp, "none");
								else
									sprintf(&pgrp[0], "%d", p->procs[i].pi_pgrp);
								/* skip over processes with 0 CPU */
								if(!show_all && (topper[j].time/elapsed < ignore_procdisk_threshold) && !cmdfound) 
									break;
								if( x + j + 2 - skipped > LINES+2) /* +2 to for safety :-) */
									break;
								mvwprintw(padtop,j + 2 - skipped, 1, "%7d %7d %6s %4d %4d %9s 0x%08x %1s %-32s",
												p->procs[i].pi_pid,
												p->procs[i].pi_ppid,
												pgrp,
												p->procs[i].pi_nice,
												p->procs[i].pi_pri,
												
												(topper[j].time * 100 / elapsed) ? "Running "
												: get_state(p->procs[i].pi_state),
												p->procs[i].pi_flags,
												(p->procs[i].pi_tty_nr ? "F" : " "),
												p->procs[i].pi_comm);
							}
							break;
						case 3:
						case 4:
						case 5:
							
							if(show_args == ARGS_ONLY)  {
								formatstring = "  PID    %%CPU ResSize    Command                                            ";
							} else if(COLS > 119) {
								if(show_topmode == 5)
									formatstring = "  PID       %%CPU    Size     Res    Res     Res     Res    Shared   StorageKB Command";
								else
									formatstring = "  PID       %%CPU    Size     Res    Res     Res     Res    Shared    Faults   Command";
							} else {
								if(show_topmode == 5)
									formatstring = "  PID    %%CPU  Size   Res   Res   Res   Res Shared StorageKB Command";
								else
									formatstring = "  PID    %%CPU  Size   Res   Res   Res   Res Shared   Faults  Command";
							}
							mvwprintw(padtop,1, y, formatstring);
							
							if(show_args == ARGS_ONLY) {
								formatstring = "         Used      KB                                                        ";
							} else if(COLS > 119) {
								if(show_topmode == 5)
									formatstring = "            Used      KB     Set    Text    Data     Lib    KB    Read Write";
								else
									formatstring = "            Used      KB     Set    Text    Data     Lib    KB     Min   Maj";
							} else {
								if(show_topmode == 5)
									formatstring = "         Used    KB   Set  Text  Data   Lib    KB ReadWrite ";
								else
									formatstring = "         Used    KB   Set  Text  Data   Lib    KB  Min  Maj ";
							}
							mvwprintw(padtop,2, 1, formatstring);
							for (j = 0; j < max_sorted; j++) {
								i = topper[j].index;
								if(!show_all) { 
									/* skip processes with zero CPU/io */
									if(show_topmode == 3 && (topper[j].time/elapsed) < ignore_procdisk_threshold && !cmdfound)
										break;
									if(show_topmode == 5 && (topper[j].io < ignore_io_threshold && !cmdfound))
										break;
								}
								if( x + j + 3 - skipped > LINES+2) /* +2 to for safety :-) XYZXYZ*/
									break;
								if(cmdfound && !cmdcheck(p->procs[i].pi_comm)) {
									skipped++;
									continue;
								}
								if(show_args == ARGS_ONLY){
									mvwprintw(padtop,j + 3 - skipped, 1, 
											  "%7d %5.1f %7lu %-120s",
											  p->procs[i].pi_pid,
											  topper[j].time / elapsed,
											  p->procs[i].statm_resident*pagesize/1024, /* in KB */
											  args_lookup(p->procs[i].pi_pid,
														  p->procs[i].pi_comm));
								}
								else {
									if(COLS > 119)
										formatstring = "%8d %7.1f %7lu %7lu %7lu %7lu %7lu %5lu %6d %6d %-32s";
									else
										formatstring = "%7d %5.1f %5lu %5lu %5lu %5lu %5lu %5lu %4d %4d %-32s";
									
									mvwprintw(padtop,j + 3 - skipped, 1, formatstring,
											  p->procs[i].pi_pid,
											  topper[j].time/elapsed,
											  /* topper[j].time /1000.0 / elapsed,*/
											  p->procs[i].statm_size*pagesize/1024UL, /* in KB */
											  p->procs[i].statm_resident*pagesize/1024UL, /* in KB */
											  p->procs[i].statm_trs*pagesize/1024UL, /* in KB */
											  p->procs[i].statm_drs*pagesize/1024UL, /* in KB */
											  p->procs[i].statm_lrs*pagesize/1024UL, /* in KB */
											  p->procs[i].statm_share*pagesize/1024UL, /* in KB */
											  show_topmode == 5 ? (int)(COUNTDELTA(read_io)  / elapsed / 1024) : (int)(COUNTDELTA(pi_minflt) / elapsed),
											  show_topmode == 5 ? (int)(COUNTDELTA(write_io) / elapsed / 1024) : (int)(COUNTDELTA(pi_majflt) / elapsed),
											  p->procs[i].pi_comm);
								}
							}
							break;
					}
				}
				DISPLAY(padtop,3 + j);
			}
			
			/* underline the end of the stats area border */
			if(x < LINES-2)mvwhline(stdscr, x, 1, ACS_HLINE,COLS-2);
			
			wmove(stdscr,0, 0);
			wrefresh(stdscr);
			doupdate();
			
			for (i = 0; i < seconds; i++) {
				sleep(1);

				// stollcri - move to top of program loop
				if(getinput(&winsets, &settings)) {
					break;
				}
			}

			if(x<LINES-2) mvwhline(stdscr, x, 1, ' ', COLS-2);
			if(first_key_pressed == 0){
				first_key_pressed=1;
				wmove(stdscr,0, 0);
				wclear(stdscr);
				wmove(stdscr,0,0);
				wclrtobot(stdscr);
				wrefresh(stdscr);
				doupdate();
			}
			
			switcher();
		}
	}
