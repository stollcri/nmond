#ifndef NMOND_H
#define NMOND_H

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

#include <stdio.h> // FILE
#include <sys/time.h> // timeval

#define LARGEMEM 1
/* This adds the following to the disk stats
	pi_num_threads,
	pi_rt_priority,
	pi_policy,
	pi_delayacct_blkio_ticks
 */

#define P_CPUINFO	0
#define P_STAT		1
#define P_VERSION	2
#define P_MEMINFO 	3
#define P_UPTIME 	4
#define P_LOADAVG	5
#define P_NFS 		6
#define P_NFSD 		7
#define P_VMSTAT	8 /* new in 13h */
#define P_NUMBER	9 /* one more than the max */

/* Maximum number of lines in /proc files */
/* Intel already has 26 (so here 30) per Hypterthread CPU (max 128*2 CPUs here) */
/* POWER has only 6 to 7 lines but gets  1536 SMT threads soon */
/* Erring on the saf side below */
#define PROC_MAXLINES (16*1024) /*MAGIC COOKIE WARNING */

struct {
	FILE *fp;
	char *filename;
	int size;
	int lines;
	char *line[PROC_MAXLINES];
	char *buf;
	int read_this_interval; /* track updates for each update to stop  double data collection */
} proc[P_NUMBER];

struct procsinfo {
	int pi_pid;
	char pi_comm[64];
	char pi_state;
	int pi_ppid;
	int pi_pgrp;
	int pi_session;
	int pi_tty_nr;
	int pi_tty_pgrp;
	unsigned long pi_flags;
	unsigned long pi_minflt;
	unsigned long pi_cmin_flt;
	unsigned long pi_majflt;
	unsigned long pi_cmaj_flt;
	unsigned long pi_utime;
	unsigned long pi_stime;
	long pi_cutime;
	long pi_cstime;
	long pi_pri;
	long pi_nice;
	long pi_num_threads;
	long pi_it_real_value;
	unsigned long pi_start_time;
	unsigned long pi_vsize;
	long pi_rss; /* - 3 */
	unsigned long pi_rlim_cur;
	unsigned long pi_start_code;
	unsigned long pi_end_code;
	unsigned long pi_start_stack;
	unsigned long pi_esp;
	unsigned long pi_eip;
	/* The signal information here is obsolete. */
	unsigned long pi_pending_signal;
	unsigned long pi_blocked_sig;
	unsigned long pi_sigign;
	unsigned long pi_sigcatch;
	unsigned long pi_wchan;
	unsigned long pi_nswap;
	unsigned long pi_cnswap;
	int pi_exit_signal;
	int pi_cpu;
	unsigned long pi_rt_priority;
	unsigned long pi_policy;
	unsigned long long pi_delayacct_blkio_ticks;
	unsigned long statm_size;       /* total program size */
	unsigned long statm_resident;   /* resident set size */
	unsigned long statm_share;      /* shared pages */
	unsigned long statm_trs;        /* text (code) */
	unsigned long statm_drs;        /* data/stack */
	unsigned long statm_lrs;        /* library */
	unsigned long statm_dt;         /* dirty pages */
	
	unsigned long long read_io;     /* storage read bytes */
	unsigned long long write_io;    /* storage write bytes */
};

/* Full Args Mode stuff here */

#define ARGSMAX 1024*8
#define CMDLEN 4096

struct {
	int pid;
	char *args;
} arglist[ARGSMAX];

/* Main data structure for collected stats.
 * Two versions are previous and current data.
 * Often its the difference that is printed.
 * The pointers are swaped i.e. current becomes the previous
 * and the previous over written rather than moving data around.
 */
struct cpu_stat {
	long long user;
	long long sys;
	long long wait;
	long long idle;
	long long irq;
	long long softirq;
	long long steal;
	long long nice;
	long long intr;
	long long ctxt;
	long long btime;
	long long procs;
	long long running;
	long long blocked;
	float uptime;
	float idletime;
	float mins1;
	float mins5;
	float mins15;
};

#define ulong unsigned long
struct dsk_stat {
	char	dk_name[32];
	int	dk_major;
	int	dk_minor;
	long	dk_noinfo;
	ulong	dk_reads;
	ulong	dk_rmerge;
	ulong	dk_rmsec;
	ulong	dk_rkb;
	ulong	dk_writes;
	ulong	dk_wmerge;
	ulong	dk_wmsec;
	ulong	dk_wkb;
	ulong	dk_xfers;
	ulong	dk_bsize;
	ulong	dk_time;
	ulong	dk_inflight;
	ulong	dk_backlog;
	ulong	dk_partition;
	ulong	dk_blocks; /* in /proc/partitions only */
	ulong	dk_use;
	ulong	dk_aveq;
};

