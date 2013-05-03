set term postscript
set xlabel "RID Index"
set size 0.6, 0.6
set ylabel "PDF"
set y2label "CDF"
#set yrange [0 :1]
#set xrange [1:1000000]
set key top left 
set logscale x
set autoscale y2
set ytics nomirror
set y2tics nomirror
set output "rid_dist.ps"
set terminal postscript monochrome 12
set pointsize 1
set data style linespoints
plot 'filelist.2011_10_09.rid_dist' using ($1+1):($3/104794) t 'PDF' with l 1, 'filelist.2011_10_09.rid_dist' using ($1+1):($4/104794) axes x1y2 t 'CDF' with l 2


