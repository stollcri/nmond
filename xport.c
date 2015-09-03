/**
 * nmond_xport.c -- Export data to file rather than displaying it on the screen
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

// ~~~~~~~~~~~~~
// xport_part_01
// ~~~~~~~~~~~~~

			/* Output the header lines for the spread sheet */
			timer = time(0);
			tim = localtime(&timer);
			tim->tm_year += 1900 - 2000;  /* read localtime() manual page!! */
			tim->tm_mon  += 1; /* because it is 0 to 11 */
			if(varperftmp)
				sprintf( str, "/var/perf/tmp/%s_%02d.nmon", hostname, tim->tm_mday);
			else if(user_filename_set)
				strcpy( str, user_filename);
			else
				sprintf( str, "%s_%02d%02d%02d_%02d%02d.nmon",
						hostname,
						tim->tm_year,
						tim->tm_mon,
						tim->tm_mday,
						tim->tm_hour,
						tim->tm_min);
			if((fp = fopen(str,"w")) ==0 ) {
				perror(MSG_ERR_NOPENFILE);
				printf(MSG_INF_OUTFILENM,str);
				exit(42);
			}
			/* disconnect from terminal */
			fflush(NULL);
			if (!debug && (childpid = fork()) != 0) {
				if(ralfmode)
					printf("%d\n",childpid);
				exit(0); /* parent returns OK */
			}
			if(!debug) {
				close(0);
				close(1);
				close(2);
				setpgrp(); /* become process group leader */
				signal(SIGHUP, SIG_IGN); /* ignore hangups */
			}
			/* Do the nmon_start activity early on */
			if (nmon_start) {
				timer = time(0);
				child_start(CHLD_START, nmon_start, time_stamp_type, 1, timer);
			}
			
			if(show_aaa) {
				fprintf(fp,"AAA,progname,%s\n", progname);
				fprintf(fp,"AAA,command,");
				for(i=0;i<argc;i++)
					fprintf(fp,"%s ",argv[i]);
				fprintf(fp,"\n");
				fprintf(fp,"AAA,version,%s\n", VERSION);
				fprintf(fp,"AAA,disks_per_line,%d\n", disks_per_line);
				fprintf(fp,"AAA,max_disks,%d,set by -d option\n", diskmax);
				fprintf(fp,"AAA,disks,%d,\n", disks);
				
				fprintf(fp,"AAA,host,%s\n", hostname);
				fprintf(fp,"AAA,user,%s\n", getenv("USER"));
				fprintf(fp,"AAA,OS,Linux,%s,%s,%s\n",uts.release,uts.version,uts.machine);
				fprintf(fp,"AAA,runname,%s\n", run_name);
				fprintf(fp,"AAA,time,%02d:%02d.%02d\n", tim->tm_hour, tim->tm_min, tim->tm_sec);
				fprintf(fp,"AAA,date,%02d-%3s-%02d\n", tim->tm_mday, month[tim->tm_mon-1], tim->tm_year+2000);
				fprintf(fp,"AAA,interval,%d\n", seconds);
				fprintf(fp,"AAA,snapshots,%d\n", maxloops);

				fprintf(fp,"AAA,cpus,%d\n", cpus);
				fprintf(fp,"AAA,x86,VendorId,%s\n",       vendor_ptr);
				fprintf(fp,"AAA,x86,ModelName,%s\n",      model_ptr);
				fprintf(fp,"AAA,x86,MHz,%s\n",            mhz_ptr);
				fprintf(fp,"AAA,x86,bogomips,%s\n",       bogo_ptr);
				fprintf(fp,"AAA,x86,ProcessorChips,%d\n", processorchips);
				fprintf(fp,"AAA,x86,Cores,%d\n",          cores);
				fprintf(fp,"AAA,x86,hyperthreads,%d\n",   hyperthreads);
				fprintf(fp,"AAA,x86,VirtualCPUs,%d\n",    cpus);
				fprintf(fp,"AAA,proc_stat_variables,%d\n", stat8);
				
				fprintf(fp,"AAA,note0, Warning - use the UNIX sort command to order this file before loading into a spreadsheet\n");
				fprintf(fp,"AAA,note1, The First Column is simply to get the output sorted in the right order\n");
				fprintf(fp,"AAA,note2, The T0001-T9999 column is a snapshot number. To work out the actual time; see the ZZZ section at the end\n");
			}
			fflush(NULL);
			
			for (i = 1; i <= cpus; i++)
				fprintf(fp,"CPU%03d,CPU %d %s,User%%,Sys%%,Wait%%,Idle%%,Steal%%\n", i, i, run_name);
			fprintf(fp,"CPU_ALL,CPU Total %s,User%%,Sys%%,Wait%%,Idle%%,Steal%%,Busy,CPUs\n", run_name);
			fprintf(fp,"MEM,Memory MB %s,memtotal,hightotal,lowtotal,swaptotal,memfree,highfree,lowfree,swapfree,memshared,cached,active,bigfree,buffers,swapcached,inactive\n", run_name);
			
			fprintf(fp,"PROC,Processes %s,Runnable,Blocked,pswitch,syscall,read,write,fork,exec,sem,msg\n", run_name);
			/*
			 fprintf(fp,"PAGE,Paging %s,faults,pgin,pgout,pgsin,pgsout,reclaims,scans,cycles\n", run_name);
			 fprintf(fp,"FILE,File I/O %s,iget,namei,dirblk,readch,writech,ttyrawch,ttycanch,ttyoutch\n", run_name);
			 */
			
			
			fprintf(fp,"NET,Network I/O %s", run_name);
			for (i = 0; i < networks; i++)
				fprintf(fp,",%-2s-read-KB/s", (char *)p->ifnets[i].if_name);
			for (i = 0; i < networks; i++)
				fprintf(fp,",%-2s-write-KB/s", (char *)p->ifnets[i].if_name);
			fprintf(fp,"\n");
			fprintf(fp,"NETPACKET,Network Packets %s", run_name);
			for (i = 0; i < networks; i++)
				fprintf(fp,",%-2s-read/s,", (char *)p->ifnets[i].if_name);
			for (i = 0; i < networks; i++)
				fprintf(fp,",%-2s-write/s,", (char *)p->ifnets[i].if_name);
			/* iremoved as it is not below in the BUSY line fprintf(fp,"\n"); */
