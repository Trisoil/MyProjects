#!/usr/bin/perl

# Process the raw log of client side DAC log.
# input is the DIR name that contains all the unencrypted logs.

# This script focuses on field T, C6, this reflects the failure rate of Live Data.


$indir = $ARGV[0];

open(FIN, "$indir") || die "inputfile $indir cannot be opened.\n";
close(FIN);

@files = `ls $indir/*.log`;

printf STDERR "$#files+1 files listed.\n";

$linepp = 0;
$allcount1 = 0;
$allcount2 = 0;
for ($fid = 0; $fid <= $#files; $fid ++) {
  
  $zcount1 = 0;
  $zcount2 = 0;
  chomp($files[$fid]);
  open( FIN, $files[$fid]) || die "processing $files[$fid] failed.\n";
  print STDERR "$fid: File $files[$fid] started.\n";

  @lines = <FIN>;
  %seen = ();
  $seencount = 0;
  for ($i = 0; $i <= $#lines; $i ++ ) {
    chomp($lines[$i]);
##    if ( defined( $seen{$lines[$i]} )){ 
##      $seencount ++;
##      next;
##    }else{
##      $seen{$lines[$i]} = 1;
##    }
    ($gar, $subline) = split('URL:Action=', $lines[$i]);
    @words = split(/&/, $subline);
    if ($#words < 2) {
      printf STDERR "Format error at line $i\n";
      next;
    }
    if ( ($words[0] != 12) || ($words[1] ne "A=0") ) {
      next; 
    }else{
       #printf STDERR "passed $subline\n";
      $linepp ++;
    }
    $waittime = -100;
    $waittime2 = -100;
    $wfailed = 0;
    for ($j = 0; $j <= $#words; $j ++){
      @items = split(/=/, $words[$j]);
##print @items;
##print "\n";
      if ($items[0] eq "T") {
        $waittime = $items[1];
      }elsif ($items[0] eq "F"){
        $userid = $items[1];
      }elsif ($items[0] eq "C6"){
        $waittime2 = $items[1];
      }elsif ($items[0] eq "U"){
        if ($items[1]<=0) {
          $wfailed = 1;
        }else{
          #not a failed record, no need to go on.
          last;
        }
      } 
    }
    if ($wfailed == 0) {
      #not a failed record. 
      next;
    }
    if ( ($waittime == -100) || ($waittime2 == -100) ) {
      if ($waittime == -100) { $zcount1 ++; }
      if ($waittime2 == -100) { $zcount2 ++; }
      next; 
    }
    $diff = $waittime - $waittime2;
    printf STDOUT "$userid $waittime $waittime2 $diff\n";

  }
  printf STDERR "line passed $linepp\n";
  close ( FIN );
  print STDERR "$fid: File $files[$fid] done. InvalidT=$zcount1, C6=$zcount2\n";
  $allcount1 += $zcount1;
  $allcount2 += $zcount2;
}

print STDERR "InvalidT=$allcount1, C6=$allcount2, done, bye.\n";


