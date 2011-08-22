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
                'graph',
                'help',
                );

        usage() if ($opts{help});

        $| = 1;

        if ($opts{graph}) {
                my $w = 60;
                my $h = 60;
                my $num_samples = $w - 2;
                my $graph = dirname($0) . "/graph.pl";
                while (1) {
                        my $x = 10;
                        my $y = 10;
                        print "\033[2J\033[20H\n";
                        my $noclear = "";
                        foreach my $f (glob("/tmp/coll.loadavg*")) {
                                my $legend = basename($f);
                                $legend =~ s/^coll\.//;
                                spawn("$graph -last $num_samples $noclear -legend $legend -box -width $w -height $h -x $x -y $y $f");
                                $noclear = "-noclear";
                                $x += 100;
                        }
                        $x = 10;
                        $y += 85;
                        foreach my $f (glob("/tmp/coll.pg*")) {
                                my $legend = basename($f);
                                $legend =~ s/^coll\.//;
                                spawn("$graph -last $num_samples $noclear -legend $legend -box -width $w -height $h -x $x -y $y $f");
                                $noclear = "-noclear";
                                $x += 100;
                        }
                        $x = 10;
                        $y += 85;
                        foreach my $f (glob("/tmp/coll.int.*")) {
                                my $legend = basename($f);
                                $legend =~ s/^coll\.//;
                                spawn("$graph -last $num_samples $noclear -legend $legend -box -width $w -height $h -x $x -y $y $f");
                                $noclear = "-noclear";
                                $x += 100;
                        }
                        $x = 10;
                        $y += 85;
                        spawn("$graph -last $num_samples $noclear -legend sockets -box -width $w -height $h -x $x -y $y /tmp/coll.sockets");
                        $x += 100;
                        spawn("$graph -last $num_samples $noclear -legend memfree -box -width $w -height $h -x $x -y $y /tmp/coll.memfree");
                        $x += 100;
                        spawn("$graph -last $num_samples $noclear -legend memdirty -box -width $w -height $h -x $x -y $y /tmp/coll.memdirty");
                        $x += 100;
                        spawn("$graph -last $num_samples $noclear -legend swapfree -box -width $w -height $h -x $x -y $y /tmp/coll.swapfree");

                        my $ans = get_line();
                        next if !$ans;

                        while (1) {
                                print "\033[2J\033[H";
                                spawn("$graph -last 398 -legend $ans -box -width 400 -height 400 -x 10 -y 10 /tmp/coll.$ans");
                                last if get_line();
                        }
                }
                exit(0);
        }

        while (1) {
                proc_interrupts();
                proc_loadavg();
                proc_mem();
                proc_sockstat();
                proc_vmstat();
                sleep(5);
        }
}
sub append
{       my $fname = shift;
        my $val = shift;

        my $fh = new FileHandle(">>$fname");
        print $fh "$val\n";
}
sub get_line
{
        my $rbits = '';
        vec($rbits, STDIN->fileno(), 1) = 1;
        my $n = select($rbits, undef, undef, 5);
        return if $n <= 0;
        my $ans = <STDIN>;
        chomp($ans);
        return $ans;
}

my @old_interrupts;
my @interrupts;
sub proc_interrupts
{
        my $fh = new FileHandle("/proc/interrupts");
        my $hdr = <$fh>;
        my $cpus = <$fh>;
        $cpus =~ s/^ +\d+:\s+//;
        @interrupts = (split(" ", $cpus))[0..3];
        if (@old_interrupts == 0) {
                @old_interrupts = @interrupts;
                return;
        }

        for (my $i = 0; $i < 4; $i++) {
                my $fh = new FileHandle(">>/tmp/coll.int.cpu$i");
                print $fh $interrupts[$i] - $old_interrupts[$i], "\n";
        }
        @old_interrupts = @interrupts;

}
sub proc_loadavg
{
        my $fh = new FileHandle("/proc/loadavg");
        my $line = <$fh>;
        chomp($line);
        my @avg = split(" ", $line);
        for (my $i = 0; $i < 3; $i++) {
                my $fh = new FileHandle(">>/tmp/coll.loadavg$i");
                print $fh "$avg[$i]\n";
        }
}
sub proc_mem
{
        my $fh = new FileHandle("/proc/meminfo");
        while (<$fh>) {
                chomp;
                if (/^MemFree:\s+(\d+) /) {
                        append("/tmp/coll.memfree", $1);
                        next;
                }
                if (/^(Dirty|Inact_dirty):\s+(\d+) /) {
                        append("/tmp/coll.memdirty", $2);
                        next;
                }
                if (/^(SwapFree):\s+(\d+) /) {
                        append("/tmp/coll.swapfree", $2);
                        next;
                }
        }
}
sub proc_sockstat
{
        my $fh = new FileHandle("/proc/net/sockstat");
        my $line = <$fh>;
        chomp($line);
        my @cols = split(" ", $line);
        $fh = new FileHandle(">>/tmp/coll.sockets");
        print $fh "$cols[2]\n";
}
sub proc_vmstat
{
        my $fh = new FileHandle("/proc/vmstat");
        while (<$fh>) {
                chomp;
                foreach my $p (qw/pgpgin pgpgout pgfault pgmajfault/) {
                        if (/^$p\s+(\d+)/) {
                                append("/tmp/coll.$p", $1);
                                next;
                        }
                }
        }
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
Some help...
Usage:

Switches:

EOF

        exit(1);
}

main();
0;
