/**
 * nmond_ui_cli.c -- Command line user interface elements of nmond
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 */

#include "nmond_ui_cli.h"
#include <stdio.h>

void uiclhint()
{
	printf("\nHint: nmond [-h] [-s <seconds>] [-c <count>] [-f -d <disks> -t -r <name>] [-x]\n\n");
	printf("\t-h            FULL help information\n");
	printf("\tInteractive-Mode:\n");
	printf("\tread startup banner and type: \"h\" once it is running\n");
	printf("\tFor Data-Collect-Mode (-f)\n");
	printf("\t-f            spreadsheet output format [note: default -s300 -c288]\n");
	printf("\toptional\n");
	printf("\t-s <seconds>  between refreshing the screen [default 2]\n");
	printf("\t-c <number>   of refreshes [default millions]\n");
	printf("\t-d <disks>    to increase the number of disks [default 256]\n");
	printf("\t-t            spreadsheet includes top processes\n");
	printf("\t-x            capacity planning (15 min for 1 day = -fdt -s 900 -c 96)\n");
	printf("\n");
}

void uiclhelp(char *version)
{
	uiclhint();
	printf("nmond %s\n\n", version);
	printf("For Interactive-Mode\n");
	printf("\t-s <seconds>  time between refreshing the screen [default 2]\n");
	printf("\t-c <number>   count of screen refreshes [default millions]\n");
	printf("\t-g <filename> User Defined Disk Groups [hit g to show them]\n");
	printf("\t              - file = on each line: group_name <disks list> space separated\n");
	printf("\t              - like: database sdb sdc sdd sde\n");
	printf("\t              - upto 64 disk groups, 512 disks per line\n");
	printf("\t              - disks can appear more than once and in many groups\n");
	printf("\t-g auto       - will make a file called \"auto\" with just disks fron \"lsblk|grep disk\" output\n");
	printf("\t-b            black and white [default is colour]\n");
	printf("\texample: nmond -s 1 -c 100\n");
	printf("\n");
	printf("For Data-Collect-Mode = spreadsheet format (comma separated values)\n");
	printf("\tNote: use only one of f,F,z,x or X and make it the first argument\n");
	printf("\t-f            spreadsheet output format [note: default -s300 -c288]\n");
	printf("\t\t\t output file is <hostname>_YYYYMMDD_HHMM.nmon\n");
	printf("\t-F <filename> same as -f but user supplied filename\n");
	printf("\t-r <runname>  used in the spreadsheet file [default hostname]\n");
	printf("\t-t            include top processes in the output\n");
	printf("\t-T            as -t plus saves command line arguments in UARG section\n");
	printf("\t-s <seconds>  between snap shots\n");
	printf("\t-c <number>   of snapshots before nmon stops\n");
	printf("\t-d <disks>    to increase the number of disks [default 256]\n");
	printf("\t-l <dpl>      disks/line default 150 to avoid spreadsheet issues. EMC=64.\n");
	printf("\t-g <filename> User Defined Disk Groups (see above) - see BBBG & DG lines\n");
	printf("\t-g auto       As above but makes the file \"auto\" for you of just the disks like sda etc.\n");
	printf("\t-D            Use with -g to add the Disk wait/service time & inflight stats.\n");
	printf("\t-N            include NFS Network File System\n");
	printf("\t-I <percent>  Include process & disks busy threshold (default 0.1)\n");
	printf("\t              don't save or show proc/disk using less than this percent\n");
	printf("\t-m <directory> nmon changes to this directory before saving to file\n");
	printf("\texample: collect for 1 hour at 30 second intervals with top procs\n");
	printf("\t\t nmond -f -t -r Test1 -s30 -c120\n");
	printf("\n");
	printf("\tTo load into a spreadsheet:\n");
	printf("\tsort -A *nmon >stats.csv\n");
	printf("\ttransfer the stats.csv file to your PC\n");
	printf("\tStart spreadsheet & then Open type=comma-separated-value ASCII file\n");
	printf("\t The nmon analyser or consolidator does not need the file sorted.\n");
	printf("\n");
	printf("Capacity planning mode - use cron to run each day\n");
	printf("\t-x            sensible spreadsheet output for CP =  one day\n");
	printf("\t              every 15 mins for 1 day ( i.e. -ft -s 900 -c 96)\n");
	printf("\t-X            sensible spreadsheet output for CP = busy hour\n");
	printf("\t              every 30 secs for 1 hour ( i.e. -ft -s 30 -c 120)\n");
	printf("\n");
	printf("Interactive Mode Commands\n");
	printf("\tkey --- Toggles to control what is displayed ---\n");
	printf("\th   = Online help information\n");
	printf("\tr   = Machine type, machine name, cache details and OS version + LPAR\n");
	printf("\tc   = CPU by processor stats with bar graphs\n");
	printf("\tl   = long term CPU (over 75 snapshots) with bar graphs\n");
	printf("\tm   = Memory stats\n");
	printf("\tL   = Huge memory page stats\n");
	printf("\tV   = Virtual Memory and Swap stats\n");
	printf("\tk   = Kernel Internal stats\n");
	printf("\tn   = Network stats and errors\n");
	printf("\tN   = NFS Network File System\n");
	printf("\td   = Disk I/O Graphs\n");
	printf("\tD   = Disk I/O Stats\n");
	printf("\to   = Disk I/O Map (one character per disk showing how busy it is)\n");
	printf("\tg   = User Defined Disk Groups        (assumes -g <file> when nmon started)\n");
	printf("\tG   = Change Disk stats to just disks (assumes -g auto   when nmon started)\n");
	printf("\tj   = File Systems \n");
	printf("\tt   = Top Process stats: select the data & order 1=Basic, 3=Perf 4=Size 5=I/O=root only\n");
	printf("\tt   = Top Process stats use 1,3,4,5 to select the data & order\n");
	printf("\tu   = Top Process full command details\n");
	printf("\tv   = Verbose mode - tries to make recommendations\n");
	printf("\tP   = Partitions Disk I/O Stats\n");
	printf("\tb   = black and white mode (or use -b option)\n");
	printf("\t.   = minimum mode i.e. only busy disks and processes\n");
	printf("\n");
	printf("\tkey --- Other Controls ---\n");
	printf("\t+   = double the screen refresh time\n");
	printf("\t-   = halves the screen refresh time\n");
	printf("\tq   = quit (also x, e or control-C)\n");
	printf("\t0   = reset peak counts to zero (peak = \">\")\n");
	printf("\tspace = refresh screen now\n");
	printf("\n");
	printf("Startup Control\n");
	printf("\tIf you find you always type the same toggles every time you start\n");
	printf("\tthen place them in the NMON shell variable. For example:\n");
	printf("\t export NMON=cmdrvtan\n");
	printf("\n");
	printf("Others:\n");
	printf("\ta) To you want to stop nmon - kill -USR2 <nmon-pid>\n");
	printf("\tb) Use -p and nmon outputs the background process pid\n");
	printf("\tc) To limit the processes nmon lists (online and to a file)\n");
	printf("\t   Either set NMONCMD0 to NMONCMD63 to the program names\n");
	printf("\t   or use -C cmd:cmd:cmd etc. example: -C ksh:vi:syncd\n");
	printf("\td) If you want to pipe nmon output to other commands use a FIFO:\n");
	printf("\t   mkfifo /tmp/mypipe\n");
	printf("\t   nmon -F /tmp/mypipe &\n");
	printf("\t   grep /tmp/mypipe\n");
	printf("\te) If nmon fails please report it with:\n");
	printf("\t   1) nmon version like: %s\n", version);
	printf("\t   2) the output of cat /proc/cpuinfo\n");
	printf("\t   3) some clue of what you were doing\n");
	printf("\t   4) I may ask you to run the debug version\n");
	printf("\n");
	printf("\tDeveloper Nigel Griffiths\n");
	printf("\tFeedback welcome - on the current release only and state exactly the problem\n");
	printf("\tNo warranty given or implied.\n");
}
