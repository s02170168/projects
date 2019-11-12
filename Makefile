all: task3

task3: main.o
	gcc -o task3 main.o
	
main.o: main.c
	gcc -c -std=c11 main.c
	
clean:
	rm -rf *.o task3
