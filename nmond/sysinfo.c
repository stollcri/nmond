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
#include <ifaddrs.h>
#include <libproc.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <mach/host_info.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <sys/ioctl.h>
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

	free(hw->architecture);
	hw->architecture = stringFromSysctl(CTL_HW, HW_MACHINE_ARCH);

	free(hw->cpuvendor);
	hw->cpuvendor = stringFromSysctlByName("machdep.cpu.vendor");

	free(hw->cpubrand);
	hw->cpubrand = stringFromSysctlByName("machdep.cpu.brand_string");

	free(hw->machine);
	hw->machine = stringFromSysctl(CTL_HW, HW_MACHINE);

	free(hw->model);
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

    free(kern->ostype);
	kern->ostype = stringFromSysctl(CTL_KERN, KERN_OSTYPE);

    free(kern->osrelease);
    kern->osrelease = stringFromSysctl(CTL_KERN, KERN_OSRELEASE);

    free(kern->osversion);
    kern->osversion = stringFromSysctl(CTL_KERN, KERN_OSVERSION);

    free(kern->version);
    kern->version = stringFromSysctl(CTL_KERN, KERN_VERSION);

    free(kern->bootfile);
    kern->bootfile = stringFromSysctl(CTL_KERN, KERN_BOOTFILE);

    free(kern->hostname);
    kern->hostname = stringFromSysctl(CTL_KERN, KERN_HOSTNAME);

    free(kern->domainname);
    kern->domainname = stringFromSysctl(CTL_KERN, KERN_NISDOMAINNAME);

	// free(kern->boottime);
	kern->boottime = timevalFromSysctl(CTL_KERN, KERN_BOOTTIME);

	free(kern->boottimestring);
	kern->boottimestring = timeStringFromTimestamp(kern->boottime.tv_sec, DATE_TIME_FORMAT);

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
		inres->loadavg1 = thisload.ldavg[0] / thisload.fscale;
		inres->loadavg5 = thisload.ldavg[1] / thisload.fscale;
		inres->loadavg15 = thisload.ldavg[2] / thisload.fscale;
	}

	// does the CPU count really change? Set it on the first pass
	if (!inres->cpucount || (newcpucount != inres->cpucount)) {
		inres->cpucount = newcpucount;
		inres->cpus = (struct sysrescpu *)calloc(sizeof(struct sysrescpu), (size_t)inres->cpucount);
		if(inres->cpus == NULL) {
			// TODO: handle memory allocation failure
		}

		for (int cpuno = 0; cpuno < inres->cpucount; ++cpuno) {
			inres->cpus[cpuno].olduser = 0;
			inres->cpus[cpuno].oldsys = 0;
			inres->cpus[cpuno].oldidle = 0;
			inres->cpus[cpuno].oldnice = 0;
			inres->cpus[cpuno].oldtotal = 0;
		}

		int physcpu = (int)intFromSysctlByName("hw.physicalcpu");
		int logicpu = (int)intFromSysctlByName("hw.logicalcpu");
		inres->cpuhyperthreadmod = logicpu / physcpu;
	}

	double total = 0;
	natural_t cpuCount;
	host_info_t hostinfo;
	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
	// TODO: consider using host_statistics() instead?
	//  -- http://stackoverflow.com/questions/20471920/how-to-get-total-cpu-idle-time-in-objective-c-c-on-os-x
	error = host_processor_info(mach_host_self(), PROCESSOR_CPU_LOAD_INFO, &cpuCount, &hostinfo, &count);

	if (!error) {
		processor_cpu_load_info_data_t *r_load = (processor_cpu_load_info_data_t*)hostinfo;

		inres->avgpercentuser = 0;
		inres->avgpercentsys = 0;
		inres->avgpercentidle = 0;
		inres->avgpercentnice = 0;

		for (int cpuno = 0; cpuno < inres->cpucount; ++cpuno) {
			inres->cpus[cpuno].olduser = inres->cpus[cpuno].user;
			inres->cpus[cpuno].oldsys = inres->cpus[cpuno].sys;
			inres->cpus[cpuno].oldidle = inres->cpus[cpuno].idle;
			inres->cpus[cpuno].oldnice = inres->cpus[cpuno].nice;
			inres->cpus[cpuno].oldtotal = inres->cpus[cpuno].total;

			inres->cpus[cpuno].user = (int)r_load[cpuno].cpu_ticks[CPU_STATE_USER];
			inres->cpus[cpuno].sys = (int)r_load[cpuno].cpu_ticks[CPU_STATE_SYSTEM];
			inres->cpus[cpuno].idle = (int)r_load[cpuno].cpu_ticks[CPU_STATE_IDLE];
			inres->cpus[cpuno].nice = (int)r_load[cpuno].cpu_ticks[CPU_STATE_NICE];
			inres->cpus[cpuno].total =
				(int)r_load[cpuno].cpu_ticks[CPU_STATE_USER] + (int)r_load[cpuno].cpu_ticks[CPU_STATE_SYSTEM] +
				(int)r_load[cpuno].cpu_ticks[CPU_STATE_IDLE] + (int)r_load[cpuno].cpu_ticks[CPU_STATE_NICE];

			total = (double)(inres->cpus[cpuno].total - inres->cpus[cpuno].oldtotal);

			if(total > 0){
				inres->cpus[cpuno].percentuser =
					(double)(inres->cpus[cpuno].user - inres->cpus[cpuno].olduser) / total * 100;
				inres->cpus[cpuno].percentsys =
					(double)(inres->cpus[cpuno].sys - inres->cpus[cpuno].oldsys) / total * 100;
				inres->cpus[cpuno].percentidle =
					(double)(inres->cpus[cpuno].idle - inres->cpus[cpuno].oldidle) / total * 100;
				inres->cpus[cpuno].percentnice =
					(double)(inres->cpus[cpuno].nice - inres->cpus[cpuno].oldnice) / total * 100;
			} else {
				inres->cpus[cpuno].percentuser = 0;
				inres->cpus[cpuno].percentsys = 0;
				inres->cpus[cpuno].percentidle = 0;
				inres->cpus[cpuno].percentnice = 0;
			}

			if(COUNT_HYPERTHREADS_IN_CPU_AVG) {
				inres->avgpercentuser += inres->cpus[cpuno].percentuser;
				inres->avgpercentsys += inres->cpus[cpuno].percentsys;
				inres->avgpercentidle += inres->cpus[cpuno].percentidle;
				inres->avgpercentnice += inres->cpus[cpuno].percentnice;
			} else {
				if(!(cpuno % inres->cpuhyperthreadmod)) {
					inres->avgpercentuser += inres->cpus[cpuno].percentuser;
					inres->avgpercentsys += inres->cpus[cpuno].percentsys;
					inres->avgpercentidle += inres->cpus[cpuno].percentidle;
					inres->avgpercentnice += inres->cpus[cpuno].percentnice;
				}
			}
		}

		inres->percentallcpu = (inres->avgpercentuser + inres->avgpercentsys + inres->avgpercentnice) / 100;
		if(COUNT_HYPERTHREADS_IN_CPU_AVG) {
			inres->avgpercentuser /= inres->cpucount;
			inres->avgpercentsys /= inres->cpucount;
			inres->avgpercentidle /= inres->cpucount;
			inres->avgpercentnice /= inres->cpucount;
		} else {
			inres->avgpercentuser /= (inres->cpucount / inres->cpuhyperthreadmod);
			inres->avgpercentsys /= (inres->cpucount / inres->cpuhyperthreadmod);
			inres->avgpercentidle /= (inres->cpucount / inres->cpuhyperthreadmod);
			inres->avgpercentnice /= (inres->cpucount / inres->cpuhyperthreadmod);
		}
	}

	kern_return_t task_info_error;
	task_power_info_v2_data_t power_info_data_v2;
	mach_msg_type_number_t tpi_count = TASK_POWER_INFO_V2_COUNT;
	task_info_error = task_info(mach_task_self(), TASK_POWER_INFO_V2, (task_info_t)&power_info_data_v2, &tpi_count);

	if(power_info_data_v2.cpu_energy.total_system > inres->energysystem) {
		inres->energysystemlast = inres->energysystem;
		inres->energysystem = power_info_data_v2.cpu_energy.total_system;
	} else {
		inres->energysystemlast = power_info_data_v2.cpu_energy.total_system;
		inres->energysystem = power_info_data_v2.cpu_energy.total_system;
	}
	if(power_info_data_v2.cpu_energy.total_user > inres->energyuser) {
		inres->energyuserlast = inres->energyuser;
		inres->energyuser = power_info_data_v2.cpu_energy.total_user;
	} else {
		inres->energyuserlast = power_info_data_v2.cpu_energy.total_user;
		inres->energyuser = power_info_data_v2.cpu_energy.total_user;
	}

	vm_deallocate(mach_task_self(), (vm_address_t)hostinfo, count);
}

