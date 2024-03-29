const char *usage_text = 
"fcterm [-isc | -sco] [standard X11 options]\n"
"	[[+|-]sf] [-ut] [-geometry WxH+X+Y] [-Ce]\n"
"\n"
"  -C		  Make terminal the 'console' window.\n"
"  -debug    	  Enable debug output.\n"
"  -dump     	  Dump screens to /tmp\n"
"  -e cmd ...	  Command to execute (last on line)\n"
"  -f		  Do not fork from shell.\n"
"  -font_file <filename>\n"
"                  List of fonts to use for smaller/larger font change\n"
"		  Use '*' at start of line to indicate the default.\n"
"  -freetype	  Enable freetype fonts.\n"
"  -label <name>   Run using a label, so we can restore session\n"
"                  on startup (cwd and screen content).\n"
"  -ls		  Execute a login shell.\n"
"  -[no]group      Group fcterms together to maintain position.\n"
"  -spill  	  Enable infinite log spilling\n"
"  +/-sf		  +sf => disable sun function keys\n"
"  		  -sf => enable Sun Function keys.\n"
"  -theme [name | nn]\n"
"                  Use a different colored theme. Omit name/nn to\n"
"		  see list of themes. \n"
"  -ut		  Inhibit utmp entry.\n"
"  -tmpdir <dir>   Directory to store logfiles. Default is /tmp/$USER.\n"
"  -verbose	  Verbose debug messages.\n"
"  -version	  Display version number.\n"
"  \n"
"Emulation switches (legacy):\n"
"\n"
"  -isc		Emulate Interactive Unix console driver.\n"
"  -sco		Emulate SCO Unix console driver.\n"
"\n"
"Description\n"
"\n"
"  fcterm is a color xterm replacement with many features.\n"
"\n"
"  * multi terminal support\n"
"  * infinite scroll\n"
"  * minimap - see what is happening on all screens\n"
"  * fast\n"
"  * freetype fonts\n"
"  * automatic grouping\n"
"  * automatic recovery\n"
"  * pty server support\n"
"  * 8-bit and 24-bit color support\n"
"  * error/warning highlighting.\n"
"  * string searching\n"
"  * URL highlighting\n"
"\n"
"Useful resources:\n"
"\n"
" -xrm *gridLineColor:<color>     Color to use for gridlines escape sequence.\n"
" -xrm *spillForeground:<color>   Foreground color of spilled lines.\n"
" -xrm *spillBackground:<color>   Background color of spilled lines.\n"
" -xrm *logDir:/tmp               Place where overflow lines kept\n"
" -xrm *saveLines:NNN             Number of lines to keep in memory\n"
" -xrm *spillSize:NNN[mkg]        Set default spill size.\n"
"\n"
"Environment variables:\n"
"\n"
"CTW_BACKTRACK_SIZE\n"
"    When resizing terminal, replay last NN bytes (default: 64k) so\n"
"    that the lines are autowrapped on resize.\n"
"\n"
"CTW_ENABLE_PRIMARY       default: true\n"
"CTW_ENABLE_SECONDARY     default: false\n"
"CTW_ENABLE_CLIPBOARD     default: false\n"
"CTW_ENABLE_CUT_BUFFER0   default: false\n"
"    Set default clipboard mode.\n"
"\n"
"CTW_DO_CONT\n"
"    Show end-of-line autowrap continuations.\n"
"\n"
"CTW_DRAW_WATCH\n"
"    Enable visual drawing actions (for debugging).\n"
"\n"
"CTW_ENABLE_SPILL\n"
"    Enable old-style terminal logging to logfile \n"
"    (/var/tmp/$USER/fcterm-ttyN-YYYYMMDD.log). New style simply\n"
"    logs all pty output, and allows color-replay on a restart.\n"
"\n"
"CTW_FILL_TO_BLACK\n"
"    Enable fill-to-black ISO standard.\n"
"\n"
"CTW_FREETYPE\n"
"    Enable freetype font drawing.\n"
"\n"
"CTW_GZIP_ROLLOVER (default: 1)\n"
"    When the history log file (/var/tmp/$USER/fcterm-ttyN.log) exceeds the\n"
"    spill/rollover size, then create a new log file, and gzip the old one.\n"
"\n"
"CTW_GZIP_SIZE (default: 50MB)\n"
"    When log file reaches this size, and spill/rollover is enabled, create\n"
"    a new log file.\n"
"\n"
"CTW_HIGHLIGHT_WORDFILE\n"
"    Specify a file of words to read to be automatically highlighted.\n"
"    Format is:\n"
"    word fg bg flags\n"
"       fg is the foreground color\n"
"       bg is the background color\n"
"       flags: 64 == error color (whole line)\n"
"              128 == warning color (whole line)\n"
"	      256 == search color (only actual word)\n"
"\n"
"CTW_HISTORY\n"
"    Enable history (infinite) scrolling.\n"
"\n"
"CTW_LOGDIR\n"
"    Directory where fcterm stores its state and screen logs.\n"
"\n"
"CTW_MKSVG\n"
"    Path to the \"mksvg.pl\" script to create screen recordings.\n"
"\n"
"CTW_UTF8_MAP_TO_SPACE\n"
"    If set to 1, then unprintable unicode/utf-8 characters will be\n"
"    replaced by a space.\n"
"\n"
"CTW_WORD_CHARS\n"
"    Specify the word-break characters for double-click selection.\n"
"\n"
"CTW_WHEEL_INIT_DECAY\n"
"    (Default 5) Time for the wheel mouse to decay. This is the initial\n"
"    ramping speed of the mouse.\n"
"\n"
"CTW_WHEEL_INCR_DECAY\n"
"    (Default 10) Time for the wheel mouse to decay. After the initial time,\n"
"    we decay at this rate.\n"
"\n"
"FCTERM_LABEL\n"
"    Used to identify this running fcterm instance. You can you \n"
"    \"-label <name>\" on the command line, else an automatic one \n"
"    will be assigned.\n"
"\n"
"FCTERM_PID\n"
"    Process id of the controlling fcterm binary.\n"
"\n"
"FCTERM_TERM=1\n"
"    Used to indicate subshell came from fcterm.\n"
"\n"
"PTY_LABEL\n"
"    Name used to identify this terminal.\n"
"\n"
"fcterm help commands:\n"
"\n"
"  This is accessed via Shift-Alt-F10\n"
"\n"
"  cmap   show colormap.\n"
"  code   show sample code for drawing graphics.\n"
"  esc    show escape sequence summary.\n"
;
