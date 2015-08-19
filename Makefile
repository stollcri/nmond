CC = cc
# CFLAGS = -g -O3 -D JFS -D GETUSER -Wall -D LARGEMEM -lm
CFLAGS = -Wall -O3 -D JFS -D LARGEMEM
# CFLAGS = -Weverything -O3 -D JFS -D LARGEMEM
LDFLAGS = -lncurses -g
FILES = nmon.c mntent.c

default: nmon

nmon:
	@mkdir -p ./bin/
	${CC} ${CFLAGS} ${LDFLAGS} -o ./bin/nmon ${FILES}
