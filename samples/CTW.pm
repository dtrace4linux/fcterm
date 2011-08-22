#!/usr/bin/perl

# $Header: $

# Author: Paul Fox
# Date: March 2010

# Perl module to interface to the fcterm graphical drawing escape sequences.

package CTW;

use strict;
use warnings;

sub new
{       my $class = shift;
        my %args = @_;

        my $self = bless {$class, "CTW"};
        $self->{autoflush} = $args{autoflush} || 0;
        $self->{buf} = '';
        return $self;
}
sub Buffer
{	my $self = shift;
	my $reset = shift;

	my $str = $self->{buf};
	$self->{buf} = '' if $reset;
	return $str;
}
sub Clear
{	my $self = shift;
	$self->{buf} = '';
}
sub ClearChain
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1924m");
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
	return $str;
}
sub ClearChainRefresh
{       my $self = shift;
        my %args = @_;

        my $str = "\033[1925m";
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub DrawLine
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1922;%d;%d;%d;%dm",
                $args{x1},
                $args{y1},
                $args{x2},
                $args{y2},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub DrawPixel
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1923;%d;%dm",
                $args{x},
                $args{y},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub DrawArc
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1928;%d;%d;%d;%d;%d;%dm",
                $args{x},
                $args{y},
                $args{width},
                $args{height},
                $args{arc1},
                $args{arc2},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub DrawImageString
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1933;%d;%d;\007%s\033m",
                $args{x},
                $args{y},
                $args{text});
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub DrawRectangle
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1920;%d;%d;%d;%dm",
                $args{x},
                $args{y},
                $args{width},
                $args{height});
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub DrawString
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1927;%d;%d;\007%s\033m",
                $args{x},
                $args{y},
                $args{text});
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub FillArc
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1929;%d;%d;%d;%d;%d;%dm",
                $args{x},
                $args{y},
                $args{width},
                $args{height},
                $args{arc1},
                $args{arc2},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub FillRectangle
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1921;%d;%d;%d;%dm",
                $args{x},
                $args{y},
                $args{width},
                $args{height});
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub Refresh
{       my $self = shift;

        print $self->{buf};
	STDOUT->flush();
        $self->{buf} = '';
}
sub SetBackground
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1930;%dm",
                $args{color},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub SetForeground
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1931;%dm",
                $args{color},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub SetFont
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1932;\007%s\033m",
                $args{name},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub SetName
{       my $self = shift;
        my %args = @_;

        my $str = sprintf("\033[1926;%d;%d;%d;%d;\007%s\033m",
                $args{x},
                $args{y},
                $args{width},
                $args{height},
                $args{name},
                );
        if ($self->{autoflush}) {
                print $str;
        } else {
                $self->{buf} .= $str;
        }
        return $str;
}
sub WindowSize
{	my $self = shift;

        print "\033[1934m";
	my $ans = <STDIN>;
	my ($win_width, $win_height) = split(/x/, $ans);
	return ($win_width, $win_height);
}

1;
