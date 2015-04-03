#include <stdio.h>      // standard input / output functions
#include <iostream>
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions

int main(){
	int serial = open( "/dev/ttyAMA0", O_WRONLY| O_NOCTTY );
//	int serial = open( "/dev/ttyUSB0", O_WRONLY| O_NOCTTY );

	struct termios tty;
	struct termios tty_old;
	memset (&tty, 0, sizeof tty);

	/* Error Handling */
	if ( tcgetattr ( serial, &tty ) != 0 ) {
	   std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << std::endl;
	}

	/* Save old tty parameters */
	tty_old = tty;

	/* Set Baud Rate */
	cfsetospeed (&tty, (speed_t)B9600);
	cfsetispeed (&tty, (speed_t)B9600);

	/* Setting other Port Stuff */
	tty.c_cflag     &=  ~PARENB;            // Make 8n1
	tty.c_cflag     &=  ~CSTOPB;
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;

	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  1;                  // read doesn't block
	tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
	tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

	/* Make raw */
	cfmakeraw(&tty);

	/* Flush Port, then applies attributes */
	tcflush( serial, TCIFLUSH );
	if ( tcsetattr ( serial, TCSANOW, &tty ) != 0) {
	   std::cout << "Error " << errno << " from tcsetattr" << std::endl;
	}
	
	//-------------------------------------- write
	
	unsigned char cmd[8];
	cmd[0]=0x01;	cmd[1]=0x03;
	cmd[2]=0x00;	cmd[3]=0x01;
	cmd[4]=0x00;	cmd[5]=0x01;
	cmd[6]=0xd5;	cmd[7]=0xca;

/*	int n_written = 0,
		spot = 0;
	do {
		n_written = write( serial, &cmd[spot], 1 );
		spot += n_written;
	} while (cmd[spot-1] != '\r' && n_written > 0);
	//*/

	std::cout << "serial = " << serial << std::endl;
	int n_written = write( serial, cmd, sizeof(cmd));
	std::cout << "Write " << n_written << std::endl;
	if(n_written<0){
		std::cout << "Error " << errno << ": " << strerror(errno) << std::endl;
	}

	return 0;
}