#ifdef DEBUG
			if(debug)printf("disks=%d x%sx\n",(char *)disks,p->dk[0].dk_name);
#endif /*DEBUG*/
			for (i = 0; i < disks; i++)  {
				if(NEWDISKGROUP(i))
					fprintf(fp,"\nDISKBUSY%s,Disk %%Busy %s", dskgrp(i) ,run_name);
				fprintf(fp,",%s", (char *)p->dk[i].dk_name);
			}
			for (i = 0; i < disks; i++) {
				if(NEWDISKGROUP(i))
					fprintf(fp,"\nDISKREAD%s,Disk Read KB/s %s", dskgrp(i),run_name);
				fprintf(fp,",%s", (char *)p->dk[i].dk_name);
			}
			for (i = 0; i < disks; i++) {
				if(NEWDISKGROUP(i))
					fprintf(fp,"\nDISKWRITE%s,Disk Write KB/s %s", (char *)dskgrp(i),run_name);
				fprintf(fp,",%s", (char *)p->dk[i].dk_name);
			}
			for (i = 0; i < disks; i++) {
				if(NEWDISKGROUP(i))
					fprintf(fp,"\nDISKXFER%s,Disk transfers per second %s", (char *)dskgrp(i),run_name);
				fprintf(fp,",%s", p->dk[i].dk_name);
			}
			for (i = 0; i < disks; i++) {
				if(NEWDISKGROUP(i))
					fprintf(fp,"\nDISKBSIZE%s,Disk Block Size %s", dskgrp(i),run_name);
				fprintf(fp,",%s", (char *)p->dk[i].dk_name);
			}
			if( extended_disk == 1 && disk_mode == DISK_MODE_DISKSTATS )    {
				for (i = 0; i < disks; i++) {
					if(NEWDISKGROUP(i))
						fprintf(fp,"\nDISKREADS%s,Disk Rd/s %s", dskgrp(i),run_name);
					fprintf(fp,",%s", (char *)p->dk[i].dk_name);
				}
				for (i = 0; i < disks; i++) {
					if(NEWDISKGROUP(i))
						fprintf(fp,"\nDISKWRITES%s,Disk Wrt/s %s", dskgrp(i),run_name);
					fprintf(fp,",%s", (char *)p->dk[i].dk_name);
				}
			}
			
			fprintf(fp,"\n");
			list_dgroup(p->dk);
			jfs_load(LOAD);
			fprintf(fp,"JFSFILE,JFS Filespace %%Used %s", hostname);
			for (k = 0; k < jfses; k++) {
				if(jfs[k].mounted && strncmp(jfs[k].name,"/Users/stollcri/Documents/code/c/nmond/dbg",5)
				   && strncmp(jfs[k].name,"/sys",4)
				   && strncmp(jfs[k].name,"/run/",5)
				   && strncmp(jfs[k].name,"/dev/",5)
				   && strncmp(jfs[k].name,"/var/lib/nfs/rpc",16)
				   )  /* /proc gives invalid/insane values */
					fprintf(fp,",%s", jfs[k].name);
			}
			fprintf(fp,"\n");
			jfs_load(UNLOAD);

			if(show_top){
				fprintf(fp,"TOP,%%CPU Utilisation\n");
				fprintf(fp,"TOP,+PID,Time,%%CPU,%%Usr,%%Sys,Size,ResSet,ResText,ResData,ShdLib,MinorFault,MajorFault,Command,Threads,IOwaitTime\n");
			}
			linux_bbbp("/etc/release",     "/bin/cat /etc/*ease 2>/dev/null", WARNING);
			linux_bbbp("lsb_release",      "/usr/bin/lsb_release -a 2>/dev/null", WARNING);
			linux_bbbp("fdisk-l",          "/sbin/fdisk -l 2>/dev/null", WARNING);
			linux_bbbp("lsblk",            "/usr/bin/lsblk 2>/dev/null", WARNING);
			linux_bbbp("lscpu",            "/usr/bin/lscpu 2>/dev/null", WARNING);
			linux_bbbp("lshw",             "/usr/bin/lshw 2>/dev/null", WARNING);
			linux_bbbp("/proc/cpuinfo",    "/bin/cat /proc/cpuinfo 2>/dev/null", WARNING);
			linux_bbbp("/proc/meminfo",    "/bin/cat /proc/meminfo 2>/dev/null", WARNING);
			linux_bbbp("/proc/stat",       "/bin/cat /proc/stat 2>/dev/null", WARNING);
			linux_bbbp("/proc/version",    "/bin/cat /proc/version 2>/dev/null", WARNING);
			linux_bbbp("/proc/net/dev",    "/bin/cat /proc/net/dev 2>/dev/null", WARNING);
			linux_bbbp("/proc/diskinfo",   "/bin/cat /proc/diskinfo 2>/dev/null", WARNING);
			linux_bbbp("/proc/diskstats",   "/bin/cat /proc/diskstats 2>/dev/null", WARNING);
			
			linux_bbbp("/sbin/multipath",   "/sbin/multipath -l 2>/dev/null", WARNING);
			linux_bbbp("/dev/mapper",   	"ls -l /dev/mapper 2>/dev/null", WARNING);
			linux_bbbp("/dev/mpath",   		"ls -l /dev/mpath 2>/dev/null", WARNING);
			linux_bbbp("/dev/dm-*",   		"ls -l /dev/dm-* 2>/dev/null", WARNING);
			linux_bbbp("/dev/md*",   		"ls -l /dev/md* 2>/dev/null", WARNING);
			linux_bbbp("/dev/sd*",   		"ls -l /dev/sd* 2>/dev/null", WARNING);
			linux_bbbp("/proc/partitions", "/bin/cat /proc/partitions 2>/dev/null", WARNING);
			linux_bbbp("/proc/1/stat",     "/bin/cat /proc/1/stat 2>/dev/null", WARNING);
			linux_bbbp("/proc/net/rpc/nfs",        "/bin/cat /proc/net/rpc/nfs 2>/dev/null", WARNING);
			linux_bbbp("/proc/net/rpc/nfsd",        "/bin/cat /proc/net/rpc/nfsd 2>/dev/null", WARNING);
			linux_bbbp("/proc/modules",    "/bin/cat /proc/modules 2>/dev/null", WARNING);
			linux_bbbp("ifconfig",        "/sbin/ifconfig 2>/dev/null", WARNING);
			linux_bbbp("/bin/df-m",        "/bin/df -m 2>/dev/null", WARNING);
			linux_bbbp("/bin/mount",        "/bin/mount 2>/dev/null", WARNING);
			linux_bbbp("/etc/fstab",    "/bin/cat /etc/fstab 2>/dev/null", WARNING);
			linux_bbbp("netstat -r",    "/bin/netstat -r 2>/dev/null", WARNING);
			linux_bbbp("uptime",    "/usr/bin/uptime  2>/dev/null", WARNING);
			linux_bbbp("getconf PAGESIZE",    "/usr/bin/getconf PAGESIZE  2>/dev/null", WARNING);
			
			sleep(1); /* to get the first stats to cover this one second and avoids divide by zero issues */

