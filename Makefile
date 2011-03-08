run: test
	./test
test: test_prog.c mm.o
	gcc -o test test_prog.c mm.o
mm.o: mm.c mm.h
	gcc -c mm.c -o mm.o
