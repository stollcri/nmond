# Ncurses based System Performance Monitor for Darwin (Mac OS X)

This is a near complete rewrite of [nmon](http://nmon.sourceforge.net/) (file lmon15g.c dated 2015-07-13). The original nmon would not run under OS X since Darwin (the version of BSD which underlies Mac OS X) does not provide a /proc directory; system calls needed to replace the reading of files in the /proc directory. Upon attempting to switch /proc file reads with system calls it quickly became apearant why no one else had performed the conversion; nmond is an older program, built using older development techniques, which had grow organically over the years. Simply swapping out the relevant code would have resulted in a buggy mess, so the entire program was restructured.

Since there is already a working version for Linux (and some UNIX distributions), this project does not seek to preserve compatibility with them. All of the special code required to work with those distros was be removed to make porting easier. The program was designed and tested for OS X, but the code should compile on any other BSD variant as well.

## Building

Just type `make` in the nmond direectory to build nmond. It can be installed by issuing the `make install` command. However, the pre
