build: angecryption.c
	gcc angecryption.c -o angecryption.o -c

.PHONY test: build
	gcc angecryption.o test/test.c -o runtests -lcrypto
	@./runtests || echo '\033[31mTests failed\033[0m'
	@echo -e '\033[0;32mTests passed\033[0m'
