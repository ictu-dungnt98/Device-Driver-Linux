all:
	gcc -c -Wall -Werror -fpic foo.c
	gcc -shared -o libfoo.so foo.o
	gcc -L./ -Wl,-rpath=/usr/lib -Wall -o test main.c -lfoo
	cp libfoo.so /usr/lib
clean:
	rm test foo.o libfoo.so