/*
 * Get Mach Virtual Memory Statistics
 *
 * see also:
 *   https://opensource.apple.com/source/system_cmds/system_cmds-496/vm_stat.tproj/vm_stat.c.auto.html
 */
unsigned long long getsysvminfo()
{
	mach_port_t host = mach_host_self();
	struct vm_statistics64 vm_stat;
	natural_t count = HOST_VM_INFO64_COUNT;
	// vm_size_t pageSize = 4096;
	if (host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count) == KERN_SUCCESS) {
		return (unsigned long long)vm_stat.pageouts * (unsigned long long)4096;
	}
	return 0;
}

//
// Processes information
//

/*
 * Convert kinfo_proc data structure into a simple sysproc data structure
 */
static struct sysproc **sysprocfromkinfoproc(struct kinfo_proc *processes, int count, struct sysproc **procsin, struct hashitem **hashtable, double cpupercent, struct sysres *res)
{
	// struct sysproc **procsin;
	if(procsin == NULL) {
		procsin = (struct sysproc **)malloc(sizeof(struct sysproc*) * (size_t)count);
		if(procsin == NULL) {
			// TODO: handle memory allocation failure
		}
	} else {
		struct sysproc **procsold = procsin;
		procsin = (struct sysproc **)realloc(procsold, sizeof(struct sysproc*) * (size_t)count);
		if(!procsin) {
			// TODO: handle error and memory leak
		}
	}

