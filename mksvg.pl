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
		'help',
		);

	usage(0) if $opts{help};

	my $fn = shift @ARGV;
	usage(0) if !$fn;

	my $fh = new FileHandle($fn);
	my $nf = 0;
	my $t = 0;
	while (<$fh>) {
		chomp;
		if (/^frame:.*time: ([0-9.]+) rows: (\d+) columns: (\d+)/) {
			$nf++;
			$t = $1;
			$rows = $2;
			$columns = $3;
		} elsif (/^color (\d+): (.*)/) {
			push @colors, $2;
		}
	}
	if ($nf == 0) {
		print "cannot locate frames - is this the correct file?\n";
		exit(1);
	}
	print "Frames: $nf Time: $t\n";
	my $fht = 17;
	my $fw = 8;
	my $page_ht = $rows * $fht;
	my $page_width = $columns * $fw;
	$t = int($t * 1000);

	print "Creating: /tmp/test.svg\n";
	my $ofh = new FileHandle(">/tmp/test.svg");
	print $ofh <<EOF;
<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" 
  id="terminal" baseProfile="full" 
  viewBox="0 0 $page_width $page_ht" width="$page_width" version="1.1">
  <defs>
  	<!-- rows: $rows columns: $columns -->
	  <style type="text/css" id="generated-style"><![CDATA[#screen {
                font-family: 'DejaVu Sans Mono', monospace;
                font-style: normal;
                font-size: 14px;
            }
	    text {
            dominant-baseline: text-before-edge;
            white-space: pre;
        }
    
            :root {
                --animation-duration: ${t}ms;
            }

	    \@keyframes roll {
EOF

	for (my $i = 0; $i <= $nf; $i++) {
		my $pc = ($i * 100) / $nf;
#		my $y = -$page_ht * $i;
		my $y = -($i * 340);
		printf $ofh "%.3f%%{transform:translateY(${y}px)}\n", $pc;
	}
	print $ofh <<EOF;
	}

	#screen_view {
		animation-duration: ${t}ms;
		animation-iteration-count: infinite;
		animation-name: roll;
		animation-timing-function: steps(1,end);
		animation-fill-mode: forwards;
		}

]]></style>
        <style type="text/css" id="user-style">
	.foreground {fill: #c5c5c5;}
        .background {fill: #1c1c1c;}
EOF
	for (my $i = 0; $i < @colors; $i++) {
		print $ofh "\t\t.c$i {fill: #$colors[$i];}\n";
	}
	print $ofh <<EOF;

	    \@keyframes progress-bar-animation {
                from {
                    transform: translate(0px, calc(100% - 5px)) scale(0, 1);
                }
                to {
                    transform: translate(0px, calc(100% - 5px)) scale(1, 1);
                }
            }
            #progress-bar {
                animation-duration: var(--animation-duration);
                animation-iteration-count: infinite;
                animation-name: progress-bar-animation;
                animation-timing-function: linear;
                transform-origin: left;
            }
	    screen:hover {
	    	animation-play-sate: paused;
		}

        </style>

    </defs>

    <rect width="100%" height="100%" class="background"/>
    <svg id="screen" width="$page_width" height="$page_ht" viewBox="0 0 $page_width $page_ht" preserveAspectRatio="xMidYMin slice">
    <rect class="background" height="100%" width="100%" x="0" y="0"/>
    <defs>
EOF

	$fh = new FileHandle($fn);
	my $y = 0;
	my $x = 0;
	my @frames;
	my @frame;

#print $ofh "<g id=\"screen_view\">\n";
	while (<$fh>) {
		chomp;
		next if /^color/;
		if (/^frame/) {
			push @frames, \@frame if @frame;
			@frame = ();
			next;
		}

		$x = 0;
		my $ln = $_;
		my $line = '';
#print $ofh "<rect x=\"0\" height=\"$fht\" width=\"$page_width\" />\n";
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
		push @frame, $line;
#print $ofh "</rect>\n";
	}
#print $ofh "</g>\n";

	my @flines;
	my $f = 0;
	$y = 0;
	my $sv = '';
	foreach my $lns (@frames) {
		$sv .= "<g><rect x=\"0\" y='0' width='8' height='17' class='foreground' />\n";
		foreach my $ln (@$lns) {
			$sv .= "<use xlink:href='#g$f' y='$y'/>\n";
			$y += 17; #$rows * $fht;
			print $ofh "<g id='g$f'>\n";
				print $ofh $ln;
			print $ofh "</g>\n";
			$f++;
		}
		$sv .= "</g>\n";
	}

	print $ofh "</defs>\n";
	print $ofh "<g id=\"screen_view\">\n";
	print $ofh $sv;
	print $ofh "</g>\n";
	print $ofh <<EOF;
</svg>
    <rect id="progress-bar" width="100%" height="25" class="c1"/>
</svg>
EOF


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

