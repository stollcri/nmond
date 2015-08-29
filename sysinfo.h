#ifndef SYSINFO_H
#define SYSINFO_H

/**
 * sysinfo.h -- Gather system information from *BSD* based systems
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 */

#include <stdlib.h>

#define STR_INIT "-"
#define DATE_FORMAT "%Y-%m-%d %H:%M:%S"

//
// Hardware based information
// 

// TODO: this is legacy code
/* Supports up to 780, but not POWER6 595 follow-up with POWER7 */
/* XXXX needs rework to cope to with fairly rare but interesting higher numbers of CPU machines */
#define CPUMAX (192 * 8) /* MAGIC COOKIE WARNING */

struct syshw { // CTL_HW
	unsigned int cpufrequency; // hw.cpufrequency
	unsigned int cpufrequencymin; // hw.cpufrequency_min
	unsigned int cpufrequencymax; // hw.cpufrequency_max
	unsigned int cpucount; // hw.ncpu
	unsigned int cpuactive; // hw.activecpu
	unsigned int physicalcpucount; // hw.physicalcpu
	unsigned int physicalcpumax; // hw.physicalcpu_max
	unsigned int logicalcpucount; // hw.logicalcpu
	unsigned int logicalcpumax; // hw.logicalcpu_max
	unsigned int hyperthreads;
	//int floatingpoint; // HW_FLOATINGPOINT (hardware support)
	
	unsigned int byteorder; // HW_BYTEORDER
	unsigned int memorysize; // HW_MEMSIZE (total memory, 64bit int)
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

	char *architecture; // HW_MACHINE_ARCH
	char *cpuvendor; // machdep.cpu.vendor ("GenuineIntel")
	char *cpubrand; // machdep.cpu.brand_string ("Intel(R) Core(TM) i7-4650U CPU @ 1.70GHz")
	char *machine; // HW_MACHINE ("x86_64")
	char *model; // HW_MODEL ("MacbookAir6,2")
};
#define SYSHW_INIT { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, STR_INIT, STR_INIT, STR_INIT, STR_INIT, STR_INIT };

extern struct syshw getsyshwinfo(void);

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
#define SYSKERN_INIT { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, STR_INIT, STR_INIT, STR_INIT, STR_INIT, STR_INIT, STR_INIT, STR_INIT, STR_INIT, STR_INIT, { 0, 0 }, { 0, 0 } }

extern struct syskern getsyskerninfo(void);

//
// Processes information
//

struct sysproc {
	int pid;
	int parentpid;
	int status;
	int ttydev;
	int priority;
	unsigned int realuid;
	unsigned int effectiveuid;
	char *name;
	char *path;
	char *statustext;
	char *realusername;
	char *effectiveusername;
};
#define SYSPROC_INIT { 0, 0, 0, 0, 0, 0, 0, STR_INIT, STR_INIT, STR_INIT, STR_INIT }

extern struct sysproc *getsysprocinfoall(size_t);
extern struct sysproc *getsysprocinfobypid(int, size_t);
extern struct sysproc *getsysprocinfobypgrp(int, size_t);
extern struct sysproc *getsysprocinfobytty(int, size_t);
extern struct sysproc *getsysprocinfobyuid(int, size_t);
extern struct sysproc *getsysprocinfobyruid(int, size_t);

//
// CPU Utlization information
// 

struct syscpu {
	int count;
	double user;
	double sys;
	double wait;
	double idle;
	double steal;
	double scale;
	double busy;
};
#define SYSCPU_INIT { 0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }

extern struct syscpu getsyscpuinfo(void);

//
// Mem
// 
// vm.swapusage: total = 2048.00M  used = 988.00M  free = 1060.00M  (encrypted)
// vmmeter.h

#endif
