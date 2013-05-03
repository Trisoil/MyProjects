#!/usr/bin/perl

#Generate the CDF/PDF of distributions. 
# input is <word1> <word2> ....
# <word2> is the source of the CDF/PDF input key.
# S defines the filtering threshold, number larger than S will be ignored.

$S= 1000000;

$infile = $ARGV[0];

open(FIN, "$infile") || die "inputfile $infile cannot be opened.\n";
open(FOUT, ">$infile.ab") || die "outputfile $infile.ab cannot be opened.\n";

@lines = <FIN>;

%rddist = ();
$count = 0;
$cmin = -1;
$cmax = -1;
$ctotal = 0;
$count_filter = 0;
$ctotal_filter = 0;
$zz1 = 0;
$zz2 = 0;
for ($i = 0; $i <= $#lines; $i ++ ) {
  chomp($lines[$i]);
  @words = split(/ /, $lines[$i]);
  if ($#words < 1) {
    printf STDERR "Format error at line $i\n";
    next;
  }
  if ( $words[2] == -1 ) {
    $zz2 ++;
    next;
  }
  if ( $words[1] == -1 ) {
    $zz1 ++;
    next;
  }
#if ($words[2] <=1) {next;}
  if ( !defined( $rddist{$words[2]} ))  {
    $rddist{$words[2]} = 1;
  }else{
    $tmpccc = $rddist{$words[2]};
    $rddist{$words[2]} = $tmpccc + 1;
  }
  $count ++;
  if ( ($cmin == -1) || ($cmin > $words[2]) ) { $cmin = $words[2]; }
  if ( ($cmax == -1) || ($cmax < $words[2]) ) { $cmax = $words[2]; }
  $ctotal += $words[2]; 
  if ( $words[2] < $S) {
    $count_filter ++;
    $ctotal_filter += $words[2];
  }
  if ($words[1] != $words[2]) {
    printf FOUT "$lines[$i]\n";
  }
}
close ( FIN );
close ( FOUT );

printf STDERR  "sum: $count, $cmin, $cmax\n";
$avg1 = $ctotal / $count;
$avg2 = $ctotal_filter / $count_filter;
printf STDERR  "avg: unfiltered: $avg1, filtered: $avg2\n";

sub numerically {$a<=>$b};

@keys = ();
@keyback = ();
@keysbak = keys %rddist;
@keys = sort numerically @keysbak;
$ccent = 0;
for ($key = 0; $key <= $#keys; $key ++ ) {
  $avgnodes= $rddist{$keys[$key]};
  $pcent = $avgnodes / $count;
  $ccent += $pcent;
  print "$keys[$key] $avgnodes $ccent $pcent\n";
}


print STDERR "done, bye.\n";


