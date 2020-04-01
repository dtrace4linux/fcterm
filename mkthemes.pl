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

	my $fh = new FileHandle("src/themes.json");
	local $/ = undef;
	my $str = <$fh>;
	my $json = JSON->new->allow_nonref;
	my $info = $json->decode($str);

	my %names;

	my $ofh = new FileHandle(">src/themes.c");
	die "Cannot create src/themes.c - $!" if !$ofh;

	foreach my $item (sort(keys(%$info))) {
		next if "$item" eq 'source';
#		print "$item\n";
		my $name = $item;
		$name =~ s/[-. :]/_/g;
		$name =~ s/[()]//g;
		if ($name =~ /^[0-9]/) {
			$name = "name_$name";
		}
		$names{$name} = $item;
		my $pal = $info->{$item}->{"color-palette-overrides"};
		print $ofh "static char *$name\[] = {\n";
		my $i = 0;
		print $ofh "  /* background */ \"",
			$info->{$item}->{"background-color"}, 
			"\",\n";
		print $ofh "  /* foreground */ \"",
			$info->{$item}->{"foreground-color"}, 
			"\",\n";

		foreach my $c (@$pal) {
			#$c =~ s/#/0x/;
			printf $ofh "  /* %2d */ \"$c\",\n", $i++;
		}
		print $ofh "  0\n";
		print $ofh "};\n";
	}

	print $ofh "struct map {\n";
	print $ofh "\tchar *name;\n";
	print $ofh "\tchar **value;\n";
	print $ofh "} themes[] = {\n";
	print $ofh "\t{\"$names{$_}\", $_},\n" foreach sort(keys(%names));
	print $ofh "\t{0, 0}\n";
	print $ofh "};\n";
	print $ofh "\n";
	print $ofh "int num_themes = ", scalar(keys(%names)), ";\n";
}
#######################################################################
#   Print out command line usage.				      #
#######################################################################
sub usage
{	my $ret = shift;
	my $msg = shift;

	print $msg if $msg;

	print <<EOF;
Some help...
Usage:

Switches:

EOF

	exit(defined($ret) ? $ret : 1);
}

main();
0;

