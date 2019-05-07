all:
	gcc -c -Wall -Werror -fpic foo.c
	gcc -shared -o libfoo.so foo.o
	gcc -L./ -Wl,-rpath=/usr/bin -Wall -o gen main.c -lfoo
clean:
	rm foo.o libfoo.so gen