struct mem_stat {
	long memtotal;
	long memfree;
	long memshared;
	long buffers;
	long cached;
	long swapcached;
	long active;
	long inactive;
	long hightotal;
	long highfree;
	long lowtotal;
	long lowfree;
	long swaptotal;
	long swapfree;
#ifdef LARGEMEM
	long dirty;
	long writeback;
	long mapped;
	long slab;
	long committed_as;
	long pagetables;
	long hugetotal;
	long hugefree;
	long hugesize;
#else
	long bigfree;
#endif /*LARGEMEM*/
};

struct vm_stat {
	long long nr_dirty;
	long long nr_writeback;
	long long nr_unstable;
	long long nr_page_table_pages;
	long long nr_mapped;
	long long nr_slab;
	long long pgpgin;
	long long pgpgout;
	long long pswpin;
	long long pswpout;
	long long pgalloc_high;
	long long pgalloc_normal;
	long long pgalloc_dma;
	long long pgfree;
	long long pgactivate;
	long long pgdeactivate;
	long long pgfault;
	long long pgmajfault;
	long long pgrefill_high;
	long long pgrefill_normal;
	long long pgrefill_dma;
	long long pgsteal_high;
	long long pgsteal_normal;
	long long pgsteal_dma;
	long long pgscan_kswapd_high;
	long long pgscan_kswapd_normal;
	long long pgscan_kswapd_dma;
	long long pgscan_direct_high;
	long long pgscan_direct_normal;
	long long pgscan_direct_dma;
	long long pginodesteal;
	long long slabs_scanned;
	long long kswapd_steal;
	long long kswapd_inodesteal;
	long long pageoutrun;
	long long allocstall;
	long long pgrotated;
};

#define NFS_V2_NAMES_COUNT 18
#define NFS_V3_NAMES_COUNT 22
#define NFS_V4S_NAMES_COUNT 72
#define NFS_V4C_NAMES_COUNT 60

struct nfs_stat {
	long v2c[NFS_V2_NAMES_COUNT];	/* verison 2 client */
	long v3c[NFS_V3_NAMES_COUNT];	/* verison 3 client */
	long v4c[NFS_V4C_NAMES_COUNT];	/* verison 4 client */
	long v2s[NFS_V2_NAMES_COUNT];	/* verison 2 SERVER */
	long v3s[NFS_V3_NAMES_COUNT];	/* verison 3 SERVER */
	long v4s[NFS_V4S_NAMES_COUNT];	/* verison 4 SERVER */
};

#define NETMAX 32
struct net_stat {
	unsigned long if_name[17];
	unsigned long long if_ibytes;
	unsigned long long if_obytes;
	unsigned long long if_ipackets;
	unsigned long long if_opackets;
	unsigned long if_ierrs;
	unsigned long if_oerrs;
	unsigned long if_idrop;
	unsigned long if_ififo;
	unsigned long if_iframe;
	unsigned long if_odrop;
	unsigned long if_ofifo;
	unsigned long if_ocarrier;
	unsigned long if_ocolls;
};

#define PARTMAX 256
struct part_stat {
	int part_major;
	int part_minor;
	unsigned long part_blocks;
	char part_name[16];
	unsigned long part_rio;
	unsigned long part_rmerge;
	unsigned long part_rsect;
	unsigned long part_ruse;
	unsigned long part_wio;
	unsigned long part_wmerge;
	unsigned long part_wsect;
	unsigned long part_wuse;
	unsigned long part_run;
	unsigned long part_use;
	unsigned long part_aveq;
};

#define DISKMIN 256
#define DISKMAX diskmax

/* Supports up to 780, but not POWER6 595 follow-up with POWER7 */
/* XXXX needs rework to cope to with fairly rare but interesting higher numbers of CPU machines */
#define CPUMAX (192 * 8) /* MAGIC COOKIE WARNING */

struct data {
	struct dsk_stat *dk;
	struct cpu_stat cpu_total;
	struct cpu_stat cpuN[CPUMAX];
	struct mem_stat mem;
	struct vm_stat vm;
	struct nfs_stat nfs;
	struct net_stat ifnets[NETMAX];
	struct part_stat parts[PARTMAX];
	
	struct timeval tv;
	double time;
	struct procsinfo *procs;
	
	int    nprocs;
} database[2], *p, *q;

#define MAX_SNAPS 72
#define MAX_SNAP_ROWS 20
#define SNAP_OFFSET 6

struct {
	double user;
	double kernel;
	double iowait;
	double idle;
	double steal;
} cpu_snap[MAX_SNAPS];

#define JFSMAX 128
#define LOAD 1
#define UNLOAD 0
#define JFSNAMELEN 64
#define JFSTYPELEN 8

struct jfs {
	char name[JFSNAMELEN];
	char device[JFSNAMELEN];
	char type[JFSNAMELEN];
	int  fd;
	int  mounted;
} jfs[JFSMAX];

/* We order this array rather than the actual process tables
 * the index is the position in the process table and
 * the size is the memory used  in bytes
 * the io is the storge I/O performed in the the last period in bytes
 * the time is the CPU used in the last period in seconds
 */
struct topper {
	int	index;
	int	other;
	double	size;
	double	io;
	int	time;
} *topper;
int	topper_size = 200;

#endif