// ~~~~~~~~~~~~~
// xport_part_02
// ~~~~~~~~~~~~~

				if (!cursed && nmon_snap && (loop % nmon_one_in) == 0 ) {
					child_start(CHLD_SNAP, nmon_snap, time_stamp_type, loop, timer);
				}
				
				
				if(!show_rrd)
					fprintf(fp,"ZZZZ,%s,%02d:%02d:%02d,%02d-%s-%4d\n", LOOP,
							tim->tm_hour, tim->tm_min, tim->tm_sec,
							tim->tm_mday, month[tim->tm_mon], tim->tm_year+1900);
				fflush(NULL);

// ~~~~~~~~~~~~~
// xport_part_03
// ~~~~~~~~~~~~~

					for (i = max_cpus+1; i <= cpus; i++)
						fprintf(fp,"CPU%03d,CPU %d %s,User%%,Sys%%,Wait%%,Idle%%\n", i, i, run_name);
					max_cpus= cpus;

// ~~~~~~~~~~~~~
// xport_part_04
// ~~~~~~~~~~~~~

						if( bbbr_line == 0)	{
							fprintf(fp,"BBBR,0,Reconfig,action,old,new\n");
							bbbr_line++;
						}
						fprintf(fp,"BBBR,%03d,%s,cpuchg,%d,%d\n",bbbr_line++,LOOP,old_cpus,cpus);

// ~~~~~~~~~~~~~
// xport_part_05
// ~~~~~~~~~~~~~

	// Put back

// ~~~~~~~~~~~~~
// xport_part_06
// ~~~~~~~~~~~~~

					if(show_rrd)
						str_p = "rrdtool update mem.rrd %s:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f\n";
					else
						str_p = "MEM,%s,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n";
					fprintf(fp,str_p,
							LOOP,
							p->mem.memtotal/1024.0,
							p->mem.hightotal/1024.0,
							p->mem.lowtotal/1024.0,
							p->mem.swaptotal/1024.0,
							p->mem.memfree/1024.0,
							p->mem.highfree/1024.0,
							p->mem.lowfree/1024.0,
							p->mem.swapfree/1024.0,
							p->mem.memshared/1024.0,
							p->mem.cached/1024.0,
							p->mem.active/1024.0,
#ifdef LARGEMEM
							-1.0,
#else
							p->mem.bigfree/1024.0,
#endif /*LARGEMEM*/
							p->mem.buffers/1024.0,
							p->mem.swapcached/1024.0,
							p->mem.inactive/1024.0);

