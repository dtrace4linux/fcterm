#! /usr/bin/perl

# $Header:$

use strict;
use warnings;

use File::Basename;
use FileHandle;
use Getopt::Long;
use IO::File;
use POSIX;

#######################################################################
#   Command line switches.                                            #
#######################################################################
my %opts;

sub main
{
	Getopt::Long::Configure('require_order');
	Getopt::Long::Configure('no_ignore_case');
	usage() unless GetOptions(\%opts,
	        'help',
	        );

	usage() if ($opts{help});

	my $pid = shift @ARGV;
	my $fh = new FileHandle("/proc/$pid/smaps");
	my $max_size = 0;
	my @maps;
	while (<$fh>) {
		chomp;
		my ($addr, $perms, $inode, $dev, $size, $path) = split(" ");
		my %info;
		for (my $i = 0; $i < 6; $i++) {
		        my $ln = <$fh>;
		        chomp($ln);
		        my ($name, $size) = split(" ", $ln);
		        $name =~ s/://;
		        $info{$name} = $size;
		#print "name=$name\n";
		}
		$size = $info{Size};
		$info{addr} = $addr;
		$info{perms} = $perms;
		$info{name} = $path || "<anon>";
		$max_size = $size if $size > $max_size;
		push @maps, \%info;
	}

	my $x = 0;
	my $y = 0;
	my $h = 10;
	printf "\033[1925m";
	foreach my $info (@maps) {
		my $rgb = 0xff80c0;
		my $w = ($info->{Size} / $max_size) * 500;
		printf "\033[1931;%dm", $rgb;
		printf "\033[1920;$x;$y;%d;%dm", $w, $h * 2;

		$w = ($info->{Rss} / $max_size) * (500 - 4);
		$rgb = 0xffe020;
		printf "\033[1931;%dm", $rgb;
		printf "\033[1921;%d;%d;%d;%dm", $x+2, $y+2, $w, $h - 4;

		$w = ($info->{Private_Dirty} / $max_size) * (500 - 4);
		$rgb = 0xff4040;
		printf "\033[1931;%dm", $rgb;
		printf "\033[1921;%d;%d;%d;%dm", $x+2, $y+2 + $h,
		        $w, $h - 4;

		$y += $h * 2 + 10;
	}
}
#######################################################################
#   Print out command line usage.                                     #
#######################################################################
sub usage
{
        print <<EOF;
Some help...
Usage:

Switches:

EOF

        exit(1);
}

main();
0;

