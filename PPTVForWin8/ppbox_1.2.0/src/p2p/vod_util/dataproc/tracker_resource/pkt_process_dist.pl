#!/usr/bin/perl

#Generate the CDF/PDF of distributions. 
# input is <word1> <word2> ....
# <word2> is the source of the CDF/PDF input key.
# S defines the filtering threshold, number larger than S will be ignored.

$S= 1000000;

$directory = $ARGV[0];

@filelist = `ls $directory/*.out`;

for ($sss = 0; $sss <= $#filelist; $sss ++ ) {

  chomp( $filelist[$sss] );
  $infile = $filelist[$sss];

  print "$infile\n";

  open(FIN, "$infile") || die "inputfile $infile cannot be opened.\n";
  open(FPLOT, ">$infile.distplot") || die "outputfile $infile.distplot cannot be opened.\n";

  printf FPLOT <<PPLEND;

set term postscript
set xlabel "Process Time"
set size 0.6, 0.6
set ylabel "CDF"
#set yrange [0 :1]
set xrange [1:1000000]
set key bottom right
set logscale x
set output "$infile.distplot.ps"
set terminal postscript monochrome 12
set pointsize 1
set data style linespoints
plot '-' using (\$1):(\$3) t 'Report' with l 1, '-' using (\$1):(\$3) t 'List' with l 2, '-' using (\$1):(\$3) t 'QueryCnt' with l 3

PPLEND

  @lines = <FIN>;

  %distReport = ();
  %distList = ();
  %distQuery = ();
  $cntreport = 0;
  $cntlist  = 0;
  $cntquery = 0;
  $cntother = 0;
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
    chop($lines[$i]);
    @words = split(/ /, $lines[$i]);
    if ($#words < 1) {
      printf STDERR "Format error at line $i\n";
      next;
    }
    if ($words[0] ne "N") {
      next;
    }
    $decimal_notation = sprintf("%.10g", $words[4]);
    $mytime = $decimal_notation * 1000000;
    if ($mytime < 0) {
      print STDERR "$i $words[4] $words[7]\n";
      print STDERR "@words\n";
      next;
    }
    if ($words[7] eq "Report") {
      if ( !defined( $distReport{$mytime} ))  {
        $distReport{$mytime} = 1;
      }else{
        $tmpccc = $distReport{$mytime};
        $distReport{$mytime} = $tmpccc + 1;
      }
      $cntreport ++;
    }elsif ($words[7] eq "List") {
      if ( !defined( $distList{$mytime} ))  {
        $distList{$mytime} = 1;
      }else{
        $tmpccc = $distList{$mytime};
        $distList{$mytime} = $tmpccc + 1;
      }
      $cntlist ++;
    }elsif ($words[7] eq "QueryPeerCount") {
      if ( !defined( $distQuery{$mytime} ))  {
        $distQuery{$mytime} = 1;
      }else{
        $tmpccc = $distQuery{$mytime};
        $distQuery{$mytime} = $tmpccc + 1;
      }
      $cntquery ++;
    }else{
      $cntother ++;
    }
    $count ++;
    if ( ($cmin == -1) || ($cmin > $mytime) ) { $cmin = $mytime; }
    if ( ($cmax == -1) || ($cmax < $mytime) ) { $cmax = $mytime; }
  }
  close ( FIN );

  printf STDERR  "sum: $count, $cmin, $cmax\n";
  printf STDERR  "Report: $cntreport, List: $cntlist, Query: $cntquery, Other: $cntother\n";

  sub numerically {$a<=>$b};

  @keys = ();
  @keyback = ();
  @keysbak = keys %distReport;
  @keys = sort numerically @keysbak;
  $ccent = 0;
  for ($key = 0; $key <= $#keys; $key ++ ) {
    $avgnodes= $distReport{$keys[$key]};
    $pcent = $avgnodes / $cntreport;
    $ccent += $pcent;
    print FPLOT "$keys[$key] $avgnodes $ccent $pcent\n";
  }
  print FPLOT "\ne\n\n";

  @keys = ();
  @keyback = ();
  @keysbak = keys %distList;
  @keys = sort numerically @keysbak;
  $ccent = 0;
  for ($key = 0; $key <= $#keys; $key ++ ) {
    $avgnodes= $distList{$keys[$key]};
    $pcent = $avgnodes / $cntlist;
    $ccent += $pcent;
    print FPLOT "$keys[$key] $avgnodes $ccent $pcent\n";
  }
  print FPLOT "\ne\n\n";

  @keys = ();
  @keyback = ();
  @keysbak = keys %distQuery;
  @keys = sort numerically @keysbak;
  $ccent = 0;
  for ($key = 0; $key <= $#keys; $key ++ ) {
    $avgnodes= $distQuery{$keys[$key]};
    $pcent = $avgnodes / $cntquery;
    $ccent += $pcent;
    print FPLOT "$keys[$key] $avgnodes $ccent $pcent\n";
  }
  print FPLOT "\ne\n\n";
  close( FPLOT );

}

printf STDERR "done, bye.\n";


