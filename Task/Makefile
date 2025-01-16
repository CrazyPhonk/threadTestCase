all: prog1 prog2 lib

prog1:
	@make -C prog1

prog2:
	@make -C prog2

lib:
	@make -C lib

clean:
	@make -C prog1 clean
	@make -C prog2 clean
	@make -C lib clean
	rm -rf *.o

.PHONY: all clean prog1 prog2 lib
