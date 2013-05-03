#!/usr/bin/perl

#Based on PPVAlogs, calcuate the number of cases where HTTP/P2P/Combined
# speeds are lower than the target data rate. 
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

#output: list of files identified as HTTP slow/fast P2P slow/fast, etc. 
#        stats regarding the number and ratio of slow cases.

#HOWTO produce "filelist.DATA" file 
#  in shell, "ls DATE > filelist.DATA"
#REASON: PPVAlog put all logs from one day in one folder, internal "ls" will
#        take a long time to finish.

$directory =  $ARGV[0];
$dirin = "$directory-out";

if ($directory eq "") {
  print STDERR "usage: analyze.pl <directory_name>\nNote that the file
  list should be listed from a text file named
  filelist.<directory_name>\n\n";
  exit;
}

open(DIN, "filelist.$directory") || die "filelist.$directory cannot be opened\n";
@filelist = <DIN>;
close(DIN);

$file_hslow = "filelist.$directory.hslow";
$file_pslow = "filelist.$directory.pslow";
$file_cslow = "filelist.$directory.cslow";
$file_cfast = "filelist.$directory.cfast";
$file_pfast = "filelist.$directory.pfast";
$file_hfast = "filelist.$directory.hfast";
open(FHS, ">$file_hslow") || die "cannot open $file_hslow to write.\n";
open(FPS, ">$file_pslow") || die "cannot open $file_pslow to write.\n";
open(FCS, ">$file_cslow") || die "cannot open $file_cslow to write.\n";
open(FHF, ">$file_hfast") || die "cannot open $file_hfast to write.\n";
open(FPF, ">$file_pfast") || die "cannot open $file_pfast to write.\n";
open(FCF, ">$file_cfast") || die "cannot open $file_cfast to write.\n";

$TGATE = 0.3;

$cnt_cases = 0;
$cnt_httpslow = 0;
$cnt_p2pslow = 0;
$cnt_combinedslow = 0;


for ($sss = 0; $sss <= $#filelist; $sss ++ ) {
#for ($sss = 0; $sss <= 1000; $sss ++ ) {
  
  chomp( $filelist[$sss] );
  $infile = "$dirin"."/"."$filelist[$sss].csv";
   #print STDOUT "$infile\n";
   
  open(FIN, "$infile") or next; 

  @lines = <FIN>;
  if ($#lines < 5) {
    close( FIN );
    next;
  }
  $cnt_cases ++;

  $slow_http = 0;
  $slow_p2p = 0;
  $slow_combined = 0;
  $total_lines = $#lines;
  for ($i = 1; $i < $#lines; $i++) {
    chomp( $lines[$i] );
    @words = split(/,/, $lines[$i]); 
    $trate = $words[2]/1024;
    
    if ( $words[11] < $trate ) {
      $slow_http ++;
    }
    if ( $words[12] < $trate ) {
      $slow_p2p ++;
    }
    if ( ($words[11] + $words[12]) < $trate ) {
      $slow_combined ++;
    }
     #print STDOUT "$trate $words[11] $words[12]\n";
  }
  if ( ($slow_http / $total_lines) >= $TGATE ) {
    $cnt_httpslow ++; 
    print FHS "$infile\n";
  }else{
    print FHF "$infile\n";
  }
  if ( ($slow_p2p/ $total_lines) >= $TGATE ) {
    $cnt_p2pslow ++; 
    print FPS "$infile\n";
  }else{
    print FPF "$infile\n";
  }
  if ( ($slow_combined / $total_lines) >= $TGATE ) {
    $cnt_combinedslow ++; 
    print FCS "$infile\n";
  }else{
    print FCF "$infile\n";
  }
}

$pct_hslow = $cnt_httpslow/$cnt_cases;
$pct_pslow = $cnt_p2pslow/$cnt_cases;
$pct_cslow = $cnt_combinedslow/$cnt_cases;
printf STDOUT "HTTP_SLOW $cnt_httpslow $pct_hslow\n";
printf STDOUT "P2P_SLOW $cnt_p2pslow $pct_pslow\n";
printf STDOUT "COMBINED_SLOW $cnt_combinedslow $pct_cslow\n";
printf STDOUT "TOTAL $cnt_cases\n";

printf STDERR "done, bye.\n";


