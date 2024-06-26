#! /bin/sh

case $(uname) in
  Darwin)
  	exit 0
	;;
esac

err=0
for i in "/usr/bin/pkgconf pkgconf" \
	"/usr/include/X11/Xaw/Label.h libxaw7-dev" \
	"/usr/include/X11/Xft/Xft.h libxft-dev"
do
	set -- $i
	fn=$1
	pkg=$2
	if [ ! -f $fn ]; then
		echo "$fn is missing, from package $pkg"
		echo ""
		echo "Please run:"
		echo ""
		echo "  apt-get install $pkg"
		echo ""
		err=1
	fi
done
exit $err
