# nmond Roadmap

## v0.1 -- betas
- Work-in-progress builds

## v0.2 -- MVP
- ☐ APP: Make [GUI portion of] the program run without /proc files
	- ✓ Remove code for specail cases (IFDEFs)
	- ☐ Get data from sysctl and other low-level C calls
	- ✓ Pull out UI elements
	- ✓ Streamline main()
- ☐ APP: Create long output for: cpu, mem, disk, net
- ☐ GUI: Update layout, text, and color (see: http://elmon.sourceforge.net)
	- ☐ Improve spacing
		- ✓ CPU Load
		- ✓ CPU Long-Term
		- ✓ Top [processes]
		- ☐ 
	- ☐ Improve header line
		- ✓ Remove version number
		- ✓ Remove blinking "H for help" (prehaps replace with alert messages)
		- ✓ Move hostname over and change wording
		- ✓ Change refresh rate wording
	- ✓ Display splash screen when nothing is selected
	- ☐ Only update changed portions of windows rather than recreating entire windows each pass
	- ✓ CPU Load: remove top/bottom graph bars
	- ✓ CPU Load: increase number of divisions on scale
	- ✓ CPU L-T: remove top/bottom graph bars, move legend to top line (begins with "CPU")
	- ☐ Improve ncurses use (http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/printw.html)
	- ✗ Move titles to the left side
- ☐ APP: restructure command options (lower-case for horizontal graph, upper-case for vertical graph)
	- ☐      a -- All-in-one display (cpu long + disk + mem + net)
	- ☐      A -- All-in-one raw data display
	- ☐ b => b -- black & white mode
	- ✓ c => c -- CPU Load
	- ✓ l => C -- CPU Load, long-term
	- ☐ d => d -- Disk I/O
	- ☐      D -- Disk I/O, long-term
	- ☐ j => f -- Filesystems (JFS)
	- ☐ N => F -- Filesystems (NFS)
	- ☐ g      -- (removed: user defined disk groups)
	- ✓ h => h -- help
	- ✓ H => H -- help
	- ✓ r => i -- Info A
	- ☐      I -- Info B
	- ☐ k => k -- Kernel Stats
	- ☐ L      -- (removed: large memory)
	- ☐ m => m -- Memory & Swap
	- ☐      M -- Memory & Swap
	- ☐ n => n -- Network
	- ☐      N -- Network, long-term
	- ☐ o => o -- Disk busy map
	- ☐ q => q -- quit
	- ✓ t => t -- Top
	- ☐ u => T -- Top, show command arguments
	- ☐ V => v -- Virtual Memory
	- ☐ v => w -- warn mode
	- ☐ x      -- (removed: exit)
	- ☐ 0 => z -- zero out peak markers
	- ☐ 0 => 0 -- sort by
	- ☐ 1 => 1 -- sort by
	- ☐ 2 => 2 -- sort by
	- ☐ 3 => 3 -- sort by
	- ☐ 4 => 4 -- sort by
	- ☐ 5 => 5 -- sort by
	- ✓ + => + -- double refresh rate
	- ✓ - => - -- half refresh rate
	- ☐ .      -- (removed: minimal display)
	- ✓ ? => ? -- help
- ☐ APP: Static analysis
	- ☐ malloc
	- ☐ strcpy -- use strncpy or strlcpy (OpenBSD)
	- ☐ strcat -- use strncat or strlcat (OpenBSD)
	- ☐ printf
	- ☐ scanf -- use fgets
	- ☐ gets -- use fgets
	- ☐ make sure variables are nulled after free to prevent double free (e.g. free(x); x = NULL;)
- ☐ APP: Create a manpage

## v0.4 -- Phase 2
- GUI: Update layout, text, and color (see: http://elmon.sourceforge.net)
	- Option for VT100 box drawings (https://en.wikipedia.org/wiki/Box-drawing_character#Unix.2C_CP.2FM.2C_BBS)
	- Option for UTF8 box drawings (https://en.wikipedia.org/wiki/Box-drawing_character)
	- Change cursor location
- INF: Read/Display CPU frequency (vs min and max)
- INF: Read/Display HDD/CPU/GPU temperature

## v0.8 -- Phase 3
- APP: Add 'logging to an external file' capabilities back
- APP: Full logging system: automatically roll up data for different intervals (hour/day/week/month)
- APP: Pipe logged data in to stdin for replay (and generate averages at different intervals)
- APP: Visual/Audio alerts for alarm levels (temperature, sustained utliization, etc.)
- GUI: Allow for window scrolling (up/down arrows and mouse)
	- `scrollok()`
	- `int wscrl(WINDOW *win, int n);`

## v1.0 -- Phase 4
- INF: Add GPU load stats
- GUI: Optional log scale
- GUI: add disk I/O below long CPU output
- GUI: Use ncurses progress bars for vertical bars
- GUI: CPU L-T: follow processes, show their usage in the graph
- GUI: CPU L-T: drop top-bar down (show gap) when cpu speed is decreased
- GUI: CPU L-T: generalize chart so that it can be used for other data sets
- GUI: CPU L-T: show last data (faded, behind) for each bar in the graph (doubles visible data points)
- GUI: CPU L-T: show candlestick chart instead of bar graph

## v1.6 -- Phase 5
- APP: Plugins

# Development Notes

## VT100 Tricks
- Use double height text characters (the top half is printed on one line, the bottom half on another)
	- `echo $'\e'#3abcdefg; echo $'\e'#4abcdefg`
	- `echo -e "\033#3abcdefg"; echo -e "\033#4abcdefg"`
- Use box drawing characters
	- `echo $'\e'\(0 "( abcdef | ghijklm | nopqrst | uvwxyz )" $'\e'\(B`
	- `echo -e "\033(0 ( abcdef | ghijklm | nopqrst | uvwxyz ) \033(B"`

## Data Fields

### top
- PID
- USER
- PR -- priority (0, 20)
- NI -- nice (-20, 0)
- VIRT -- virtual memory
- RES -- resource
- SHR
- S -- status (R, S)
- %CPU
- %MEM
- TIME+ -- total CPU time
- COMMAND

### nmon
#### r - Linux and Processor Details
- Linux: Linux version 3.2.0-80-virtual (buildd@batsu)
- Build: (gcc version 4.6.3 (Ubuntu/Linaro 4.6.3-1ubuntu5) )
- Release: 3.2.0-80-virtual
- Version: #116-Ubuntu SMP Mon Mar 23 17:28:52 UTC 2015
- cpuinfo: model name : Intel(R) Xeon(R) CPU E5-2690 0 @ 2.90GHz
- cpuinfo: vendor_id : GenuineIntel
- cpuinfo: microcode : 0x70d
- cpuinfo: cpuid level : 10
- # of CPUs: 2
- Machine  : x86_64
- Nodename : a-web-1
- /etc/*ease[1]: DISTRIB_ID=Ubuntu
- /etc/*ease[2]: DISTRIB_RELEASE=12.04
- /etc/*ease[3]: DISTRIB_CODENAME=precise
- /etc/*ease[4]: DISTRIB_DESCRIPTION="Ubuntu 12.04.5 LTS"
- lsb_release: Distributor ID: Ubuntu
- lsb_release: Description: Ubuntu 12.04.5 LTS
- lsb_release: Release: 12.04
- lsb_release: Codename: precise

#### t -- Top Processes
##### 3=Perf / 4=Size / 5=I/O
Each options shows the same output, the srot order is just changed
- PID
- %CPU / Used
- Size / KB
- Res / Set
- Res / Text
- Res / Data
- Res / Lib
- Shared / KB
- Faults / Min
- Faults / Maj
- Command
##### 1=Basic
- PID
- PPID
- Pgrp
- Nice (-20, 0)
- Prio (-100, 0, 20)
- Status  (Running, Sleeping)
- proc-Flag (0x00402100, 0x84208040)
- ?
- Command
##### u (mode=3)
- PID
- %CPU / Used
- ResSize / KB
- Command (full command path)

#### c -- CPU Utilizaton
- one line per cpu
- one line with average
	- CPU -- cpu number
	- User%
	- Sys%
	- Wiat%
	- Idle (Idle%)
	- bar graph

#### l -- CPU (long-term)
- bar graph of cpu usage (user, sys, wait)

#### m -- Memory Stats
- Total MB
- Free MB
- Free Percent
	- RAM
	- High
	- Low
	- Swap
- Cached
- Active
- Buffers
- Swapcached
- Inactive
- Dirty
- Writeback
- Mapped
- Slab
- Commit_AS
- PageTables

#### j -- Filesystems
- Filesystem
- SizeMB
- FreeMB
- %Used
- Type
- MountPoint

#### n -- Network I/O
- I/F Name
- Recv=KB/s
- Trans/KB/s
- packin
- pakout
- insize
- outsize
- Peak->Recv
- Trans

#### N -- Network Filesystems (NFS) I/O
- Version 2
- Client
- Server
- Version 3
- Client
- Server

#### d -- Disk I/O
- DiskName
- Busy
- Read
- Write
- (bar graph)

##### o -- Disk %Busy Map
- (simplified bar graph)

##### k -- Kernel Stats
- RunQueue
- ContextSwitch
- Forks
- Interrupts
- Load Average
	- 1 mins
	- 5 mins
	- 15 mins
- CPU use since boot time
	- Uptime
	- Idle
	- Average CPU use

##### V -- Virtual Memory
(TODO: complete)

# procs
```
	print_procs(int index)
	{
		printf("procs[%d].pid           =%d\n",index,procs[index].pi_pid);
		printf("procs[%d].comm[0]       =%s\n",index,&procs[index].pi_comm[0]);
		printf("procs[%d].state         =%c\n",index,procs[index].pi_state);
		printf("procs[%d].ppid          =%d\n",index,procs[index].pi_ppid);
		printf("procs[%d].pgrp          =%d\n",index,procs[index].pi_pgrp);
		printf("procs[%d].session       =%d\n",index,procs[index].pi_session);
		printf("procs[%d].tty_nr        =%d\n",index,procs[index].pi_tty_nr);
		printf("procs[%d].tty_pgrp      =%d\n",index,procs[index].pi_tty_pgrp);
		printf("procs[%d].flags         =%lu\n",index,procs[index].pi_flags);
		printf("procs[%d].minflt       =%lu\n",index,procs[index].pi_minflt);
		printf("procs[%d].cmin_flt     =%lu\n",index,procs[index].pi_cmin_flt);
		printf("procs[%d].majflt       =%lu\n",index,procs[index].pi_majflt);
		printf("procs[%d].cmaj_flt     =%lu\n",index,procs[index].pi_cmaj_flt);
		printf("procs[%d].utime        =%lu\n",index,procs[index].pi_utime);
		printf("procs[%d].stime        =%lu\n",index,procs[index].pi_stime);
		printf("procs[%d].cutime       =%ld\n",index,procs[index].pi_cutime);
		printf("procs[%d].cstime       =%ld\n",index,procs[index].pi_cstime);
		printf("procs[%d].pri           =%d\n",index,procs[index].pi_pri);
		printf("procs[%d].nice          =%d\n",index,procs[index].pi_nice);
		// #ifndef KERNEL_2_6_18
		printf("procs[%d].junk          =%d\n",index,procs[index].junk);
		// #else
		printf("procs[%d].num_threads   =%ld\n",index,procs[index].num_threads);
		// #endif
		printf("procs[%d].it_real_value =%lu\n",index,procs[index].pi_it_real_value);
		printf("procs[%d].start_time    =%lu\n",index,procs[index].pi_start_time);
		printf("procs[%d].vsize         =%lu\n",index,procs[index].pi_vsize);
		printf("procs[%d].rss           =%lu\n",index,procs[index].pi_rss);
		printf("procs[%d].rlim_cur      =%lu\n",index,procs[index].pi_rlim_cur);
		printf("procs[%d].start_code    =%lu\n",index,procs[index].pi_start_code);
		printf("procs[%d].end_code      =%lu\n",index,procs[index].pi_end_code);
		printf("procs[%d].start_stack   =%lu\n",index,procs[index].pi_start_stack);
		printf("procs[%d].esp           =%lu\n",index,procs[index].pi_esp);
		printf("procs[%d].eip           =%lu\n",index,procs[index].pi_eip);
		printf("procs[%d].pending_signal=%lu\n",index,procs[index].pi_pending_signal);
		printf("procs[%d].blocked_sig   =%lu\n",index,procs[index].pi_blocked_sig);
		printf("procs[%d].sigign        =%lu\n",index,procs[index].pi_sigign);
		printf("procs[%d].sigcatch      =%lu\n",index,procs[index].pi_sigcatch);
		printf("procs[%d].wchan         =%lu\n",index,procs[index].pi_wchan);
		printf("procs[%d].nswap         =%lu\n",index,procs[index].pi_nswap);
		printf("procs[%d].cnswap        =%lu\n",index,procs[index].pi_cnswap);
		printf("procs[%d].exit_signal   =%d\n",index,procs[index].pi_exit_signal);
		printf("procs[%d].cpu           =%d\n",index,procs[index].pi_cpu);
		// #ifdef KERNEL_2_6_18
		printf("procs[%d].rt_priority   =%lu\n",index,procs[index].pi_rt_priority);
		printf("procs[%d].policy        =%lu\n",index,procs[index].pi_policy);
		printf("procs[%d].delayacct_blkio_ticks=%llu\n",index,procs[index].pi_delayacct_blkio_ticks);
		// #endif // KERNEL_2_6_18
		printf("OK\n");
	}
```
