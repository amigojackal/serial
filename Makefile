all: read write test 
	
read: read.c
	cc read.c -o read
write: write.c
	cc write.c -o write
test:
	cc `pkg-config --cflags --libs libmodbus` test.c -o test
cpp: readcpp writecpp
	
readcpp: read.cpp
	g++ read.cpp -o readp
writecpp: write.cpp
	g++ write.cpp -o writep