// ~~~~~~~~~~~~~
// xport_part_07
// ~~~~~~~~~~~~~

					if(p->mem.hugetotal > 0) {
						if(first_huge == 1){
							first_huge=0;
							fprintf(fp,"HUGEPAGES,Huge Page Use %s,HugeTotal,HugeFree,HugeSizeMB\n", run_name);
						}
						fprintf(fp,"HUGEPAGES,%s,%ld,%ld,%.1f\n",
								LOOP,
								p->mem.hugetotal,
								p->mem.hugefree,
								p->mem.hugesize/1024.0);
					}

// ~~~~~~~~~~~~~
// xport_part_08
// ~~~~~~~~~~~~~

					if( ret < 0) {
						show_vm=0;
					} else if(vm_first_time) {
						vm_first_time=0;
						fprintf(fp,"VM,Paging and Virtual Memory,nr_dirty,nr_writeback,nr_unstable,nr_page_table_pages,nr_mapped,nr_slab,pgpgin,pgpgout,pswpin,pswpout,pgfree,pgactivate,pgdeactivate,pgfault,pgmajfault,pginodesteal,slabs_scanned,kswapd_steal,kswapd_inodesteal,pageoutrun,allocstall,pgrotated,pgalloc_high,pgalloc_normal,pgalloc_dma,pgrefill_high,pgrefill_normal,pgrefill_dma,pgsteal_high,pgsteal_normal,pgsteal_dma,pgscan_kswapd_high,pgscan_kswapd_normal,pgscan_kswapd_dma,pgscan_direct_high,pgscan_direct_normal,pgscan_direct_dma\n");
					}
					if(show_rrd)
						str_p = "rrdtool update vm.rrd %s"
						":%lld:%lld:%lld:%lld:%lld"
						":%lld:%lld:%lld:%lld:%lld"
						":%lld:%lld:%lld:%lld:%lld"
						":%lld:%lld:%lld:%lld:%lld"
						":%lld:%lld:%lld:%lld:%lld"
						":%lld:%lld:%lld:%lld:%lld"
						":%lld:%lld:%lld:%lld:%lld"
						":%lld:%lld\n";
					else
						str_p = "VM,%s"
						",%lld,%lld,%lld,%lld,%lld"
						",%lld,%lld,%lld,%lld,%lld"
						",%lld,%lld,%lld,%lld,%lld"
						",%lld,%lld,%lld,%lld,%lld"
						",%lld,%lld,%lld,%lld,%lld"
						",%lld,%lld,%lld,%lld,%lld"
						",%lld,%lld,%lld,%lld,%lld"
						",%lld,%lld\n";
					
					fprintf(fp, str_p,
							LOOP,
							VMCOUNT(nr_dirty),
							VMCOUNT(nr_writeback),
							VMCOUNT(nr_unstable),
							VMCOUNT(nr_page_table_pages),
							VMCOUNT(nr_mapped),
							VMCOUNT(nr_slab),
							VMDELTA(pgpgin),
							VMDELTA(pgpgout),
							VMDELTA(pswpin),
							VMDELTA(pswpout),
							VMDELTA(pgfree),
							VMDELTA(pgactivate),
							VMDELTA(pgdeactivate),
							VMDELTA(pgfault),
							VMDELTA(pgmajfault),
							VMDELTA(pginodesteal),
							VMDELTA(slabs_scanned),
							VMDELTA(kswapd_steal),
							VMDELTA(kswapd_inodesteal),
							VMDELTA(pageoutrun),
							VMDELTA(allocstall),
							VMDELTA(pgrotated),
							VMDELTA(pgalloc_high),
							VMDELTA(pgalloc_normal),
							VMDELTA(pgalloc_dma),
							VMDELTA(pgrefill_high),
							VMDELTA(pgrefill_normal),
							VMDELTA(pgrefill_dma),
							VMDELTA(pgsteal_high),
							VMDELTA(pgsteal_normal),
							VMDELTA(pgsteal_dma),
							VMDELTA(pgscan_kswapd_high),
							VMDELTA(pgscan_kswapd_normal),
							VMDELTA(pgscan_kswapd_dma),
							VMDELTA(pgscan_direct_high),
							VMDELTA(pgscan_direct_normal),
							VMDELTA(pgscan_direct_dma));

// ~~~~~~~~~~~~~
// xport_part_09
// ~~~~~~~~~~~~~

					if(proc_first_time) {
						q->cpu_total.ctxt = p->cpu_total.ctxt;
						q->cpu_total.procs= p->cpu_total.procs;
						proc_first_time=0;
					}
					if(show_rrd)
						str_p = "rrdtool update proc.rrd %s:%.0f:%.0f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f:%.1f\n";
					else
						str_p = "PROC,%s,%.0f,%.0f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f,%.1f\n";
					
					fprintf(fp,str_p,
							LOOP,
							(float)p->cpu_total.running,/*runqueue*/
							(float)p->cpu_total.blocked,/*swapin (# of processes waiting for IO completion */
							/*pswitch*/
							(float)(p->cpu_total.ctxt - q->cpu_total.ctxt)/elapsed,
							-1.0,		/*syscall*/
							-1.0,		/*read*/
							-1.0,		/*write*/
							/*fork*/
							(float)(p->cpu_total.procs - q->cpu_total.procs)/elapsed,
							-1.0,		/*exec*/
							-1.0,		/*sem*/
							-1.0);		/*msg*/

