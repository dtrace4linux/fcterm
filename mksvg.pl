#! /usr/bin/perl

# $Header:$

# TODO:
#   -label - needs bg color
#   progress bar display/pause/rewind

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
		'duration=s',
		'frames=s',
		'help',
		'label',
		'nohash',
		'slow=s',
		'template=s',
		);

	usage(0) if $opts{help};

	my $fn = shift @ARGV;
	usage(0) if !$fn;

	my $hashed = $opts{nohash} ? 0 : 1;

	if ($fn !~ /\.raw$/) {
		$fn .= ".raw";
	}

	my $is_player = ($opts{template} || '') =~ /player/;

	my $fh = new FileHandle($fn);
	die "Cannot open $fn - $!" if !$fh;
	my $nf = 0;
	my $t = 0;
	my @times;
	my $csr_x;
	my $csr_y;
	my $cursor_color;

	while (<$fh>) {
		chomp;
		if (/^frame:.*time: ([0-9.]+) rows: (\d+) columns: (\d+) csr: (\d+),(\d+)/) {
			$nf++;
			$t = $1;
			$rows = $2;
			$columns = $3;
			$csr_x = $4;
			$csr_y = $5;
			push @times, $t;
		} elsif (/^color (\d+): (.*)/) {
			push @colors, $2;
		} elsif (/^cursor (.*)/) {
			$cursor_color = $1;
		}
	}
	if ($nf == 0) {
		print "cannot locate frames - is this the correct file?\n";
		exit(1);
	}
	print "Frames: $nf Time: $t\n";
	my $duration = $opts{duration} || $t;
	my $fht = 17;
	my $fw = 8;
	my $page_ht = $rows * $fht;
	my $page_width = $columns * $fw;
	my $progress_ht = 5 + ($is_player ? 100 : 0);
	my $view_ht = $page_ht + $progress_ht;

	$t = int($t * 1000 * ($opts{slow} || 1));

	my $tfn = $opts{template} || "templates/defaults.svg";
	my $tfh = new FileHandle($tfn);
	die "Cannot open template: $tfn - $!" if !$tfh;
	my $template = '';
	while (<$tfh>) {
		$template .= $_;
	}
	$tfh->close();


	my $transforms = '';
	for (my $i = 0; $i < $nf; $i++) {
#		my $pc = ($times[$i] / $duration) * 100;
		my $pc = ($i * 100) / $nf;
		my $y = -($i * $fht * $rows);
		if ($is_player) {
			$transforms .= sprintf "{transform:'translate3D(0, ${y}px, 0)', easing: 'steps(1, end)'},\n";
		} else {
			$transforms .= sprintf "%.3f%%{transform:translateY(${y}px)}\n", $pc;
		}
	}

	my $colors = '';
	for (my $i = 0; $i < @colors; $i++) {
		$colors .= sprintf "\t.c$i {fill: #$colors[$i];}\n";
	}

	$fh = new FileHandle($fn);
	my $frame_time;
	my $y = 0;
	my $x = 0;
	my @frames;
	my @frame;
	my %finfo;
	my %rects;
	my $frame_no = 0;
	my $row = 0;
	my $col = 0;
	my $g = 0;

	my %line_hash;
	my $lh = 0;

	while (<$fh>) {
		chomp;
		next if /^color/;
		next if /^cursor/;
		if (/^frame.*time: ([^\s]*).* csr: (\d+),(\d+)/) {
			$frame_time = $1;
			$csr_x = $2;
			$csr_y = $3;
			if (@frame) {
				push @frames, [@frame];
#print "scalar: ", scalar(@frame), " frames=$opts{frames}\n";
				@frame = ();
				last if $opts{frames} && @frames > $opts{frames};
			}
print "$_\n";
			$row = 0;
			$col = 0;
			$frame_no++;
			$finfo{$frame_no}{csr_x} = $csr_x;
			$finfo{$frame_no}{csr_y} = $csr_y;
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

			my ($attr, $fg, $bg, $len) = split(/[.]/, $code);
			$len = hex($len);
			$attr = hex($attr);

			my $s = '';
			for (my $k = $i; $k < $i + $len; $k++) {
				my $ch = substr($ln, $k, 1);
				if ($ch eq '<') {
					$s .= "&lt;";
				} elsif ($ch eq '&') {
					$s .= "&amp;";
				} elsif ($attr & 0x10 && $ch eq 'l') {
					$s .= "&#x250c;";
				} elsif ($attr & 0x10 && $ch eq 'q') {
					$s .= "&#x2500;";
				} elsif ($attr & 0x10 && $ch eq 'x') {
					$s .= "&#x2502;";
				} elsif ($attr & 0x10 && $ch eq 'm') {
					$s .= "&#x2514;";
				} elsif ($attr & 0x10 && $ch eq 'k') {
					$s .= "&#x2510;";
				} elsif ($attr & 0x10 && $ch eq 'j') {
					$s .= "&#x2518;";
				} elsif (ord($ch) >= 0 && ord($ch) < 0x20) {
					#$s .= sprintf("<$ch:%2x>", ord($ch));
					$s .= " ";
				} else {
					$s .= $ch;
				}
			}
			my $tl = $len * $fw;
			$rects{$frame_no} .= "<rect x=\"$x\" y=\"$y\" width=\"$tl\" height=\"$fht\" class=\"c$bg\"/>\n" if $bg;
			$line .= "<text x=\"$x\" textLength=\"$tl\" class=\"c$fg\">";
			$line .= $s;
			$line .= "</text>\n";

			$i += $len;
			$x += $len * $fw;
		}
		if (!defined($line_hash{$line})) {
			$line_hash{$line} = $lh++;
		}
		$g = $line_hash{$line} if $hashed;
		$rects{$frame_no} .= "<use xlink:href=\"#g$g\" y=\"$y\"/>\n";
		$y += $fht;
		if (@frame == 0 && $opts{label}) {
			$line .= "<text>                                    [frame $frame_no Time: $frame_time]</text>";
		}
		push @frame, $line;
		$row++;
		$g++;
	}
	push @frames, \@frame if @frame;

	my @flines;
	my $f = 0;
	$frame_no = 0;
	$y = 0;
	my $defs2 = '';
	my $defs1 = '';

	if ($hashed) {
		foreach my $k (sort(keys(%line_hash))) {
			$defs1 .= "<g id='g$line_hash{$k}'>\n$k</g>\n";
		}
	}

	foreach my $lns (@frames) {
		$y = $page_ht + $frame_no++ * $page_ht;
		my $top_y = $y;
		$defs2 .= "<g>";
		$defs2 .= $rects{$frame_no};
		foreach my $ln (@$lns) {
			last if $hashed;
			$defs1 .= "<g id='g$f'>\n";
			$defs1 .= $ln;
			$defs1 .= "</g>\n";
			$f++;
		}

		###############################################
		#   Display cursor at the right point of the  #
		#   screen.				      #
		###############################################
		my $csr_x = $finfo{$frame_no}{csr_x} * $fw + $fw;
		my $csr_y = $finfo{$frame_no}{csr_y} * $fht + $top_y;
		$defs2 .= "<rect x=\"$csr_x\" y='$csr_y' width='$fw' height='$fht' class='cursor' />\n";
		$defs2 .= "</g>\n";
	}

	###############################################
	#   Generate output file.		      #
	###############################################
	$template =~ s/\$cursor/$cursor_color/g;
	$template =~ s/\$rows/$rows/g;
	$template =~ s/\$columns/$columns/g;
	$template =~ s/\$page_width/$page_width/g;
	$template =~ s/\$view_ht/$view_ht/g;
	$template =~ s/\$page_ht/$page_ht/g;
	$template =~ s/\$transforms/$transforms/g;
	$template =~ s/\$colors/$colors/g;
	$template =~ s/\$\{t}/$t/g;
	$template =~ s/\$defs1/$defs1/g;
	$template =~ s/\$defs2/$defs2/g;
	my $progress_y = $page_ht;
	$template =~ s/\$progress_y/$progress_y/g;

	printf "Creating: /tmp/test.svg, size %d KB\n", length($template) / (1024);
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
mksvg.pl -- create SVG/HTML session out of a terminal recording
Usage: mksvg.pl [switches]

Switches:

  -duration NN   Set animation duration to NN milliseconds.
  -frames NN     Only render the first N frames.
  -label         Put a frame marker on the top of each page.
  -nohash        Dont hash the chunks of text - bigger output file, but
                 easier to debug.
  -slow NN       Slow down animation by this multiplier
EOF

	exit(defined($ret) ? $ret : 1);
}

main();
0;

