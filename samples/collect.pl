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
my %opts = (
	f => "/tmp/collect.$ENV{USER}",
	t => 5,
	);

my %info;
my %delta;
my %old_info;

sub main
{
	Getopt::Long::Configure('require_order');
	Getopt::Long::Configure('no_ignore_case');
	usage() unless GetOptions(\%opts,
		'f=s',
		'help',
		't=s',
		);

	usage() if ($opts{help});

	$| = 1;

	###############################################
	#   Avoid  two  people  writing  to the same  #
	#   file.				      #
	###############################################
	if ( -f "/tmp/collect.$ENV{USER}.pid" ) {
		my $fh = new FileHandle("/tmp/collect.$ENV{USER}.pid");
		my $line = <$fh>;
		chomp($line);
		my ($pid, $owner) = split(" ", $line);
		if (-d "/proc/$pid" && (stat("/proc/$pid"))[4] == getuid()) {
			print "PID $pid is already collecting data. (/tmp/collect.$ENV{USER}.pid)\n";
			exit(0);
		}
	}
	my $fh = new FileHandle(">/tmp/collect.$ENV{USER}.pid");
	print $fh "$$ $<\n";
	$fh->close();
	undef $fh;

	my $sample = 0;
	if ( -f $opts{f} ) {
		$sample = `wc -l <$opts{f}`;
		chomp($sample);
		$sample =~ s/^ +//g;
	}

	my $first_sample = 1;
	my $delay = 2;
        while (1) {
		%old_info = %info;

		proc_diskstats();
		proc_interrupts();
		proc_loadavg();
		proc_mem();
		proc_net();
		proc_nfs();
		proc_proc();
		proc_stat();
		proc_vmstat();

		my $do_hdr = ! -f $opts{f};
		if (!$fh) {
			$fh = new FileHandle(">>$opts{f}");
			if ($do_hdr) {
				print $fh "timestamp,";
				foreach my $k (sort(keys(%info))) {
					print $fh "$k,";
				}
				print $fh "\n";
			}
		}
		###############################################
		#   Dont save the first sample.		      #
		###############################################
		if (!$first_sample) {
			print $fh strftime("%Y%m%d %H:%M:%S,", localtime);
			foreach my $k (sort(keys(%info))) {
				if (defined($delta{$k})) {
if ($info{$k} !~ /^\d+$/) {
print "subtraction issue for $k: $info{$k}\n";
}
					print $fh ($info{$k} - $old_info{$k}) / $opts{t} . ",";
				} else {
					print $fh "$info{$k},";
				}
			}
			print $fh "\n";
		}
		$first_sample = 0;
		my $ngraph = scalar(keys(%info));
if (scalar(keys(%old_info)) && scalar(keys(%info)) != scalar(keys(%old_info))) {
foreach my $k (sort(keys(%info))) {
	print "NEW $k\n" if !defined($old_info{$k});
}
}
		print strftime("%H:%M:%S sample $sample, $ngraph data sets\n", localtime);
		$fh->flush();
		$sample++;
                sleep($delay);
		$delay = $opts{t};
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

sub proc_diskstats
{
        my $fh = new FileHandle("/proc/diskstats");
	return if !$fh;
	my @cols = (
		"#rd",
		"#rdmrg",
		"#rdsect",
		"ms/rd",
		"#totms/rd",
		"#wr",
		"#wrmrg",
		"#wrsect",
		"ms/rd",
		"#io",
		"#msio",
		"msio"
		);
	while (<$fh>) {
		chomp();
		my ($maj, $min, $name, @counts) = split(" ");
#		next if $counts[0] == 0;
		$name = basename($name);
		for (my $i = 0; $i < @counts; $i++) {
			my $idx = "disk/$name/$cols[$i]";
			$info{$idx} = $counts[$i];
		}
	}

}
sub proc_interrupts
{
        my $fh = new FileHandle("/proc/interrupts");
        my $hdr = <$fh>;
	$hdr =~ s/^ //;
	my @cpus = split(" ", $hdr);
	my $ncpus = @cpus;
	while (<$fh>) {
		chomp();
		my ($name, @counts) = split(" ");
		$name =~ s/://;
		for (my $i = 0; $i < $ncpus; $i++) {
			my $idx = "int.$name.cpu$i";
			last if !defined($counts[$i]);
			$info{$idx} = $counts[$i];
			$delta{$idx} = 1;
#			$info{$idx} -= $old_info{$idx} if defined($old_info{$idx});
		}
	}

}
sub proc_loadavg
{
        my $fh = new FileHandle("/proc/loadavg");
        my $line = <$fh>;
        chomp($line);
        my @avg = split(" ", $line);
	$info{"loadavg1"} = $avg[0];
	$info{"loadavg5"} = $avg[1];
	$info{"loadavg15"} = $avg[2];
	($info{prun}, $info{numprocs}) = split(/\//, $avg[3]);
	$info{lastpid} = $avg[4];
}
sub proc_mem
{
        my $fh = new FileHandle("/proc/meminfo");
        while (<$fh>) {
                chomp;
		my ($name, $val) = split(" ");
		$name =~ s/://;
		$info{"mem.$name"} = $val;
        }
}
######################################################################
#   Collect network and interface statistics.			     #
######################################################################
sub proc_net
{
        my $fh = new FileHandle("/proc/net/sockstat");
        my $line = <$fh>;
        chomp($line);
        my @cols = split(" ", $line);
        $info{sockets} = $cols[2];
	while (<$fh>) {
		chomp;
		my ($name, @vars) = split(" ");
		$name =~ s/://;
		for (my $i = 0; $i < @vars; $i += 2) {
			my $idx = lc("$name.$vars[$i]");
			$info{$idx} = $vars[$i+1];
#print "idx $idx=$vars[$i+1]\n";
		}
	}

	$fh = new FileHandle("/proc/net/dev");
	my $hdr = <$fh>;
	$hdr = <$fh>;

	while (<$fh>) {
		chomp;
		$_ =~ s/^ +//;
		my ($name, @vals) = split(/[: ]+/);
		$info{"if.$name.rxbytes"} = shift @vals;
		$info{"if.$name.rxpkts"} = shift @vals;
		$info{"if.$name.rxerrs"} = shift @vals;
		$info{"if.$name.rxdrop"} = shift @vals;
		$info{"if.$name.rxfifo"} = shift @vals;
		$info{"if.$name.rxframe"} = shift @vals;
		$info{"if.$name.rxcomp"} = shift @vals;
		$info{"if.$name.rxmcast"} = shift @vals;

		$info{"if.$name.txbytes"} = shift @vals;
		$info{"if.$name.txpkts"} = shift @vals;
		$info{"if.$name.txerrs"} = shift @vals;
		$info{"if.$name.txdrop"} = shift @vals;
		$info{"if.$name.txfifo"} = shift @vals;
		$info{"if.$name.txframe"} = shift @vals;
		$info{"if.$name.txcomp"} = shift @vals;
		$info{"if.$name.txmcast"} = shift @vals;
	}

}
######################################################################
#   Collect NFS statistics.					     #
######################################################################
sub proc_nfs
{
        my $fh = new FileHandle("/proc/net/rpc/nfs");
	return if !$fh;

	my @cols_v2 = qw/
		null getattr setattr root lookup readlink
		read wrcache write create remove rename
		link symlink mkdir rmdir readdir fsstat
		/;
	my @cols_v3 = qw/
		null getattr setattr root lookup readlink
		read write create mkdir symlink mknod
		remove rmdir rename link readdir readdirplus
		fsstat fsinfo pathconf commit
		/;

	while (<$fh>) {
		chomp;
		my ($name, @vars) = split(" ");
		if ($name eq 'net') {
			next;
		}
		if ($name eq 'rpc') {
			$info{"nfs.rpc.calls"} = shift @vars;
			$info{"nfs.rpc.retr"} = shift @vars;
			$info{"nfs.rpc.authrf"} = shift @vars;
			next;
		}
		if ($name eq 'proc2') {
			shift @vars;
			foreach my $n (@cols_v2) {
				$info{"nfsv2.$n"} = shift @vars;
			}
			next;
		}
		if ($name eq 'proc3') {
			shift @vars;
			foreach my $n (@cols_v3) {
				$info{"nfsv2.$n"} = shift @vars;
			}
			next;
		}
	}
}
######################################################################
#   Collect process statistics.					     #
######################################################################
sub proc_proc
{
	my %state = (
		running => 0,
		sleeping => 0,
		stopped => 0,
		"disk sleep" => 0,
		"zombie" => 0,
		);
	foreach my $f (glob("/proc/[0-9]*/status")) {
		my $fh = new FileHandle($f);
		next if !$fh;
		my $name = <$fh>;
		my $state = <$fh>;
		chomp($state);
		$state =~ s/^.*\((.*)\)/$1/;
		if (!defined($state{$state})) {
			print "Unsupported proc state seen: $state\n";
			next;
		}
		$state{$state}++;
	}
	$info{"proc.$_"} = $state{$_} foreach keys(%state);
#	print "$_: $state{$_}\n" foreach sort(keys(%state));
}
sub proc_stat
{
        my $fh = new FileHandle("/proc/stat");
	while (<$fh>) {
		chomp;
		if (/^cpu/) {
			my ($name, @vals) = split(" ");
			my @cols = qw/user nice system idle irq softirq/;
			foreach my $c (@cols) {
				$info{"$name.$c"} = shift @vals;
				$delta{"$name.$c"} = 1;
			}
			next;
		}
		if (/^ctxt/) {
			my ($name, @vals) = split(/ /);
			$info{"cpu.ctxt"} = $vals[0];
			$delta{"cpu.ctx"} = 1;
			next;
		}
		if (/^page/) {
			my ($name, @vals) = split(/ /);
			$info{"vm.pgpgin"} = shift @vals;
			$info{"vm.pgpgout"} = shift @vals;
			next;
		}
	}
}
sub proc_vmstat
{
	###############################################
	#   AS3 doesnt have /proc/vmstat.	      #
	###############################################
        my $fh = new FileHandle("/proc/vmstat");
	if ($fh) {
	        while (<$fh>) {
	                chomp;
			my ($name, $val) = split(" ");
			$info{"vm.$name"} = $val;
	        }
		return;
	}

}
#######################################################################
#   Print out command line usage.                                     #
#######################################################################
sub usage
{
        print <<EOF;
collect.pl - tool to collect system information from /proc for graphing
Usage: collect.pl

Switches:

  -f filename         Name of file to write to. Default /tmp/collect.$ENV{USER}
  -t sec              Time in seconds between sampling.

EOF

        exit(1);
}

main();
0;