// ~~~~~~~~~~~~~
// xport_part_10
// ~~~~~~~~~~~~~

					if(nfs_first_time && ! show_rrd) {
						if(nfs_v2c_found) {
							fprintf(fp,"NFSCLIV2,NFS Client v2");
							for(i=0;i<18;i++) 
								fprintf(fp,",%s",nfs_v2_names[i]);
							fprintf(fp,"\n");
						}
						if(nfs_v2s_found) {
							fprintf(fp,"NFSSVRV2,NFS Server v2");
							for(i=0;i<18;i++) 
								fprintf(fp,",%s",nfs_v2_names[i]);
							fprintf(fp,"\n");
						}
						
						if(nfs_v3c_found) {
							fprintf(fp,"NFSCLIV3,NFS Client v3");
							for(i=0;i<22;i++) 
								fprintf(fp,",%s",nfs_v3_names[i]);
							fprintf(fp,"\n");
						}
						if(nfs_v3s_found) {
							fprintf(fp,"NFSSVRV3,NFS Server v3");
							for(i=0;i<22;i++) 
								fprintf(fp,",%s",nfs_v3_names[i]);
							fprintf(fp,"\n");
						}
						
						if(nfs_v4c_found) {
							fprintf(fp,"NFSCLIV4,NFS Client v4");
							for(i=0;i<nfs_v4c_names_count;i++) 
								fprintf(fp,",%s",nfs_v4c_names[i]);
							fprintf(fp,"\n");
						}
						if(nfs_v4s_found) {
							fprintf(fp,"NFSSVRV4,NFS Server v4");
							for(i=0;i<nfs_v4s_names_count;i++) 
								fprintf(fp,",%s",nfs_v4s_names[i]);
							fprintf(fp,"\n");
						}
						memcpy(&q->nfs,&p->nfs,sizeof(struct nfs_stat));
						nfs_first_time=0;
					}
					if(nfs_v2c_found) {
						fprintf(fp,show_rrd ? "rrdtool update nfscliv2.rrd %s" : "NFSCLIV2,%s", LOOP);
						for(i=0;i<18;i++) {
							fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", 
									(double)NFS_DELTA(nfs.v2c[i]));
						}
						fprintf(fp,"\n");
					}
					if(nfs_v2s_found) {
						fprintf(fp,show_rrd ? "rrdtool update nfsvrv2.rrd %s" : "NFSSVRV2,%s", LOOP);
						for(i=0;i<18;i++) {
							fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", 
									(double)NFS_DELTA(nfs.v2s[i]));
						}
						fprintf(fp,"\n");
					}
					if(nfs_v3c_found) {
						fprintf(fp,show_rrd ? "rrdtool update nfscliv3.rrd %s" : "NFSCLIV3,%s", LOOP);
						for(i=0;i<22;i++) {
							fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", 
									(double)NFS_DELTA(nfs.v3c[i]));
						}
						fprintf(fp,"\n");
					}
					if(nfs_v3s_found) {
						fprintf(fp,show_rrd ? "rrdtool update nfsvrv3.rrd %s" : "NFSSVRV3,%s", LOOP);
						for(i=0;i<22;i++) {
							fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", 
									(double)NFS_DELTA(nfs.v3s[i]));
						}
						fprintf(fp,"\n");
					}
					
					if(nfs_v4c_found) {
						fprintf(fp,show_rrd ? "rrdtool update nfscliv4.rrd %s" : "NFSCLIV4,%s", LOOP);
						for(i=0;i<nfs_v4c_names_count;i++) {
							fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", 
									(double)NFS_DELTA(nfs.v4c[i]));
						}
						fprintf(fp,"\n");
					}
					if(nfs_v4s_found) {
						fprintf(fp,show_rrd ? "rrdtool update nfsvrv4.rrd %s" : "NFSSVRV4,%s", LOOP);
						for(i=0;i<nfs_v4c_names_count;i++) {
							fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", 
									(double)NFS_DELTA(nfs.v4s[i]));
						}
						fprintf(fp,"\n");
					}

// ~~~~~~~~~~~~~
// xport_part_11
// ~~~~~~~~~~~~~

					fprintf(fp,show_rrd ? "rrdtool update net.rrd %s" : "NET,%s", LOOP);
					for (i = 0; i < networks; i++) {
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", IFDELTA(if_ibytes) / 1024.0);
					}
					for (i = 0; i < networks; i++) {
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", IFDELTA(if_obytes) / 1024.0);
					}
					fprintf(fp,"\n");
					fprintf(fp,show_rrd ? "rrdtool update netpacket.rrd %s" : "NETPACKET,%s", LOOP);
					for (i = 0; i < networks; i++) {
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", IFDELTA(if_ipackets) );
					}
					for (i = 0; i < networks; i++) {
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f", IFDELTA(if_opackets) );
					}
					fprintf(fp,"\n");

