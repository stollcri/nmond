/**
 * sysinfo.c -- Gather system information from *BSD* based systems
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

#include "sysinfo.h"
#include <assert.h>
#include <errno.h>
#include <libproc.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <mach/host_info.h>
#include <mach/mach_host.h>
#include <sys/sysctl.h>
#include <sys/types.h>

#include <time.h>
#include <stdio.h>
#include <sys/time.h>

#include "sysctlhelper.h"

/*
 * Get all hardware information from sysctl
 */
void getsyshwinfo(struct syshw *hw)
{
	hw->cpufrequency = intFromSysctl(CTL_HW, HW_CPU_FREQ);
	hw->cpufrequencymin = intFromSysctlByName("hw.cpufrequency_min");
	hw->cpufrequencymax = intFromSysctlByName("hw.cpufrequency_max");
	hw->cpucount = intFromSysctlByName("hw.ncpu");
	hw->cpuactive = intFromSysctlByName("hw.activecpu");
	hw->physicalcpucount = intFromSysctlByName("hw.physicalcpu");
	hw->physicalcpumax = intFromSysctlByName("hw.physicalcpu_max");
	hw->logicalcpucount = intFromSysctlByName("hw.logicalcpu");
	hw->logicalcpumax = intFromSysctlByName("hw.logicalcpu_max");
	
	hw->byteorder = intFromSysctl(CTL_HW, HW_BYTEORDER);
	hw->memorysize = int64FromSysctlByName("hw.memsize");
	hw->usermemory = intFromSysctl(CTL_HW, HW_USERMEM);
	hw->pagesize = intFromSysctl(CTL_HW, HW_PAGESIZE);

	hw->thermalsensor = intFromSysctlByName("machdep.cpu.thermal.sensor");
	hw->thermallevelcpu = intFromSysctlByName("machdep.xcpm.cpu_thermal_level");
	hw->thermallevelgpu = intFromSysctlByName("machdep.xcpm.gpu_thermal_level");
	hw->thermallevelio = intFromSysctlByName("machdep.xcpm.io_thermal_level");

	hw->architecture = stringFromSysctl(CTL_HW, HW_MACHINE_ARCH);
	hw->cpuvendor = stringFromSysctlByName("machdep.cpu.vendor"); 
	hw->cpubrand = stringFromSysctlByName("machdep.cpu.brand_string");
	hw->machine = stringFromSysctl(CTL_HW, HW_MACHINE);
	hw->model = stringFromSysctl(CTL_HW, HW_MODEL);

	if(hw->logicalcpucount > hw->physicalcpucount) {
		hw->hyperthreads=hw->logicalcpucount/hw->physicalcpucount;
	} else {
		hw->hyperthreads=0;
	}
	hw->cpucount = hw->cpucount / hw->logicalcpucount;
}

/*
 * Get all hernel information from sysctl
 */
void getsyskerninfo(struct syskern *kern)
{
	// struct syskern thissys = SYSKERN_INIT;

	kern->hostid = intFromSysctl(CTL_KERN, KERN_HOSTID);
	kern->jobcontrol = intFromSysctl(CTL_KERN, KERN_JOB_CONTROL);
	kern->maxarguments = intFromSysctl(CTL_KERN, KERN_ARGMAX);
	kern->maxfiles = intFromSysctl(CTL_KERN, KERN_MAXFILES);
	kern->maxfilespercpu = intFromSysctl(CTL_KERN, KERN_MAXFILESPERPROC);
	kern->maxprocesses = intFromSysctl(CTL_KERN, KERN_MAXPROC);
	kern->maxprocessespercpu = intFromSysctl(CTL_KERN, KERN_MAXPROCPERUID);
	kern->maxvnodes = intFromSysctl(CTL_KERN, KERN_MAXVNODES);
	kern->maxgroups = intFromSysctl(CTL_KERN, KERN_NGROUPS);
	
	kern->osdate = intFromSysctl(CTL_KERN, KERN_OSRELDATE);
	kern->osrevision = intFromSysctl(CTL_KERN, KERN_OSREV);
	kern->posixversion = intFromSysctl(CTL_KERN, KERN_POSIX1);
	kern->securitylevel = intFromSysctl(CTL_KERN, KERN_SECURELVL);
	kern->updateinterval = intFromSysctl(CTL_KERN, KERN_UPDATEINTERVAL);

	kern->corecount = intFromSysctlByName("machdep.cpu.core_count");

	kern->ostype = stringFromSysctl(CTL_KERN, KERN_OSTYPE);
	kern->osrelease = stringFromSysctl(CTL_KERN, KERN_OSRELEASE);
	kern->osversion = stringFromSysctl(CTL_KERN, KERN_OSVERSION);
	kern->version = stringFromSysctl(CTL_KERN, KERN_VERSION);
	kern->bootfile = stringFromSysctl(CTL_KERN, KERN_BOOTFILE);
	kern->hostname = stringFromSysctl(CTL_KERN, KERN_HOSTNAME);
	kern->domainname = stringFromSysctl(CTL_KERN, KERN_NISDOMAINNAME);

	kern->boottime = timevalFromSysctl(CTL_KERN, KERN_BOOTTIME);
	// now store it as a string for quick printing
	time_t timet = kern->boottime.tv_sec;
	struct tm *ptm = localtime(&timet);
	char boottimestring[64];
	strftime(boottimestring, sizeof(boottimestring), DATE_TIME_FORMAT, ptm);
	kern->boottimestring = boottimestring;

	// struct timeval tv;
	// gettimeofday(&tv, NULL);
	// kern->uptime = sinfo.uptime;
	// timet = kern->uptime.tv_sec - kern->boottime.tv_sec;
	// ptm = localtime(&timet);
	// char uptimestring[64];
	// strftime(uptimestring, sizeof(uptimestring), "%Y-%m-%d %H:%M:%S", ptm);
	// kern->uptimestring = uptimestring;
}

