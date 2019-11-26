<<<<<<< HEAD
All: my_shell

my_shell: main.o buf.o list.o cmd.o symbols.o error.o
	gcc -o my_shell main.o buf.o list.o cmd.o symbols.o error.o

main.o: main.c
	gcc -c main.c

buf.o: buf.c
	gcc -c buf.c

list.o: list.c
	gcc -c list.c

cmd.o: cmd.c
	gcc -c cmd.c

symbols.o: symbols.c
	gcc -c symbols.c

error.o: error.c
	gcc -c error.c

clean:
	rm -rf *.o my_shell
=======
all: task3

task3: main.o
	gcc -o task3 main.o
	
main.o: main.c
	gcc -c -std=c11 main.c
	
clean:
	rm -rf *.o task3
>>>>>>> craft_table
