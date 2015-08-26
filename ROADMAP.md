# Roadmap
## MVP
- Make the program run without /proc files
	- Get data from sysctl
	- Pull out UI elements
- Check memory allocation and freeing
## Phase 2
- Update UI (layout, text, and color; see: http://elmon.sourceforge.net)
- Change cursor location
## Phase 3
- Pipe logged data in to stdin for replay
- For long-term CPU: use box-drawing characters (UTF-8 or VT100) for border instead of text characters (default option)
- For long-term CPU: drop top-bar down (show gap) when cpu speed is decreased
## Phase 4
- Use ncurses progress bars for vertical bars
- For long-term CPU: follow processes, show their usage in the graph
- For long-term CPU: generalize chart so that it can be used for other data sets
- For long-term CPU: show last data (faded, behind) for each bar in the graph (doubles visible data points)
- For long-term CPU: show candlestick chart instead of bar graph

# VT100 Tricks
- Use double height text characters (the top half is printed on one line, the bottom half on another)
	- `echo $'\e'#3abcdefg; echo $'\e'#4abcdefg`
	- `echo -e "\033#3abcdefg"; echo -e "\033#4abcdefg"`
- Use box drawing characters
	- `echo $'\e'\(0 "( abcdef | ghijklm | nopqrst | uvwxyz )" $'\e'\(B`
	- `echo -e "\033(0 ( abcdef | ghijklm | nopqrst | uvwxyz ) \033(B"`

# Data Fields
## top
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

## nmon
### r - Linux and Processor Details
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

### t -- Top Processes
#### 3=Perf / 4=Size / 5=I/O
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
#### 1=Basic
- PID
- PPID
- Pgrp
- Nice (-20, 0)
- Prio (-100, 0, 20)
- Status  (Running, Sleeping)
- proc-Flag (0x00402100, 0x84208040)
- ?
- Command
#### u (mode=3)
- PID
- %CPU / Used
- ResSize / KB
- Command (full command path)

### c -- CPU Utilizaton
- one line per cpu
- one line with average
	- CPU -- cpu number
	- User%
	- Sys%
	- Wiat%
	- Idle (Idle%)
	- bar graph

### l -- CPU (long-term)
- bar graph of cpu usage (user, sys, wait)

### m -- Memory Stats
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

### j -- Filesystems
- Filesystem
- SizeMB
- FreeMB
- %Used
- Type
- MountPoint

### n -- Network I/O
- I/F Name
- Recv=KB/s
- Trans/KB/s
- packin
- pakout
- insize
- outsize
- Peak->Recv
- Trans

### N -- Network Filesystems (NFS) I/O
- Version 2
- Client
- Server
- Version 3
- Client
- Server

### d -- Disk I/O
- DiskName
- Busy
- Read
- Write
- (bar graph)

#### o -- Disk %Busy Map
- (simplified bar graph)

#### k -- Kernel Stats
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

#### V -- Virtual Memory
(TODO: complete)

# New Logos

rectangles
```
                     _ 
 ___ _____ ___ ___ _| |
|   |     | . |   | . |
|_|_|_|_|_|___|_|_|___|
```
slant
```
                                    __
   ____  ____ ___  ____  ____  ____/ /
  / __ \/ __ `__ \/ __ \/ __ \/ __  / 
 / / / / / / / / / /_/ / / / / /_/ /  
/_/ /_/_/ /_/ /_/\____/_/ /_/\__,_/   
```
small slant
```
                           __
  ___  __ _  ___  ___  ___/ /
 / _ \/  ' \/ _ \/ _ \/ _  / 
/_//_/_/_/_/\___/_//_/\_,_/  
```
soft
```
                                    ,--. 
,--,--, ,--,--,--. ,---. ,--,--,  ,-|  | 
|      \|        || .-. ||      \' .-. | 
|  ||  ||  |  |  |' '-' '|  ||  |\ `-' | 
`--''--'`--`--`--' `---' `--''--' `---' 
```
standard
```
                                  _ 
  _ __  _ __ ___   ___  _ __   __| |
 | '_ \| '_ ` _ \ / _ \| '_ \ / _` |
 | | | | | | | | | (_) | | | | (_| |
 |_| |_|_| |_| |_|\___/|_| |_|\__,_|
 ```
 ANSI shadow
 ```
 ███╗   ██╗███╗   ███╗ ██████╗ ███╗   ██╗██████╗ 
████╗  ██║████╗ ████║██╔═══██╗████╗  ██║██╔══██╗
██╔██╗ ██║██╔████╔██║██║   ██║██╔██╗ ██║██║  ██║
██║╚██╗██║██║╚██╔╝██║██║   ██║██║╚██╗██║██║  ██║
██║ ╚████║██║ ╚═╝ ██║╚██████╔╝██║ ╚████║██████╔╝
╚═╝  ╚═══╝╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝╚═════╝ 
```

JS Block Letters
```
 __  _  __  __  ____  __  _  ____ 
|  \| ||  \/  |/ () \|  \| || _) \
|_|\__||_|\/|_|\____/|_|\__||____/
```