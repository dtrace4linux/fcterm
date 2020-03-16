"\r\n"
"^N x ^O -- switch char set / line drawing\r\n"
"   x == 0x01    Mid-char vertical line\r\n"
"\r\n"
"Escape sequences:\r\n"
"   ESC ESC x  Print character x literally.\r\n"
"   ESC 7	Save cursor position\r\n"
"   ESC 8	Restore cursor position\r\n"
"   ESC @0	Not in curses.\r\n"
"   ESC @1     In curses emulation mode.\r\n"
"   ESC @2     Receive ioctl.\r\n"
"   ESC @3     Reply to ioctl.\r\n"
"   ESC c	Reset terminal\r\n"
"   ESC D	Index (down cursor or scroll).\r\n"
"   ESC H	Set tab stop at current position.\r\n"
"   ESC M	Reverse index (up cursor or insline).\r\n"
"   ESC Z	Identify terminal.\r\n"
"   ESC >	Numeric keypad\r\n"
"   ESC =	Application keypad\r\n"
"   ^H		Backspace\r\n"
"   ^I		Tabs (fixed-width -- 8 chars wide).\r\n"
"   ^J		Line feed\r\n"
"   ^L		Clear screen.\r\n"
"   ^M 	Carriage return\r\n"
"   ESC(0   ESC)0   ESC*0   ESC+0  Set character sets.\r\n"
"              0  Normal line drawing font set\r\n"
"              1  Alternate fonts\r\n"
"              2  Alternate fonts\r\n"
"              A  American ASCII\r\n"
"              B  British ASCII\r\n"
"   ESC [n @	Insert characters.\r\n"
"   ESC [n A	Move cursor up\r\n"
"   ESC [n B	Move cursor down\r\n"
"   ESC [n C	Move cursor right\r\n"
"   ESC [n D	Move cursor left\r\n"
"   ESC [n d	Goto (x, n)\r\n"
"   ESC [n E	Goto (0,y+n)\r\n"
"   ESC [n F	Goto (0,y-n)\r\n"
"   ESC [n G	Goto specified column.\r\n"
"   ESC [n;m H Move cursor to absolute position.\r\n"
"   ESC [n J   Erase screen 0==clear below; 1==clear above; 2==clear all\r\n"
"   ESC [n L	Insert lines at current row\r\n"
"   ESC [n M	Delete lines at current row\r\n"
"   ESC [n P	Delete characters.\r\n"
"   ESC [S	Index (down cursor or scroll).\r\n"
"   ESC [T	Reverse index (up cursor or insline).\r\n"
"   ESC [n X	Erase to blank (no cursor move)\r\n"
"   ESC [n Y	Set mouse button mask.\r\n"
"***ESC [n Z	Request mouse position.\r\n"
"   ESC [Z  	Backtab\r\n"
"\r\n"
"   ESC [ c	Ask for answer back message.\r\n"
"   ESC [n;m f Move cursor to absolute position.\r\n"
"   ESC [n g	Print ASCII char 'n' (SCO mode only)\r\n"
"   ESC [3 g	Clear TAB stops.\r\n"
"   ESC [4 h	Enter insert mode\r\n"
"   ESC [4 l	Exit insert mode\r\n"
"   ESC [0 m	Reset attributes.\r\n"
"   ESC [1 m	Bold\r\n"
"   ESC [4 m	Underline\r\n"
"   ESC [5 m	Blink\r\n"
"   ESC [7 m	Reverse video.\r\n"
"   ESC [8 m	Store current colors as the default.\r\n"
"   ESC [10 m  Select primary font, don't display ctrl chars, do't set bit 8\r\n"
"   ESC [11 m  Select first alternate font; lets chars < 32 be displayed\r\n"
"   ESC [21m	Reset bold.\r\n"
"   ESC [24m	Reset underline.\r\n"
"   ESC [25m	Reset blink.\r\n"
"   ESC [27m	Reset reverse video.\r\n"
"   ESC [3Xm	Set foreground color.\r\n"
"   ESC [38 ; 2 ; r ; g ; b m	Set foreground RGB.\r\n"
"   ESC [38 ; 5 ; XX m	Set foreground from 256 palette.\r\n"
"   ESC [39m	Restore foreground default color.\r\n"
"   ESC [4Xm	Set background color.\r\n"
"   ESC [48 ; 2 ; r ; g ; b m	Set foreground RGB.\r\n"
"   ESC [48 ; 5 ; XX m	Set background from 256 palette.\r\n"
"   ESC [49m	Restore background default color.\r\n"
"   ESC [7Xm	Set cursor color.\r\n"
"   ESC [78m	Restore cursor color.\r\n"
"   ESC [1900 m  Stop literal mode.\r\n"
"   ESC [1901 m  Go into literal mode.\r\n"
"   ESC [1902 m  Go into strikethru mode.\r\n"
"   ESC [1903 m  Exit strikethru mode.\r\n"
"   ESC [1904 m  Enter autoswitch mode.\r\n"
"   ESC [1905 m  Exit autoswitch mode.\r\n"
"   ESC [1920 ; x ; y ; w ; h ; m         Draw a rectangle.\r\n"
"   ESC [1921 ; x ; y ; w ; h ; m         Fill a rectangle.\r\n"
"   ESC [1922 ; x1 ; y1; x2 ; y2; m       Draw a line\r\n"
"   ESC [1923 ; x ; y ; m                 Draw a pixel.\r\n"
"   ESC [1924 m Clear graphics chain.\r\n"
"   ESC [1925 m Clear graphics chain and redraw screen.\r\n"
"   ESC [1926 ; x; y; w; h; name m        Set name\r\n"
"   ESC [1927 ; x; y; str; m              Draw string\r\n"
"   ESC [1928 ; x; y; w; h; arc1; arc2; m Arc\r\n"
"   ESC [1929 ; x; y; w; h; arc1; arc2; m FillArc\r\n"
"   ESC [1930 ; rgb m                     Set background\r\n"
"   ESC [1931 ; rgb m                     Set foreground\r\n"
"   ESC [1932 ; name ; m                  Set font\r\n"
"   ESC [1933 ; x; y; str; m              Draw image string\r\n"
"   ESC [1934 m                           Query winsize in pixels (wxh<Enter>)\r\n"
"   ESC [1935 m                           Dump internal status to a /tmp/fcterm/fcterm.3467\r\n"
"   ESC [1936 m    Box each character drawn.\r\n"
"   ESC [1937 m    Unox each character drawn.\r\n"
"   ESC [1938 ; n m  Group fcterms (n == 1 - group; == 0 - ungroup)\r\n"
"   ESC [1939 m   Show status.\r\n"
"   ESC [1940 m   Search/command mode.\r\n"
"   ESC [1941 m   Zoom\r\n"
"   ESC [1942 m   Minimap\r\n"
"   ESC [1943; <id> m   Create new screen.  \r\n"
"   ESC [n;m r	Set scrolling region to lines n..m.\r\n"
"   ESC [n;m r	Set scrolling region.\r\n"
"   ESC [s	Saved cursor position.\r\n"
"   ESC [n t	Shelltool/cmdtool compatable escape sequence\r\n"
"   		ESC [1t	Open window.\r\n"
"   		ESC [2t	Iconise window.\r\n"
"   		ESC [3t	Move window.\r\n"
"   		ESC [4t	Change size (in pixels) window.\r\n"
"   		ESC [5t	Raise window.\r\n"
"   		ESC [6t	Lower window.\r\n"
"   		ESC [8t	Change size (in chars) window.\r\n"
"   ESC [u	Restore cursor position.\r\n"
"\r\n"
"   ESC [ ? 1 h	Application cursor keys\r\n"
"   ESC [ ? 3 h	80/132 column mode switch.\r\n"
"   ESC [ ? 4 h	Set smooth scroll mode.\r\n"
"   ESC [ ? 5 h	Reverse video mode.\r\n"
"   ESC [ ? 7 h	Set autowrap\r\n"
"   ESC [ ? 8 h	Enable autorepeat (no-op) Use XAutoRepeatOn)\r\n"
"   ESC [ ? 9 h	Turn on application mouse.\r\n"
"   ESC [ ? 12 h	Move this console to the front.\r\n"
"   ESC [ ? 25 h	Turn on cursor.\r\n"
"   ESC [ ? 47 h	Use alternate screen buffer.\r\n"
"   ESC [ ? 1000 h	Enter keypad mode.\r\n"
"   ESC [ ? 1034 h	SMM mode\r\n"
"   ESC [ ? 1049 h	Use alternate screen buffer and screen clear.\r\n"
"   ESC [ ? 2004 h Bracketed paste mode\r\n"
"                send: ESC[200~ ... ESC[201~ on paste\r\n"
"\r\n"
"   ESC [ ? 1 l	Normal cursor keys\r\n"
"   ESC [ ? 3 l	80/132 column mode switch.\r\n"
"   ESC [ ? 4 l	Set jump scroll mode.\r\n"
"   ESC [ ? 5 l	Normal video mode.\r\n"
"   ESC [ ? 7 l	Disable autowrap\r\n"
"   ESC [ ? 8 l	Disable autorepeat (no-op) Use XAutoRepeatOn)\r\n"
"   ESC [ ? 9 l        Turn off application mouse.\r\n"
"   ESC [ ? 25 l	Turn off cursor.\r\n"
"   ESC [ ? 47 l	Use normal screen buffer.\r\n"
"   ESC [ ? 1000 l	Exit  keypad mode.\r\n"
"   ESC [ ? 1049 l	Use alternate screen buffer/save cursor/scr clear\r\n"
"   ESC [ ? 2004 l Disbale Bracketed paste mode\r\n"
"\r\n"
"   ESC [ 6 n          Ask terminal for rows,col - ESC[r;cR\r\n"
"   ESC [ ? 3 r	Disable window resize.\r\n"
"   ESC [ ? 4 r	Enable window resize.\r\n"
"\r\n"
"   ESC [ ? r;c;h;w;n S Scroll rectangular region up or down n lines\r\n"
"\r\n"
"   ESC ] 0;<window/icon-title><BEL>\r\n"
"   ESC ] 1;<icon name><BEL>\r\n"
"   ESC ] 2;<window name><BEL>\r\n"
"   ESC ] 46;<log-file><BEL>    Change log file name.\r\n"
"   ESC ] 50;<font-name><BEL>   Change font name.\r\n"
"   ESC ] 100;<mini-label><BEL> Change label on status line.\r\n"
"\r\n"
"   ESC [ ? 0 f	Enable Graphics Expose processing.\r\n"
"   ESC [ ? 0 F	Disable Graphics Expose processing.\r\n"
"   ESC [ ? 1 f	Toggle fg/bg color\r\n"
"   ESC [ ? 2 f	Push current font onto stack.\r\n"
"   ESC [ ? 3 f	Pop font from stack.\r\n"
"\r\n"
"   ESC [ = C    	Hide cursor\r\n"
"   ESC [ = 1C    	Invisible cursor.\r\n"
"\r\n"
"   ESC # 3    	Top-half of double height\r\n"
"   ESC # 4    	Bottom-half of double height\r\n"
"   ESC # 5    	Normal height\r\n"
"   ESC # 6    	Double width\r\n"
"\r\n"
"   ESC [ > 0 c	Device attributes request. (secondary)\r\n"
"\r\n"
"   Sequences reported:\r\n"
"     ESC [ b; m; s; r; c M  Mouse position report\r\n"
"   			b == Button being reported (1..3)\r\n"
"   			s == 0 button pressed\r\n"
"   			  == 1 button released\r\n"
"   			  == 2 button motion\r\n"
"\r\n"
"   			m == Modifier status as a set of bits\r\n"
"   			     0x01 Shift down\r\n"
"   			     0x02 Ctrl down\r\n"
"   			     0x04 Meta down\r\n"
"\r\n"
"   			r == current row (decimal 0..n)\r\n"
"   			c == current col (decimal 0..n)\r\n"
"\r\n"
"Linux Console private sequences:\r\n"
"\r\n"
"       ESC [ 1 ; n ]       Set color n as the underline color\r\n"
"       ESC [ 2 ; n ]       Set color n as the dim color\r\n"
"       ESC [ 8 ]           Make the current color pair the default attributes.\r\n"
"       ESC [ 9 ; n ]       Set screen blank timeout to n minutes.\r\n"
"       ESC [ 10 ; n ]      Set bell frequency in Hz.\r\n"
"       ESC [ 11 ; n ]      Set bell duration in msec.\r\n"
"       ESC [ 12 ; n ]      Bring specified console to the front.\r\n"
"       ESC [ 13 ]          Unblank the screen.\r\n"
"       ESC [ 14 ; n ]      Set the VESA powerdown interval in minutes.\r\n"
"\r\n"
"       ESC [ ? p1 ; p2 ; p3 c Set cursor style\r\n"
"	    p1=	0  Don't change\r\n"
"		1  No cursor\r\n"
"		2  Underline\r\n"
"		3  Lower third of the block\r\n"
"		4  Lower half of the block\r\n"
"		5  Two thirds of the block\r\n"
"		6  Block cursor\r\n"
"	    p2/p3 = 0xbf = background/foreground color\r\n"