//
// System Resource (CPU Utlization) information
// 

void getsysresinfo(struct sysres *inres)
{
	struct sysres thisres = *inres;

	int error = 0;
	int newcpucount = 4; // TODO: FIXME Below

	int mib[2];
	mib[0] = CTL_VM;
	mib[1] = VM_LOADAVG;
	struct loadavg thisload;
	size_t length = sizeof(thisload);
	error = sysctl(mib, 2, &thisload, &length, NULL, 0);
	if(!error) {
		newcpucount = (int)intFromSysctlByName("hw.ncpu");
		thisres.loadavg1 = thisload.ldavg[0] / thisload.fscale;
		thisres.loadavg5 = thisload.ldavg[1] / thisload.fscale;
		thisres.loadavg15 = thisload.ldavg[2] / thisload.fscale;
	}

	// does the CPU count really change? Set it on the first pass
	if (!thisres.cpucount || (newcpucount != thisres.cpucount)) {
		thisres.cpucount = newcpucount;
		thisres.cpus = (struct sysrescpu *)calloc(sizeof(struct sysrescpu), (size_t)thisres.cpucount);

		int physcpu = (int)intFromSysctlByName("hw.physicalcpu");
		int logicpu = (int)intFromSysctlByName("hw.logicalcpu");
		thisres.cpuhyperthreadmod = logicpu / physcpu;
	}

	double total = 0;
	natural_t cpuCount;
	host_info_t hostinfo;
	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
	error = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpuCount, &hostinfo, &count);
	if (!error) {
		processor_cpu_load_info_data_t* r_load = (processor_cpu_load_info_data_t*)hostinfo;

		thisres.avgpercentuser = 0;
		thisres.avgpercentsys = 0;
		thisres.avgpercentidle = 0;
		thisres.avgpercentnice = 0;

		for (int cpuno = 0; cpuno < thisres.cpucount; ++cpuno) {
			thisres.cpus[cpuno].olduser = thisres.cpus[cpuno].user;
			thisres.cpus[cpuno].oldsys = thisres.cpus[cpuno].sys;
			thisres.cpus[cpuno].oldidle = thisres.cpus[cpuno].idle;
			thisres.cpus[cpuno].oldnice = thisres.cpus[cpuno].nice;
			thisres.cpus[cpuno].oldtotal = thisres.cpus[cpuno].total;

			thisres.cpus[cpuno].user = (int)r_load[cpuno].cpu_ticks[CPU_STATE_USER];
			thisres.cpus[cpuno].sys = (int)r_load[cpuno].cpu_ticks[CPU_STATE_SYSTEM];
			thisres.cpus[cpuno].idle = (int)r_load[cpuno].cpu_ticks[CPU_STATE_IDLE];
			thisres.cpus[cpuno].nice = (int)r_load[cpuno].cpu_ticks[CPU_STATE_NICE];
			thisres.cpus[cpuno].total = 
				(int)r_load[cpuno].cpu_ticks[CPU_STATE_USER] + (int)r_load[cpuno].cpu_ticks[CPU_STATE_SYSTEM] + 
				(int)r_load[cpuno].cpu_ticks[CPU_STATE_IDLE] + (int)r_load[cpuno].cpu_ticks[CPU_STATE_NICE];

			total = (double)(thisres.cpus[cpuno].total - thisres.cpus[cpuno].oldtotal);

			thisres.cpus[cpuno].percentuser = 
				(double)(thisres.cpus[cpuno].user - thisres.cpus[cpuno].olduser) / total * 100;
			thisres.cpus[cpuno].percentsys = 
				(double)(thisres.cpus[cpuno].sys - thisres.cpus[cpuno].oldsys) / total * 100;
			thisres.cpus[cpuno].percentidle = 
				(double)(thisres.cpus[cpuno].idle - thisres.cpus[cpuno].oldidle) / total * 100;
			thisres.cpus[cpuno].percentnice = 
				(double)(thisres.cpus[cpuno].nice - thisres.cpus[cpuno].oldnice) / total * 100;

			if(COUNT_HYPERTHREADS_IN_CPU_AVG) {
				thisres.avgpercentuser += thisres.cpus[cpuno].percentuser;
				thisres.avgpercentsys += thisres.cpus[cpuno].percentsys;
				thisres.avgpercentidle += thisres.cpus[cpuno].percentidle;
				thisres.avgpercentnice += thisres.cpus[cpuno].percentnice;
			} else {
				if(!(cpuno % thisres.cpuhyperthreadmod)) {
					thisres.avgpercentuser += thisres.cpus[cpuno].percentuser;
					thisres.avgpercentsys += thisres.cpus[cpuno].percentsys;
					thisres.avgpercentidle += thisres.cpus[cpuno].percentidle;
					thisres.avgpercentnice += thisres.cpus[cpuno].percentnice;
				}
			}
		}

		thisres.percentallcpu = (thisres.avgpercentuser + thisres.avgpercentsys + thisres.avgpercentnice) / 100;
		// thisres.percentallcpu = (thisres.avgpercentuser + thisres.avgpercentsys + thisres.avgpercentnice) / (thisres.cpucount * 100);
		if(COUNT_HYPERTHREADS_IN_CPU_AVG) {
			thisres.avgpercentuser /= thisres.cpucount;
			thisres.avgpercentsys /= thisres.cpucount;
			thisres.avgpercentidle /= thisres.cpucount;
			thisres.avgpercentnice /= thisres.cpucount;
			// thisres.percentallcpu = (thisres.avgpercentuser + thisres.avgpercentsys + thisres.avgpercentnice) * thisres.cpucount / 100;
		} else {
			thisres.avgpercentuser /= (thisres.cpucount / thisres.cpuhyperthreadmod);
			thisres.avgpercentsys /= (thisres.cpucount / thisres.cpuhyperthreadmod);
			thisres.avgpercentidle /= (thisres.cpucount / thisres.cpuhyperthreadmod);
			thisres.avgpercentnice /= (thisres.cpucount / thisres.cpuhyperthreadmod);
			// thisres.percentallcpu = (thisres.avgpercentuser + thisres.avgpercentsys + thisres.avgpercentnice) * (thisres.cpucount / thisres.cpuhyperthreadmod) * 100;
		}
		// thisres.percentallcpu = (thisres.avgpercentuser + thisres.avgpercentsys + thisres.avgpercentnice) / 100;
	}

	*inres = thisres;
}

