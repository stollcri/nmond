#ifndef SYSINFO_H
#define SYSINFO_H

//
// Hardware based information
// 

struct syshw { // CTL_HW
	unsigned int byteorder; // HW_BYTEORDER
	unsigned int memorysize; // HW_MEMSIZE (total memory, 64bit int)
	unsigned int usermemory; // HW_USERMEM (non-kernel memory)
	unsigned int pagesize; // HW_PAGESIZE
	//int floatingpoint; // HW_FLOATINGPOINT (hardware support)
	unsigned int cpufrequency; // HW_CPU_FREQ
	unsigned int physicalcpucount; // hw.physicalcpu
	unsigned int physicalcpumax; // hw.physicalcpu_max
	unsigned int logicalcpucount; // hw.logicalcpu
	unsigned int logicalcpumax; // hw.logicalcpu_max
	char *machine; // HW_MACHINE ("x86_64")
	char *model; // HW_MODEL ("MacbookAir6,2")
	char *architecture; // HW_MACHINE_ARCH
};
#define SYSHW_INIT { 0, 0, 0, 0, 0, 0, 0, 0, 0, "-", "-", "-" };

extern struct syshw getsyshwinfo(void);

//
// Kernel based information
// 

struct syskern { // CTL_KERN
	unsigned int maxarguments; // KERN_ARGMAX
	unsigned int hostid; // KERN_HOSTID
	unsigned int jobcontrol; // KERN_JOB_CONTROL
	unsigned int maxfiles; // KERN_MAXFILES
	unsigned int maxfilespercpu; // KERN_MAXFILESPERPROC
	unsigned int maxprocesses; // KERN_MAXPROC
	unsigned int maxprocessespercpu; // KERN_MAXPROCPERUID
	unsigned int maxvnodes; // KERN_MAXVNODES
	unsigned int maxgroups; // KERN_NGROUPS
	unsigned int osdate; // KERN_OSRELDATE
	unsigned int osrevision; // KERN_OSREV
	unsigned int posixversion; // KERN_POSIX1
	//int quantum; // KERN_QUANTUM
	// int savedids; // KERN_SAVED_IDS
	unsigned int securitylevel; // KERN_SECURELVL
	unsigned int updateinterval; // KERN_UPDATEINTERVAL
	char *bootfile; // KERN_BOOTFILE
	char *hostname; // KERN_HOSTNAME
	char *domainname; // KERN_NISDOMAINNAME
	char *osrelease; // KERN_OSRELEASE
	char *ostype; // KERN_OSTYPE
	char *version; // KERN_VERSION
	//struct timeval boottime; // KERN_BOOTTIME
	//struct clockinfo clockrate; // KERN_CLOCKRATE
	//struct file filetable; // KERN_FILE
	// struct vnode vnodetable; // KERN_VNODE
	// KERN_PROC
	// KERN_PROF
};
#define SYSKERN_INIT { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "-", "-", "-", "-", "-", "-" }

extern struct syskern getsyskerninfo(void);
extern struct sysproc *getsysprocinfo(void);

#endif
