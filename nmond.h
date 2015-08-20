#ifndef NMOND_H
#define NMOND_H

/*
 * nmond.h -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *  Christopher Stoll (https://github.com/stollcri)
 *  
 *   forked from:
 *   lmon.c -- Curses based Performance Monitor for Linux
 *   Developer: Nigel Griffiths.
 */

#define VERSION "15g"
#define LARGEMEM 1

#define P_CPUINFO	0
#define P_STAT		1
#define P_VERSION	2
#define P_MEMINFO 	3
#define P_UPTIME 	4
#define P_LOADAVG	5
#define P_NFS 		6
#define P_NFSD 		7
#define P_VMSTAT	8 /* new in 13h */
#define P_NUMBER	9 /* one more than the max */

#endif