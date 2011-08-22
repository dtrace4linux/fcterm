#! /usr/bin/perl

# $Header:$
package CTWGraph;

use strict;
use warnings;

use File::Basename;
use FileHandle;
use Getopt::Long;
use IO::File;
use POSIX;

use CTW;

my $max_coords = 500;

sub new
{       my $class = shift;
        my %args = @_;

        my $self = bless {$class, "CTWGraph"};

        $self->{opts}{x} = 10;
        $self->{opts}{y} = 10;
        $self->{opts}{width} = 200;
        $self->{opts}{height} = 200;
        $self->{opts}{color} = 0xff9030;
        $self->{opts}{background_color} = 0x401010;
        $self->{autoflush} = $args{autoflush} || 0;
        $self->{opts}{$_} = $args{$_} foreach keys(%args);
        if ($args{ctw}) {
                $self->{ctw} = $args{ctw};
        } else {
                $self->{ctw} = new CTW;
        }
        $self->{coords} = [];
        return $self;
}
sub Add
{       my $self = shift;
        my %args = @_;

        push @{$self->{coords}}, { x => $args{x}, y => $args{y} };
}
sub AddXY
{       my $self = shift;
        my $x = shift;
        my $y = shift;

        push @{$self->{coords}}, [$x, $y];
#       push @{$self->{coords}}, {x => $x, y => $y};
        if (@{$self->{coords}} > $max_coords) {
                shift @{$self->{coords}};
        }
}
sub Coords
{       my $self = shift;
#use Data::Dumper;
#print Dumper($self);
        return $self->{coords};
}
sub ClearScreen
{       my $self = shift;

        $self->{ctw}->ClearChainRefresh();
}
######################################################################
#   Let  caller know if this is a trivial looking graph, so we dont  #
#   bother displaying it.                                            #
######################################################################
sub IsEmpty
{       my $self = shift;

        my @coords = @{$self->{coords}};
        return 1 if scalar(@coords) == 0;

        my $n = $#coords - $self->{opts}{width};
        $n = 0 if $n < 0;
        my @c = @coords[$n .. $#coords];
        my $min_y = $c[0]->[1] || 0;
        my $max_y = $c[0]->[1] || 0;
        foreach my $h (@c) {
                $min_y = $h->[1] if $h->[1] < $min_y;
                $max_y = $h->[1] if $h->[1] > $max_y;
                return 0 if $min_y != $max_y;
        }
        return 1 if $min_y == $max_y && $min_y == 0;
        return 0;

}
######################################################################
#   Draw the graph.                                                  #
######################################################################
sub Plot
{       my $self = shift;
        my %args = @_;

        my %opts = %{$self->{opts}};

        my $ctw = $self->{ctw};        

        my @coords = @{$self->{coords}};

        my $max_x = 0;
        my $max_y = 0;
        my $min_y;

        my $x0 = $opts{x};
        my $y0 = $opts{y};
#        $ctw->ClearChainRefresh() if !$opts{noclear};
        my $max_ent = $opts{last} || $opts{width};
        if ($max_ent && scalar(@coords) > $max_ent) {
                @coords = @coords[$#coords - $max_ent .. $#coords];
        }

        ###############################################
        #   Handle  graphs  which  have accumulating  #
        #   totals - split out into differences.      #
        ###############################################
        if ($opts{delta}) {
                my @nc;
                push @nc, [0, 0];
                for (my $i = 1; $i < @coords; $i++) {
                        push @nc, [
                                $coords[$i-1]->[0],
                                $coords[$i]->[1] - $coords[$i-1]->[1]
                                ];
                }
                @coords = @nc;
        }

        ###############################################
        #   Compute max/min Y                         #
        ###############################################
        $min_y = 0; #$coords[0]->{y} || 0;
        $max_y = @coords ? $coords[0]->[1] : 0;
        foreach my $h (@coords) {
                $min_y = $h->[1] if $h->[1] < $min_y;
                $max_y = $h->[1] if $h->[1] > $max_y;
        }
        $self->{min_y} = $min_y;
        $self->{max_y} = $max_y;

        if ($args{debug}) {
                print "min_y=$min_y max_y=$max_y\n";
        }

        if ($opts{background_color}) {
                $ctw->SetForeground(color => $opts{background_color});
        }
        $ctw->FillRectangle(x => $opts{x}, y => $opts{y},
                width => $opts{width}, height => $opts{height});

        ###############################################
        #   Now draw the graph.                       #
        ###############################################
        $ctw->SetForeground(color => $opts{color});
        my $i = 0;
        if ($max_y != $min_y) {
                my $nc = scalar(@coords);
                foreach my $h (@coords) {
                        my $x = $opts{x} +
                                $i++ / $nc * $opts{width};
                        next if !defined($h);
                        my $y = $opts{y} + $opts{height} - 
                                ($h->[1] - $min_y) / ($max_y - $min_y) * $opts{height};
#print "optsy=$opts{y}\n";
#print $h->[1], "\n";
if ($args{debug}) {print $x, " ", $y, " y1=", $opts{y} + $opts{height}, "  y=$y\n";}
        #               print "$h->{x}->$x $h->{y}->$y\n";
                        if (0) {
                                $ctw->DrawLine(x1 => $x0, y1 => $y0,
                                        x2 => $x, y2 => $y);
                        } else {
                                $ctw->DrawLine(x1 => $x, 
                                        y1 => $opts{y} + $opts{height},
                                        x2 => $x, y2 => $y);
                        }
                        $x0 = $x;
                        $y0 = $y;
        #               $x0 = $opts{x} * $n / scalar(@coords);
                }
        }
if ($args{debug}) {print "min_y=$min_y max_y=$max_y\n";}

        ###############################################
        #   Draw a box if requested.                  #
        ###############################################
        if ($opts{box}) {
                $ctw->SetForeground(color => 0x40c0ff);
                $ctw->DrawRectangle(
                        x => $opts{x} - 2, y => $opts{y} - 2,
                        width => $opts{width} + 4,
                        height => $opts{height} + 4);
        }
        $ctw->SetName(
                x => $opts{x} - 2, y => $opts{y} - 2,
                width => $opts{width} + 4,
                height => $opts{height} + 4,
                name => $opts{realname} || $opts{name} || $opts{legend} || "nothing");

        ###############################################
        #   Draw legend details.                      #
        ###############################################
        if ($opts{legend}) {
                $ctw->SetForeground(color => 0xffffff);
                $ctw->SetBackground(color => 0xffffff);
                $ctw->SetFont(name => "6x9");
                $ctw->DrawString(x => $opts{x} - 2,
                        y => $opts{y} + $opts{height} + 16,
                        text => $opts{legend}
                        );

                $ctw->SetForeground(color => 0x90c0c0);
                my $lines = 5;
                if ($opts{legendflags} =~ /yminmax/) {
                        $ctw->DrawString(
                                x => $opts{x} + $opts{width} + 6,
                                y => $opts{y} + $opts{height},
                                text => $min_y);
                        $ctw->DrawString(
                                x => $opts{x} + $opts{width} + 6,
                                y => $opts{y},
                                text => $max_y);
                } elsif ($min_y != $max_y) {
                        for (my $i = 1; $i < $lines; $i++) {
                                $ctw->DrawLine(
                                        x1 => $opts{x},
                                        y1 => $opts{y} + $opts{height} / $lines * $i,
                                        x2 => $opts{x} + $opts{width},
                                        y2 => $opts{y} + $opts{height} / $lines * $i);

                                my $n;
                                if ($min_y > 50) {
                                        $n = $min_y + int(($max_y - $min_y) * $i / $lines);
                                } else {
                                        $n = sprintf("%.2f", $min_y + ($max_y - $min_y) * $i / $lines);
                                        if (length($n) > 5) {
                                                $n = $min_y + int(($max_y - $min_y) * $i / $lines);
                                        }
                                }
                                $ctw->DrawString(
                                        x => $opts{x} + $opts{width} + 6,
                                        y => $opts{y} + $opts{height} / $lines * ($lines - $i),
                                        text => $n);
                        }
                }

                ###############################################
                #   Bottom ticks.                             #
                ###############################################
                if ($opts{width} > 300) {
                        for (my $i = 1; $i < $lines; $i++) {
                                my $xstr = $coords[$i / $lines * scalar(@coords)]->[0];
                                $xstr =~ s/^.* //;
                                $ctw->DrawLine(
                                        x1 => $opts{x} + $opts{width} * $i / $lines,
                                        y1 => $opts{y} + $opts{height} + -4,
                                        x2 => $opts{x} + $opts{width} * $i / $lines,
                                        y2 => $opts{y} + $opts{height} + 4);
                                $ctw->DrawString(
                                        x => $opts{x} + $opts{width} * $i / $lines,
                                        y => $opts{y} + $opts{height} + 13,
                                        text => $xstr);
                        }
                }
        }
        return $ctw->Buffer();
}
######################################################################
#   Set per graph settings.                                          #
######################################################################
sub Set
{       my $self = shift;
        my $ln = shift;
        my $val = shift;

        if ($val) {
                $self->{opts}{$ln} = $val;
                return;
        }

        if ($ln =~ /^set\s+([A-Za-z_]+)\s*=\s*(\d+)/) {
                $self->{opts}{$1} = $2;
                return;
        }
        if ($ln =~ /^set\s+([A-Za-z_]+)\s*=\s*"(.*)"/) {
                $self->{opts}{$1} = $2;
                return;
        }
}
1;