/**
 * sysinfo.c -- Gather system information from *BSD* based systems
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 *
 *   huge shout-out to: man 3 sysctl
 */

#include "sysinfo.h"
#include <assert.h>
#include <libproc.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>

#include <time.h>
#include <stdio.h>
#include <sys/time.h>

/*
 * Get a character string from sysctl (level 2)
 */
static char *stringFromSysctl(int mib0, int mib1)
{
	char *result = NULL;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = 0;
	int complete = 0;
	int error = 0;

	// catch memory errors (ENOMEM),
	// assert to catch program errors
	while(!complete && !error) {
		assert(result == NULL);
		length = 0;

		// get the expected length of the result
		error = sysctl(mib, 2, NULL, &length, NULL, 0);

		// allocate memory for the result
		if(!error) {
			result = malloc(length);
			if (result == NULL) {
				error = 1;
			}
		}

		// get the result
		if(!error) {
			error = sysctl(mib, 2, result, &length, NULL, 0);
			if(!error) {
				complete = 1;
			} else {
				assert(result != NULL);
				free(result);
				result = NULL;
				
				complete = 0;
				error = 0;
			}
		}
	}

	return result;
}

/*
 * Get a character string from sysctlbyname
 */
static char *stringFromSysctlByName(char *name)
{
	char *result = NULL;
	size_t length = 0;
	int complete = 0;
	int error = 0;

	// catch memory errors (ENOMEM),
	// assert to catch program errors
	while(!complete && !error) {
		assert(result == NULL);
		length = 0;

		// get the expected length of the result
		error = sysctlbyname(name, NULL, &length, NULL, 0);

		// allocate memory for the result
		if(!error) {
			result = malloc(length);
			if(result == NULL) {
				error = 1;
			}
		}

		// get the result
		if(!error) {
			error = sysctlbyname(name, result, &length, NULL, 0);
			if(!error) {
				complete = 1;
			} else {
				assert(result != NULL);
				free(result);
				result = NULL;

				complete = 0;
				error = 0;
			}
		}
	}

	return result;
}

/*
 * Get an integer from sysctl (level 2)
 */
static unsigned int intFromSysctl(int mib0, int mib1)
{
	unsigned int result = 0;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = sizeof(result);
	sysctl(mib, 2, &result, &length, NULL, 0);

	return result;
}

/*
 * Get an integer from sysctlbyname
 */
static unsigned int intFromSysctlByName(char *name)
{
	unsigned int result = 0;

	size_t length = sizeof(result);
	sysctlbyname(name, &result, &length, NULL, 0);

	return result;
}

/*
 * Get all hardware information from sysctl
 */
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
	thissys.cpufrequency = intFromSysctlByName("hw.cpufrequency");
	thissys.cpufrequencymin = intFromSysctlByName("hw.cpufrequency_min");
	thissys.cpufrequencymax = intFromSysctlByName("hw.cpufrequency_max");
	thissys.cpucount = intFromSysctlByName("hw.ncpu");
	thissys.cpuactive = intFromSysctlByName("hw.activecpu");
	thissys.physicalcpucount = intFromSysctlByName("hw.physicalcpu");
	thissys.physicalcpumax = intFromSysctlByName("hw.physicalcpu_max");
	thissys.logicalcpucount = intFromSysctlByName("hw.logicalcpu");
	thissys.logicalcpumax = intFromSysctlByName("hw.logicalcpu_max");
	thissys.cpubrand = stringFromSysctlByName("machdep.cpu.brand_string");

	if(thissys.logicalcpucount>thissys.physicalcpucount)
		thissys.hyperthreads=thissys.logicalcpucount/thissys.physicalcpucount;
	else
		thissys.hyperthreads=0;

	return thissys;
}

/*
 * Get all hernel information from sysctl
 */
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
	thissys.osversion = stringFromSysctl(CTL_KERN, KERN_OSVERSION);

	return thissys;
}

/*
 * Convert kinfo_proc data structure into a simple sysproc data structure
 */
struct sysproc *sysprocfromkinfoproc(struct kinfo_proc *processes, int count)
{
	struct sysproc *result = (struct sysproc *)malloc(sizeof(struct sysproc) * (size_t)count);
	
