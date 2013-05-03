#!/usr/bin/perl

#Generate the CDF/PDF of distributions. 

$S= 1600;
$s = 10;

$directory = $ARGV[0];

@filelist = `ls $directory/*.out`;

for ($sss = 0; $sss <= $#filelist; $sss ++ ) {

  chomp( $filelist[$sss] );
  $infile = $filelist[$sss];

  print "$infile\n";

  open(FIN, "$infile") || die "inputfile $infile cannot be opened.\n";
  open(FPLOT, ">$infile.sizeplot") || die "outputfile $infile.distplot cannot be opened.\n";
  open(FPLOT2, ">$infile.sizecnt") || die "outputfile $infile.distplot cannot be opened.\n";

  printf FPLOT <<PPLEND;

set term postscript
set xlabel "Packet Size"
set size 0.6, 0.6
set ylabel "Avg Process Time"
#set yrange [0 :1]
#set xrange [1:1000000]
set key top right
set output "$infile.sizeplot.ps"
set terminal postscript monochrome 12
set pointsize 1
set data style linespoints
plot '-' using (\$2):(\$3) t 'Report' with p 1, '-' using (\$2):(\$3) t 'List' with p 2, '-' using (\$2):(\$3) t 'QueryCnt' with p 3

PPLEND

  printf FPLOT2 <<PPLEND2;

set term postscript
set xlabel "Packet Size"
set size 0.6, 0.6
set ylabel "Packet Count"
#set yrange [0 :1]
#set xrange [1:1000000]
set key top right
set logscale y
set output "$infile.sizecnt.ps"
set terminal postscript monochrome 12
set pointsize 1
set data style linespoints
plot '-' using (\$2):(\$3) t 'Report' with p 1, '-' using (\$2):(\$3) t 'List' with p 2, '-' using (\$2):(\$3) t 'QueryCnt' with p 3

PPLEND2



  @lines = <FIN>;

  @distReport = ();
  @distList = ();
  @distQuery = ();
  @distReportCnt = ();
  @distListCnt = ();
  @distQueryCnt = ();
  for ($i = 0; $i <= $S/$s; $i ++) {
    $distReport[$i] = 0;
    $distReportCnt[$i] = 0;
    $distList[$i] = 0;
    $distListCnt[$i] = 0;
    $distQuery[$i] = 0;
    $distQueryCnt[$i] = 0;
  }
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
    $sizeindex = int $words[5]/$s;
    if ($words[7] eq "Report") {
      $distReport[$sizeindex] += $mytime;
      $distReportCnt[$sizeindex] ++;
      $cntreport ++;
    }elsif ($words[7] eq "List") {
      $distList[$sizeindex] += $mytime;
      $distListCnt[$sizeindex] ++;
      $cntlist ++;
    }elsif ($words[7] eq "QueryPeerCount") {
      $distQuery[$sizeindex] += $mytime;
      $distQueryCnt[$sizeindex] ++;
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

  for ($key = 0; $key <= $S/$s; $key ++ ) {
    if ($distReportCnt[$key] == 0 ) {
      next;
    }else{
      $avgtime = $distReport[$key]/$distReportCnt[$key];
    }
    $keyvalue = $key * $s + $s;
    print FPLOT "$key $keyvalue $avgtime\n";
    print FPLOT2 "$key $keyvalue $distReportCnt[$key]\n";
  }
  print FPLOT "\ne\n\n";
  print FPLOT2 "\ne\n\n";

  for ($key = 0; $key <= $S/$s; $key ++ ) {
    if ($distListCnt[$key] == 0 ) {
      next;
    }else{
      $avgtime = $distList[$key]/$distListCnt[$key];
    }
    $keyvalue = $key * $s + $s;
    print FPLOT "$key $keyvalue $avgtime\n";
    print FPLOT2 "$key $keyvalue $distListCnt[$key]\n";
  }
  print FPLOT "\ne\n\n";
  print FPLOT2 "\ne\n\n";

  for ($key = 0; $key <= $S/$s; $key ++ ) {
    if ($distQueryCnt[$key] == 0 ) {
      next;
    }else{
      $avgtime = $distQuery[$key]/$distQueryCnt[$key];
    }
    $keyvalue = $key * $s + $s;
    print FPLOT "$key $keyvalue $avgtime\n";
    print FPLOT2 "$key $keyvalue $distQueryCnt[$key]\n";
  }
  print FPLOT "\ne\n\n";
  print FPLOT2 "\ne\n\n";
  close( FPLOT );
  close( FPLOT2 );

  system("gnuplot $infile.sizeplot");
  system("gnuplot $infile.sizecnt");

}

printf STDERR "done, bye.\n";