//
// Processes information
//

/*
 * Convert kinfo_proc data structure into a simple sysproc data structure
 */
static void sysprocfromkinfoproc(struct kinfo_proc *processes, int count, struct sysproc **procsin, struct hashitem **hashtable, double cpupercent)
{
	if(*procsin == NULL) {
		*procsin = (struct sysproc *)malloc(sizeof(struct sysproc) * (size_t)count);
	}
	struct sysproc *procs = *procsin;

	int error = 0;
	struct rusage_info_v3 rusage;
	uint64_t total = 0;
	uint64_t oldtotal = 0;
	unsigned int oldtotaltime = 0;

	time_t timet = 0;
	struct tm *ptm = NULL;
	// char boottimestring[8];

	for (int i = 0; i < count; ++i) {
		// Process identifier.
		procs[i].pid = processes[i].kp_proc.p_pid;
		// Process group identifier.
		procs[i].pgid = processes[i].kp_eproc.e_pgid;
		// parent process id
		procs[i].parentpid = processes[i].kp_eproc.e_ppid;
		// S* process status
		procs[i].status = processes[i].kp_proc.p_stat;
		// controlling tty dev
		procs[i].ttydev = processes[i].kp_eproc.e_tdev;
		// Process priority.
		procs[i].priority = processes[i].kp_proc.p_priority;
		// process credentials, real user id
		procs[i].realuid = processes[i].kp_eproc.e_pcred.p_ruid;
		// current credentials, effective user id
		procs[i].effectiveuid = processes[i].kp_eproc.e_ucred.cr_uid;
		// Process name
		procs[i].name = processes[i].kp_proc.p_comm;
		// process path
		char path[PROC_PIDPATHINFO_MAXSIZE];
		proc_pidpath(procs[i].pid, path, sizeof(path));
		procs[i].path = path;
		// status text
		switch(procs[i].status){
			case SIDL:
				procs[i].statustext = "IDLE";
				break;
			case SRUN:
				procs[i].statustext = "RUN";
				break;
			case SSLEEP:
				procs[i].statustext = "SLEEP";
				break;
			case SSTOP:
				procs[i].statustext = "STOP";
				break;
			case SZOMB:
				procs[i].statustext = "ZOMB";
				break;
		}		
		// real username
		struct passwd *realuser = getpwuid(procs[i].realuid);
		procs[i].realusername = realuser->pw_name;
		// effectiver user, name
		struct passwd *effectiveuser = getpwuid(processes[i].kp_eproc.e_ucred.cr_uid);
		procs[i].effectiveusername = effectiveuser->pw_name;
		// setlogin() name
		procs[i].setloginname = processes[i].kp_eproc.e_login;

		// get additional info not available from sysctl
		error = proc_pid_rusage(procs[i].pid, RUSAGE_INFO_V3, (rusage_info_t *)&rusage);
		if(!error) {
			procs[i].utime = rusage.ri_user_time;
			procs[i].stime = rusage.ri_system_time;
			procs[i].totaltime = rusage.ri_user_time + rusage.ri_system_time;
			procs[i].billedtime = rusage.ri_billed_system_time;
			procs[i].idlewakeups = rusage.ri_pkg_idle_wkups;

			procs[i].wiredmem = rusage.ri_wired_size;
			procs[i].residentmem = rusage.ri_resident_size;
			procs[i].physicalmem = rusage.ri_phys_footprint;

			procs[i].diskior = rusage.ri_diskio_bytesread;
			procs[i].diskiow = rusage.ri_diskio_byteswritten;

			// now store it as a string for quick printing
			// char boottimestring[9];
			// timet = procs[i].totaltime;
			// ptm = localtime(&timet);
			// strftime(boottimestring, sizeof(boottimestring), TIME_FORMAT, ptm);

			// char *timestring = malloc(9);
			// strcpy(timestring, boottimestring);
			// procs[i].timestring = timestring;
		} else {
			procs[i].name = "";//strerror(errno);
			procs[i].utime = 0;
			procs[i].stime = 0;
			procs[i].totaltime = 0;
			procs[i].billedtime = 0;
			procs[i].idlewakeups = 0;

			procs[i].wiredmem = 0;
			procs[i].residentmem = 0;
			procs[i].physicalmem = 0;

			procs[i].diskior = 0;
			procs[i].diskiow = 0;
		}

		// TODO: add/update PIDs to hashtable
		oldtotaltime = hashtget(*hashtable, procs[i].pid);
		if(oldtotaltime == -1) {
			hashtadd(*hashtable, procs[i].pid, procs[i].totaltime);
			procs[i].lasttotaltime = procs[i].totaltime;
		} else {
			hashtset(*hashtable, procs[i].pid, procs[i].totaltime);
			procs[i].lasttotaltime = oldtotaltime;
		}

		oldtotal = total;
		total += (procs[i].totaltime - procs[i].lasttotaltime);
		if(total < oldtotal) {
			// TODO: handle integer overflows
		}
	}

	for (int i = 0; i < count; ++i) {
		procs[i].percentage = (((double)(procs[i].totaltime - procs[i].lasttotaltime) / total) * 100) * cpupercent;
	}
	**procsin = *procs;
}

