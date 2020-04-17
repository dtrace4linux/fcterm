#! /usr/bin/perl

# Author: Paul D. Fox
# Date: April 2020

# TODO:
#   cursor positioning out byu (-1,-1)?

use strict;
use warnings;

use File::Basename;
use FileHandle;
use Getopt::Long;
use IO::File;
use POSIX;
use FindBin;

#######################################################################
#   Command line switches.					      #
#######################################################################
my %opts;
my @colors;
my $rows;
my $columns;

my $nf = 0;
my $t = 0;
my @times;
my $csr_x;
my $csr_y;
my $cursor_color;
my $is_player;
my $fn;
my $hashed;

######################################################################
#   Handle terminal drawing character sets and avoid html entities.  #
######################################################################
sub encode
{	my $attr = shift;
	my $ch = shift;

	if ($ch eq '<') {
		return "&lt;";
	} elsif ($ch eq '&') {
		return "&amp;";
	} elsif ($attr & 0x10 && $ch eq 'l') {
		return "&#x250c;";
	} elsif ($attr & 0x10 && $ch eq 'q') {
		return "&#x2500;";
	} elsif ($attr & 0x10 && $ch eq 'x') {
		return "&#x2502;";
	} elsif ($attr & 0x10 && $ch eq 'm') {
		return "&#x2514;";
	} elsif ($attr & 0x10 && $ch eq 'k') {
		return "&#x2510;";
	} elsif ($attr & 0x10 && $ch eq 'j') {
		return "&#x2518;";
	} elsif ($attr & 0x10 && $ch eq 'u') {
		return "&#x2524;";
	} elsif ($attr & 0x10 && $ch eq 't') {
		return "&#x251c;";
	} elsif (ord($ch) >= 0 && ord($ch) < 0x20) {
		return " ";
#	} elsif ($ch eq ' ') {
#		return "&nbsp;";
	}
	return $ch;
}

sub gen_svg
{
	my $duration = $opts{duration} || $t;
	my $fht = 17;
	my $fw = 8;
	my $page_ht = $rows * $fht;
	my $page_width = $columns * $fw;
	my $progress_ht = 5 + ($is_player ? 100 : 0);
	my $view_ht = $page_ht + $progress_ht;

	$t = int($t * 1000 * ($opts{slow} || 1));

	my $tfn = $opts{template};
	if ($tfn !~ /\//) {
		$tfn = "$FindBin::RealBin/templates/$tfn.svg";
		if (! -f $tfn) {
			$tfn =~ s/svg$/html/;
		}
	}
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
		if (!$opts{js}) {
			$colors .= sprintf "\t.c$i {fill: #$colors[$i];}\n";
		}
	}

	my $fh = new FileHandle($fn);
	my %color_hash;
	my $frame_time;
	my $y = 0;
	my $x = 0;
	my @jsframes;
	my @frames;
	my @frame;
	my %finfo;
	my %rects;
	my $frame_no = 0;
	my @timings;
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
			print "$_\n" if $opts{verbose};
			$row = 0;
			$col = 0;
			$jsframes[$frame_no] .= "</div>\n" if $frame_no;
			$frame_no++;
			$finfo{$frame_no}{csr_x} = $csr_x;
			$finfo{$frame_no}{csr_y} = $csr_y;
			push @timings, $frame_time;

			my $hidden = $frame_no > 1 ? " style=\"display: none\"" : "";
			$jsframes[$frame_no] =
				"<div id=\"frame$frame_no\"$hidden>\n";
			next;
		}

		$x = 0;
		my $ln = $_;
		my $line = '';
		my $jsline = "";
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
				$s .= encode($attr, $ch);
			}
			my $tl = $len * $fw;

			$jsline .= "<ln class='c${fg}_$bg'>$s</ln>";
			$color_hash{"${fg}_$bg"} = 1;

			$rects{$frame_no} .= "<rect x=\"$x\" y=\"$y\" width=\"$tl\" height=\"$fht\" class=\"c$bg\"/>\n" if $bg;
			$line .= "<text x=\"$x\" textLength=\"$tl\" class=\"c$fg\">";
			$line .= $s;
			$line .= "</text>\n";

			$i += $len;
			$x += $len * $fw;
		}
		$jsframes[$frame_no] .= "$jsline<br>";
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
	$jsframes[$frame_no] .= "</div>\n" if $frame_no;

	foreach my $c (keys(%color_hash)) {
		my ($fg, $bg) = split("_", $c);
		$colors .= sprintf("\t.c$c {color: #%s; background-color: #%s;}\n",
			$colors[$fg], $colors[$bg]);
	}

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
	$jsframes[0] = "";
	my $jsframe = join("", @jsframes);
	my $timings = join(", ", @timings);

	$template =~ s/\$frames/$jsframe/g;
	$template =~ s/\$timings/$timings/g;
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
	my $num_frames = scalar(@frames);
	$template =~ s/\$num_frames/$num_frames/g;

	my $progress_y = $page_ht;
	$template =~ s/\$progress_y/$progress_y/g;

	my $ofn = $opts{o};
	if (!$ofn) {
		$ofn = $opts{js} ? "$fn.html" : "$fn.svg";
		$ofn =~ s/\.cast.raw//;
	}

	printf "Creating: $ofn, size %d KB\n", length($template) / (1024);
	my $ofh = new FileHandle(">$ofn");
	die "Cannot create: $ofn - $!" if !$ofh;
	print $ofh $template;
}

