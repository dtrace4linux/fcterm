#! /usr/bin/perl

# $Header:$

use strict;
use warnings;

use File::Basename;
use FileHandle;
use Getopt::Long;
use IO::File;
use POSIX;
use JSON;

#######################################################################
#   Command line switches.					      #
#######################################################################
my %opts;

sub main
{
	Getopt::Long::Configure('require_order');
	Getopt::Long::Configure('no_ignore_case');
	usage() unless GetOptions(\%opts,
		'help',
		);

	usage(0) if $opts{help};

	my %t;

	foreach my $fn (@ARGV) {
		my $fh = new FileHandle($fn);
		local $/ = undef;
		my $str = <$fh>;

		my $json = JSON->new->allow_nonref;
		my $info = $json->decode($str);

		my $s = '';
		$s .= "\"$info->{name}\":\n";
		$s .= "{\n";
		$s .= "\t\"author\": \"$info->{author}\",\n";
		$s .= "\t\"filename\": \"$fn\",\n";
		$s .= "\t\"background-color\": \"$info->{background}\",\n";
		$s .= "\t\"foreground-color\": \"$info->{foreground}\",\n";
		$s .= "\t\"color-palette-overrides\":\n";
		$s .= "\t\t[";
		my $comma = '';
		foreach my $c (@{$info->{color}}) {
			$s .= "$comma\"$c\"";
			$comma = ", ";
		}
		$s .= "]\n";
		$s .= "}\n";
		$s .= "\n";
		$t{$info->{name}} = $s;
	}

	my $comma = '';
	print "{\n";
	print "  \"source\": \"http://terminal.sexy\",\n";
	print "\n";

	foreach my $s (sort(keys(%t))) {
		print $comma, $t{$s};
		$comma = ",";
	}
	print "}\n";
}
#######################################################################
#   Print out command line usage.				      #
#######################################################################
sub usage
{	my $ret = shift;
	my $msg = shift;

	print $msg if $msg;

	print <<EOF;
mktheme2.pl -- convert terminal.sexy json files to ctw format.
Usage: mktheme2.pl <list of filenames>

Switches:

EOF

	exit(defined($ret) ? $ret : 1);
}

main();
0;