// ~~~~~~~~~~~~~
// xport_part_12
// ~~~~~~~~~~~~~

					jfs_load(LOAD);
					fprintf(fp,show_rrd ? "rrdtool update jfsfile.rrd %s" : "JFSFILE,%s", LOOP);
					for (k = 0; k < jfses; k++) {
						if(jfs[k].mounted && strncmp(jfs[k].name,"/Users/stollcri/Documents/code/c/nmond/dbg",5)
						   && strncmp(jfs[k].name,"/sys",4)
						   && strncmp(jfs[k].name,"/dev/",5)
						   && strncmp(jfs[k].name,"/run/",5)
						   && strncmp(jfs[k].name,"/var/lib/nfs/rpc",16)
						   )   { /* /proc gives invalid/insane values */
							if(fstatfs( jfs[k].fd, &statfs_buffer) != -1) {
								if(statfs_buffer.f_bsize == 0) 
									fs_bsize = 4.0 * 1024.0;
								else
									fs_bsize = statfs_buffer.f_bsize;
								/* convert blocks to MB */
								fs_size = (float)statfs_buffer.f_blocks * fs_bsize/1024.0/1024.0;
								
								/* find the best size info available f_bavail is like df reports
								 otherwise use f_bsize (this includes inode blocks) */
								if(statfs_buffer.f_bavail == 0) 
									fs_free = (float)statfs_buffer.f_bfree  * fs_bsize/1024.0/1024.0;
								else
									fs_free = (float)statfs_buffer.f_bavail  * fs_bsize/1024.0/1024.0;
								
								
								
								if(fs_size <= 0.0 || fs_bsize <= 0.0) /* some pseudo filesystems have zero size but we get a -nan result */
									fs_size_used = 0.0;
								else
									fs_size_used = (fs_size - (float)statfs_buffer.f_bfree  * fs_bsize/1024.0/1024.0)/fs_size * 100.0;
								
								if (fs_size_used >100.0)
									fs_size_used = 100.0;
								
								fprintf(fp, show_rrd ? ":%.1f" : ",%.1f", fs_size_used );
							}
							else
								fprintf(fp, show_rrd? ":U" : ",0.0");
						}
					}
					fprintf(fp, "\n");
					jfs_load(UNLOAD);

// ~~~~~~~~~~~~~
// xport_part_13
// ~~~~~~~~~~~~~

					for (i = 0; i < disks; i++) {
						if(NEWDISKGROUP(i))
							fprintf(fp,show_rrd ? "%srrdtool update diskbusy%s.rrd %s" : "%sDISKBUSY%s,%s",i == 0 ? "": "\n", dskgrp(i), LOOP);
						/* check percentage is correct */
						ftmp = DKDELTA(dk_time) / elapsed;
						if(ftmp > 100.0 || ftmp < 0.0)
							fprintf(fp,show_rrd ? ":U" : ",101.00");
						else
							fprintf(fp,show_rrd ? ":%.1f" : ",%.1f",
									DKDELTA(dk_time) / elapsed);
					}
					for (i = 0; i < disks; i++) {
						if(NEWDISKGROUP(i))
							fprintf(fp,show_rrd ? "\nrrdtool update diskread%s.rrd %s" : "\nDISKREAD%s,%s", dskgrp(i),LOOP);
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f",
								DKDELTA(dk_rkb) / elapsed);
					}
					for (i = 0; i < disks; i++) {
						if(NEWDISKGROUP(i))
							fprintf(fp,show_rrd ? "\nrrdtool update diskwrite%s.rrd %s" : "\nDISKWRITE%s,%s", dskgrp(i),LOOP);
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f",
								DKDELTA(dk_wkb) / elapsed);
					}
					for (i = 0; i < disks; i++) {
						if(NEWDISKGROUP(i))
							fprintf(fp,show_rrd ? "\nrrdtool update diskxfer%s.rrd %s" : "\nDISKXFER%s,%s", dskgrp(i),LOOP);
						disk_xfers = DKDELTA(dk_xfers);
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f",
								disk_xfers / elapsed);
					}
					for (i = 0; i < disks; i++) {
						if(NEWDISKGROUP(i))
							fprintf(fp,show_rrd ? "\nrrdtool update diskbsize%s.rrd %s" : "\nDISKBSIZE%s,%s", dskgrp(i),LOOP);
						disk_xfers = DKDELTA(dk_xfers);
						fprintf(fp,show_rrd ? ":%.1f" : ",%.1f",
								disk_xfers == 0.0 ? 0.0 :
								(DKDELTA(dk_rkb) + DKDELTA(dk_wkb) ) / disk_xfers);
					}
					
					if( extended_disk == 1 && disk_mode == DISK_MODE_DISKSTATS )	{
						for (i = 0; i < disks; i++) {
							if(NEWDISKGROUP(i))	{
								fprintf(fp,"\nDISKREADS%s,%s", dskgrp(i),LOOP);
							}
							disk_read = DKDELTA(dk_reads);
							fprintf(fp,",%.1f", disk_read / elapsed);
						}
						
						for (i = 0; i < disks; i++) {
							if(NEWDISKGROUP(i))	{
								fprintf(fp,"\nDISKWRITES%s,%s", dskgrp(i),LOOP);
							}
							disk_write = DKDELTA(dk_writes);
							fprintf(fp,",%.1f", disk_write / elapsed);
						}
					}
					fprintf(fp,"\n");

