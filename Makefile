clean:
	rm -f test/out.jpg

build: angecryption.c
	gcc angecryption.c -o angecryption.o -c -fpic -maes

shared: build
	gcc -shared angecryption.o -o angecryption.so

.PHONY test: build clean
	gcc angecryption.o test/test.c -o runtests -lcrypto
	@./runtests || (echo '\033[31mTests failed\033[0m' && exit 1)
	@echo -e '\033[0;32mTests passed\033[0m'
