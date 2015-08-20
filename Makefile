CC = cc
# CFLAGS = -g -O3 -D JFS -D GETUSER -Wall -D LARGEMEM -lm
CFLAGS = -Wall -O3 -D JFS -D LARGEMEM
# CFLAGS = -Weverything -O3 -D JFS -D LARGEMEM
LDFLAGS = -l ncurses -g
FILES = nmond.c mntent.c

default: nmond
test: nmond run

nmond:
	@mkdir -p ./bin/
	${CC} ${CFLAGS} ${LDFLAGS} -o ./bin/nmond ${FILES}

run:
	bin/nmond

clean:
	rm -r bin/*
