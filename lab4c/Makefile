default:
	gcc -o lab4c_tcp lab4c_tcp.c -lmraa -g -Wall -Wextra -w -lm -std=c99
	gcc -o lab4c_tls lab4c_tls.c -lmraa -g -Wall -Wextra -w -lm -std=c99 -lssl -lcrypto

clean:
	rm -f lab4c_tls lab4c_tcp lab4c-804769644.tar.gz

dist:
	tar -czvf lab4c-804769644.tar.gz Makefile README lab4c_tcp.c lab4c_tls.c

check:
	./lab4b --log=test

	if [ -e "test" ]; then \
	    echo "File exists" ; \
	else \
	    echo "File does not exist" ; \
	fi; \

	
