#!/bin/sh
IFS=$'\n'
./ts_01 -B run-01.mac -D > domrun.dat &
for line in `cat runlist.dat`; do
    IFS=$' '
    set -- $line
    ./ts_01 -B run-01.mac -$2 -r $3 -d $4 -n $5 > fibrun-$1.dat &
    IFS=$'\n'
done
