#! /usr/bin/perl

# $Header:$
# Author: Paul Fox
# Date: 2010
#
# Script to display graphs in an fcterm window based on data collected from
# the collect.pl script.

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

######################################################################
#   Selections for the menu view.				     #
######################################################################
my %filters = (
	"All" => {
		mask => ".",
		},
	"Common" => {
		mask => "mem.MemFree|mem.Buffers|mem.Cached|" .
			"mem.SwapFree|mem.Dirty|lastpid|loadavg|proc|" .
			"if.*rxpkt|if.*txpkt",
		},
	"CPU" => {
		mask => "^cpu|numprocs|prun",
		},
	"Disk" => {
		mask => "disk",
		},
	"Interrupts" => {
		mask => "int",
		},
	"Memory" => {
		mask => "mem",
		},
	"NFS" => {
		mask => "nfs",
		},
	"Network" => {
		mask => "if.|sock|tcp|udp",
		},
	"Process" => {
		mask => "lastpid|loadavg|numprocs|prun|proc",
		},
	"VM" => {
		mask => "vm",
		},
	);

my $filter_sel = "All";
my $filter;
my @strip_names = (
	"^disk/",
	);

######################################################################
#   Per-graph options.						     #
######################################################################
my @graph_options = (
	"cpu.ctxt   	    	    delta",
	"vm.pgfree  	    	    delta",
	"vm.pgmajfault      	    delta",
	"vm.pgfault  	    	    delta",
	"vm.pgpgin  	    	    delta",
	"vm.pgpgout  	    	    delta",
	"vm.pgalloc_normal  	    delta",
	"vm.nr_mapped  	    	    delta",
	"vm.numa_local  	    delta",
	"vm.numa_hit  	    	    delta",
	"vm.pgactivate  	    delta",
	"vm.pgalloc_dma32  	    delta",
	"/#rd                       delta",
	"/#wr                       delta",
	"/#io                       delta",
	"if.*rx                     delta",
	"if.*tx                     delta",
	"nfs                        delta",
	);

#######################################################################
#   Command line switches.                                            #
#######################################################################
my %opts = (
	x => 120,
	y => 10,
	width => 60,
	height => 60
	);

