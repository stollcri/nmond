#ifndef SYSINFO_H
#define SYSINFO_H

struct sys {
	// CTL_HW
	char *machine; // HW_MACHINE ("x86_64")
	char *model; // HW_MODEL ("MacbookAir6,2")
	int byteorder; // HW_BYTEORDER
	int memorysize; // HW_MEMSIZE (total memory, 64bit int)
	int usermemory; // HW_USERMEM (non-kernel memory)
	int pagesize; // HW_PAGESIZE
	int floatingpoint; // HW_FLOATINGPOINT (hardware support)
	char *architecture; // HW_MACHINE_ARCH
	int cpufrequency; // HW_CPU_FREQ
};

extern struct sys newSys();

extern struct sys getsysinfo();

#endif