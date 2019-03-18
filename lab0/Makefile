#name: Mehar Nallamalli
#email: mnallamalli97@ucla.edu
#ID: 804-769-644



default: lab0.c
	gcc -Wall -Wextra lab0.c -o lab0 -g

dist:
	tar -czvf lab0-804769644.tar.gz README breakpoint.png backtrace.png lab0.c Makefile P0_check.sh

clean: 
	rm -f lab0 lab0-804769644.tar.gz

check:
	echo "Please run individual smoke screen tests with the make [name_of_check] command".




testCopy:
	echo "copy into output" > input.txt; \
	./lab0 --input=input.txt --output=output.txt; \
	if [ [$$? -ne 0] ]; then \
		echo 'failed, did not have same output'; \
	else \
		echo "success, same output"; \
	fi
	rm input.txt output.txt

validInput:
	./lab0 --input=input.txt; \
	if [[$$? -ne 2]]; then \
		echo "this failed. the args are not valid (input)"; \
	else \
		echo "this success because exit code 2."; \
	fi

dumpCoreCaught:  
	./lab0 --segfault --catch; \
	if [[$$? -ne 139]]; then \
		echo "the cores were not dumped, did not exit with code 139"; \
	else \
		echo "the return code is 139, dumped cores."; \
	fi


segfaultCaught: 
	./lab0 --segfault --catch; \
	if [[$$? -ne 4]]; then \
		echo "the segfault was not caught, did not exit with code 4"; \
	else \
		echo "the return code is 4, segfault caught."; \
	fi
