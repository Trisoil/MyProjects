#!/usr/bin/perl

# Based on the DAC Channel-based Statistics to do the following things
# 1) find the new content from previous days, read their VV and PerVV BufEv
# 2) based on the PPVAlog analysis, find out the BufEv count/ratio for each file. 
# 3) calculate the VV ratio of these new files, and BufEv ratio of these file.
# 4) estimate if we lower the BufEv ratio to normal level, how much could we
# reduce the number of BufEv. 

#input: content copied from DAC stats page (top 100 perday): rank-2011-10-09/08.txt
#       PPVAlog's BufEv count per file: filelist.2011_10_09.fname_dist
#output: Hot and new content's BufEv count/ratio and VV count/ratio.

use Encode;
use Encode::CN;

$topcnt = 100;
$totalVV = 36383692;
$totalBUF = 104794;

$infile1 = "rank-2011-10-09.txt";
$infile2 = "rank-2011-10-08.txt";
$pfile = "filelist.2011_10_09.fname_dist";
$outfile = "rank-2011-10-09-new.txt";

open( FIN1, "$infile1") || die "cannot open $infile1\n";
open( FIN2, "$infile2") || die "cannot open $infile2\n";
open( FOUT, ">$outfile") || die "cannot open $outfile\n";
@lines1 = <FIN1>;
@lines2 = <FIN2>;
close( FIN1 );
close( FIN2 );

open( FIN, "$pfile") || die "cannot open $pfile\n";
@lines = <FIN>;
close( FIN );

%bufhash = ();
for ($i = 0; $i < $#lines; $i++) {
  chomp( $lines[$i] );
  @words = split(/\t/, $lines[$i]); 
    
  $fname1 = decode("gb2312", $words[3]);
  $fname = "$fname1 ";
  $bufevent = $words[1];
  if (!defined( $bufhash{$fname} ) ) {
    $bufhash{$fname} = $bufevent;
  }else{
    print STDERR "duplicate entry? $pfile:$i\n";
  }
}

%filehash = ();
%rankhash = ();

for ($i = 0; $i < $#lines1; $i++) {
  chomp( $lines1[$i] );
  @words = split(/\t/, $lines1[$i]); 
    
  $fname = decode("gb2312", $words[2]);
  @forvv = split(/"/, $words[3]);
  $vv = $forvv[1];
  $vv =~ s/,//g;
  if (!defined( $filehash{$fname} ) ) {
    $filehash{$fname} = $vv;
    $rankhash{$fname} = $i+1;
  }else{
    print STDERR "duplicate entry? file1:$i\n";
  }
}

for ($i = 0; $i < $#lines2; $i++) {
  chomp( $lines2[$i] );
  @words = split(/\t/, $lines2[$i]); 
    
  $fname = decode("gb2312", $words[2]);
  if (defined( $filehash{$fname} ) ) {
    $filehash{$fname} = 0;
  }
}

sub numerically {$filehash{$b}<=>$filehash{$a}};
@keys = keys %filehash;
@rkeys = sort numerically @keys;
$sum = 0;
$bufsum = 0;
for ($k = 0; $k <= $#rkeys; $k ++) {
  if ( $filehash{$rkeys[$k]} > 0 ) { 
    $sum += $filehash{$rkeys[$k]};
    if (defined($bufhash{$rkeys[$k]})) {
      $bufe = $bufhash{$rkeys[$k]};
    }else{
      $bufe = 0;
    }
    $bufrate = $bufe / $totalBUF;
    $vvrate = $filehash{$rkeys[$k]} / $totalVV;
    $pervv = $bufrate / $vvrate;
    $hhname = encode("gb2312", $rkeys[$k]);
    print FOUT "$k $rankhash{$rkeys[$k]} $pervv $filehash{$rkeys[$k]} $vvrate $bufe $bufrate $sum $hhname\n";
    if ( $pervv > 1 ) {
#$bufsum += $bufe * (($pervv - 1)/$pervv);
      $bufsum += $bufe * 0.5;
      
    }
  }
}
$rrate = $bufsum / $totalBUF;
print STDOUT "Reduction $bufsum, $rrate\n";

close( FOUT );
printf STDERR "done, bye.\n";

#104794