	int error = 0;
	struct rusage_info_v3 rusage;

	unsigned int totaldiskr = 0;
	unsigned int totaldiskw = 0;
	unsigned long long totalmem = 0;

	unsigned long long total = 0;
	unsigned long long oldtotal = 0;

	struct sysproc *procinfo = NULL;
	for (int i = 0; i < count; ++i) {
		// TODO: get per process gpu usage information
		//   see also: /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/mach/task_info.h
		//   example below:
		// kern_return_t task_info_error;
		// task_power_info_v2_data_t power_info_data_v2;
		// mach_msg_type_number_t tpi_count = TASK_POWER_INFO_V2_COUNT;
		// task_info_error = task_info(i, TASK_POWER_INFO_V2, (task_info_t)&power_info_data_v2, &tpi_count);
		// char *result = malloc(16);
		// snprintf(result, 16, "%16llu\n", power_info_data_v2.gpu_energy.task_gpu_utilisation);
		// snprintf(result, 16, "%16llu\n", power_info_data_v2.gpu_energy.task_gpu_stat_reserved0);
		// snprintf(result, 16, "%16llu\n", power_info_data_v2.gpu_energy.task_gpu_stat_reserved1);
		// snprintf(result, 16, "%16llu\n", power_info_data_v2.gpu_energy.task_gpu_stat_reserved2);

		procinfo = (struct sysproc *)hashtget(*hashtable, processes[i].kp_proc.p_pid);
		if(!procinfo) {
			procinfo = (struct sysproc *)calloc(sizeof(struct sysproc), 1);
			procinfo->path = malloc(SYSPROC_PATH_LENGTH + 1);
			procinfo->realusername = malloc(SYSPROC_REAL_USER_NAME_LENGTH + 1);
			hashtadd(*hashtable, processes[i].kp_proc.p_pid, procinfo);
		}

		//
		// sysctl.h > proc.h
		//
		// S* process status
		procinfo->status = processes[i].kp_proc.p_stat;
		// Process identifier.
		procinfo->pid = processes[i].kp_proc.p_pid;
		// Real time
		procinfo->realtime = processes[i].kp_proc.p_rtime;
		// // CPU ticks
		// procinfo->cticks = processes[i].kp_proc.p_cpticks;
		// // User ticks
		// procinfo->uticks = processes[i].kp_proc.p_uticks;
		// // System ticks
		// procinfo->sticks = processes[i].kp_proc.p_sticks;
		// // Interupt ticks
		// procinfo->iticks = processes[i].kp_proc.p_iticks;
		// Process priority.
		procinfo->priority = processes[i].kp_proc.p_priority;
		// "nice" value
		procinfo->nice = processes[i].kp_proc.p_nice;
		// Process name
		procinfo->name = processes[i].kp_proc.p_comm;

		//
		// sysctl.h
		//
		// process credentials, real user id
		procinfo->realuid = processes[i].kp_eproc.e_pcred.p_ruid;
		// real username
		struct passwd *realuser = getpwuid(procinfo->realuid);
		// procinfo->realusername = realuser->pw_name;
		strlcpy(procinfo->realusername, realuser->pw_name, sizeof(procinfo->realusername)+1);
		// current credentials, effective user id
		procinfo->effectiveuid = processes[i].kp_eproc.e_ucred.cr_uid;
		// effectiver user, name
		struct passwd *effectiveuser = getpwuid(processes[i].kp_eproc.e_ucred.cr_uid);
		procinfo->effectiveusername = effectiveuser->pw_name;
		// parent process id
		procinfo->parentpid = processes[i].kp_eproc.e_ppid;
		// Process group identifier.
		procinfo->pgid = processes[i].kp_eproc.e_pgid;
		// controlling tty dev
		procinfo->ttydev = processes[i].kp_eproc.e_tdev;
		// setlogin() name
		procinfo->setloginname = processes[i].kp_eproc.e_login;

		processArguments(procinfo->pid, SYSPROC_PATH_LENGTH, procinfo->path);
		if(procinfo->path == NULL) {
			procinfo->path = procinfo->name;
		}

		procinfo->lasttotaltime = procinfo->totaltime;

		// get additional info not available from sysctl
		error = proc_pid_rusage(procinfo->pid, RUSAGE_INFO_V3, (rusage_info_t *)&rusage);
		if(!error) {
			//
			// resource.h
			//
			procinfo->utime = rusage.ri_user_time;
			procinfo->stime = rusage.ri_system_time;
			procinfo->totaltime = rusage.ri_user_time + rusage.ri_system_time;
			procinfo->idlewakeups = rusage.ri_pkg_idle_wkups;

			procinfo->wiredmem = rusage.ri_wired_size;
			procinfo->residentmem = rusage.ri_resident_size;
			procinfo->physicalmem = rusage.ri_phys_footprint;
			procinfo->diskior = rusage.ri_diskio_bytesread;
			procinfo->diskiow = rusage.ri_diskio_byteswritten;
			procinfo->billedtime = rusage.ri_billed_system_time;

			totaldiskr += procinfo->diskior;
			totaldiskw += procinfo->diskiow;
			totalmem += procinfo->residentmem;
		} else {
			procinfo->utime = 0;
			procinfo->stime = 0;
			procinfo->totaltime = 0;
			procinfo->idlewakeups = 0;

			procinfo->wiredmem = 0;
			procinfo->residentmem = 0;
			procinfo->physicalmem = 0;
			procinfo->diskior = 0;
			procinfo->diskiow = 0;
			procinfo->billedtime = 0;
		}

		if(procinfo->totaltime && !procinfo->lasttotaltime) {
			procinfo->lasttotaltime = procinfo->totaltime;
		}

		oldtotal = total;
		total += (procinfo->totaltime - procinfo->lasttotaltime);
		if(total < oldtotal) {
			// TODO: handle integer overflows
		}

		procsin[i] = procinfo;
	}

