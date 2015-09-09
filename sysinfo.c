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
struct syshw getsyshwinfo()
{
	struct syshw thissys = SYSHW_INIT;

	thissys.cpufrequency = intFromSysctl(CTL_HW, HW_CPU_FREQ);
	thissys.cpufrequencymin = intFromSysctlByName("hw.cpufrequency_min");
	thissys.cpufrequencymax = intFromSysctlByName("hw.cpufrequency_max");
	thissys.cpucount = intFromSysctlByName("hw.ncpu");
	thissys.cpuactive = intFromSysctlByName("hw.activecpu");
	thissys.physicalcpucount = intFromSysctlByName("hw.physicalcpu");
	thissys.physicalcpumax = intFromSysctlByName("hw.physicalcpu_max");
	thissys.logicalcpucount = intFromSysctlByName("hw.logicalcpu");
	thissys.logicalcpumax = intFromSysctlByName("hw.logicalcpu_max");
	
	thissys.byteorder = intFromSysctl(CTL_HW, HW_BYTEORDER);
	thissys.memorysize = int64FromSysctlByName("hw.memsize");
	thissys.usermemory = intFromSysctl(CTL_HW, HW_USERMEM);
	thissys.pagesize = intFromSysctl(CTL_HW, HW_PAGESIZE);

	thissys.thermalsensor = intFromSysctlByName("machdep.cpu.thermal.sensor");
	thissys.thermallevelcpu = intFromSysctlByName("machdep.xcpm.cpu_thermal_level");
	thissys.thermallevelgpu = intFromSysctlByName("machdep.xcpm.gpu_thermal_level");
	thissys.thermallevelio = intFromSysctlByName("machdep.xcpm.io_thermal_level");

	thissys.architecture = stringFromSysctl(CTL_HW, HW_MACHINE_ARCH);
	thissys.cpuvendor = stringFromSysctlByName("machdep.cpu.vendor"); 
	thissys.cpubrand = stringFromSysctlByName("machdep.cpu.brand_string");
	thissys.machine = stringFromSysctl(CTL_HW, HW_MACHINE);
	thissys.model = stringFromSysctl(CTL_HW, HW_MODEL);

	if(thissys.logicalcpucount>thissys.physicalcpucount) {
		thissys.hyperthreads=thissys.logicalcpucount/thissys.physicalcpucount;
	} else {
		thissys.hyperthreads=0;
	}
	thissys.cpucount = thissys.cpucount / thissys.logicalcpucount;

	return thissys;
}

/*
 * Get all hernel information from sysctl
 */
struct syskern getsyskerninfo()
{
	struct syskern thissys = SYSKERN_INIT;

	thissys.hostid = intFromSysctl(CTL_KERN, KERN_HOSTID);
	thissys.jobcontrol = intFromSysctl(CTL_KERN, KERN_JOB_CONTROL);
	thissys.maxarguments = intFromSysctl(CTL_KERN, KERN_ARGMAX);
	thissys.maxfiles = intFromSysctl(CTL_KERN, KERN_MAXFILES);
	thissys.maxfilespercpu = intFromSysctl(CTL_KERN, KERN_MAXFILESPERPROC);
	thissys.maxprocesses = intFromSysctl(CTL_KERN, KERN_MAXPROC);
	thissys.maxprocessespercpu = intFromSysctl(CTL_KERN, KERN_MAXPROCPERUID);
	thissys.maxvnodes = intFromSysctl(CTL_KERN, KERN_MAXVNODES);
	thissys.maxgroups = intFromSysctl(CTL_KERN, KERN_NGROUPS);
	
	thissys.osdate = intFromSysctl(CTL_KERN, KERN_OSRELDATE);
	thissys.osrevision = intFromSysctl(CTL_KERN, KERN_OSREV);
	thissys.posixversion = intFromSysctl(CTL_KERN, KERN_POSIX1);
	thissys.securitylevel = intFromSysctl(CTL_KERN, KERN_SECURELVL);
	thissys.updateinterval = intFromSysctl(CTL_KERN, KERN_UPDATEINTERVAL);

	thissys.corecount = intFromSysctlByName("machdep.cpu.core_count");

	thissys.ostype = stringFromSysctl(CTL_KERN, KERN_OSTYPE);
	thissys.osrelease = stringFromSysctl(CTL_KERN, KERN_OSRELEASE);
	thissys.osversion = stringFromSysctl(CTL_KERN, KERN_OSVERSION);
	thissys.version = stringFromSysctl(CTL_KERN, KERN_VERSION);
	thissys.bootfile = stringFromSysctl(CTL_KERN, KERN_BOOTFILE);
	thissys.hostname = stringFromSysctl(CTL_KERN, KERN_HOSTNAME);
	thissys.domainname = stringFromSysctl(CTL_KERN, KERN_NISDOMAINNAME);

	thissys.boottime = timevalFromSysctl(CTL_KERN, KERN_BOOTTIME);
	// now store it as a string for quick printing
	time_t timet = thissys.boottime.tv_sec;
	struct tm *ptm = localtime(&timet);
	char boottimestring[64];
	strftime(boottimestring, sizeof(boottimestring), DATE_FORMAT, ptm);
	thissys.boottimestring = boottimestring;

