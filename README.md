# Ncurses based System Performance Monitor for Darwin (Mac OS X)

This is a fork/port of [nmon](http://nmon.sourceforge.net/) (file lmon15g.c dated 2015-07-13); the original nmon would not run under OS X since Darwin (the version of BSD which underlies Mac OS X) does not provide a /proc directory, hence the fork. Since there is already a working version for Linux/Unix, this project does not seek to preserve compatibility with them. Most of the special code will be removed to make porting easier. We are primarily concerned with OS X at this time, but the code should compile on any other BSD variant as well.

## Building

Just type `make` in the nmond direectory to build nmond. It can be installed by issuing the `make install` command. However, the pre

### Required Headers

Additional header files for OS X are required and they can be downloaded from [Apple's Open Source project](https://github.com/opensource-apple/xnu). They can be installed using the `make installhdrs` (or `make installhdrs VERBOSE=YES ARCH_CONFIGS=X86_64`) command, however a few modifications are needed to get the additional header files built.

- xnu/bsd/sys/Makefile needs to be modified; add "proc_internal.h" to the end of the "DATAFILES = " statement.
- xnu/osfk/kern/Makefile needs to be modified; add "locks.h kern_types.h" to the end of the "DATAFILES = " statement.
- xnu/osfk/machinie/Makefile needs to be modified; add "INSTALL_MI_LIST = locks.h" above "INSTALL_MI_LCL_LIST = cpu_capabilities.h"
- (there are too many to list, finding another way)

### Problems with XNU

#### macosx.internal cannot be located

If the error "xcodebuild: error: SDK macosx.internal cannot be located" is received, then it may be necessary to change the line `#SDKROOT ?= macosx.internal` to `SDKROOT ?= macosx10.10` in MakeInc.cmd.

#### Unable to find utility ctfmerge

If the error "xcrun: error: unable to find utility ctfmerge", not a developer tool or in PATH" is received, then dtrace needs to be built and installed:

```
$ curl http://opensource.apple.com/tarballs/dtrace/dtrace-96.tar.gz > dt.tar.gz
$ tar zxvf dt.tar.gz
$ cd dtrace-96
$ mkdir -p obj sym dst
$ xcodebuild install -target ctfconvert -target ctfdump -target ctfmerge \
ARCHS="i386 x86_64" SRCROOT=$PWD OBJROOT=$PWD/obj SYMROOT=$PWD/sym \
DSTROOT=$PWD/dust
```
Then add the dust directory to your path or copy the binary files to /usr/local/bin

#### Unable to locate availability.pl

If the error "Unable to locate /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/usr/local/libexec/availability.pl (or not executable)" is received, then download [availability.pl](http://www.opensource.apple.com/source/AvailabilityVersions/AvailabilityVersions-4.2/availability.pl?txt) and put it in the desired location.

```
curl http://www.opensource.apple.com/source/AvailabilityVersions/AvailabilityVersions-4.2/availability.pl?txt > availability.pl
sudo mkdir -p /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/usr/local/libexec/
sudo cp availability.pl /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/usr/local/libexec/
sudo chmod +x /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/usr/local/libexec/availability.pl
```