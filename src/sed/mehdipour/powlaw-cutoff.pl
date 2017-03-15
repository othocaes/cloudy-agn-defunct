#!/usr/local/bin/perl

use strict; use warnings; use 5.010;  use utf8;
use IO::Handle;
use File::Temp "tempfile";

open(my $spectrum_file,"spectrum");

# my @x = ();
# my @y = ();
# my($xi,$yi);
# ($xi,$yi)=(0,0);

# while(my $line = <$spectrum_file>) {
# 	$line =~ /([0-9\.]+)\s+([0-9\.]+)/;
# 	($x[$xi],$y[$yi])=($1,$2);
# 	$xi++; $yi++;
# }

# while (my $line = <$spectrum_file>) {
# 	print $line;
# }

my($T,$N) = tempfile("spectrum-XXXXXXXX", "UNLINK", 1);
# for my $t (100..500)
#         { say $T $t*sin($t*0.1), " ", $t*cos($t*0.1); }
while (my $_ = <$spectrum_file>) {
	chomp;
	say $T $_;
}
close $T;
open my $P, "|-", "gnuplot" or die;
printflush $P qq[
        unset key
        set logscale xy
        set xrange [.001,1000]
        plot "$N"
];
<STDIN>;
close $P;


# sub histogram_table(n) {
#     my @output = ()
#     my @x = ()
#     my @y = ()
#     output.append(x)
#     output.append(y)
#     max=0
#     min=1
#     indices = range(n)
#     for i in range(0,n):
#         hνᗉkeVᗆ = hνᗉkeVᗆ_at(i,n);
#         x.append(hνᗉkeVᗆ)
#         value = total(hνᗉkeVᗆ,1,1,1)
#         y.append(value)
#         if (value > max): max = value;
#         if (value < min): min = value;
#     # Add a final point at 100 KeV
#     hνᗉkeVᗆ = 1e2;
#     x.append(hνᗉkeVᗆ)
#     y.append(total(hνᗉkeVᗆ,1,1,1))
#     output.append(x)
#     output.append(y)
#     return output;
# }




close($spectrum_file)
__END__