	for (int i = 0; i < count; ++i) {
		procsin[i]->percentage = (((double)(procsin[i]->totaltime - procsin[i]->lasttotaltime) / total) * 100) * cpupercent;
	}

	if(totaldiskr > res->diskuser) {
		res->diskuserlast = res->diskuser;
		res->diskuser = totaldiskr;
	} else {
		res->diskuserlast = totaldiskr;
		res->diskuser = totaldiskr;
	}
	if(totaldiskw > res->diskusew) {
		res->diskusewlast = res->diskusew;
		res->diskusew = totaldiskw;
	} else {
		res->diskusewlast = totaldiskw;
		res->diskusew = totaldiskw;
	}

	res->memused = totalmem;

	return procsin;
}

/*
 * Get all process information from sysctl
 */
static struct sysproc **getsysprocinfo(int processinfotype, int criteria, size_t *length, struct sysproc **procs, struct hashitem **hashtable, double cpupercent, struct sysres *res)
{
	struct sysproc **result = NULL;
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
			result = sysprocfromkinfoproc(processlist, processcount, procs, hashtable, cpupercent, res);
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
	return result;
}

struct sysproc **getsysprocinfoall(size_t *length, struct sysproc **procs, struct hashitem **hashtable, double cpupercent, struct sysres *res)
{
	return getsysprocinfo(KERN_PROC_ALL, 0, length, procs, hashtable, cpupercent, res);
}

