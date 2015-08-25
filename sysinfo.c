/**
 * sysinfo.c -- Gather system information from *BSD* based systems
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 *
 *   huge shout-out to: man 3 sysctl
 */

#include "sysinfo.h"
#include <assert.h>
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
	
	// debug
	char *status;
	// 
	struct tm *nowtm;
	char tmbuf[64];
	time_t nowtime;

	for (int i = 0; i < count; ++i) {
		struct kinfo_proc currentprocess = processes[i];
		struct sysproc currentresult = result[i];

		// struct proc currentproc = currentprocess.kp_proc;
		// struct eproc currenteproc = currentprocess.kp_eproc;

		// currentresult.loginname = currenteproc.e_login;
		
		printf("***** %3d ****************************** \n", i);
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
		printf("* kp_proc.p_stat = %d (%s)\n", currentresult.status, currentresult.statustext);
		
		// Process identifier.
		currentresult.pid = processes[i].kp_proc.p_pid;
		printf("* kp_proc.pid = %d |\n", processes[i].kp_proc.p_pid);
		
		// Process identifier.
		currentresult.name = processes[i].kp_proc.p_comm;
		printf("* kp_proc.p_comm = %s |\n", currentresult.name);
		
		// Process priority.
		currentresult.priority = processes[i].kp_proc.p_priority;
		printf("* kp_proc.p_priority = %d |\n", currentresult.priority);
		
		//
		// nowtime = processes[i].kp_proc.p_rtime.tv_sec;
		// nowtm = localtime(&nowtime);
		// strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
		// printf("* kp_proc.p_pctcpu = (%d) %s|\n", nowtime, tmbuf);

		// parent process id
		currentresult.parentpid = processes[i].kp_eproc.e_ppid;
		printf("* kp_eproc.e_ppid = %d |\n", currentresult.parentpid);

		// controlling tty dev
		currentresult.ttydev = processes[i].kp_eproc.e_tdev;
		printf("* kp_eproc.e_tdev = %d |\n", currentresult.ttydev);

		// process credentials, real user id
		currentresult.realuid = processes[i].kp_eproc.e_pcred.p_ruid;
		struct passwd *realuser = getpwuid(currentresult.realuid);
		currentresult.realusername = realuser->pw_name;
		printf("* kp_eproc.e_pcred.p_ruid = %d (%s)\n", currentresult.realuid, currentresult.realusername);

		// current credentials, effective user id
		currentresult.effectiveuid = processes[i].kp_eproc.e_ucred.cr_uid;
		struct passwd *effectiveuser = getpwuid(currentresult.effectiveuid);
		currentresult.effectiveusername = effectiveuser->pw_name;
		printf("* kp_eproc.e_ucred.cr_uid = %d (%s)\n", currentresult.effectiveuid, currentresult.effectiveusername);


		// struct proc *tmproc = processes[i].kp_eproc.e_paddr;
		// printf(" kp_eproc.e_tdev = %d |\n", tmproc->p_stat);

		// nowtime = processes[i].kp_eproc.e_paddr->p_starttime.tv_sec;
		// nowtm = localtime(&nowtime);
		// strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
		// printf(" p_starttime=%s|\n", tmbuf);
	}
	printf("***** ***** ***** ***** ***** \n");

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
