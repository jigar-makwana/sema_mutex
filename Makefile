all:pro1 pro2

pro1:
	gcc Ass2_1.c -pthread -o 1

pro2:
	gcc Ass2_2.c -pthread -o 2

clean:
	rm*.o 1 2


