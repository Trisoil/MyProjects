#!/usr/bin/perl

#Generate the CDF/PDF of distributions. 
# input is <word1> <word2> ....
# <word2> is the source of the CDF/PDF input key.
# S defines the filtering threshold, number larger than S will be ignored.

$S= 100000;

$directory = $ARGV[0];

@filelist = `ls $directory/*.upload`;

for ($sss = 0; $sss <= $#filelist; $sss ++ ) {

  chomp( $filelist[$sss] );
  $infile = $filelist[$sss];

  print "$infile\n";

  open(FIN, "$infile") || die "inputfile $infile cannot be opened.\n";
  open(FPLOT, ">$infile.upplot") || die "outputfile $infile.distplot cannot be opened.\n";

  printf FPLOT <<PPLEND;

set term postscript
set xlabel "Usage Ratio"
set size 0.6, 0.6
set ylabel "CDF"
#set yrange [0 :1]
#set xrange [1:1000000]
set key bottom right
#set logscale x
set output "$infile.upplot.ps"
set terminal postscript monochrome 12
set pointsize 1
set data style linespoints
plot '-' using (\$1):(\$3) t 'Report' with l 1

PPLEND

  @lines = <FIN>;

  %distReport = ();
  %peerBW = ();
  $cntreport = 0;
  $count = 0;
  $cmin = -1;
  $cmax = -1;
  $ctotal = 0;
  for ($i = 0; $i <= $#lines; $i ++ ) {
    chomp($lines[$i]);
    chop($lines[$i]);
    @words = split(/ /, $lines[$i]);
    if ($#words < 1) {
      print STDERR "Format error at line $i\n";
      next;
    }
    if ($words[0] eq "DIS") {
      next;
    }
    if ( ($words[6] ne "0106") && ($words[6] ne "0107") ) {
      next;
    }
     #calculate usage ratio.
    $mrate = $words[9];
    if ($words[10] < $mrate) {
      $mrate = $words[10];
    }
    if ($mrate == 0) {
      $cntreport ++;
      next;
    }
    $ratio = $words[11]/$mrate;
    if ( !defined( $distReport{$mytime} ))  {
      $distReport{$words[12]} = 1;
      $peerBW{$words[12]} = $ratio;
    }else{
      $tmpccc = $distReport{$words[12]};
      $distReport{$words[12]} = $tmpccc + 1;
      $tmpccc = $peerBW{$words[12]};
      $peerBW{$words[12]} = $tmpccc + $ratio;
    }
    $count ++;
  }
  close ( FIN );

  printf STDERR  "zero limit: $cntreport, total: $count\n";

#@keys = sort numerically @keysbak;

  @keys = ();
  @keys = keys %distReport;
  $numkeys = $#keys + 1;
  $ccent = 0;
  $mindex = -1;
  for ($key = 0; $key <= $#keys; $key ++ ) {
    $reportcount = $distReport{$keys[$key]};
    $avgcount = $peerBW{$keys[$key]} / $reportcount;
    $peerBW{$keys[$key]} = $avgcount;
    if ($avgcount > $cmax) {
      $cmax = $avgcount;
      $mindex = $keys[$key];
    }
    $ccent += $avgcount;
  }
  $avg = $ccent / $numkeys;
  print STDOUT "max is $cmax, pguid $mindex; avg is $avg\n";

  $cmin = 0;
  $step = (1.5 - $cmin)/$S;
  sub numerically {$peerBW{$a}<=>$peerBW{$b}};

  @newkeys = sort numerically @keys;

  $key = 0;
  for ($eachstep = 1; $eachstep <= $S; $eachstep ++) {
    $ceiling = $eachstep * $step;
    while ($key <= $#newkeys) {
       #print STDERR "$key $newkeys[$key] $peerBW{$newkeys[$key]}\n";
      if ($peerBW{$newkeys[$key]} < $ceiling) {
        $key ++;
      }else{
        last;
      }
    }
    $cdf = $key / $numkeys;
    print FPLOT "$ceiling $key $cdf\n";
  }
 
  print FPLOT "\ne\n\n";

}

printf STDERR "done, bye.\n";


