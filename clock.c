//#include <unistd.h>
#include <stdlib.h>
#include <string.h> // memset
#include <stdio.h>
#include <fcntl.h> // defines O_RDWR|O_NONBLOCK|O_NDELAY
#include <termios.h>
#include <errno.h>

void go(){
	struct termios tty;
	int fd = open("/dev/ttyAMA0", O_RDWR);

	if(fd<0){
		fprintf(stderr, "Unable to open serial port\n");
		return;
	}else{
		printf("ttyAMA0 opened!(%d)\n", fd);
	}

	memset(&tty,0,sizeof(tty));
	
	if(tcgetattr(fd, &tty) != 0){
		printf("Error: %d, from tcgetattr: %s\n",errno,strerror(errno));
		return;
	}
	
	// Set Baud Rate 
	cfsetospeed (&tty, B38400);
	cfsetispeed (&tty, B38400);

	// 8 Bits, No Parity and 1 Stop bit settings
	tty.c_cflag     &=  ~PARENB;            // No Parity
	tty.c_cflag     &=  ~CSTOPB;            // 1 Stop Bit
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;                // 8 Bits
	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  1;                  // read doesn't block
	tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
	tty.c_cflag     |=  (CLOCAL | CREAD );    // turn on READ & ignore ctrl lines

	// Make raw 
	cfmakeraw(&tty);

	// Flush Port, then applies attributes
	tcflush( fd, TCIFLUSH );
	if ( tcsetattr ( fd, TCSANOW, &tty ) != 0)
	{
		printf("Error %d from tcsetattr %s\n",errno,strerror(errno));
		return;
	}
	
	unsigned char *cmd = "x150102010203z";
	int crc;
	int n_written, n;
	int i,j;
	unsigned char addr;
	char buf [16];
	
	n_written = write( fd, cmd, 14);
	printf("%d sent >> ",n_written);
	for(i=0;i<14;i++){
		printf("%c",cmd[i]);
	}
	printf("\n");
	fflush(stdout); 

	close(fd);
}

void main(){
	int i=0;
	int delay = 0;
	srand(time(NULL));
	go();
	while(0){
		go();
		i = delay;
		while(i>0) i--;
	}
}
