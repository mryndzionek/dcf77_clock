set lmargin at screen 0.17
set rmargin at screen 0.95

set multiplot layout 5, 1 title "DCF77 signal trace\n".trace_title

set datafile separator ","
set tmargin 0.3; set bmargin 0 

set timefmt "%Y-%m-%d %H:%M:%S"

set xdata time
set xtics rotate by 40 offset -9,-4 out nomirror font ", 12" 
set key below
set format x ""

set style line 100 lt 1 lc rgb "gray" lw 2
set style line 101 lt 0.5 lc rgb "gray" lw 1
set grid xtics ytics ls 100
set grid mxtics mytics ls 101

set title ""
set ylabel "Signal"
set xlabel ""
unset key
plot 'phase.csv' using 1:3 pt 1 ps 0.4 lc rgb "blue", \
'phase.csv' using 1:4 pt 1 ps 0.4 lc rgb "red"
#
set title ""
set ylabel "Phase"
set xlabel ""
unset key
plot 'phase.csv' using 1:2 pt 1 ps 0.4 lc rgb "dark-green"
#
set title ""
set ylabel "Syn mark strength\n and noise"
set xlabel ""
unset key
plot 'sync_mark.csv' using 1:3 pt 1 ps 0.4 lc rgb "blue", \
    'sync_mark.csv' using 1:4 pt 1 ps 0.4 lc rgb "red"
#
set format x "%Y-%m-%d %H:00"
set title ""
set ylabel "Sync mark"
set xlabel ""
unset key
plot 'sync_mark.csv' using 1:2 pt 1 ps 0.4 lc rgb "dark-green"
#
unset multiplot
