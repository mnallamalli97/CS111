#Mehar Nallamalli
#804769644
#mnallamalli97@ucla.edu

build: 
	#gcc SortedList.c -c
	gcc lab2_add.c -o lab2_add -lpthread -g -Wall -Wextra -w
	gcc lab2_list.c  -o lab2_list -lpthread -g -Wall -Wextra -w


dist: tests graphs 
	tar -czvf lab2a-804769644.tar.gz lab2_add.c lab2_list.c SortedList.h SortedList.c Makefile README tests.sh *.png *.csv *.gp

clean: 
	rm -f lab2a-804769644.tar.gz lab2_list lab2_add 

graphs:
	chmod a+x lab2_add.gp
	chmod a+x lab2_list.gp
	gnuplot -p lab2_add.gp
	gnuplot -p lab2_list.gp

tests: build
	chmod a+x ./tests.sh
	./tests.sh