/*
 * Get all process information from sysctl
 */
static void getsysprocinfo(int processinfotype, int criteria, size_t *length, struct sysproc **procs, struct hashitem **hashtable, double cpupercent)
{
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
			processcount = (unsigned int)templength / sizeof(struct kinfo_proc);
			sysprocfromkinfoproc(processlist, processcount, procs, hashtable, cpupercent);
			free(processlist);
			processlist = NULL;
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
}

void getsysprocinfoall(size_t *length, struct sysproc **procs, struct hashitem **hashtable, double cpupercent)
{
	return getsysprocinfo(KERN_PROC_ALL, 0, length, procs, hashtable, cpupercent);
}
/*
struct sysproc getsysprocinfobypid(int processid, size_t length)
{
	struct sysres thisres = SYSRES_INIT;
	return getsysprocinfo(KERN_PROC_PID, processid, &length);
}

struct sysproc getsysprocinfobypgrp(int processgroupid, size_t length)
{
	struct sysres thisres = SYSRES_INIT;
	return getsysprocinfo(KERN_PROC_PGRP, processgroupid, &length);
}

struct sysproc getsysprocinfobytty(int tty, size_t length)
{
	struct sysres thisres = SYSRES_INIT;
	return getsysprocinfo(KERN_PROC_TTY, tty, &length);
}

struct sysproc getsysprocinfobyuid(int userid, size_t length)
{
	struct sysres thisres = SYSRES_INIT;
	return getsysprocinfo(KERN_PROC_UID, userid, &length);
}

struct sysproc getsysprocinfobyruid(int realuserid, size_t length)
{
	struct sysres thisres = SYSRES_INIT;
	return getsysprocinfo(KERN_PROC_RUID, realuserid, &length);
}
*/
