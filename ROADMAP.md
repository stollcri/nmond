# nmond Roadmap

## 0.1 -- betas
- Initial work-in-progress builds (progress towards v0.2)

## 0.2 -- MVP
- ✓ APP: Make [GUI portion of] the program run without /proc files
	- ✓ Remove code for special cases (IFDEFs)
	- ✓ Get data from sysctl and other system calls
	- ✓ Pull out UI elements (refactor)
	- ✓ Streamline main()
- ☐ APP: Create long output for:
	- ✓ cpu
	- ✗ mem (this is not, or should not be, very interesting)
	- ☐ disk
	- ✓ net
- ☐ GUI: Update layout, text, and color (see: http://elmon.sourceforge.net)
	- ✓ Improve spacing
		- ✓ CPU Load
		- ✓ CPU Long-Term
		- ✓ Top [processes]
	- ✓ Improve header line (simplify, remove redundant information)
		- ✓ Remove version number
		- ✓ Remove blinking "H for help" (prehaps replace with alert messages)
		- ✓ Move hostname over and change wording
		- ✓ Change refresh rate wording
	- ✓ Display splash screen when nothing is selected
	- ✗ Only update changed portions of windows rather than recreating entire windows each pass
	- ✓ CPU Load: remove top/bottom graph bars
	- ✓ CPU Load: increase number of divisions on scale
	- ✓ CPU L-T: remove top/bottom graph bars, move legend to top line (begins with "CPU")
	- ✗ Move titles to the left side
- ☐ APP: restructure command options (lower-case for horizontal graph, upper-case for vertical graph)
	- ☐      a -- All-in-one display (cpu long + disk + mem + net + top) <= shortcut to my preferences ;-)
	- ☐      A -- All-in-one raw data display
	- ✓ b => b -- black & white mode
	- ✓ c => c -- CPU Load
	- ✓ l => C -- CPU Load, long-term
	- ✓ d => d -- Disk I/O
	- ☐      D -- Disk I/O, long-term
	- ☐ j => f -- Filesystems (JFS)
	- ☐ N => F -- Filesystems (NFS)
	- ☐ g      -- (removed: user defined disk groups)
	- ✓ h => h -- help
	- ✓ H => H -- help
	- ✓ r => i -- Info A
	- ☐      I -- Info B
	- ☐ k => k -- Kernel Stats
	- ✓ L      -- (removed: large memory)
	- ✓ m => m -- Memory & Swap
	- ✗      M -- Memory & Swap
	- ✓ n => n -- Network
	- ✓      N -- Network, long-term
	- ☐ o => o -- Disk busy map
	- ✓ q => q -- quit
	- ✓      r -- Top (sort by mem)
	- ✓      R -- Top (sort by mem), show command arguments
	- ✓ t => t -- Top (sort by CPU)
	- ✓ u => T -- Top (sort by CPU), show command arguments
	- ☐ V => v -- Virtual Memory
	- ☐ v => w -- warn mode
	- ✓ x      -- (removed: exit)
	- ✓ 0 => z -- (removed: zero out peak markers, N/A)
	- ✓ 0      -- (removed: sort by)
	- ✓ 1      -- (removed: sort by)
	- ✓ 2      -- (removed: sort by)
	- ✓ 3      -- (removed: sort by)
	- ✓ 4      -- (removed: sort by)
	- ✓ 5      -- (removed: sort by)
	- ✓ + => + -- double refresh rate
	- ✓ - => - -- half refresh rate
	- ✓ .      -- (removed: minimal display)
	- ✓ ? => ? -- help
- ☐ APP: Static analysis
	- ☐ Valgrind checks
	- ☐ scrutinize all malloc
	- ☐ scrutinize all strcpy -- use strncpy or strlcpy (OpenBSD)
	- ☐ scrutinize all strcat -- use strncat or strlcat (OpenBSD)
	- ☐ scrutinize all printf
	- ☐ scrutinize all scanf -- use fgets
	- ☐ scrutinize all gets -- use fgets
	- ☐ make sure variables are nulled after free to prevent double free (e.g. free(x); x = NULL;)
- ✓ APP: Read environemnt variables
	- ✓ Read new NMOND environment variable (process as if it contained key presses)
	- ✓ Read old NMON environment variable and translate it
- ☐ APP: Read command line arguments
	- ☐ 
	- ☐
	- ☐
	- ☐
- ☐ APP: Create a manpage
- ☐ APP: Clean out dead (reference) code
- ☐ APP: Create dedicated branch for dev

## 0.3
- ☐ Improve ncurses use (http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/printw.html)
- ☐ GUI: Horizontal window resizing (update ncurses when terminal width changes
	- ☐ check how goaccess does it (http://goaccess.io)
- ☐ GUI: Allow for per-pane sorting
	- ☐ check how goaccess does it (http://goaccess.io)
- ☐ GUI: Allow for window scrolling (up/down arrows and mouse)
	- ☐ `scrollok()`
	- ☐ `int wscrl(WINDOW *win, int n);`
	- ☐ check how goaccess does it (http://goaccess.io)

## 0.4
- ☐ INF: Read/Display CPU frequency (vs min and max)
- ☐ INF: Read/Display HDD/CPU/GPU temperature

## 0.8
- ☐ APP: Add 'logging to an external file' capabilities back
- ☐ APP: Full logging system: automatically roll up data for different intervals (hour/day/week/month)
- ☐ APP: Pipe logged data in to stdin for replay (and generate averages at different intervals)
- ☐ APP: Pipe logged data in to stdin to show period averages and/or totals (as appropriate)
- ☐ APP: Visual/Audio alerts for alarm levels (temperature, sustained utliization, etc.)

## 1.0
- ☐ INF: Add GPU load stats
- ☐ GUI: Optional log scale
- ☐ GUI: add disk I/O below long CPU output
- ☐ GUI: Use ncurses progress bars for vertical bars
- ☐ GUI: CPU L-T: follow processes, show their usage in the graph
- ☐ GUI: CPU L-T: drop top-bar down (show gap) when cpu speed is decreased
- ☐ GUI: CPU L-T: generalize chart so that it can be used for other data sets
- ☐ GUI: CPU L-T: show last data (faded, behind) for each bar in the graph (doubles visible data points)
- ☐ GUI: CPU L-T: show candlestick chart instead of bar graph
- ☐ APP: Port to other BSDs

## 1.0+
- APP: Plugins
- GUI: When the window is wide enough show top processes in another column (is that possible with curses?)
- GUI: Update layout, text, and color (see: http://elmon.sourceforge.net)
	- Option for VT100 box drawings (https://en.wikipedia.org/wiki/Box-drawing_character#Unix.2C_CP.2FM.2C_BBS)
	- Option for UTF8 box drawings (https://en.wikipedia.org/wiki/Box-drawing_character)
	- Change cursor location
- APP: 'Backport' to other Linux
