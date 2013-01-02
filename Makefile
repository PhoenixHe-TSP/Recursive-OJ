runprog:runprog.c oj_system.h
	gcc -Wall -Werror -g runprog.c -o runprog
clear:
	rm runprog compare compile control *.in *.out *.log *.err *~ *.o
test:
	gcc -g runprog.c -o runprog
	./runprog a a 1 64 64