	for (int i = 0; i < count; ++i) {
		struct sysproc currentresult = result[i];
		
		// S* process status
		currentresult.status = processes[i].kp_proc.p_stat;
		switch(currentresult.status){
			case SIDL:
				currentresult.statustext = "IDLE";
				break;
			case SRUN:
				currentresult.statustext = "RUN";
				break;
			case SSLEEP:
				currentresult.statustext = "SLEEP";
				break;
			case SSTOP:
				currentresult.statustext = "STOP";
				break;
			case SZOMB:
				currentresult.statustext = "ZOMB";
				break;
		}		
		// Process identifier.
		currentresult.pid = processes[i].kp_proc.p_pid;
		// Process name
		currentresult.name = processes[i].kp_proc.p_comm;
		// process path
		char path[PROC_PIDPATHINFO_MAXSIZE];
		proc_pidpath(currentresult.pid, path, sizeof(path));
		currentresult.path = path;
		// Process priority.
		currentresult.priority = processes[i].kp_proc.p_priority;
		// parent process id
		currentresult.parentpid = processes[i].kp_eproc.e_ppid;
		// controlling tty dev
		currentresult.ttydev = processes[i].kp_eproc.e_tdev;
		// process credentials, real user id
		currentresult.realuid = processes[i].kp_eproc.e_pcred.p_ruid;
		struct passwd *realuser = getpwuid(currentresult.realuid);
		currentresult.realusername = realuser->pw_name;
		// current credentials, effective user id
		currentresult.effectiveuid = processes[i].kp_eproc.e_ucred.cr_uid;
		struct passwd *effectiveuser = getpwuid(currentresult.effectiveuid);
		currentresult.effectiveusername = effectiveuser->pw_name;
	}

	return result;
}

/*
 * Get all process information from sysctl
 */
struct sysproc *getsysprocinfo(int processinfotype, int criteria, size_t *length)
{
	struct sysproc *result = NULL;
	struct kinfo_proc *processlist = NULL;

	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = processinfotype;
	mib[3] = criteria;
	size_t templength = 0;
	int processcount = 0;
	int complete = 0;
	int error = 0;

	// catch memory errors (ENOMEM),
	// assert to catch program errors
	while(!complete && !error) {
		assert(processlist == NULL);
		templength = 0;

		// get the expected length of the result
		error = sysctl(mib, 4, NULL, &templength, NULL, 0);

		// allocate memory for the result
		if(!error) {
			processlist = malloc(templength);
			if(processlist == NULL) {
				error = 1;
			}
		}

		// get the result
		if(!error) {
			error = sysctl(mib, 4, processlist, &templength, NULL, 0);
		}

		// fill the sysproc struct from the returned information
		if(!error) {
			processcount = templength / sizeof(struct kinfo_proc);
			result = sysprocfromkinfoproc(processlist, processcount);
			complete = 1;
		} else {
			assert(processlist != NULL);
			free(processlist);
			processlist = NULL;

			complete = 0;
			error = 0;
		}
	}

	*length = (size_t)processcount;
	return result;
}

struct sysproc *getsysprocinfoall(size_t length)
{
	return getsysprocinfo(KERN_PROC_ALL, 0, &length);
}

struct sysproc *getsysprocinfobypid(int processid, size_t length)
{
	return getsysprocinfo(KERN_PROC_PID, processid, &length);
}

struct sysproc *getsysprocinfobypgrp(int processgroupid, size_t length)
{
	return getsysprocinfo(KERN_PROC_PGRP, processgroupid, &length);
}

struct sysproc *getsysprocinfobytty(int tty, size_t length)
{
	return getsysprocinfo(KERN_PROC_TTY, tty, &length);
}

struct sysproc *getsysprocinfobyuid(int userid, size_t length)
{
	return getsysprocinfo(KERN_PROC_UID, userid, &length);
}

struct sysproc *getsysprocinfobyruid(int realuserid, size_t length)
{
	return getsysprocinfo(KERN_PROC_RUID, realuserid, &length);
}
