sed -e 's/^/"/g' -e 's/$/\\r\\n"/g' <<EOF

   ESC ESC x  Print character x literally.
   ESC 7	Save cursor position
   ESC 8	Restore cursor position
   ESC @0	Not in curses.
   ESC @1     In curses emulation mode.
   ESC @2     Receive ioctl.
   ESC @3     Reply to ioctl.
   ESC c	Reset terminal
   ESC D	Index (down cursor or scroll).
   ESC H	Set tab stop at current position.
   ESC M	Reverse index (up cursor or insline).
   ESC Z	Identify terminal.
   ESC >	Numeric keypad
   ESC =	Application keypad
   ^H		Backspace
   ^I		Tabs (fixed-width -- 8 chars wide).
   ^J		Line feed
   ^L		Clear screen.
   ^M 	Carriage return
   ESC(0   ESC)0   ESC*0   ESC+0  Set character sets.
              0  Normal line drawing font set
              1  Alternate fonts
              2  Alternate fonts
              A  American ASCII
              B  British ASCII
   ESC [n @	Insert characters.
   ESC [n A	Move cursor up
   ESC [n B	Move cursor down
   ESC [n C	Move cursor right
   ESC [n D	Move cursor left
   ESC [n d	Goto (x, n)
   ESC [n E	Goto (0,y+n)
   ESC [n F	Goto (0,y-n)
   ESC [n G	Goto specified column.
   ESC [n;m H Move cursor to absolute position.
   ESC [n J   Erase screen 0==clear below; 1==clear above; 2==clear all
   ESC [n L	Insert lines at current row
   ESC [n M	Delete lines at current row
   ESC [n P	Delete characters.
   ESC [S	Index (down cursor or scroll).
   ESC [T	Reverse index (up cursor or insline).
   ESC [n X	Erase to blank (no cursor move)
   ESC [n Y	Set mouse button mask.
***ESC [n Z	Request mouse position.
   ESC [Z  	Backtab

   ESC [ c	Ask for answer back message.
   ESC [n;m f Move cursor to absolute position.
   ESC [n g	Print ASCII char 'n' (SCO mode only)
   ESC [3 g	Clear TAB stops.
   ESC [4 h	Enter insert mode
   ESC [4 l	Exit insert mode
   ESC [0 m	Reset attributes.
   ESC [1 m	Bold
   ESC [4 m	Underline
   ESC [5 m	Blink
   ESC [7 m	Reverse video.
   ESC [8 m	Store current colors as the default.
   ESC [10 m  Select primary font, don't display ctrl chars, do't set bit 8
   ESC [11 m  Select first alternate font; lets chars < 32 be displayed
   ESC [21m	Reset bold.
   ESC [24m	Reset underline.
   ESC [25m	Reset blink.
   ESC [27m	Reset reverse video.
   ESC [3Xm	Set foreground color.
   ESC [38 ; 5 ; XX m	Set foreground from 256 palette.
   ESC [39m	Restore foreground default color.
   ESC [4Xm	Set background color.
   ESC [48 ; 5 ; XX m	Set background from 256 palette.
   ESC [49m	Restore background default color.
   ESC [7Xm	Set cursor color.
   ESC [78m	Restore cursor color.
   ESC [1900 m  Stop literal mode.
   ESC [1901 m  Go into literal mode.
   ESC [1902 m  Go into strikethru mode.
   ESC [1903 m  Exit strikethru mode.
   ESC [1904 m  Enter autoswitch mode.
   ESC [1905 m  Exit autoswitch mode.
   ESC [1920 ; x ; y ; w ; h ; m         Draw a rectangle.
   ESC [1921 ; x ; y ; w ; h ; m         Fill a rectangle.
   ESC [1922 ; x1 ; y1; x2 ; y2; m       Draw a line
   ESC [1923 ; x ; y ; m                 Draw a pixel.
   ESC [1924 m Clear graphics chain.
   ESC [1925 m Clear graphics chain and redraw screen.
   ESC [1926 ; x; y; w; h; name m        Set name     		
   ESC [1927 ; x; y; str; m              Draw string  		
   ESC [1928 ; x; y; w; h; arc1; arc2; m Arc      		
   ESC [1929 ; x; y; w; h; arc1; arc2; m FillArc      		
   ESC [1930 ; rgb m                     Set background
   ESC [1931 ; rgb m                     Set foreground
   ESC [1932 ; name ; m                  Set font   
   ESC [1933 ; x; y; str; m              Draw image string  		
   ESC [1934 m                           Query winsize in pixels (wxh<Enter>)
   ESC [1935 m                           Dump internal status to a /tmp/fcterm/fcterm.$CTW_PID
   ESC [1936 m    Box each character drawn.
   ESC [1937 m    Unox each character drawn.
   ESC [n;m r	Set scrolling region to lines n..m.
   ESC [n;m r	Set scrolling region.
   ESC [s	Saved cursor position.
   ESC [n t	Shelltool/cmdtool compatable escape sequence
   		ESC [1t	Open window.
   		ESC [2t	Iconise window.
   		ESC [3t	Move window.
   		ESC [4t	Change size (in pixels) window.
   		ESC [5t	Raise window.
   		ESC [6t	Lower window.
   		ESC [8t	Change size (in chars) window.
   ESC [u	Restore cursor position.

   ESC [ ? 1 h	Application cursor keys
   ESC [ ? 3 h	80/132 column mode switch.
   ESC [ ? 4 h	Set smooth scroll mode.
   ESC [ ? 5 h	Reverse video mode.
   ESC [ ? 7 h	Set autowrap
   ESC [ ? 8 h	Enable autorepeat (no-op) Use XAutoRepeatOn)
   ESC [ ? 9 h	Turn on application mouse.
   ESC [ ? 12 h	Move this console to the front.
   ESC [ ? 25 h	Turn on cursor.
   ESC [ ? 47 h	Use alternate screen buffer.
   ESC [ ? 1000 h	Enter keypad mode.
   ESC [ ? 1034 h	SMM mode
   ESC [ ? 1049 h	Use alternate screen buffer and screen clear.

   ESC [ ? 1 l	Normal cursor keys
   ESC [ ? 3 l	80/132 column mode switch.
   ESC [ ? 4 l	Set jump scroll mode.
   ESC [ ? 5 l	Normal video mode.
   ESC [ ? 7 l	Disable autowrap
   ESC [ ? 8 l	Disable autorepeat (no-op) Use XAutoRepeatOn)
   ESC [ ? 9 l        Turn off application mouse.
   ESC [ ? 25 l	Turn off cursor.
   ESC [ ? 47 l	Use normal screen buffer.
   ESC [ ? 1000 l	Exit  keypad mode.
   ESC [ ? 1049 l	Use alternate screen buffer/save cursor/scr clear

   ESC [ 6 n          Ask terminal for rows,col - ESC[r;cR
   ESC [ ? 3 r	Disable window resize.
   ESC [ ? 4 r	Enable window resize.

   ESC [ ? r;c;h;w;n S Scroll rectangular region up or down n lines

   ESC ] 0;<window/icon-title><BEL>
   ESC ] 1;<icon name><BEL>
   ESC ] 2;<window name><BEL>
   ESC ] 46;<log-file><BEL>    Change log file name.
   ESC ] 50;<font-name><BEL>   Change font name.
   ESC ] 100;<mini-label><BEL> Change label on status line.

   ESC [ ? 0 f	Enable Graphics Expose processing.
   ESC [ ? 0 F	Disable Graphics Expose processing.
   ESC [ ? 1 f	Toggle fg/bg color
   ESC [ ? 2 f	Push current font onto stack.
   ESC [ ? 3 f	Pop font from stack.

   ESC [ = C    	Hide cursor
   ESC [ = 1C    	Invisible cursor.

   ESC # 3    	Top-half of double height
   ESC # 4    	Bottom-half of double height
   ESC # 5    	Normal height
   ESC # 6    	Double width

   ESC [ > 0 c	Device attributes request. (secondary)

   Sequences reported:
     ESC [ b; m; s; r; c M  Mouse position report
   			b == Button being reported (1..3)
   			s == 0 button pressed
   			  == 1 button released
   			  == 2 button motion

   			m == Modifier status as a set of bits
   			     0x01 Shift down
   			     0x02 Ctrl down
   			     0x04 Meta down

   			r == current row (decimal 0..n)
   			c == current col (decimal 0..n)

EOF
