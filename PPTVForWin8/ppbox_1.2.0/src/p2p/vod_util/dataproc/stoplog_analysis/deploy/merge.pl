
use strict;
use warnings;

sub process_dir
{
    my $dirname = shift @_;
    my $dirhandle;
    my @filenames = ();
    opendir($dirhandle, $dirname) || die("can't opendir $dirname: $!");
    while (my $filename = readdir($dirhandle))
    {
        if ($filename eq '.' || $filename eq '..') {
            next;
        }
        elsif ($filename =~ /^AtStop_.+\.log$/) {
            push (@filenames, "\"$dirname\\$filename\"");
        }
    }
    closedir($dirhandle);
    # merge and process
    #my $cmd = "p2p_merge.exe order=ResourceID,HuageID @filenames | p2p_movie.exe \"$dirname\\Movie_Summary_Merge.log\" > \"$dirname\\Movie_Summary_Merge_Data.log\"";
    my $cmd = "p2p_merge.exe order=LogTime:TIME @filenames > \"$dirname\\Movie_Merge_Data.log\"";
    print STDERR "$cmd\n";
    system($cmd);
}


my @childs = ();
foreach my $dir (@ARGV)
{
    my $pid = fork();
    if ($pid)
    {
        push(@childs, $pid);
    }
    elsif ($pid == 0)
    {
        &process_dir($dir);
        exit(0);
    }
    else
    {
        print STDERR "Can not fork: $!";
        exit(0);
    }
}

foreach my $child (@childs)
{
    waitpid($child, 0);
}
