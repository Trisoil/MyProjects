#!/usr/bin/perl

# Process the raw dump log of Trackers.
# input is the DIR name that contains all the unencrypted logs.

use Tie::File;

$indir = $ARGV[0];

open(FIN, "$indir") || die "inputfile $indir cannot be opened.\n";
close(FIN);

@files = `ls $indir/*.log`;

printf STDERR "$#files+1 files listed.\n";

$resource_map_by_user = {};
$resource_count_by_user = {};
$resource_map_by_rid = {};
$linepp = 0;
$allcount1 = 0;
$allcount2 = 0;
for ($fid = 0; $fid <= $#files; $fid ++) {
  
  $zcount1 = 0;
  $zcount2 = 0;
  chomp($files[$fid]);
  tie @lines, 'Tie::File', $files[$fid] or die "processing $files[$fid] failed.\n";
  print STDERR "$fid: File $files[$fid] started.\n";
  open(Fout, ">$files[$fid].rcount") or die "output file cannot be opened\n";

  %seen = ();
  $seencount = 0;
  $started = 0;
  $i = -1;
  for ($i = 0; $i <= $#lines; $i++ ){
    $line = $lines[$i];
    chomp($line);
    if (($started == 0)&&!($line =~ m/Address:/) ) {
      #printf STDERR "$line\n";
      next; 
    }else{
      $started = 1;
    }
    @words = split('\|', $line);
    if ($#words < 2) {
      printf STDERR "Format error at line $i\n";
      next;
    }
#    if ( defined( $seen{$words[0]} )){ 
#      $seencount ++;
#      printf STDERR "$words[0] seen before\n";
#      next;
#    }else{
#      $seen{$words[0]} = 1;
#    }
     #@items = split(/,/, $words[1]);
#do we need to extra IPs?
# we will skip it for now.
    $ridcount = $words[2]; 
    if ( ($ridcount+3) != $#words ) {
      printf STDERR "format warning: RID count does not match, $ridcount, $#words\n";
    }else{
      #printf STDERR "$words[0] $words[2]\n";
    }

    @ridarray = ();
    for ($j = 3; $j < $#words; $j++) {
      #the last item is empty, so we iterate until $#words, instead +1.
       #print "$words[$j]\n";
      #we put the rid in the RID array and later store it in the usermap.
      push(@ridarray, $words[$j]);
      #we put the userid into the RID->user map.
      if (defined $resource_map_by_rid{$words[$j]}) {
#if ($fid > 0) {
#          printf STDERR "duplicate found in diff files\n"; 
#        }else{
#          push(@{$resource_map_by_rid{$words[$j]}}, $words[0]);
#        }
      }else{
#        $resource_map_by_rid{$words[$j]} = [ $words[0] ];
      }
    }
    if (defined $resource_count_by_user{$words[0]} ) {
      printf STDERR "duplicate user found $words[0]\n";
#      @newr = ( @{$resource_map_by_rid{$words[$j]}}, @ridarray );
#      $resource_map_by_user{$words[0]} = [ @newr ];
#      $tmpstring = $resource_count_by_user{$words[0]};
#      $resource_count_by_user{$words[0]} = "$tmpstring.$fid-$ridcount";
    }else{
#      $resource_map_by_user{$words[0]} = [ @ridarray ];
      $resource_count_by_user{$words[0]} = "$ridcount";
      printf Fout "$words[0] $ridcount\n";
    }
    if ($i % 1000 == 0) { printf STDERR "$i ";}
  }
  untie @lines;
  %resource_count_by_user = {};
  print STDERR "$fid: File $files[$fid] done.\n";
}

#@userkeys = keys %resource_count_by_user;
#printf STDERR "The number of users is $#userkeys + 1\n";
#foreach ( @userkeys ) {
# printf STDERR "@{$resource_map_by_user{$userkeys[0]}}\n";
#  printf STDERR "$resource_count_by_user{$_}\n";
#}

#@ridkeys = keys %resource_map_by_rid;
#foreach (@ridkeys) {
#  $asize = scalar (@{$resource_map_by_rid{$_}});
#  if ($asize > 1) {
#    printf STDERR "@{$resource_map_by_rid{$_}}\n";
#  }
#}

print STDERR "Done, bye.\n";


