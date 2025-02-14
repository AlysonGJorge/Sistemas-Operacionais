all:
	gcc -g -c bootsector.c
	gcc -g -c fat.c
	gcc -g -c directory.c
	gcc -g -c shell.c
	gcc -g -c comandos.c
	gcc -g -c main.c
	gcc -o main *.o -lm

clean:
	rm *.o main