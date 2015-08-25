
LANG = en

ifeq ($(LANG),en)
	LANGFILES = nmond_ui_curses.c
else ifeq ($(LANG),de)
	LANGFILES = nmond_ui_curses.c
endif

CC = cc

# TODO: Does Oz remove inlines to get size smaller, maybe use O2 or O3?
CFLAGS = -Oz -Wall -D NDEBUG
LFLAGS = -l ncurses
AOFILE = ./bin/nmond
CFILES = nmond.c mntent.c sysinfo.c $(LANGFILES)

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
	rm -r dbg

.PHONY: default, test, debug, nmond, run, clean