void getsysnetinfo(struct sysnet *net)
{
	struct ifaddrs *if_list;
	struct ifaddrs *if_info;

	// get interface addresses
	if (getifaddrs(&if_list) < 0) {
		return;
	}

	// verify interface list exists
	if (if_list == NULL) {
		return;
	}

	net->oldibytes = net->ibytes;
	net->oldobytes = net->obytes;
	net->ibytes = 0;
	net->obytes = 0;

	if_info = if_list;
	int numberofifs = 0;
	for (if_info = if_list; if_info != NULL; if_info = if_info->ifa_next) {
		if((if_info->ifa_flags & IFF_UP) && (if_info->ifa_flags & IFF_RUNNING)) {
			if(if_info->ifa_addr->sa_family == AF_LINK) {
				struct if_data *ifdat = (struct if_data *)if_info->ifa_data;
				if(ifdat) {
					++numberofifs;

					net->ipackets += ifdat->ifi_ipackets;
					net->ierrors += ifdat->ifi_ierrors;
					net->ibytes += ifdat->ifi_ibytes;

					net->opackets += ifdat->ifi_opackets;
					net->oerrors += ifdat->ifi_oerrors;
					net->obytes += ifdat->ifi_obytes;

					net->drops += ifdat->ifi_iqdrops;
					net->noproto += ifdat->ifi_noproto;
				}
			}
		}
	}
	/* future use
	if(!net->ifcount || (numberofifs != net->ifcount)) {
		net->ifcount = numberofifs;
		net->ifs = (struct sysnetif *)calloc(sizeof(struct sysnetif), (size_t)numberofifs);
		if(net->ifs == NULL) {
			// TODO: handle memory allocation failure
		}
	}

	if_info = if_list;
	int ifnumber = 0;
	for (if_info = if_list; if_info != NULL; if_info = if_info->ifa_next) {
		if((if_info->ifa_flags & IFF_UP) && (if_info->ifa_flags & IFF_RUNNING)) {
			if(if_info->ifa_addr->sa_family == AF_LINK) {
				struct if_data *ifdat = (struct if_data *)if_info->ifa_data;
				if(ifdat) {
					net->ifs[ifnumber].ipackets += ifdat->ifi_ipackets;
					net->ifs[ifnumber].ierrors += ifdat->ifi_ierrors;
					net->ifs[ifnumber].ibytes += ifdat->ifi_ibytes;

					net->ifs[ifnumber].opackets += ifdat->ifi_opackets;
					net->ifs[ifnumber].oerrors += ifdat->ifi_oerrors;
					net->ifs[ifnumber].obytes += ifdat->ifi_obytes;

					net->ifs[ifnumber].drops += ifdat->ifi_iqdrops;
					net->ifs[ifnumber].noproto += ifdat->ifi_noproto;
				}
			}
		}
	}
	*/
	freeifaddrs(if_list);
}
