#
# nmond.h -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
#  Copyright (c) 2015 Christopher Stoll (https://github.com/stollcri)
#  

# cheap i18n (not localized)
LANG = en
ifeq ($(LANG),en)
	LANGFILES = uicli.c uicurses.c uibytesize.c
else ifeq ($(LANG),de)
	LANGFILES = uicli.c uicurses.c uibytesize.c
endif


CC = cc

CFLAGS = -O3 -Wall -D NDEBUG
LFLAGS = -l ncurses
AOFILE = ./bin/nmond
CFILES = nmond.c sysctlhelper.c sysinfo.c pidhash.c $(LANGFILES)

#CFLAGS_DBG = -O0 -g -Weverything
# be quiet, at least until I get further along with the update)
CFLAGS_DBG = -O0 -g
LFLAGS_DBG = -l ncurses
AOFILE_DBG = ./bin/nmond
CFILES_DBG = $(CFILES)


# make sure scan-build is available
SCAN_BUILD_EXISTS := @true
SCAN_BUILD_WHICH := $(shell which scan-build)
ifeq ($(strip $(SCAN_BUILD_WHICH)),)
	SCAN_BUILD_EXISTS := @echo "\nERROR:\n scan-build not found.\n See: http://clang-analyzer.llvm.org/installation\n" && exit 1
endif


default: nmond
test: debug run

debug:
	@mkdir -p ./bin/
	$(CC) $(CFLAGS_DBG) $(LFLAGS_DBG) -o $(AOFILE_DBG) $(CFILES_DBG)

analysis:
	$(SCAN_BUILD_EXISTS)
	scan-build -o tst/out -v -V make debug

run:
	# bin/nmond noarg1 noarg2 2> errors.txt
	bin/nmond noarg1 noarg2

clean:
	rm -r bin/*
	rm -r dbg
	rm -r tst/out/20*

nmond:
	@mkdir -p ./bin/
	$(CC) $(CFLAGS) $(LFLAGS) -o $(AOFILE) $(CFILES)

install:
	cp bin/nmond /usr/bin/nmond
	chown root:wheel /usr/bin/nmond
	chmod 4555 /usr/bin/nmond

.PHONY: default, test, debug, analysis, run, clean, nmond, install
