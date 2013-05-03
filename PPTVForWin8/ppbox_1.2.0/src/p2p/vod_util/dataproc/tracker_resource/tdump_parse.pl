#!/usr/bin/perl

use Net::TcpDumpLog;
use NetPacket::Ethernet;
use NetPacket::IP;
use NetPacket::UDP;

@filelist = `ls tcpdump/splitted/*`;

for ($i = 0; $i <= $#filelist; $i ++) {
  chomp( $filelist[$i] );
  print STDOUT "$filelist[$i]\n";
  $outfile = "$filelist[$i].upload";
  $outfile =~ s/splitted/result/g;
  print STDOUT "$outfile\n";

  open( FOUT, ">$outfile" ) || die "cannot open output file $outfile\n";

  $log = Net::TcpDumpLog->new(); 
  $log->read("$filelist[$i]");

  print STDERR "file loaded, $log->{count}\n";

  $pktcount = $log->{count};
  $cnt_proccesed = 0;
  $cnt_ignored = 0;
  for ($index = 0; $index <= $pktcount; $index ++) { 
    ($length_orig,$length_incl,$drops,$secs,$msecs) = $log->header($index); 
    $data = $log->data($index);
    $eth_obj = NetPacket::Ethernet->decode($data);    
    next unless $eth_obj->{type} == NetPacket::Ethernet::ETH_TYPE_IP;
    
    $ip_obj = NetPacket::IP->decode($eth_obj->{data});
    next unless $ip_obj->{proto} == NetPacket::IP::IP_PROTO_UDP;
#  print("$secs $msecs $ip_obj->{src_ip} $ip_obj->{dest_ip}\n");

    $udp_obj = NetPacket::UDP->decode($ip_obj->{data});
    $udp_len = $udp_obj->{len};
#  print("udplength $udp_len ");

    ($check_sum, $action, $transcation_id, $rest) = unpack("h8Ch8a*", $udp_obj->{data});
#  print("checksum $check_sum, action $action, tid $transcation_id\n");

    if ($action == 53) {
      if ($udp_len >= 78) {

        ($isrequest, $pversion, $reserved, $pguid, $localrcnt, $serverrcnt,
         $nattype, $reserved1, $udpport, $tcpport, $stunip, $stunport1, $stunport2,
         $uploadbw, $uploadcap, $uploadrate, $pktrest) =
                unpack("Ch4h4H32SSCh10SSh8SSh8h8h8a*", $rest);

        $pversion = reverse $pversion;
        $uploadbw = hex( reverse $uploadbw);
        $uploadcap = hex( reverse $uploadcap);
        $uploadrate = hex( reverse $uploadrate);
        
#print STDOUT "$isrequest, $pversion, $reserved, $pguid, $localrcnt, $serverrcnt, $nattype, $reserved1, $udpport, $tcpport, $stunip, $stunport1, $stunport2, $uploadbw, $uploadcap, $uploadrate\n"; 
        print FOUT "$index $secs $msecs $ip_obj->{src_ip} $ip_obj->{dest_ip} $udp_len $pversion $localrcnt $serverrcnt $uploadbw $uploadcap $uploadrate $pguid\n";
        $cnt_proccesed ++;

      }else{
        ($isrequest, $pversion, $reserved, $pguid, $localrcnt, $serverrcnt,
         $nattype, $reserved1, $udpport, $tcpport, $stunip, $stunport1, $stunport2,
         $pktrest) = unpack("Ch4h4H32SSCh10SSh8SSa*", $rest);

        $pversion = reverse $pversion;
        if ($isrequest == 1) {
          print FOUT "DIS $index $isrequest $pversion $secs $msecs $ip_obj->{src_ip} $ip_obj->{dest_ip} $udp_len\n";
          $cnt_ignored ++;
        }
      }
    }
  }

  $log->clear();
  print STDERR "processed $cnt_proccesed, ignored $cnt_ignored\n";
}
