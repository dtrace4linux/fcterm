#! /usr/bin/perl

for (my $i = 0; $i < 10; $i++) {
	my $x = int(rand(300));
	my $y = int(rand(300));
	my $rgb = 0xff8040 + $x;
	printf "\033[1931;%dm", $rgb;
	printf "\033[1920;$x;$y;200;200;m";
	$rgb = 0xffffff - $rgb;
	printf "\033[1931;%dm", $rgb;
	printf "\033[1921;%d;%d;190;190m", $x + 5, $y + 5;
}
my $rgb = 0x4070ff;
printf "\033[1931;%dm", $rgb;
printf "\033[1922;0;0;400;400m";
