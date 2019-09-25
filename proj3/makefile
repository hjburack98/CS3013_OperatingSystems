
adder: adder.o mailbox.o
	gcc -Wall -g adder.o mailbox.o -o adder -lpthread

adder.o: adder.c mailbox.h
	gcc -Wall -g -c adder.c -o adder.o

mailbox.o: mailbox.c
	gcc -Wall -g -c mailbox.c -o mailbox.o

clean:
	rm -f adder.o mailbox.o adder