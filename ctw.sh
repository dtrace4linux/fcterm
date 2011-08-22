#! /bin/sh
# Simple shell script to generate appropriate escape sequences so
# that user can change attributes of the CTW widget on the fly without
# having to remember those sequences.
ESC=""
BEL=""

set_font ()
{
	echo -n "${ESC}]50;$1$BEL"
}
set_log_file ()
{
	echo -n "${ESC}]46;$1$BEL"
}
set_window_size ()
{
	echo -n "${ESC}[8;$1;$2t"
}

get_answer ()
{
	echo -n $1
	read answer
}
option=""
while [ "$option" = "" ];
do
	echo ""
	echo "	Ctw Options Setup"
	echo "	================="
	echo ""
	echo "Select an option to set:"
	echo ""
	echo "	1. Change font"
	echo "	2. Change log file"
	echo "	3. Change window size"
	echo ""
	echo "Enter option: \c"
	read option
	case "$option" in
	  1)	get_answer "Font name: "
		;;
	  2)	get_answer "Log file name: "
		;;
	  3)	get_answer "New size (height width): "
		;;
	  *) option= ; ;;
	esac
done

if [ "$answer" = "" ]; then
	exit 0
fi

case "$option" in
	1)	set_font $answer
		;;
	2)	set_log_file $answer
		;;
	3)	set_window_size $answer
		;;
esac
exit 0