// ~~~~~~~~~~~~~
// xport_part_14
// ~~~~~~~~~~~~~

					if (dgroup_loaded == 2) {
						fprintf(fp, show_rrd ? "rrdtool update dgbusy.rdd %s" : "DGBUSY,%s", LOOP);
						for (k = 0; k < dgroup_total_groups; k++) {
							if (dgroup_name[k] != 0) {
								disk_total = 0.0;
								for (j = 0; j < dgroup_disks[k]; j++) {
									i = dgroup_data[k*DGROUPITEMS+j];
									if (i != -1) {
										disk_total += DKDELTA(dk_time) / elapsed;
									}
								}
								fprintf(fp, show_rrd ? ":%.1f" : ",%.1f", (float)(disk_total / dgroup_disks[k]));
							}
						}
						fprintf(fp, "\n");
						fprintf(fp, show_rrd ? "rrdtool update dgread.rdd %s" : "DGREAD,%s", LOOP);
						for (k = 0; k < dgroup_total_groups; k++) {
							if (dgroup_name[k] != 0) {
								disk_total = 0.0;
								for (j = 0; j < dgroup_disks[k]; j++) {
									i = dgroup_data[k*DGROUPITEMS+j];
									if (i != -1) {
										/*
										 disk_total += DKDELTA(dk_reads) * p->dk[i].dk_bsize / 1024.0;
										 */
										disk_total += DKDELTA(dk_rkb);
									}
								}
								fprintf(fp, show_rrd ? ":%.1f" : ",%.1f", disk_total / elapsed);
							}
						}
						fprintf(fp, "\n");
						fprintf(fp, show_rrd ? "rrdtool update dgwrite.rdd %s" : "DGWRITE,%s", LOOP);
						for (k = 0; k < dgroup_total_groups; k++) {
							if (dgroup_name[k] != 0) {
								disk_total = 0.0;
								for (j = 0; j < dgroup_disks[k]; j++) {
									i = dgroup_data[k*DGROUPITEMS+j];
									if (i != -1) {
										/*
										 disk_total += DKDELTA(dk_writes) * p->dk[i].dk_bsize / 1024.0;
										 */
										disk_total += DKDELTA(dk_wkb);
									}
								}
								fprintf(fp, show_rrd ? ":%.1f" : ",%.1f", disk_total / elapsed);
							}
						}
						fprintf(fp, "\n");
						fprintf(fp, show_rrd ? "rrdtool update dgbsize.rdd %s" : "DGSIZE,%s", LOOP);
						for (k = 0; k < dgroup_total_groups; k++) {
							if (dgroup_name[k] != 0) {
								disk_write = 0.0;
								disk_xfers  = 0.0;
								for (j = 0; j < dgroup_disks[k]; j++) {
									i = dgroup_data[k*DGROUPITEMS+j];
									if (i != -1) {
										/*
										 disk_write += (DKDELTA(dk_reads) + DKDELTA(dk_writes) ) * p->dk[i].dk_bsize / 1024.0;
										 */
										disk_write += (DKDELTA(dk_rkb) + DKDELTA(dk_wkb) );
										disk_xfers  += DKDELTA(dk_xfers);
									}
								}
								if ( disk_write == 0.0 || disk_xfers == 0.0)
									disk_size = 0.0;
								else
									disk_size = disk_write / disk_xfers;
								fprintf(fp, show_rrd ? ":%.1f" : ",%.1f", disk_size);
							}
						}
						fprintf(fp, "\n");
						fprintf(fp, show_rrd ? "rrdtool update dgxfer.rdd %s" : "DGXFER,%s", LOOP);
						for (k = 0; k < dgroup_total_groups; k++) {
							if (dgroup_name[k] != 0) {
								disk_total = 0.0;
								for (j = 0; j < dgroup_disks[k]; j++) {
									i = dgroup_data[k*DGROUPITEMS+j];
									if (i != -1) {
										disk_total  += DKDELTA(dk_xfers);
									}
								}
								fprintf(fp, show_rrd ? ":%.1f" : ",%.1f", disk_total / elapsed);
							}
						}
						fprintf(fp, "\n");
						
						if( extended_disk == 1 && disk_mode == DISK_MODE_DISKSTATS )	{
							fprintf(fp,"DGREADS,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_reads);
										}
									}
									fprintf(fp,",%.1f", disk_total / elapsed);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGREADMERGE,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_rmerge);
										}
									}
									fprintf(fp,",%.1f", disk_total / elapsed);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGREADSERV,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_rmsec);
										}
									}
									fprintf(fp,",%.1f", disk_total);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGWRITES,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_writes);
										}
									}
									fprintf(fp,",%.1f", disk_total / elapsed);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGWRITEMERGE,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_wmerge);
										}
									}
									fprintf(fp,",%.1f", disk_total / elapsed);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGWRITESERV,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_wmsec);
										}
									}
									fprintf(fp,",%.1f", disk_total);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGINFLIGHT,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += p->dk[i].dk_inflight;
										}
									}
									fprintf(fp,",%.1f", disk_total);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGIOTIME,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_time);
										}
									}
									fprintf(fp,",%.1f", disk_total);
								}
							}
							fprintf(fp, "\n");
							fprintf(fp,"DGBACKLOG,%s", LOOP);
							for (k = 0; k < dgroup_total_groups; k++) {
								if (dgroup_name[k] != 0) {
									disk_total = 0.0;
									for (j = 0; j < dgroup_disks[k]; j++) {
										i = dgroup_data[k*DGROUPITEMS+j];
										if (i != -1) {
											disk_total  += DKDELTA(dk_backlog);
										}
									}
									fprintf(fp,",%.1f", disk_total);
								}
							}
							fprintf(fp, "\n");
						} /* if( extended_disk == 1 && disk_mode == DISK_MODE_DISKSTATS */
					}	/* if (dgroup_loaded == 2) */

