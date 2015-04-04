all: read write 
	
read: read.c
	cc read.c -o read
write: write.c
	cc write.c -o write
cpp: readcpp writecpp
	
readcpp: read.cpp
	g++ read.cpp -o readp
writecpp: write.cpp
	g++ write.cpp -o writep
