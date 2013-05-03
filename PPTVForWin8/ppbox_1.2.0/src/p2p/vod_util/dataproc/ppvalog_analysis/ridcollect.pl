#!/usr/bin/perl

#Based on PPVAlogs, calcuate the number of BufEv for RIDs and Content files. 
#input: DATE to be processed.
#    list of files from the original PPVAlog files in format of "filelist.DATE".
#    directory name of the processed files from PPVAlog (extracted data in
#    the following format).
#0: filename (RID)
#1: filelength
#2: datarate
#3: CDNIP
#4: totalsent
#5: totalrecv
#6: lossrate
#7: redundency
#8: httpdownload
#9: p2pdownload
#10: remaintime
#11: httpspeed
#12: p2pspeed
#13: totalspeed
#14: statemachine
#15: numpeer-fullres
#16: numpeer-connect
#17: IPPool
#18: filename

#output: two files, 1) mapping the RID with the number reported BufEv. 
#        2)  mapping the content files with the number reported BufEv.

#HOWTO produce "filelist.DATA" file 
#  in shell, "ls DATE > filelist.DATA"
#REASON: PPVAlog put all logs from one day in one folder, internal "ls" will
#        take a long time to finish.

use Encode;
use Encode::CN;

$directory = $ARGV[0]; 
$dirin = "$ARGV[0]-out";

if ($directory eq "") {
  print STDERR "usage: ridcollect.pl <directory_name>\nNote that the file
  list should be listed from a text file named
  filelist.<directory_name>\n\n";
  exit;
}

open(DIN, "filelist.$directory") || die "file cannot be opened\n";
@filelist = <DIN>;
close(DIN);

$ridfile = "filelist.$directory.rid_dist";
$namefile = "filelist.$directory.fname_dist";

open( FRID, ">$ridfile") || die "cannot open $ridfile\n";
open( FNAME, ">$namefile") || die "cannot open $namefile\n";

$cnt_cases = 0;

%ridhash = ();
%filehash = ();

for ($sss = 0; $sss <= $#filelist; $sss ++ ) {
#for ($sss = 0; $sss <= 1000; $sss ++ ) {
  
  chomp( $filelist[$sss] );
  $infile = "$dirin"."/"."$filelist[$sss].csv";
  #  print STDOUT "$infile\n";
   
  open(FIN, "$infile") or next; 

  @lines = <FIN>;
  if ($#lines < 2) {
    next;
  }
  close( FIN );

  $cnt_cases ++;

  $total_lines = $#lines;
  for ($i = 1; $i < $#lines; $i++) {
    chomp( $lines[$i] );
    @words = split(/,/, $lines[$i]); 
    
    $rname = decode("gb2312", $words[18]);
    for ($sa = 19; $sa <= $#words; $sa ++) {
      $tname = decode("gb2312", $words[$sa]);
      $rname = $rname.",$tname";
      print STDOUT "format join at line $infile:$i\n";
    }
    $filename = $rname;
    @fname = $filename =~ /(^.*)\[(\d+)\].mp4/;
    if ($#fname != 1) {
      print STDOUT "format trouble at line $infile:$i\n";
    }

#print STDOUT "$fname[0]\n";

    if ( length($words[0]) == 32) {
#      if ( $words[0] eq "60DD7B251375400A0CC61481BFECAFCA" ) {
#        print STDOUT "$infile\n";
#        exit;
#      }
      if (!defined( $ridhash{$words[0]} ) ) {
        $ridhash{$words[0]} = 1;
      }else{
        $tmp = $ridhash{$words[0]} ;
        $ridhash{$words[0]} = ($tmp + 1);
      }
      if (!defined( $filehash{$fname[0]} ) ) {
        $filehash{$fname[0]} = 1;
      }else{
        $tmp = $filehash{$fname[0]} ;
        $filehash{$fname[0]} = ($tmp + 1);
      }
      last;
    }
  }
}

sub numerically {$ridhash{$b}<=>$ridhash{$a}};

@keys = keys %ridhash;
@rkeys = sort numerically @keys;
$sum = 0;
for ($k = 0; $k <= $#rkeys; $k ++) {
  $sum += $ridhash{$rkeys[$k]};
  print FRID "$k $rkeys[$k] $ridhash{$rkeys[$k]} $sum\n";
}

printf FRID "TOTAL $cnt_cases\n";

sub numerically2 {$filehash{$b}<=>$filehash{$a}};
@keys = keys %filehash;
@rkeys = sort numerically2 @keys;
$sum = 0;
for ($k = 0; $k <= $#rkeys; $k ++) {
  $sum += $filehash{$rkeys[$k]};
  $hhname = encode("gb2312", $rkeys[$k]);
  print FNAME "$k\t$filehash{$rkeys[$k]}\t$sum\t$hhname\n";
}

printf FNAME "TOTAL $cnt_cases\n";

close( FRID );
close( FNAME );

printf STDERR "done, bye.\n";

#104794
