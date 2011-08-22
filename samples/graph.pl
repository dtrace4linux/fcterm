#! /usr/bin/perl

# $Header:$

use strict;
use warnings;

use File::Basename;
use FileHandle;
use Getopt::Long;
use IO::File;
use POSIX;

use lib dirname($0);
use CTW;
use CTWGraph;

#######################################################################
#   Command line switches.					      #
#######################################################################
my %opts;

sub main
{
        Getopt::Long::Configure('require_order');
        Getopt::Long::Configure('no_ignore_case');
        usage() unless GetOptions(\%opts,
                'csv=s',
                'legend=s',
                'box',
                'clear',
                'color=s',
                'height=s',
                'help',
                'last=s',
                'noclear',
                'width=s',
                'x=s',
                'y=s',
                );

        usage() if ($opts{help});

        if ($opts{clear}) {
        	my $ctw = new CTW;
                $ctw->ClearChain();
                $ctw->Refresh();
                exit(0);
        }


	my $fname = shift @ARGV;
	my $fh = new FileHandle($fname);
	if (!$fh) {
	        print "ERROR: $fname -- $!\n";
	        exit(1);
	}

	$| = 1;

	my $graph = new CTWGraph(%opts);
	$graph->ClearScreen() if !$opts{noclear};

        if ($opts{csv}) {
#               $fh = new FileHandle("head -1 $fname ; tail +$opts{width} $fname |");
                my $hdr = <$fh>;
                chomp($hdr);
                my @cols = split(/,/, $hdr);
                my %cols;
                for (my $i = 0; $i < @cols; $i++) {
                        $cols{$cols[$i]} = $i;
                }
                my $x = 0;
                while (<$fh>) {
                        chomp;
                        my @vals = split(/,/);
                        my $y = $vals[$cols{$opts{csv}}];
                        $x = $vals[0];
			$graph->Add(x => $x, y => $y);
                }
        } else {
                my $x = 0;
                while (<$fh>) {
                        chomp;
                        if (/^set\s/) {
                                do_set($_);
                                next;
                        }

                        my $y = $_;
                        $x++;

			$graph->Add(x => $x, y => $y);
                }
        }
	$graph->Plot();
}
#######################################################################
#   Print out command line usage.                                     #
#######################################################################
sub usage
{
        print <<EOF;
Utility to plot graphs in the fcterm terminal emulator using the CTW Widget
Usage:

Switches:

EOF

        exit(1);
}

main();
0;