// ~~~~~~~~~~~~~
// xport_part_15
// ~~~~~~~~~~~~~

									if((cmdfound && cmdcheck(p->procs[i].pi_comm)) || 
									   (!cmdfound && ((topper[j].time / elapsed) > ignore_procdisk_threshold)) )
									{
												fprintf(fp,"TOP,%07d,%s,%.2f,%.2f,%.2f,%lu,%lu,%lu,%lu,%lu,%d,%d,%s,%ld,%llu\n",
														/* 1 */ p->procs[i].pi_pid,
														/* 2 */ LOOP,
														/* 3 */ topper[j].time / elapsed,
														/* 4 */ TIMEDELTA(pi_utime,i,topper[j].other) / elapsed,
														/* 5 */ TIMEDELTA(pi_stime,i,topper[j].other) / elapsed,
														/* 6 */ p->procs[i].statm_size*pagesize/1024UL, /* in KB */
														/* 7 */ p->procs[i].statm_resident*pagesize/1024UL, /* in KB */
														/* 8 */ p->procs[i].statm_trs*pagesize/1024UL, /* in KB */
														/* 9 */ p->procs[i].statm_drs*pagesize/1024UL, /* in KB */
														/* 10*/ p->procs[i].statm_share*pagesize/1024UL, /* in KB */
														/* 11*/ (int)(COUNTDELTA(pi_minflt) / elapsed),
														/* 12*/ (int)(COUNTDELTA(pi_majflt) / elapsed),
														/* 13*/ p->procs[i].pi_comm
												,
												p->procs[i].pi_num_threads,
												COUNTDELTA(pi_delayacct_blkio_ticks)
												);
										
										if(show_args)
											args_output(p->procs[i].pi_pid,loop, p->procs[i].pi_comm);
										
									} else skipped++;

// ~~~~~~~~~~~~~
// xport_part_16
// ~~~~~~~~~~~~~

				fflush(NULL);
				
				gettimeofday(&nmon_tv, 0);
				nmon_end_time = (double)nmon_tv.tv_sec + (double)nmon_tv.tv_usec * 1.0e-6;
				if(nmon_run_time  < 0.0){
					nmon_start_time = nmon_end_time;
					nmon_run_time = 0.0;
				}
				nmon_run_time += (nmon_end_time - nmon_start_time);
				if(nmon_run_time < 1.0) {
					secs = seconds;  /* sleep for the requested number of seconds */
				}
				else {
					seconds_over = (int)nmon_run_time; /* reduce the sleep time by whole number of seconds */
					secs = seconds - seconds_over;
					nmon_run_time -= (double)seconds_over;
				}
				if(secs < 1) /* sanity check in case CPUs are flat out and nmon taking far to long to complete */
					secs = 1;
				
			redo:
				errno = 0;
				ret = sleep(secs); 
				if( (ret != 0 || errno != 0) && loop != maxloops ) {
					fprintf(fp,"ERROR,%s, sleep interrupted, sleep(%d seconds), return value=%d",LOOP, secs, ret);
					fprintf(fp,", errno=%d\n",errno);
					secs=ret;
					goto redo;
				}
				gettimeofday(&nmon_tv, 0);
				nmon_start_time = (double)nmon_tv.tv_sec + (double)nmon_tv.tv_usec * 1.0e-6;

// ~~~~~~~~~~~~~
// xport_part_17
// ~~~~~~~~~~~~~

	if(firsttime) {
		fprintf(fp,"CPUTICKS_ALL,AAA,user,sys,wait,idle,nice,irq,softirq,steal\n");
		fprintf(fp,"CPUTICKS%03d,AAA,user,sys,wait,idle,nice,irq,softirq,steal\n", cpu_no);
		firsttime=0;
	}
	if(cpu_no==0) {
		fprintf(fp,"CPUTICKS_ALL,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
				LOOP, user, kernel, iowait, idle, nice, irq, softirq, steal);
	} else {
		fprintf(fp,"CPUTICKS%03d,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
				cpu_no, LOOP, user, kernel, iowait, idle, nice, irq, softirq, steal);
	}

// ~~~~~~~~~~~~~
// xport_part_18
// ~~~~~~~~~~~~~

		/* Sanity check the numnbers */
		if( user < 0.0 || kernel < 0.0 || iowait < 0.0 || idle < 0.0 || idle >100.0 || steal <0 ) {
			user = kernel = iowait = idle = steal = 0;
		}
		
		if(first_steal && steal >0 ) {
			fprintf(fp,"AAA,steal,1\n");
			first_steal=0;
		}
		if(cpu_no == 0)
			fprintf(fp,"CPU_ALL,%s,%.1lf,%.1lf,%.1lf,%.1f,%.1lf,,%d\n", LOOP,
					user, kernel, iowait, idle, steal, cpus);
		else {
			fprintf(fp,"CPU%03d,%s,%.1lf,%.1lf,%.1lf,%.1lf,%.1f\n", cpu_no, LOOP,
					user, kernel, iowait, idle, steal );
		}


