#ifndef SYSINFO_H
#define SYSINFO_H

/**
 * sysinfo.h -- Gather system information from *BSD* based systems
 *
 *
 * nmond -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *  https://github.com/stollcri/nmond
 *
 * 
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

#include <stdlib.h>
#include "pidhash.h"

#define DATE_FORMAT "%Y-%m-%d"
#define TIME_FORMAT "%H:%M:%S"
#define DATE_TIME_FORMAT "%Y-%m-%d %H:%M:%S"
//#define CPU_TIME_DENOMINATOR 1000000
#define COUNT_HYPERTHREADS_IN_CPU_AVG 1

//
// Hardware based information
// 

struct syshw { // CTL_HW
	unsigned int cpufrequency; // hw.cpufrequency
	unsigned int cpufrequencymin; // hw.cpufrequency_min
	unsigned int cpufrequencymax; // hw.cpufrequency_max
	unsigned int cpucount; // hw.ncpu -- DEPRECATED
	unsigned int cpuactive; // hw.activecpu
	unsigned int physicalcpucount; // hw.physicalcpu
	unsigned int physicalcpumax; // hw.physicalcpu_max
	unsigned int logicalcpucount; // hw.logicalcpu
	unsigned int logicalcpumax; // hw.logicalcpu_max
	unsigned int hyperthreads;
	
	unsigned int byteorder; // HW_BYTEORDER
	unsigned int usermemory; // HW_USERMEM (non-kernel memory)
	unsigned int pagesize; // HW_PAGESIZE
	unsigned int l1icachesize;
	unsigned int l1dcachesize;
	unsigned int l2cachesize;
	unsigned int l3cachesiz;

	unsigned int thermalsensor; // machdep.cpu.thermal.sensor
	unsigned int thermallevelcpu; // machdep.xcpm.cpu_thermal_level
	unsigned int thermallevelgpu; // machdep.xcpm.gpu_thermal_level
	unsigned int thermallevelio; // machdep.xcpm.io_thermal_level

	unsigned long long memorysize; // HW_MEMSIZE (total memory, 64bit int)

	char *architecture; // HW_MACHINE_ARCH
	char *cpuvendor; // machdep.cpu.vendor ("GenuineIntel")
	char *cpubrand; // machdep.cpu.brand_string ("Intel(R) Core(TM) i7-4650U CPU @ 1.70GHz")
	char *machine; // HW_MACHINE ("x86_64")
	char *model; // HW_MODEL ("MacbookAir6,2")
};
#define SYSHW_INIT { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
0, 0, 0, 0, 0, 0, 0, \
0, 0, 0, 0, 0, \
NULL, NULL, NULL, NULL, NULL };

extern void getsyshwinfo(struct syshw*);

//
// Kernel based information
// 

struct syskern { // CTL_KERN
	unsigned int hostid; // KERN_HOSTID
	unsigned int jobcontrol; // KERN_JOB_CONTROL
	unsigned int maxarguments; // KERN_ARGMAX
	unsigned int maxfiles; // KERN_MAXFILES
	unsigned int maxfilespercpu; // KERN_MAXFILESPERPROC
	unsigned int maxprocesses; // KERN_MAXPROC
	unsigned int maxprocessespercpu; // KERN_MAXPROCPERUID
	unsigned int maxvnodes; // KERN_MAXVNODES
	unsigned int maxgroups; // KERN_NGROUPS

	unsigned int osdate; // KERN_OSRELDATE
	unsigned int osrevision; // KERN_OSREV
	unsigned int posixversion; // KERN_POSIX1
	unsigned int securitylevel; // KERN_SECURELVL
	unsigned int updateinterval; // KERN_UPDATEINTERVAL

	unsigned int corecount; // machdep.cpu.core_count
	unsigned int dummy;

	char *ostype; // KERN_OSTYPE ("Darwin")
	char *osrelease; // KERN_OSRELEASE ("14.5.0")
	char *osversion; // KERN_OSVERSION ("199506")
	char *version; // KERN_VERSION ("Darwin Kernel Version 14.5.0: Wed Jul 29 02:26:53 PDT 2015; root:xnu-2782.40.9~1/RELEASE_X86_64")
	char *bootfile; // KERN_BOOTFILE
	char *hostname; // KERN_HOSTNAME
	char *domainname; // KERN_NISDOMAINNAME
	char *boottimestring;
	char *uptimestring;

	struct timeval boottime; // KERN_BOOTTIME ("{ sec = 1439860179, usec = 0 }")
	struct timeval uptime;

	//struct clockinfo clockrate; // KERN_CLOCKRATE ("{ hz = 100, tick = 10000, tickadj = 22, profhz = 100, stathz = 100 }")
	//struct file filetable; // KERN_FILE
	// struct vnode vnodetable; // KERN_VNODE
	// KERN_PROC
	// KERN_PROF
};
#define SYSKERN_INIT { 0, 0, 0, 0, 0, 0, 0, 0, 0, \
0, 0, 0, 0, 0, \
0, 0, \
NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, \
{ 0, 0 }, { 0, 0 } }

extern void getsyskerninfo(struct syskern*);

//
// System resource utilization information
// 

struct sysrescpu {
	int user;
	int sys;
	int idle;
	int nice;
	int total;

	int olduser;
	int oldsys;
	int oldidle;
	int oldnice;
	int oldtotal;

	double percentuser;
	double percentsys;
	double percentidle;
	double percentnice;
};
#define SYSRESCPU_INIT { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, 0.0, 0.0 }

struct sysres {
	int cpucount;
	int cpuhyperthreadmod;

	struct sysrescpu *cpus;

	double percentallcpu;

	double avgpercentuser;
	double avgpercentsys;
	double avgpercentidle;
	double avgpercentnice;

	double loadavg1;
	double loadavg5;
	double loadavg15;

	unsigned long diskuser;
	unsigned long diskusew;
	unsigned long diskuserlast;
	unsigned long diskusewlast;
	unsigned long long memused;
};
#define SYSRES_INIT { 0, 0, NULL, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0 }

extern void getsysresinfo(struct sysres *);

//
// Processes information
//

#define SYSPROC_PATH_LENGTH 45

struct sysproc {
	char status;
	int pid;
	struct timeval realtime;
	// int cticks;
	// unsigned long long uticks;
	// unsigned long long sticks;
	// unsigned long long iticks;
	unsigned int priority;
	char nice;
	char *name;

	unsigned int realuid;
	char *realusername;
	unsigned int effectiveuid;
	char *effectiveusername;
	int pgid;
	int parentpid;
	int ttydev;
	char *setloginname;

	char *path;
	
	unsigned long long utime;
	unsigned long long stime;
	unsigned long long totaltime;
	unsigned long long idlewakeups;
	unsigned long long wiredmem;
	unsigned long long residentmem;
	unsigned long long physicalmem;
	unsigned long long diskior;
	unsigned long long diskiow;
	unsigned long long billedtime;

	unsigned long long lasttotaltime;
	double percentage;
};
// #define SYSPROC_INIT { ' ', 0, 0, 0, ' ', ' ', \
// 0, 0, 0, 0, 0, 0, 0, 0, \
// 0, \
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
// 0, 0.0 }

extern struct sysproc **getsysprocinfoall(size_t*, struct sysproc**, struct hashitem**, double, struct sysres*);
/*
extern struct sysproc getsysprocinfobypid(int, size_t);
extern struct sysproc getsysprocinfobypgrp(int, size_t);
extern struct sysproc getsysprocinfobytty(int, size_t);
extern struct sysproc getsysprocinfobyuid(int, size_t);
extern struct sysproc getsysprocinfobyruid(int, size_t);
*/

//
// Network information
//

struct sysnetif {
	unsigned long long ipackets;
	unsigned long long ierrors;
	unsigned long long ibytes;
	unsigned long long oldibytes;

	unsigned long long opackets;
	unsigned long long oerrors;
	unsigned long long obytes;
	unsigned long long oldobytes;

	unsigned long long drops;
	unsigned long long noproto;
};

struct sysnet {
	int ifcount;
	struct sysnetif *ifs;

	unsigned long long ipackets;
	unsigned long long ierrors;
	unsigned long long ibytes;
	unsigned long long oldibytes;

	unsigned long long opackets;
	unsigned long long oerrors;
	unsigned long long obytes;
	unsigned long long oldobytes;

	unsigned long long drops;
	unsigned long long noproto;
};
#define SYSNET_INIT { 0, NULL, \
0, 0, 0, 0, \
0, 0, 0, 0, \
0, 0 }

extern void getsysnetinfo(struct sysnet *net);

//
// Mem
// 
// vm.swapusage: total = 2048.00M  used = 988.00M  free = 1060.00M  (encrypted)
// vmmeter.h

#endif
