# Ncurses based System Performance Monitor for Darwin (Mac OS X)

This is a fork/port of [nmon](http://nmon.sourceforge.net/) (file lmon15g.c dated 2015-07-13); the original nmon would not run under OS X since Darwin (the version of BSD which underlies Mac OS X) does not provide a /proc directory, hence the fork. Since there is already a working version for Linux (and some UNIX distributions), this project does not seek to preserve compatibility with them. All of the special code required to work with those distros will be removed to make porting easier. We are primarily concerned with OS X at this time, but the code should compile on any other BSD variant as well.

## Building

Just type `make` in the nmond direectory to build nmond. It can be installed by issuing the `make install` command. However, the pre