my $fname;
my %graphs;
my @cols;
my $win_width;
my $win_height;
my $ctw;
sub main
{
        Getopt::Long::Configure('require_order');
        Getopt::Long::Configure('no_ignore_case');
        usage() unless GetOptions(\%opts,
		'graph=s',
                'help',
		'x=s',
		'y=s',
		'width=s',
		'height=s',
                );

	usage() if ($opts{help});

	$filter = $opts{graph} if $opts{graph};
	$fname = shift @ARGV;
	$fname = "/tmp/collect.$ENV{USER}" if !$fname;

	print "Please wait whilst initial data is loaded...\n";
	print "\n";

	my $fh = new FileHandle($fname);
	die "Cannot open $fname -- $!" if !$fh;

	my $hdr = <$fh>;
	chomp($hdr);
	@cols = split(/,/, $hdr);
	my $pos = tell($fh);

	###############################################
	#   If file is big, start from the end.	      #
	###############################################
	my $nlines = `wc -l <$fname`;
	chomp($nlines);
	$nlines =~ s/^ +//g;
	if ($nlines > 1000) {	
		my @poses;
		while (<$fh>) {
			push @poses, tell($fh);
		}
		$fh = new FileHandle($fname);
		seek($fh, $poses[-500], 0);
		$pos = tell($fh);
	}

	$ctw = new CTW;
	$SIG{INT} = sub {
		$ctw->ClearChainRefresh();
		$ctw->Refresh();
		_exit(0);
		};

	###############################################
	#   Create the graphs.			      #
	###############################################
	foreach my $col (@cols) {
		next if $col eq 'timestamp';
		my $color = 0x4040ff;
		$color = 0xffff00 if $col =~ /^int/;
		$color = 0x00ff00 if $col =~ /^loadvg/;
		$color = 0xa0d000 if $col =~ /^mem/;
		$color = 0x80ffff if $col =~ /^vm/;
		$color = 0xff9050 if $col =~ /\//;

		my $name = $col;
		foreach my $r (@strip_names) {
			$name =~ s/$r//;
		}

		my %options;
		foreach my $s (@graph_options) {
			my ($g, @vals) = split(" ", $s);
			next if $col !~ /$g/;
			$options{$_} = 1 foreach @vals;
		}
		$graphs{$col} = new CTWGraph(
			ctw => $ctw,
			realname => $col,
			name => $name,
			legend => $name,
			box => 1,
			x => $opts{x},
			y => $opts{y},
			width => $opts{width},
			height => $opts{height},
			color => $color,
			csv => $col,
			noclear => 1,
			%options,
			);
	}


	###############################################
	#   Display loop.			      #
	###############################################
	my $force_redraw = 1;
        while (1) {
                my $x;
                my $y;
		my $ans;
		my $nlines;

#		print "Winsize: $win_width $win_height\n";

		###############################################
		#   Get new data.			      #
		###############################################
		($pos, $nlines) = get_data($fh, $pos);

		###############################################
		#   Draw the menu bar.			      #
		###############################################
		if ($nlines || $force_redraw) {
			display_menu();

	                $x = $opts{x};
	                $y = $opts{y};
			foreach my $col (@cols) {
				next if $col eq 'timestamp';

				next if $filter && $col !~ /$filter/;
				my $g = $graphs{$col};

				$g->Set(x => $x);
				$g->Set(y => $y);
				$g->Set(width => $opts{width});
				$g->Set(height => $opts{height});
				next if $g->IsEmpty();

				$g->Plot(debug => 0);
				print $ctw->Buffer(1);

	                        if (($x += 100) >= $win_width - 100) {
					$x = $opts{x};
					$y += 85;
				}
				last if $y + 85 >= $win_height;
	                }
			$ctw->Refresh();
			$force_redraw = 0;
		}

		$ans = get_line(10);
		next if !$ans;
		$force_redraw = 1;
		next if !defined($graphs{$ans});
#printf("\033[1924m\033[2J\n ans=$ans\n");

#exit;
		while (1) {
			($pos, $nlines) = get_data($fh, $pos);
			if ($nlines || $force_redraw) {
				display_menu();

				my $g = $graphs{$ans};
				$g->Set(x => $opts{x});
				$g->Set(y => $opts{y});
				$g->Set(width => $win_width - 60 - $opts{x});
				$g->Set(height => $win_height - 100);
				$g->Plot(debug => 0);
				$ctw->Refresh();

				$force_redraw = 0;
			}

#			spawn(sprintf("$graph -last 398 -legend '$ans' -csv '$ans' -box " .
#				"-width %d -height %d -x 10 -y 10 $fname", $win_width - 50, $win_height - 50));
			last if get_line(10);
		}
		print $ctw->ClearChainRefresh();
		$force_redraw = 1;
        }
        exit(0);
}
######################################################################
#   Show the menu to the left.					     #
######################################################################
sub display_menu
{
	my $y;

	($win_width, $win_height) = $ctw->WindowSize();
	print "\033[2J\033[20H\n";
	$ctw->ClearChainRefresh();
	$ctw->SetForeground(color => 0x40ff80);

	$ctw->DrawRectangle(x => 10, y => 10, 
		width => $opts{x} - 20, 
		height => $win_height - 50);
	$ctw->FillRectangle(x => 11, y => 11, 
		width => $opts{x} - 20 - 2, 
		height => $win_height - 50 - 2);

	$y = $opts{y} + 13;
	$ctw->SetFont(name => "7x13bold");
	foreach my $label (sort(keys(%filters))) {
		my $text = $label;
		$text =~ s/-/ /g;
		if ($label eq $filter_sel) {
			$ctw->SetBackground(color => 0xff40c0);
			$ctw->SetForeground(color => 0x000000);
		} else {
			$ctw->SetForeground(color => 0x4040c0);
			$ctw->SetBackground(color => 0x80ff90);
		}
		$ctw->DrawImageString(x => 12, y => $y, text => $text);
		$ctw->SetName(x => 12, y => $y - 10,
			width => 12 + $opts{x} - 24,
			height => 13, 
			name => "select-$label");
		$y += 13;
	}
	$ctw->DrawLine(x1 => 12, y1 => $y, x2 => 12 + $opts{x} - 24, y2 => $y);
	$ctw->SetFont(name => "6x10");
	$y += 13 * 2;
	$ctw->DrawString(x => 12, y => $y, text => "Click on graphs");
	$y += 13;
	$ctw->DrawString(x => 12, y => $y, text => "or buttons above");

	$ctw->Refresh();
}
######################################################################
#   Get next batch of data from the collector.			     #
######################################################################
sub get_data
{	my $fh = shift;
	my $pos = shift;

	seek($fh, $pos, 0);

	my %coords;
	my $c;
	foreach my $f (keys(%graphs)) {
		$coords{$f} = $graphs{$f}->Coords();
	}

	my $lines = 0;
	while (<$fh>) {
		$lines++;
		$pos = tell($fh);

		chomp;
		my @vals = split(/,/);
		my $val0 = $vals[0];
		for (my $i = 1; $i < @cols; $i++) {
			my $g = $graphs{$cols[$i]};
#print "Adding '$cols[$i]' '$vals[$i]'\n";
#			$c = $coords{$g};
#			push @{$c}, {x => $time, y => $vals[$i]};
			$g->AddXY($val0, $vals[$i]);
		}
	}
	return ($pos, $lines);
}
######################################################################
#   Wait  a  while before redrawing graphs, but allow user to click  #
#   on a graph or button to select a command/action.		     #
######################################################################
sub get_line
{	my $t = shift;

	my $rbits = '';
	vec($rbits, STDIN->fileno(), 1) = 1;
	my $n = select($rbits, undef, undef, $t);
	return if $n <= 0;
	my $ans = <STDIN>;
	chomp($ans);
	if ($ans =~ /^select-/) {
		$filter = $ans;
		$filter =~ s/^select-//;
		$filter_sel = $filter;
		$filter = $filters{$filter}{mask};
	}
	return $ans;
}

sub spawn
{       my $cmd = shift;

        return system($cmd);
}
#######################################################################
#   Print out command line usage.                                     #
#######################################################################
sub usage
{
        print <<EOF;
display.pl - Display graph data collected via collect.pl in a CSV file.
Usage: display.pl [graph-name]

Switches:

EOF

        exit(1);
}

main();
0;
