#!/usr/bin/perl

use Encode;
use Encode::CN;

$directory = "rank";
$startmonth = 9;

@range = (15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);

$outfile = "$directory-$startmonth-days$#range";
open( FOUT1, ">$outfile-1.plot" ) || die "$outfile cannot be opened\n";
open( FOUT2, ">$outfile-2.plot" ) || die "$outfile cannot be opened\n";

printf FOUT1 <<PPLEND;

set term postscript
set xlabel "Day"
set size 0.6, 0.6
set ylabel "VV"
#set yrange [0 :1]
#set xrange [1:1000000]
set key top right
#set logscale x
set output "$outfile-1.ps"
set terminal postscript monochrome 12
set pointsize 1
set data style linespoints

PPLEND

printf FOUT2 <<PPLEND2;

set term postscript
set xlabel "Day"
set size 0.6, 0.6
set ylabel "Buf Event Per VV"
#set yrange [0 :1]
#set xrange [1:1000000]
set key top right
#set logscale x
set output "$outfile-2.ps"
set terminal postscript monochrome 12
set pointsize 1
set data style linespoints

PPLEND2


%filevv = ();
%filebuf = ();
%filelast = ();
$month = $startmonth;

%initlist = ();
for ($findex = 0; $findex < $#range; $findex ++) {

  if ( $range[$findex] == 1) {
    $month ++;
    if ($month > 12) { $month = 1; }
  }
  $infile = sprintf("%s/rank-2011-%02d-%02d.txt", $directory, $month, $range[$findex]); 

  open( FIN, "$infile") || die "cannot open $infile\n";
  @lines = <FIN>;
  close( FIN );

  %dup = ();
  for ($i = 0; $i < $#lines; $i++) {
    chomp( $lines[$i] );
    @words = split(/\t/, $lines[$i]); 

    if ($words[0] eq "  ") {
      next;
    }
      
    $fname = decode("gb2312", $words[1]);
    $vv = $words[3];
    $vv =~ s/,//g;
    $bufev = $words[9];
    chop($bufev);
    if ($findex == 0) {
      $initlist{$fname} = 1;
    }else{
      if (!defined( $initlist{$fname} ) ) {
        if (!defined( $dup{$fname} ) ) {
          $dup{$fname} = $vv;
          if (!defined( $filevv{$fname} ) ) {
            $filevv{$fname} = "$findex $vv $bufev $i\n";
            $filebufev{$fname} = 1;
            $filelast{$fname} = $i;
          }else{
            $tt = $filevv{$fname}."$findex $vv $bufev $i\n";
            $filevv{$fname} = $tt;
            $tt = $filebufev{$fname} + 1;
            $filebufev{$fname} = $tt;
            $filelast{$fname} = $i;
          }
        }else{
          print STDERR "duplicate entry? $infile:$i $fname\n";
        }
      }
    }
  }
}

@rkeys = keys %filevv;
$sum = 0;
$bufsum = 0;

$kk = 0;
for ($k = 0; $k <= $#rkeys; $k ++) {
  if ( $filebufev{$rkeys[$k]} > 6) {
    if ($kk == 0) {
      print FOUT1 "plot '-' using (\$1):(\$2) notitle with l $kk";
      print FOUT2 "plot '-' using (\$1):(\$3) notitle with l $kk";
    }else{
      print FOUT1 ", '-' using (\$1):(\$2) notitle with l $kk";
      print FOUT2 ", '-' using (\$1):(\$3) notitle with l $kk";
    }
    $kk ++;
  }
}
print FOUT1 "\n\n";
print FOUT2 "\n\n";

%cnthash = ();
$cnttotal = 0;
$kk = 0;
for ($k = 0; $k <= $#rkeys; $k ++) {
  if ( $filebufev{$rkeys[$k]} > 6) {
    if ( $filevv{$rkeys[$k]} > 0 ) { 
      $hhname = encode("gb2312", $rkeys[$k]);
      print FOUT1 "# $k $hhname\n";
      print FOUT2 "# $k $hhname\n";
      print FOUT1 "$filevv{$rkeys[$k]}\n\ne\n\n";
      print FOUT2 "$filevv{$rkeys[$k]}\n\ne\n\n";
      if ( $filebufev{$rkeys[$k]} > 28) {
        print STDERR "$filebufev{$rkeys[$k]} $hhname\n";
      }
    }
    $kk ++;
  }
  $fcnt = $filebufev{$rkeys[$k]};
  if ($filelast{$rkeys[$k]} < 100 ) {
    if (!defined( $cnthash{$fcnt} ) ) {
      $cnthash{$fcnt} = 1;
    }else{
      $cnthash{$fcnt} = $cnthash{$fcnt} + 1;
    }
    $cnttotal ++;
  }
}

close( FOUT1 );
close( FOUT2 );

printf STDERR "Plotting done. Now to study distribution...\n";

#sub numerically {$cnthash{$a}<=>$cnthash{$b}};
sub numerically {$a<=>$b};

@keys = ();
@keyback = ();
@keysbak = keys %cnthash;
@keys = sort numerically @keysbak;
$ccent = 0;
for ($key = 0; $key <= $#keys; $key ++ ) {
  $events= $cnthash{$keys[$key]};
  $pcent = $events / $cnttotal;
  $ccent += $pcent;
  print STDOUT "$keys[$key] $events $ccent $pcent $cnttotal\n";
}
print STDOUT "\ne\n\n";


printf STDERR "done, bye.\n";

#104794
