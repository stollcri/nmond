/*
 * nmond.h -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
 *  Christopher Stoll (https://github.com/stollcri), 2015
 */

#include "sysinfo.h"
#include <stdlib.h>
#include <string.h>
#include <sys/sysctl.h>

struct sys getsysinfo()
{
	struct sys thissys = { "-", "-", 0, 0, 0, 0, 0, "-", 0 };

	int mib[2];
	size_t len = 0;
	unsigned int returnedInteger = 0;

	mib[0] = CTL_HW;

	mib[1] = HW_MACHINE;
	sysctl(mib, 2, NULL, &len, NULL, 0);
	char *returnedMachine = NULL;
	returnedMachine = malloc(len);
	sysctl(mib, 2, returnedMachine, &len, NULL, 0);
	thissys.machine = returnedMachine;

	mib[1] = HW_MODEL;
	sysctl(mib, 2, NULL, &len, NULL, 0);
	char *returnedModel = NULL;
	returnedModel = malloc(len);
	sysctl(mib, 2, returnedModel, &len, NULL, 0);
	thissys.model = returnedModel;

	mib[1] = HW_BYTEORDER;
	len = sizeof(returnedInteger);
	sysctl(mib, 2, &returnedInteger, &len, NULL, 0);
	thissys.byteorder = returnedInteger;

	mib[1] = HW_MEMSIZE;
	len = sizeof(returnedInteger);
	sysctl(mib, 2, &returnedInteger, &len, NULL, 0);
	thissys.memorysize = returnedInteger;

	mib[1] = HW_USERMEM;
	len = sizeof(returnedInteger);
	sysctl(mib, 2, &returnedInteger, &len, NULL, 0);
	thissys.usermemory = returnedInteger;

	mib[1] = HW_PAGESIZE;
	len = sizeof(returnedInteger);
	sysctl(mib, 2, &returnedInteger, &len, NULL, 0);
	thissys.pagesize = returnedInteger;

	// mib[1] = HW_FLOATINGPOINT;
	// len = sizeof(returnedInteger);
	// sysctl(mib, 2, &returnedInteger, &len, NULL, 0);
	// thissys.floatingpoint = returnedInteger;

	mib[1] = HW_MACHINE_ARCH;
	sysctl(mib, 2, NULL, &len, NULL, 0);
	char *returnedArchitecture = NULL;
	returnedArchitecture = malloc(len);
	sysctl(mib, 2, returnedArchitecture, &len, NULL, 0);
	thissys.architecture = returnedArchitecture;

	mib[1] = HW_CPU_FREQ;
	len = sizeof(returnedInteger);
	sysctl(mib, 2, &returnedInteger, &len, NULL, 0);
	thissys.cpufrequency = returnedInteger;

	return thissys;
}
