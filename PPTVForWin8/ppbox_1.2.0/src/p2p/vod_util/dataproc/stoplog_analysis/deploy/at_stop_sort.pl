
use strict;
use warnings;

sub process_dir
{
    my $dirname = shift @_;
    my $dirhandle;
    opendir($dirhandle, $dirname) || die("can't opendir $dirname: $!");
    while (my $filename = readdir($dirhandle))
    {
        if ($filename eq '.' || $filename eq '..') {
            next;
        }
        elsif ($filename =~ /^AtStop_.+\.log$/) {
            my $sorted_filename = "Sorted_$filename";
	    my $cmd = "p2p_sort.exe \"$dirname\\$filename\" order=LogTime:TIME > \"$dirname\\$sorted_filename\"";
	    print "$cmd\n";
	    system($cmd);
        }
    }
    closedir($dirhandle);
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
