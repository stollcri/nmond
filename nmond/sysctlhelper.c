/**
 * sysctlhelper.c -- Wrappers for sysctl
 *  huge shout-out to: man 3 sysctl
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

#include "sysctlhelper.h"
#include <assert.h>
#include <stdlib.h>
#include <sys/sysctl.h>

/*
 * Get a character string from sysctl (level 2)
 */
char *stringFromSysctl(int mib0, int mib1)
{
	char *result = NULL;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = 0;
	int complete = 0;
	int error = 0;

	// catch memory errors (ENOMEM),
	// assert to catch program errors
	while(!complete && !error) {
		assert(result == NULL);
		length = 0;

		// get the expected length of the result
		error = sysctl(mib, 2, NULL, &length, NULL, 0);

		// allocate memory for the result
		if(!error) {
			result = malloc(length);
			if (result == NULL) {
				error = 1;
			}
		}

		// get the result
		if(!error) {
			error = sysctl(mib, 2, result, &length, NULL, 0);
			if(!error) {
				complete = 1;
			} else {
				assert(result != NULL);
				free(result);
				result = NULL;
				
				complete = 0;
				error = 0;
			}
		}
	}

	return result;
}

/*
 * Get a character string from sysctlbyname
 */
char *stringFromSysctlByName(char *name)
{
	char *result = NULL;
	size_t length = 0;
	int complete = 0;
	int error = 0;

	// catch memory errors (ENOMEM),
	// assert to catch program errors
	while(!complete && !error) {
		assert(result == NULL);
		length = 0;

		// get the expected length of the result
		error = sysctlbyname(name, NULL, &length, NULL, 0);

		// allocate memory for the result
		if(!error) {
			result = malloc(length);
			if(result == NULL) {
				error = 1;
			}
		}

		// get the result
		if(!error) {
			error = sysctlbyname(name, result, &length, NULL, 0);
			if(!error) {
				complete = 1;
			} else {
				assert(result != NULL);
				free(result);
				result = NULL;

				complete = 0;
				error = 0;
			}
		}
	}

	return result;
}

/*
 * Get an integer from sysctl (level 2)
 */
unsigned int intFromSysctl(int mib0, int mib1)
{
	unsigned int result = 0;

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = sizeof(result);
	sysctl(mib, 2, &result, &length, NULL, 0);

	return result;
}

/*
 * Get an integer from sysctlbyname
 */
unsigned int intFromSysctlByName(char *name)
{
	unsigned int result = 0;

	size_t length = sizeof(result);
	sysctlbyname(name, &result, &length, NULL, 0);

	return result;
}

/*
 * Get a 64bit integer from sysctlbyname
 */
unsigned long long int64FromSysctlByName(char *name)
{
	unsigned long long result = 0;

	size_t length = sizeof(result);
	sysctlbyname(name, &result, &length, NULL, 0);

	return result;
}

/*
 * Get a time value from sysctl (level 2)
 */
struct timeval timevalFromSysctl(int mib0, int mib1)
{
	struct timeval result = { 0, 0 };

	int mib[2];
	mib[0] = mib0;
	mib[1] = mib1;
	size_t length = sizeof(result);
	sysctl(mib, 2, &result, &length, NULL, 0);

	return result;
}

/*
 * Get process name and arguments given a PID
 */
#include <stdio.h>
void processArguments(int pid, int sizelimit, char *resultLoc)
{
	// char *result = NULL;

	int mib[4];
	mib[0] = CTL_KERN;
	mib[1] = KERN_PROCARGS;//KERN_PROCARGS2;
	mib[2] = pid;
	size_t templength = 0;
	int error = 0;
	char *arglist = NULL;

	error = sysctl(mib, 4, NULL, &templength, NULL, 0);
	// allocate memory for the result
	if(!error) {
		arglist = malloc(templength);
		if(arglist == NULL) {
			error = 1;
		}
	}

	// get the result
	if(!error) {
		error = sysctl(mib, 3, arglist, &templength, NULL, 0);

		if(!error) {
			//
			// TODO: merge loops together
			// 
			int argcount = 0;
			static int argstarts[1024];
			int argstart = 0;
			static int argsizes[1024];
			int argsize = -1;
			int skipcount = 0;
			for(int i = 0; i < (int)templength; ++i) {
				if(arglist[i] == '\0') {

					if(arglist[i-1] == '\0') {
						++skipcount;
					} else {
						argstarts[argcount] = argstart + skipcount;
						argstart = i + 1;

						argsizes[argcount] = i - (argsize + skipcount + 1);
						argsize = i;
						
						skipcount = 0;
						++argcount;
					}
				} else if(arglist[i] == '/') {
					// argstart = i + 1;
					// argsize = i;
				} else if(arglist[i] == '=') {
					break;
				}
			}

			int resultlen = 0;
			int resultoffset = 0;

			for(int argno = 1; argno < argcount; ++argno) {
				resultlen += (argsizes[argno] + 1);
			}

			if(resultlen > sizelimit) {
				resultoffset = resultlen - sizelimit + TRUNC_STRING_LENGTH;
				resultlen = sizelimit - TRUNC_STRING_LENGTH;
			}

			int currentarg = 1;
			int currentargpos = argstarts[currentarg];
			char currentchar = 0;

			int stringpos = 0;

			for(int i = 0; i < (int)(resultoffset + resultlen - 1); ++i) {
				currentchar = arglist[currentargpos];

				if(i >= resultoffset) {
					if(currentchar != '\0'){
						resultLoc[stringpos] = currentchar;
					} else {
						resultLoc[stringpos] = ' ';
					}
					++stringpos;
				} else if (i >= (resultoffset - TRUNC_STRING_LENGTH)) {
					resultLoc[stringpos] = TRUNC_CHAR;
					++stringpos;
				}

				++currentargpos;
				if(currentargpos > (argstarts[currentarg] + argsizes[currentarg])) {
					++currentarg;
					currentargpos = argstarts[currentarg];
				}
			}
			resultLoc[stringpos] = '\0';
		}
		free(arglist);
	}

	// return result;
}
