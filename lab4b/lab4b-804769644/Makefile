default:
	gcc -o lab4b lab4b.c -lmraa -g -Wall -Wextra -w -lm -std=c99

clean:
	rm -f lab4b lab4b-804769644.tar.gz

dist:
	tar -czvf lab4b-804769644.tar.gz Makefile README lab4b.c

check:
	./lab4b --log=test

	if [ -e "test" ]; then \
	    echo "File exists" ; \
	else \
	    echo "File does not exist" ; \
	fi; \

	
