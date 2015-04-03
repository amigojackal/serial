all: read write test
	
read: read.c
	cc read.c -o read
write: write.c
	cc write.c -o write
test:
	cc `pkg-config --cflags --libs libmodbus` test.c -o test
