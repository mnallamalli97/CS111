#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

set title "graph-1: Throughput vs # of threads"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Throughput"
set logscale y 10
set output 'lab2b_1.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep add-m lab2_add.csv" using ($2):(1000000000/($6)) \
	title 'mutex add sync' with points lc rgb 'red', \
 	"< grep add-s lab2_add.csv" using ($2):(1000000000/($6)) \
	title 'spin-lock add sync' with points lc rgb 'red', \
 	"< grep list-none-s lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock list sync' with points lc rgb 'green', \
 	"< grep list-none-m lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list sync' with points lc rgb 'green'









set title "graph-2: Threads vs mean time"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Mean-time"
set logscale y 10
set output 'lab2b_2.png'

# note that unsuccessful runs should have produced no output
plot \
     "< grep list-none-m lab2b_list.csv" using ($2):($7) \
	title 'avg time for op' with points lc rgb 'green', \
     "< grep list-none-m lab2b_list.csv" using ($2):($8) \
	title 'avg time for mutex' with points lc rgb 'red'





set title "graph-3: Threads vs iterations"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Iterations"
set logscale y 10
set output 'lab2b_3.png'

# note that unsuccessful runs should have produced no output
plot \
     "< grep list-id-none lab2b_list.csv" using ($2):($3) \
	title 'no sync' with points lc rgb 'red', \
     "< grep list-id-m lab2b_list.csv" using ($2):($3) \
	title 'mutex sync' with points lc rgb 'green', \
	"< grep list-id-s lab2b_list.csv" using ($2):($3) \
	title 'spin-lock sync' with points lc rgb 'green'



set title "graph-4: Threads vs Mutex"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Mutex Sync"
set logscale y 10
set output 'lab2b_4.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep list-none-m,.*,1, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list 1' with linespoints lc rgb 'red', \
 	"< grep list-none-m,.*,4, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list 4' with linespoints lc rgb 'blue', \
 	"< grep list-none-m,.*,8, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list 8' with linespoints lc rgb 'green', \
 	"< grep list-none-m,.*,16, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list 16' with linespoints lc rgb 'yellow'


set title "graph-5: Threads vs spin-lock"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "spin-lock Sync"
set logscale y 10
set output 'lab2b_5.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep list-none-s,.*,1, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock list 1' with linespoints lc rgb 'red', \
 	"< grep list-none-s,.*,4, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock list 4' with linespoints lc rgb 'blue', \
 	"< grep list-none-s,.*,8, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock list 8' with linespoints lc rgb 'green', \
 	"< grep list-none-s,.*,16, lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spin-lock list 16' with linespoints lc rgb 'yellow'










