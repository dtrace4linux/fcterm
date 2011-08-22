sed -e 's/^/"/g' -e 's/$/\\r\\n"/g' <<'EOF'
#! /usr/bin/perl

my $esc = chr(033);
for (my $i = 0; $i < 10; $i++) {
	my $x = int(rand(300));
	my $y = int(rand(300));
	my $rgb = 0xff8040 + $x;
	printf \"${esc}[1920;$x;$y;200;200;%dm\", $rgb;
	$rgb = 0xffffff - $rgb;
	printf \"${esc}[1921;%d;%d;190;190;%dm\", $x + 5, $y + 5, $rgb;
}
my $rgb = 0x4070ff;
printf \"${esc}[1922;0;0;400;400;%sm\", $rgb;
EOF

