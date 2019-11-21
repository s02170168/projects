All: my_shell

my_shell: main.o buf.o list.o symbols.o error.o cmd.o
	gcc -o my_shell main.o buf.o list.o symbols.o error.o cmd.o

main.o: main.c
	gcc -c main.c

buf.o: buf.c
	gcc -c buf.c

list.o: list.c
	gcc -c list.c

symbols.o: symbols.c
	gcc -c symbols.c

error.o: error.c
	gcc -c error.c

cmd.o: cmd.c
	gcc -c cmd.c

clean:
	rm -rf *.o my_shell
