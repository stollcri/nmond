#ifndef SYSCTLHELPER_H
#define SYSCTLHELPER_H

/**
 * sysctlhelper.h -- Wrappers for sysctl
 *  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
 *   (for license, see included LICENSE file)
 */

extern char *stringFromSysctl(int, int);
extern char *stringFromSysctlByName(char*);
extern unsigned int intFromSysctl(int, int);
extern unsigned int intFromSysctlByName(char*);
extern struct timeval timevalFromSysctl(int, int);

#endif
