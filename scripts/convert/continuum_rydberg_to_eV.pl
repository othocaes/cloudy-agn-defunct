#!/usr/bin/env perl

use utf8;
use Encode qw(encode decode);

use feature 'say';
use locale;

use constant PI    => 4 * atan2(1, 1);

if (!${^UTF8LOCALE}) {
    say encode($charset,"You are not using UTF-8 encoding. :(");
}
my $charset=$ENV{LANG};

my $table_input_filename=$ARGV[0];
#my $table_output_filename=$argv[1] or die $!;

open $ryd_file,'<',$table_input_filename or die $!;

while( my $line = <$ryd_file>)  {
    my ($rydbergs, $sed1, $sed2) = split ' ', $line;
    say encode($charset,
        sprintf("%e   %e   %e",
                $rydbergs*13.60569253,
                $sed1,
                $sed2));   
}

close $ryd_file;
