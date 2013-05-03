
use strict;
use warnings;

sub erase_log_files {
    my $dirname = shift @_;
    my $dirhandle;
    opendir($dirhandle, $dirname) || die("can't opendir $dirname: $!");
    while (my $filename = readdir($dirhandle)) {
        if ($filename eq '.' || $filename eq '..') {
            next;
        }
        elsif ($filename =~ /@(.+)\.zip$/i) {
            my @filelist = ("$dirname/AtStop_$1.log", "$dirname/AtN_$1.log");
            unlink @filelist;
        }
        elsif ($filename =~ /^in(.+)\.zip$/i) {
            my @filelist = ("$dirname/AtStop_in$1.log", "$dirname/AtN_in$1.log");
            unlink @filelist;
        }
    }
    closedir($dirhandle);
}

sub process_dir {

    my $dirname = shift @_;

    &erase_log_files($dirname);

    my $dirhandle;
    opendir($dirhandle, $dirname) || die("can't opendir $dirname: $!");
    my @allfiles = readdir($dirhandle);
    closedir($dirhandle);
    foreach my $filename (@allfiles)
    {
        print STDERR "Processing $filename\n";
        if ($filename eq '.' || $filename eq '..') 
        {
            next;
        }
        elsif ($filename =~ /@(.+)\.zip$/i) 
        {
            my ($outfile_stop);
            open($outfile_stop, ">>", "$dirname/AtStop_$1.log");
            my $cmd = "p2p_unzip.exe \"$dirname/$filename\" | p2p_decode.exe";
			#my $cmd = "p2p_unzip.exe \"$dirname/$filename\" 2> $dirname/1.txt | p2p_decode.exe 2> $dirname/2.txt" ;
            print STDERR "CommandLine: $cmd\n";
            my $file;
            open ($file, "$cmd |");
            while (my $line = <$file>) {
                $line =~ s/\&/|/g;
				
                if ($line =~ /A=9\|/)
                {
                    $line =~ s/\|B=/|HuageID=/;
                    $line =~ s/\|C=/|ResourceID=/;
                    $line =~ s/\|D=/|PeerVersion=/;
                    $line =~ s/\|E=/|VideoName=/;
                    $line =~ s/\|F=/|OriginalUrl=/;
                    $line =~ s/\|G=/|OriginalReferUrl=/;
                    $line =~ s/\|H=/|DiskBytes=/;
                    $line =~ s/\|I=/|VideoBytes=/;
                    $line =~ s/\|J=/|P2PDownloadBytes=/;
                    $line =~ s/\|K=/|HttpDownloadBytes=/;
                    $line =~ s/\|L=/|AvgDownloadSpeed=/;
                    $line =~ s/\|M=/|IsSaveMode=/;
                    $line =~ s/\|N=/|StartPosition=/;
                    $line =~ s/\|O=/|MaxP2PDownloadSpeed=/;
                    $line =~ s/\|P=/|MaxHttpDownloadSpeed=/;
                    $line =~ s/\|Q=/|ConnectedPeerCount=/;
                    $line =~ s/\|R=/|FullPeerCount=/;
                    $line =~ s/\|S=/|MaxActivePeerCount=/;
                    $line =~ s/\|T=/|QueriedPeerCount=/;
                    $line =~ s/\|U=/|SourceType=/;
                    $line =~ s/Action=0\|//;
                    $line =~ s/A=9\|//;
                    print $outfile_stop $line;
                }
            }
            close($outfile_stop);
        }
    }
}

my @childs = ();
foreach my $dir (@ARGV) {
    my $pid = fork();
    if ($pid) {
        push(@childs, $pid);
    }
    elsif ($pid == 0) {
        &process_dir($dir);
        exit(0);
    }
    else {
        print STDERR "Can not fork: $!";
        exit(0);
    }
}
foreach my $child (@childs) {
    waitpid($child, 0);
}
