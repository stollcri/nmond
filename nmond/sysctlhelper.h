#ifndef SYSCTLHELPER_H
#define SYSCTLHELPER_H

/**
 * sysctlhelper.h -- Wrappers for sysctl
 *
 *
 * nmond -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 * 	https://github.com/stollcri/nmond
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

#include <stdint.h>
#include <time.h>

#define TRUNC_STRING_LENGTH 3
#define TRUNC_CHAR '.'

extern char *stringFromSysctl(int, int);
extern char *stringFromSysctlByName(char*);
extern unsigned int intFromSysctl(int, int);
extern unsigned int intFromSysctlByName(char*);
extern unsigned long long int64FromSysctlByName(char*);
extern struct timeval timevalFromSysctl(int, int);
extern char *timeStringFromTimestamp(time_t, char*);
extern void processArguments(int, int, char*);

#endif
