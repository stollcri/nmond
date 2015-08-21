CC = cc

CFLAGS = -Oz -Wall
LFLAGS = -l ncurses
AOFILE = ./bin/nmond
CFILES = nmond.c mntent.c sysinfo.c

CFLAGS_DBG = -O0 -g -Weverything
LFLAGS_DBG = -l ncurses
AOFILE_DBG = ./bin/nmond
CFILES_DBG = $(CFILES)

default: nmond
test: debug run

debug:
	@mkdir -p ./bin/
	$(CC) $(CFLAGS_DBG) $(LFLAGS_DBG) -o $(AOFILE_DBG) $(CFILES_DBG)

nmond:
	@mkdir -p ./bin/
	$(CC) $(CFLAGS) $(LFLAGS) -o $(AOFILE) $(CFILES)

run:
	bin/nmond

clean:
	rm -r bin/*

.PHONY: default, test, debug, nmond, run, clean
