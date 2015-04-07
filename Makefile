all: read write clock sensor

sensor: sensor.c
	cc sensor.c -o sensor	
clock: clock.c
	cc clock.c -o clock
read: read.c
	cc read.c -o read
write: write.c
	cc write.c -o write
cpp: readcpp writecpp
	
readcpp: read.cpp
	g++ read.cpp -o readp
writecpp: write.cpp
	g++ write.cpp -o writep
