/*
 * sysinfo.c -- Gather system information from *BSD* based systems
 *  Christopher Stoll (https://github.com/stollcri), 2015
 *
 *   huge shout-out to: man 3 sysctl
 */

#include "sysinfo.h"
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>

static inline char *stringFromSysctl(int mib0, int mib1)
{
	char *result = NULL;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t len = 0;

	sysctl(mib, 2, NULL, &len, NULL, 0);
	result = malloc(len);
	sysctl(mib, 2, result, &len, NULL, 0);

	return result;
}

static inline unsigned int intFromSysctl(int mib0, int mib1)
{
	unsigned int result = 0;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t len = sizeof(result);

	sysctl(mib, 2, &result, &len, NULL, 0);

	return result;
}

static inline unsigned int intFromSysctlByName(char *name)
{
	unsigned int result = 0;

	int count;
	size_t len = sizeof(result);
	sysctlbyname(name, &count, &len, NULL, 0);

	return result;
}

struct syshw getsyshwinfo()
{
	struct syshw thissys = SYSHW_INIT;

	thissys.machine = stringFromSysctl(CTL_HW, HW_MACHINE);
	thissys.model = stringFromSysctl(CTL_HW, HW_MODEL);
	thissys.byteorder = intFromSysctl(CTL_HW, HW_BYTEORDER);
	thissys.memorysize = intFromSysctl(CTL_HW, HW_MEMSIZE);
	thissys.usermemory = intFromSysctl(CTL_HW, HW_USERMEM);
	thissys.pagesize = intFromSysctl(CTL_HW, HW_PAGESIZE);
	//thissys.floatingpoint = intFromSysctl(CTL_HW, HW_FLOATINGPOINT);
	thissys.architecture = stringFromSysctl(CTL_HW, HW_MACHINE_ARCH);
	thissys.cpufrequency = intFromSysctl(CTL_HW, HW_CPU_FREQ);
	thissys.physicalcpucount = intFromSysctlByName("hw.physicalcpu");
	thissys.physicalcpumax = intFromSysctlByName("hw.physicalcpu_max");
	thissys.logicalcpucount = intFromSysctlByName("hw.logicalcpu");
	thissys.logicalcpumax = intFromSysctlByName("hw.logicalcpu_max");

	return thissys;
}

struct syskern getsyskerninfo()
{
	struct syskern thissys = SYSKERN_INIT;

	thissys.maxarguments = intFromSysctl(CTL_KERN, KERN_ARGMAX);
	thissys.bootfile = stringFromSysctl(CTL_KERN, KERN_BOOTFILE);
	thissys.hostid = intFromSysctl(CTL_KERN, KERN_HOSTID);
	thissys.hostname = stringFromSysctl(CTL_KERN, KERN_HOSTNAME);
	thissys.jobcontrol = intFromSysctl(CTL_KERN, KERN_JOB_CONTROL);
	thissys.maxfiles = intFromSysctl(CTL_KERN, KERN_MAXFILES);
	thissys.maxfilespercpu = intFromSysctl(CTL_KERN, KERN_MAXFILESPERPROC);
	thissys.maxprocesses = intFromSysctl(CTL_KERN, KERN_MAXPROC);
	thissys.maxprocessespercpu = intFromSysctl(CTL_KERN, KERN_MAXPROCPERUID);
	thissys.maxvnodes = intFromSysctl(CTL_KERN, KERN_MAXVNODES);
	thissys.maxgroups = intFromSysctl(CTL_KERN, KERN_NGROUPS);
	thissys.domainname = stringFromSysctl(CTL_KERN, KERN_NISDOMAINNAME);
	thissys.osdate = intFromSysctl(CTL_KERN, KERN_OSRELDATE);
	thissys.osrelease = stringFromSysctl(CTL_KERN, KERN_OSRELEASE);
	thissys.osrevision = intFromSysctl(CTL_KERN, KERN_OSREV);
	thissys.ostype = stringFromSysctl(CTL_KERN, KERN_OSTYPE);
	thissys.posixversion = intFromSysctl(CTL_KERN, KERN_POSIX1);
	//thissys.quantum = intFromSysctl(CTL_KERN, KERN_QUANTUM);
	thissys.securitylevel = intFromSysctl(CTL_KERN, KERN_SECURELVL);
	thissys.updateinterval = intFromSysctl(CTL_KERN, KERN_UPDATEINTERVAL);
	thissys.version = stringFromSysctl(CTL_KERN, KERN_VERSION);

	return thissys;
}

struct sysproc *getsysprocinfo()
{
	//
}