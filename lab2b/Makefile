#Mehar Nallamalli
#804769644
#mnallamalli97@ucla.edu

build: 
	#gcc SortedList.c -c
	gcc -pthread lab2_add.c -o lab2_add -lpthread -g -Wall -Wextra -w
	gcc -pthread lab2_list.c  -o lab2_list -lpthread -g -Wall -Wextra -w


dist: tests 
	tar -czvf lab2b-804769644.tar.gz lab2_list.c lab2_add.c SortedList.h SortedList.c Makefile README tests.sh lab2b_list.csv *.png profile.out

clean: 
	rm -f lab2b-804769644.tar.gz lab2_list lab2_add 

graphs:
	chmod a+x lab2b_list.gp
	gnuplot -p lab2b_list.gp

profile: build
	rm -f raw.gperf
	LD_PRELOAD=/usr/lib64/libprofiler.so
	CPUPROFILE=
	CPUPROFILE= ./lab2_list --threads=12 --iterations=1000 --sync=s 
	pprof lab2_list /tmp/prof.out --text > profile.out
	pprof lab2_list /tmp/prof.out --list=create_thread  >> profile.out
	rm -f raw.gperf

tests: build
	chmod a+x ./tests.sh
	./tests.sh