set terminal postscript eps enhanced size 6,3.5 color
set output "result.eps"

set xtics font "Times-Roman, 24"  offset 0,-1
set ytics font "Times-Roman, 24"  offset -1,-0

#unset grid

set xtics 0,5000,25000
set ytics 0,200000,1000000

set key off

set multiplot


set arrow 1 filled from 10500,0 to 1500,555000 size 6.0,10 nohead ls 7
set arrow 2 filled from 11300,0 to 6500,555000 size 6.0,10 nohead ls 7

set style data linespoints


set xlabel "Simulations" font "Times-Roman, 34" offset -0,-2
set ylabel "Simulation duration" font "Times-Roman, 34" offset -5, -1

plot "test.csv" using 1:5 lt 1 lw 1 lc rgb "blue" title ""
# title "Step pi/180, range [-pi/36, +pi/36]"



replot




unset xtics
set ytics 0,500,1000000

set ytics font "Times-Roman, 16"  offset 0,0

set grid
set key off
unset title
unset xlabel
unset ylabel

unset arrow 1
unset arrow 2

set origin 0.13,0.57

set size 0.25,0.40

plot [10500:11300] "test.csv" using 1:5 lt 1 lw 1 lc rgb "black" title ""

