#!/bin/bash
# SPDX-License-Identifier: LicenseRef-CSSL-1.0

function die() { echo "$@"; exit 1; }

[[ $# == 1 ]] || die "usage: $0 <filename>"

FILE=${1}

#OUTPUT=/tmp/results.png
#echo "dumping to $OUTPUT"
gnuplot << EOF

#set terminal pngcairo size 2000,2000 enhanced font 'Verdana,10'
#set output "$OUTPUT"

set format x '%H:%M:%S'
set timefmt '%H:%M:%S'
set datafile separator "\t"
set xdata time
#set xrange ['08:43:17':'08:43:18']
set grid

set multiplot layout 4,1

set yrange [0:300]
set ylabel "dB"
plot "$FILE" using 1:2 w l title "SNRx10 (inst)", "$FILE" using 1:9 w l title "SNRx10 (smooth)", "$FILE" using 1:6 w l title "txpower-calc"

set yrange [5:50]
set ylabel "PHR (dB)"
set y2range [0:3]
set y2label "TPC"
set boxwidth 0.1
plot "$FILE" using 1:4 w points pt 7 axes x1y2 title "TPC", "$FILE" using 1:3 w l title "PHR"

set yrange [0:60]
set ylabel "RB"
set y2range [0:27]
set y2label "MCS"
plot "$FILE" using 1:7 w l title "RB", "$FILE" using 1:8 w l axes x1y2 title "MCS"

set yrange [0:1000]
set ylabel "TB size (B)"
plot "$FILE" using 1:5 w l

unset multiplot

pause mouse close
EOF
