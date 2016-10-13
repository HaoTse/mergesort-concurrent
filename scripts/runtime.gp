reset
set nokey
set logscale x 2
set xlabel 'thread number'
set ylabel 'time(sec)'
set style fill solid
set title 'time comparison'
set term png enhanced font 'Verdana,10'
set output 'plot.png'

plot [2:256][:]'runtime' using 1:2 with linespoints pointtype 5 pointsize 1,\
"" using 1:2:2 with labels center offset 0,0.5
