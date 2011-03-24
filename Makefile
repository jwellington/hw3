run: test test2
	./test
	./test2
test: test_prog.c mm.o
	gcc -g -o test test_prog.c mm.o
test2: test_ec.c mm.o
	gcc -g -o test2 test_ec.c mm.o
mm.o: mm.c mm.h
	gcc -c -g mm.c -o mm.o
clean:
	rm test test2 *.o
