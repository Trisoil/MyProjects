#!/usr/bin/perl

$mmonth = 7;
$mdate = int($ARGV[0]);
$mhour = $ARGV[1];

print "Search 7-$mdate, hour $mhour\n\n";

if ( ($mhour eq "") || ($ARGV[0] eq "") ) {
  print "arg0 $ARGV[0] arg1 $ARGV[1]\n";
  die "Usage: .pl <date> <hour/ALL>\n\n";
}

@filelist = `ls file*/push_env_?/statspush.log`;

$totalpas = 0;
$totalact = 0;
for ($i=0; $i <= $#filelist; $i ++) {
    $tp = 0;
    $ta = 0;
    print "Process $filelist[$i]\n";
    open(FIN, $filelist[$i]) || die "file $filelist[$i] cannot be opened.";
    while ( <FIN> ) {
        chomp();
        @words = split(' ');
        @dates = split('/', $words[0]);
        @times = split(':', $words[1]);
        if ( (int($times[0]) + 8) >= 24) {
          $newn = int($dates[1])+1;
          $dates[1] = "$newn";
          $rhour = int($times[0]) + 8 - 24;
        }else{
          $rhour = int($times[0]) + 8;
        }
        if ( int($dates[0]) != $mmonth ) {
          next;
        }
        if ( int($dates[1]) != $mdate ) {
          next;
        }
        if ( ($mhour ne "ALL") && ($rhour != int($mhour)) ) {
          next;
        }
        if ($words[5] eq "mp") {
        #print "$words[5] $words[0] $words[1] $dates[1] $rhour == $words[7]/$words[10]\n";
            $totalpas += int($words[7]);
            $totalact += int($words[10]);
            $tp += int($words[7]);
            $ta += int($words[10]);
        }
    }
    print " current $tp $ta, total $totalpas $totalact\n";
}
print "totalpas = $totalpas; totalact = $totalact\n";
$ov = $totalpas + $totalact;
print "overall = $ov\n";