	// struct timeval tv;
	// gettimeofday(&tv, NULL);
	// thissys.uptime = sinfo.uptime;
	// timet = thissys.uptime.tv_sec - thissys.boottime.tv_sec;
	// ptm = localtime(&timet);
	// char uptimestring[64];
	// strftime(uptimestring, sizeof(uptimestring), "%Y-%m-%d %H:%M:%S", ptm);
	// thissys.uptimestring = uptimestring;

	return thissys;
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

			thisres.avgpercentuser += thisres.cpus[cpuno].percentuser;
			thisres.avgpercentsys += thisres.cpus[cpuno].percentsys;
			thisres.avgpercentidle += thisres.cpus[cpuno].percentidle;
			thisres.avgpercentnice += thisres.cpus[cpuno].percentnice;
		}

		thisres.avgpercentuser /= thisres.cpucount;
		thisres.avgpercentsys /= thisres.cpucount;
		thisres.avgpercentidle /= thisres.cpucount;
		thisres.avgpercentnice /= thisres.cpucount;
	}

	*inres = thisres;
}

//
// Processes information
//

/*
 * Convert kinfo_proc data structure into a simple sysproc data structure
 */
static struct sysproc *sysprocfromkinfoproc(struct kinfo_proc *processes, int count)
{
	struct sysproc *result = (struct sysproc *)malloc(sizeof(struct sysproc) * (size_t)count);
	
	int error = 0;
	struct rusage_info_v3 rusage;
	double total = 0;
	// int totalutime = 0;
	
	for (int i = 0; i < count; ++i) {
		// Process identifier.
		result[i].pid = processes[i].kp_proc.p_pid;
		// Process group identifier.
		result[i].pgid = processes[i].kp_eproc.e_pgid;
		// parent process id
		result[i].parentpid = processes[i].kp_eproc.e_ppid;
		// S* process status
		result[i].status = processes[i].kp_proc.p_stat;
		// controlling tty dev
		result[i].ttydev = processes[i].kp_eproc.e_tdev;
		// Process priority.
		result[i].priority = processes[i].kp_proc.p_priority;
		// process credentials, real user id
		result[i].realuid = processes[i].kp_eproc.e_pcred.p_ruid;
		// current credentials, effective user id
		result[i].effectiveuid = processes[i].kp_eproc.e_ucred.cr_uid;
		// Process name
		result[i].name = processes[i].kp_proc.p_comm;
		// process path
		char path[PROC_PIDPATHINFO_MAXSIZE];
		proc_pidpath(result[i].pid, path, sizeof(path));
		result[i].path = path;
		// status text
		switch(result[i].status){
			case SIDL:
				result[i].statustext = "IDLE";
				break;
			case SRUN:
				result[i].statustext = "RUN";
				break;
			case SSLEEP:
				result[i].statustext = "SLEEP";
				break;
			case SSTOP:
				result[i].statustext = "STOP";
				break;
			case SZOMB:
				result[i].statustext = "ZOMB";
				break;
		}		
		// real username
		struct passwd *realuser = getpwuid(result[i].realuid);
		result[i].realusername = realuser->pw_name;
		// effectiver user, name
		struct passwd *effectiveuser = getpwuid(processes[i].kp_eproc.e_ucred.cr_uid);
		result[i].effectiveusername = effectiveuser->pw_name;
		// setlogin() name
		result[i].setloginname = processes[i].kp_eproc.e_login;

		// get additional info not available from sysctl
		error = proc_pid_rusage(result[i].pid, RUSAGE_INFO_V3, (rusage_info_t *)&rusage);
		if(!error) {
			result[i].utime = rusage.ri_user_time;
			result[i].stime = rusage.ri_system_time;
			result[i].oldtotaltime = result[i].totaltime;
			result[i].totaltime = rusage.ri_user_time + rusage.ri_system_time;
			result[i].billedtime = rusage.ri_billed_system_time;
			result[i].idlewakeups = rusage.ri_pkg_idle_wkups;

			result[i].wiredmem = rusage.ri_wired_size;
			result[i].residentmem = rusage.ri_resident_size;
			result[i].physicalmem = rusage.ri_phys_footprint;

			result[i].diskior = rusage.ri_diskio_bytesread;
			result[i].diskiow = rusage.ri_diskio_byteswritten;

			total += (double)result[i].totaltime;
		} else {
			result[i].name = strerror(errno);
			result[i].stime = 0;
			result[i].oldtotaltime = 0;
			result[i].totaltime = 0;
			result[i].billedtime = 0;
			result[i].idlewakeups = 0;

			result[i].wiredmem = 0;
			result[i].residentmem = 0;
			result[i].physicalmem = 0;

			result[i].diskior = 0;
			result[i].diskiow = 0;
		}
	}

	for (int i = 0; i < count; ++i) {
		result[i].percentage = (double)(result[i].totaltime - result[i].oldtotaltime) / total * 100;
	}

	return result;
}

/*
 * Get all process information from sysctl
 */
static struct sysproc *getsysprocinfo(int processinfotype, int criteria, size_t *length)
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
			processcount = (unsigned int)templength / sizeof(struct kinfo_proc);
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

struct sysproc *getsysprocinfoall(size_t *length)
{
	return getsysprocinfo(KERN_PROC_ALL, 0, length);
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
