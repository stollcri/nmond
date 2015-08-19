# NOTE: link the curent working copy of the code to lmon.c for compiling
CFLAGS=-g -O3 -D JFS -D GETUSER -Wall -D LARGEMEM -lm
# CFLAGS=-g -O2 -D JFS -D GETUSER -Wall -D POWER
#CFLAGS=-g -D JFS -D GETUSER 
LDFLAGS=-lncurses -g
FILE=lmon.c

nmon_power_rhel3: $(FILE)
	cc -o nmon_power_rhel3 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER

nmon_power_rhel4: $(FILE)
	gcc -o nmon_power_rhel4 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER

nmon_power_sles12: $(FILE)
	cc -o nmon_power_sles12 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18 

nmon_power_sles113: $(FILE)
	cc -o nmon_power_sles113 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18 -D SLES113

nmon_power_sles112: $(FILE)
	cc -o nmon_power_sles112 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18 

nmon_power_sles11: $(FILE)
	cc -o nmon_power_sles11 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18 

nmon_power_sles10: $(FILE)
	cc -o nmon_power_sles10 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER

nmon_power_rhel5: $(FILE)
	gcc -o nmon_power_rhel5 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER

nmon_power_rhel55: $(FILE)
	gcc -o nmon_power_rhel55 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18

nmon_power_rhel65: $(FILE)
	gcc -o nmon_power_rhel55 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18

nmon_power_rhel7: $(FILE)
	gcc -o nmon_power_rhel7 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18 -D RHEL7

nmon_power_ubuntu1410: $(FILE)
	gcc -o nmon_power_ubuntu1410 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18

nmon_power_ubuntu1504: $(FILE)
	gcc -o nmon_power_ubuntu1504 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER -D KERNEL_2_6_18


nmon_power_sles9: $(FILE)
	cc -o nmon_power_sles9 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER

nmon_power_sles8: $(FILE)
	cc -o nmon_power_sles8 $(FILE) $(CFLAGS) $(LDFLAGS) -D POWER

nmon_mainframe_sles8: $(FILE)
	cc -o nmon_mainframe_sles8 $(FILE) $(CFLAGS) $(LDFLAGS) -D MAINFRAME

nmon_mainframe_sles9: $(FILE)
	cc -o nmon_mainframe_sles9 $(FILE) $(CFLAGS) $(LDFLAGS) -D MAINFRAME

nmon_mainframe_sles10: $(FILE)
	cc -o nmon_mainframe_sles10 $(FILE) $(CFLAGS) $(LDFLAGS) -D MAINFRAME

nmon_x86_sles8:  $(FILE)
	cc -o nmon_x86_sles8 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_sles10:  $(FILE)
	cc -o nmon_x86_sles10 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_opensuse10:  $(FILE)
	cc -o nmon_x86_opensuse10 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_64_opensuse11:  $(FILE)
	cc -o nmon_x86_64_opensuse11 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_sles9:  $(FILE)
	cc -o nmon_x86_sles9 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_rhel45:  $(FILE)
	cc -o nmon_x86_rhel45 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_rhel52:  $(FILE)
	cc -o nmon_x86_rhel52 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_rhel54:  $(FILE)
	gcc44 -o nmon_x86_rhel54 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_rhel3:  $(FILE)
nmon_x86_rhel4:  $(FILE)
	cc -o nmon_x86_rhel4 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_rhel3:  $(FILE)
	cc -o nmon_x86_rhel3 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_redhat9:  $(FILE)
	cc -o nmon_x86_redhat9 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_rhel2:
	cc -o nmon_x86_rhel2 $(FILE) $(CFLAGS) $(LDFLAGS) -D REREAD=1 -D X86

nmon_x86_debian3:  $(FILE)
	cc -o nmon_x86_debian3 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_fedora10: 
	cc -s -o nmon_x86_fedora10 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_64_fedora10: 
	cc -s -o nmon_x86_64_fedora10 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_ubuntu810: 
	cc -o nmon_x86_ubuntu810 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_64_ubuntu810: 
	cc -o nmon_x86_64_ubuntu810 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_64_ubuntu910: 
	cc -o nmon_x86_64_ubuntu910 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_ubuntu910: 
	cc -o nmon_x86_ubuntu910 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_ubuntu134: lmon.c
	cc -o nmon_x86_ubuntu134 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_ubuntu1410: lmon.c
	cc -o nmon_x86_ubuntu1410 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

nmon_x86_ubuntu1504: lmon.c
	cc -o nmon_x86_ubuntu1504 $(FILE) $(CFLAGS) $(LDFLAGS) -D X86

steal: lmon15c_stealing.c
	cc -o nmon15cs lmon15c_stealing.c $(CFLAGS) $(LDFLAGS) -D X86
