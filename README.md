# Ncurses based System Performance Monitor for Darwin (Mac OS X)

This is a fork/port of [nmon](http://nmon.sourceforge.net/) (lmon15g.c from 2015-07-13); the original nmon will not run under OS X because Darwin (the version of BSD which underlies Mac OS X) does not provide a /proc directory, hence the fork. Since there is already a working version for most of the open source BSD/Linux/Unix systems, this project does not seek to preserve compatibility with them, in fact most of the special code will be removed to make porting easier.

	$ uname -sr
	Darwin 14.5.0
