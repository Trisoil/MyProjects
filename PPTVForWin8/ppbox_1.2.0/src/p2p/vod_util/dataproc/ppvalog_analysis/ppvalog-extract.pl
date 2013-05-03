#!/usr/bin/perl

#call the "statistics_reader.exe" to extract out necessary data PPVAlog. 
#input: DATE, list of files from PPVAlog directory.
#output: extract data in DATE-out directory, one file resulted from each src
#       file. 

#HOWTO produce "filelist.DATA" file 
#  in shell, "ls DATE > filelist.DATA"
#REASON: PPVAlog put all logs from one day in one folder, internal "ls" will
#        take a long time to finish.

$directory =  $ARGV[0];
$dirout = "$directory-out";

if ($directory eq "") {
  print STDERR "usage: analyze.pl <directory_name>\nNote that the file
  list should be listed from a text file named
  filelist.<directory_name>\n\n";
  exit;
}

open(FIN, "filelist.$directory") || die "file cannot be opened\n";
@filelist = <FIN>;

for ($sss = 0; $sss <= $#filelist; $sss ++ ) {

  chomp( $filelist[$sss] );
  $infile = "$directory"."/"."$filelist[$sss]";
  $outfile = "$dirout"."/"."$filelist[$sss].csv";

  print STDOUT "statistics_reader.exe $infile $outfile\n";
  system("statistics_reader.exe $infile $outfile\n");

}
printf STDERR "done, bye.\n";


