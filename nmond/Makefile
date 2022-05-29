#
# nmond -- Ncurses based System Performance Monitor for Darwin (Mac OS X)
#  Copyright (c) 2015, 2017, 2019 Christopher Stoll (https://github.com/stollcri)
#

# cheap i18n (not localized)
LANG = en
ifeq ($(LANG),en)
	LANGFILES = uicli.c uicurses.c uibytesize.c
else ifeq ($(LANG),de)
	LANGFILES = uicli.c uicurses.c uibytesize.c
endif


CC = cc

ARM = -target arm64-apple-macos11
X86 = -target x86_64-apple-macos10.12

CFLAGS = -O3 -Wall -D NDEBUG -Wall
LFLAGS = -l ncurses
CFILES = nmond.c sysctlhelper.c sysinfo.c pidhash.c $(LANGFILES)
AOFILE_ARM = ./bin/arm/nmond
AOFILE_X86 = ./bin/x86/nmond

CFLAGS_DBG = -O0 -g -Wall
LFLAGS_DBG = -l ncurses
CFILES_DBG = $(CFILES)
AOFILE_DBG_ARM = ./bin/arm/nmond
AOFILE_DBG_X86 = ./bin/x86/nmond


# make sure scan-build is available
SCAN_BUILD_EXISTS := @true
SCAN_BUILD_WHICH := $(shell which scan-build)
ifeq ($(strip $(SCAN_BUILD_WHICH)),)
	SCAN_BUILD_EXISTS := @echo "\nERROR:\n scan-build not found.\n See: http://clang-analyzer.llvm.org/installation\n" && exit 1
endif


default: nmond run
test: debug run

debug:
	@mkdir -p ./bin/arm/ ./bin/x86/
	$(CC) $(CFLAGS_DBG) $(LFLAGS_DBG) -o $(AOFILE_DBG_ARM) $(ARM) $(CFILES_DBG)
	$(CC) $(CFLAGS_DBG) $(LFLAGS_DBG) -o $(AOFILE_DBG_X86) $(X86) $(CFILES_DBG)

analysis:
	@echo "Variables should be checked for null after *alloc ... check the following ..."
	grep -inlR -e alloc\( . | xargs grep -inR -A1 -e alloc\(
	@echo
	@echo "Variables should be nulled after free ... check the following output ..."
	grep -inlR -e free\( . | xargs grep -inR -A1 -e free\(
	@echo
	$(SCAN_BUILD_EXISTS)
	scan-build -o tst/out -v -V make debug

findtrouble:
	grep -inR -e alloc\( .
	grep -inR -e strcpy\( -e strcat\( -e scanf\( -e gets\( .

run: run-arm

run-arm:
	NMOND="cdegimn" bin/arm/nmond noarg1 noarg2

run-x86:
	# bin/x86/nmond noarg1 noarg2 2> errors.txt
	NMOND="cdegimn" bin/x86/nmond noarg1 noarg2

clean:
	rm -rf bin/arm/*
	rm -rf bin/x86/*
	rm -rf dbg
	rm -rf tst/out/20*

nmond:
	@mkdir -p ./bin/arm/ ./bin/x86/
	$(CC) $(CFLAGS) $(LFLAGS) -o $(AOFILE_ARM) $(ARM) $(CFILES)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(AOFILE_X86) $(X86) $(CFILES)

install: install-arm

install-arm:
	cp bin/arm/nmond /usr/local/bin/nmond
	chown root:wheel /usr/local/bin/nmond
	chmod 4555 /usr/local/bin/nmond
	mkdir -p /usr/local/man/
	cp doc/nmond /usr/local/man/nmond.1
	chown root:wheel /usr/local/man/nmond.1
	xattr -d com.apple.quarantine /usr/local/bin/nmond

install-x86:
	cp bin/x86/nmond /usr/local/bin/nmond
	chown root:wheel /usr/local/bin/nmond
	chmod 4555 /usr/local/bin/nmond
	mkdir -p /usr/local/man/
	cp doc/nmond /usr/local/man/nmond.1
	chown root:wheel /usr/local/man/nmond.1
	xattr -d com.apple.quarantine /usr/local/bin/nmond

yes2:
	yes > /dev/null & yes > /dev/null &

yes3:
	yes > /dev/null & yes > /dev/null & yes > /dev/null &

yesoff:
	killall yes

.PHONY: default, test, debug, analysis, run, clean, nmond, install
