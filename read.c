//#include <stdlib.h>
//#include <unistd.h>
#include <string.h> // memset
#include <stdio.h>
#include <fcntl.h> // defines O_RDWR|O_NONBLOCK|O_NDELAY
#include <termios.h>
#include <errno.h>

void main(){
	struct termios tty;
	int fd = open("/dev/ttyAMA0", O_RDONLY);
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
	cfsetospeed (&tty, B9600);
	cfsetispeed (&tty, B9600);

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

	// Setting other Port Stuff 
	/*
	tty.c_cflag     &=  ~PARENB;        // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;
	tty.c_cflag     &=  ~CRTSCTS;       // no flow control
	tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
	tty.c_oflag     =   0;                  // no remapping, no delays
	tty.c_cc[VMIN]      =   0;                  // read doesn't block
	tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout

	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
	tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
	tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	tty.c_oflag     &=  ~OPOST;              // make raw
//*/
	// Flush Port, then applies attributes
	tcflush( fd, TCIFLUSH );
	if ( tcsetattr ( fd, TCSANOW, &tty ) != 0)
	{
		printf("Error %d from tcsetattr %s\n",errno,strerror(errno));
		return;
	}
	
	// get the byte size 
	printf("%d\n",tty.c_cflag);
switch (tty.c_cflag & CSIZE) {
    case CS5:
        printf(" - data bits = 5\n");
        break;
    case CS6:
        printf(" - data bits = 6\n");
        break;
    case CS7:
        printf(" - data bits = 7\n");
        break;
    default:
    case CS8:
        printf(" - data bits = 8\n");
        break;
}
	
	/* Allocate memory for read buffer */
	char buf [16];
	memset (&buf, '\0', sizeof buf);

	/* *** READ *** */
	int i, n;

	while(1){
		printf(".");
		n = read( fd, &buf , sizeof buf );
	/* Error Handling */
		if(n<0){
			printf("Error reading: (%d) %s\n",errno,strerror(errno));
			break;
		}
		if(n>0){
	/* Print what I read... */
			printf("n=%d\n",n);
			for(i=0;i<n;i++){
				printf("%d ",buf[i]);
			}
			printf("\n");
		}
	}
	close(fd);
}

