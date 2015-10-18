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

CFLAGS = -O3 -Wall -D NDEBUG -Wall
LFLAGS = -l ncurses
AOFILE = ./bin/nmond
CFILES = nmond.c sysctlhelper.c sysinfo.c pidhash.c $(LANGFILES)

CFLAGS_DBG = -O0 -g -Wall
LFLAGS_DBG = -l ncurses
AOFILE_DBG = ./bin/nmond
CFILES_DBG = $(CFILES)


# make sure scan-build is available
SCAN_BUILD_EXISTS := @true
SCAN_BUILD_WHICH := $(shell which scan-build)
ifeq ($(strip $(SCAN_BUILD_WHICH)),)
	SCAN_BUILD_EXISTS := @echo "\nERROR:\n scan-build not found.\n See: http://clang-analyzer.llvm.org/installation\n" && exit 1
endif


default: findtrouble nmond
test: debug run

debug:
	@mkdir -p ./bin/
	$(CC) $(CFLAGS_DBG) $(LFLAGS_DBG) -o $(AOFILE_DBG) $(CFILES_DBG)

analysis:
	echo "Variables should be checked for null after malloc ... check the following ..."
	grep -inlR -e malloc\( . | xargs grep -inR -A1 -e malloc\(
	echo
	echo "Variables should be nulled after free ... check the following output ..."
	grep -inlR -e free\( . | xargs grep -inR -A1 -e free\(
	echo
	$(SCAN_BUILD_EXISTS)
	scan-build -o tst/out -v -V make debug

findtrouble:
	grep -inR -e malloc\( .
	grep -inR -e strcpy\( -e strcat\( -e scanf\( -e gets\( .

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
	cp bin/nmond /usr/local/bin/nmond
	chown root:wheel /usr/local/bin/nmond
	chmod 4555 /usr/local/bin/nmond
	mkdir -p /usr/local/man/
	cp doc/nmond /usr/local/man/nmond.1
	chown root:wheel /usr/local/man/nmond.1

.PHONY: default, test, debug, analysis, run, clean, nmond, install
