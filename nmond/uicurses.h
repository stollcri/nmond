#ifndef UICURSES_H
#define UICURSES_H

/**
 * uicurses.h -- Ncurses user interface elements of nmond
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
#include <ncurses.h>
#include <stdbool.h>
#include <sys/time.h>

#define APPNAME "nmond"
#define VERSION "0.1.10"
#define VERDATE "2015-10-12"
#define APPURL "https://github.com/stollcri/nmond"

#define MAXROWS 256
#define MAXCOLS 150
#define BORDER_WIDTH 2

#define DISK_METER_MODE 1  // change me
#define DISK_METER_MB 0    // do NOT change
#define DISK_METER_LOG 1   // do NOT change
#define DISK_METER_SCALE 2 // do NOT change

#define TOP_MODE_NONE 0
#define TOP_MODE_A 1
#define TOP_MODE_B 2
#define TOP_MODE_C 3
#define TOP_MODE_D 4

#define MSG_WRN_NOT_SHOWN "Warning: Some Statistics may not shown"

struct uiwin {
	WINDOW *win;
	bool visible;
	int height;
};
struct uiwins {
	int visiblecount;

	struct uiwin welcome;
	struct uiwin help;
	struct uiwin cpu;
	struct uiwin cpulong;

	struct uiwin disks;
	struct uiwin disklong;
	struct uiwin diskgroup;
	struct uiwin diskmap;
	struct uiwin filesys;

	struct uiwin kernel;
	struct uiwin memory;
	struct uiwin memlarge;
	struct uiwin memvirtual;

	struct uiwin neterrors;
	struct uiwin netfilesys;
	struct uiwin network;
	struct uiwin netlong;
	struct uiwin top;

	struct uiwin sys;
	struct uiwin warn;
};
#define UIWINS_INIT { 0, \
{NULL, false}, {NULL, false}, {NULL, false}, {NULL, false}, \
{NULL, false}, {NULL, false}, {NULL, false}, {NULL, false}, {NULL, false}, \
{NULL, false}, {NULL, false}, {NULL, false}, {NULL, false}, \
{NULL, false}, {NULL, false}, {NULL, false}, {NULL, false}, {NULL, false}, \
{NULL, false}, {NULL, false} }

extern void uiheader(WINDOW**, int, int, int, char*, char*, double, time_t);

extern void uiwelcome(WINDOW**, int, int*, int, int, int, struct syshw);
extern void uihelp(WINDOW**, int, int*, int, int);
extern void uicpu(WINDOW**, int, int*, int, int, int, struct sysres, int);
extern void uicpulong(WINDOW**, int, int*, int, int, int*, int, struct sysres, bool);

extern void uidisks(WINDOW**, int, int*, int, int, int, unsigned int, unsigned int);
extern void uidisklong(WINDOW**, int, int*, int, int, int*, int, unsigned int, unsigned int, bool);
extern void uidiskgroup(WINDOW**, int, int*, int, int);
extern void uidiskmap(WINDOW**, int, int*, int, int);
extern void uifilesys(WINDOW**, int, int*, int, int);
extern void uikernel(WINDOW**, int, int*, int, int);
extern void uimemory(WINDOW**, int, int*, int, int, int, unsigned long long, unsigned long long);
extern void uimemlarge(WINDOW**, int, int*, int, int);
extern void uimemvirtual(WINDOW**, int, int*, int, int);
extern void uineterrors(WINDOW**, int, int*, int, int);
extern void uinetfilesys(WINDOW**, int, int*, int, int);
extern void uinetwork(WINDOW**, int, int*, int, int, int, struct sysnet);
extern void uinetlong(WINDOW**, int, int*, int, int, int*, int, struct sysnet, bool);
extern void uitop(WINDOW**, int, int*, int, int, int, struct sysproc**, int, int, bool, char*);
extern void uisys(WINDOW**, int, int*, int, int, struct syshw, struct syskern);
extern void uiwarn(WINDOW**, int, int*, int, int);

#endif
