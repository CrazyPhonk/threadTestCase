CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS = -shared

libmylib.so: myLib.o
	$(CC) $(LDFLAGS) -o ../libmylib.so $^ $(LDLIBS)
mylib.o: myLib.c myLib.h
	$(CC) $(CFLAGS) -c $<
clean:
	rm -rf *.o ../libmylib.so testMain
