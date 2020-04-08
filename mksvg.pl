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
#   Command line switches.					      #
#######################################################################
my %opts;
my @colors;
my $rows;
my $columns;

sub main
{
	Getopt::Long::Configure('require_order');
	Getopt::Long::Configure('no_ignore_case');
	usage() unless GetOptions(\%opts,
		'frames=s',
		'help',
		'slow=s',
		);

	usage(0) if $opts{help};

	my $fn = shift @ARGV;
	usage(0) if !$fn;

	my $fh = new FileHandle($fn);
	die "Cannot open $fn - $!" if !$fh;
	my $nf = 0;
	my $t = 0;
	my $csr_x;
	my $csr_y;
	while (<$fh>) {
		chomp;
		if (/^frame:.*time: ([0-9.]+) rows: (\d+) columns: (\d+) csr: (\d+),(\d+)/) {
			$nf++;
			$t = $1;
			$rows = $2;
			$columns = $3;
			$csr_x = $4;
			$csr_y = $5;
		} elsif (/^color (\d+): (.*)/) {
			push @colors, $2;
		}
	}
	if ($nf == 0) {
		print "cannot locate frames - is this the correct file?\n";
		exit(1);
	}
	print "Frames: $nf Time: $t\n";
	my $fht = 14;
	my $fw = 8;
	my $page_ht = $rows * $fht;
	my $page_width = $columns * $fw;
	$t = int($t * 1000 * ($opts{slow} || 1));

	my $tfh = new FileHandle("templates/default.svg");
	my $template = '';
	while (<$tfh>) {
		$template .= $_;
	}
	$tfh->close();


	my $transforms = '';
	for (my $i = 0; $i <= $nf; $i++) {
		my $pc = ($i * 100) / $nf;
#		my $y = -$page_ht * $i;
		my $y = -($i * 340);
		$transforms .= sprintf "%.3f%%{transform:translateY(${y}px)}\n", $pc;
	}

	my $colors = '';
	for (my $i = 0; $i < @colors; $i++) {
		$colors .= sprintf "\t\t.c$i {fill: #$colors[$i];}\n";
	}

	$fh = new FileHandle($fn);
	my $y = 0;
	my $x = 0;
	my @frames;
	my @frame;

	while (<$fh>) {
		chomp;
		next if /^color/;
		if (/^frame.* csr: (\d+),(\d+)/) {
			$csr_x = $1;
			$csr_y = $2;
			if (@frame) {
				push @frames, [@frame];
#print "scalar: ", scalar(@frame), " frames=$opts{frames}\n";
				@frame = ();
				last if $opts{frames} && @frames > $opts{frames};
			}
print "$_\n";
			$y = 0;
			next;
		}

		$x = 0;
		my $ln = $_;
		my $line = '';
		for (my $i = 0; $i < length($ln); ) {
			my $j = 0;
			for ($j = $i; $j < length($ln); $j++) {
				last if substr($ln, $j, 1) eq " ";
			}
			my $code = substr($ln, $i, $j-$i);
			$i = $j + 1;

			my ($attr, $fg, $b, $len) = split(/[.]/, $code);
			$len = hex($len);

			my $s = '';
			for (my $k = $i; $k < $i + $len; $k++) {
				my $ch = substr($ln, $k, 1);
				if ($ch eq '<') {
					$s .= "&lt;";
				} elsif ($ch eq '&') {
					$s .= "&amp;";
				} else {
					$s .= $ch;
				}
			}
			if ($s =~ /[^ ]/) {
				my $tl = $len * $fw;
				$line .= "<text x=\"$x\" textLength=\"$tl\" class=\"c$fg\">";
				$line .= $s;
				$line .= "</text>\n";
			}
			$i += $len;
			$x += $len * $fw;
		}
#print "line=$line\n";
		push @frame, $line;
	}
	push @frames, \@frame if @frame;

	my @flines;
	my $f = 0;
	$y = 0;
	my $sv = '';
	my $defs1 = '';
	my $frame_no = 0;
	foreach my $lns (@frames) {
		$y = $page_ht + $frame_no++ * $page_ht;
		$sv .= "<g><rect x=\"0\" y='$y' width='8' height='$fht' class='foreground' />\n";
		foreach my $ln (@$lns) {
			$sv .= "<use xlink:href='#g$f' y='$y'/>\n";
			$y += $fht; #$rows * $fht;
			$defs1 .= "<g id='g$f'>\n";
			$defs1 .= $ln;
			$defs1 .= "</g>\n";
			$f++;
		}
		$sv .= "</g>\n";
	}

	###############################################
	#   Generate output file.		      #
	###############################################
	$template =~ s/\$rows/$rows/g;
	$template =~ s/\$columns/$columns/g;
	$template =~ s/\$page_width/$page_width/g;
	$template =~ s/\$page_ht/$page_ht/g;
	$template =~ s/\$transforms/$transforms/g;
	$template =~ s/\$colors/$colors/g;
	$template =~ s/\$\{t}/$t/g;
	$template =~ s/\$defs1/$defs1/g;
	$template =~ s/\$defs2/$sv/g;

	print "Creating: /tmp/test.svg\n";
	my $ofh = new FileHandle(">/tmp/test.svg");
	print $ofh $template;


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

  -frames NN     Only render the first N frames.
  -slow NN       Slow down animation by this multiplier
EOF

	exit(defined($ret) ? $ret : 1);
}

main();
0;

