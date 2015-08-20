CC = cc
# CFLAGS = -g -O3 -D JFS -D GETUSER -Wall -D LARGEMEM -lm
CFLAGS = -Wall -O3 -D JFS -D LARGEMEM
# CFLAGS = -Weverything -O3 -D JFS -D LARGEMEM
LDFLAGS = -l ncurses -g
FILES = nmon.c mntent.c

default: nmon
test: nmon run

nmon:
	@mkdir -p ./bin/
	${CC} ${CFLAGS} ${LDFLAGS} -o ./bin/nmon ${FILES}

run:
	bin/nmon

clean:
	rm -r bin/*