sub main
{
	Getopt::Long::Configure('require_order');
	Getopt::Long::Configure('no_ignore_case');
	usage() unless GetOptions(\%opts,
		'duration=s',
		'frames=s',
		'help',
		'js',
		'label',
		'nohash',
		'o=s',
		'slow=s',
		'template=s',
		'verbose',
		);

	usage(0) if $opts{help};

	$fn = shift @ARGV;
	usage(0) if !$fn;

	$hashed = $opts{nohash} ? 0 : 1;

	if ($fn !~ /\.raw$/) {
		$fn .= ".raw";
	}

	read_file();

	if (!$opts{template} && $opts{js}) {
		$opts{template} = "js";
	} elsif (!$opts{template}) {
		$opts{template} = "player";
	}

	$is_player = ($opts{template} || '') =~ /player/;

	gen_svg();
}
sub read_file
{
	my $fh = new FileHandle($fn);
	die "Cannot open $fn - $!" if !$fh;

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
}
#######################################################################
#   Print out command line usage.				      #
#######################################################################
sub usage
{	my $ret = shift;
	my $msg = shift;

	print $msg if $msg;

	print <<EOF;
mksvg.pl -- create SVG/HTML or Javascript session out of a terminal recording
Usage: mksvg.pl [switches]

  This utility takes the raw "asciicast" files, which fcterm 
  generates, and provides a standalone html file - in SVG or
  JavaScript format, suitable for embedding in a web site.

  The advantage of these files, over, for example, screen recording,
  is crystal clear rendition of screen activity, and, very much
  smaller. SVG format is much smaller than vanilla javascript.
  But recordings can work out at 1MB per minute of screen activity,
  or more. Be wary of sending a large payload into the browser.

  The recordings have a playbar, so the playback can be paused or
  fwd/rew.

  This tool is very similar to termtosvg, but has better 
  integration with fcterm, and CRiSP terminal (line drawing) fonts.
  Additionally, this tool can generate Javascript or SVG HTML
  files. (Javascript is less overhead on the browser/cpu,
  but the files are larger than SVG).

  The algorithm for both file types is essentially a rendering of
  each frame, and then automating the "panning" over the frame to
  show each view of the screen. This is less than ideal, but avoids
  the code having to parse the escape sequences for cursor positioning
  or color handling.

  This tool relies on the specific file format generated by
  fcterm. Whilst fcterm does generate asciicast recording files,
  these are not useful without a terminal emulator to replicate
  the screen. (This is what termtosvg does). Instead, because
  fcterm is a terminal emulator, it additionally dumps a "raw" file
  containing the screen and attributes, allowing this script to 
  simply render the screen frames.

  To create a screen cast simply requires invoking fcterm. When
  invoking fcterm from inside fcterm, this will send an escape
  sequence to the emulator, to start recording. The "stop" command
  then sends a different sequence, and the parent, will close
  out the text files (and tell you where the files were
  recorded to).

  \$ fcterm record-silent
  \$ ....do your work...
  \$ fcterm stop
  ctw: recording stopped: output here:
    /var/tmp/fox/record-20200417-090738.cast
  \$

  "record-silent" will clear the screen - so prior data on the screen
  will not be recorded. ("fcterm record" will avoid clearing the screen).

  During recording, a flashing "[REC]" notation is displayed in the
  top right of the screen, so you can remember you are inside a
  recording session.

Future work:

  Might be useful to write our own escape sequence handler in javascript,
  but that is more work for the CPU to do, but would generate smaller
  files.

  Options to "edit" out unwanted frames. Current implementation
  allows the playback rate to be adjusted, but would be useful
  to remove noise created during creation.

Switches:

  -duration NN   Set animation duration to NN milliseconds.
  -frames NN     Only render the first N frames.
  -js            Generate vanilla Javascript file, instead of SVG.
  -label         Put a frame marker on the top of each page.
  -nohash        Dont hash the chunks of text - bigger output file, but
                 easier to debug.
  -o <filename>  Name of output file. Defaults to /tmp/test.svg or
  		 /tmp/test.html
  -slow NN       Slow down animation by this multiplier
  -template      Select the style of generated SVG generated. Options are
                 
		 default
		 player

Acknowledgements

   Nicholas Bedos https://nbedos.github.io

EOF

	exit(defined($ret) ? $ret : 1);
}

main();
0;

