//#include <stdlib.h>
//#include <unistd.h>
#include <string.h> // memset
#include <stdio.h>
#include <fcntl.h> // defines O_RDWR|O_NONBLOCK|O_NDELAY
#include <termios.h>
#include <errno.h>

void main(){
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
	
	/* Set Baud Rate */
	cfsetospeed (&tty, B9600);
	cfsetispeed (&tty, B9600);

	/* Setting other Port Stuff */
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

	/* Flush Port, then applies attributes */
	tcflush( fd, TCIFLUSH );
	if ( tcsetattr ( fd, TCSANOW, &tty ) != 0)
	{
		printf("Error %d from tcsetattr %s\n",errno,strerror(errno));
		return;
	}
	
	unsigned char cmd[16];
	cmd[0]=0;	cmd[1]=1;	cmd[2]=0;	cmd[3]=3;
	cmd[4]=0;	cmd[5]=0;	cmd[6]=0;	cmd[7]=1;
	cmd[8]=0;	cmd[9]=0;	cmd[10]=0;	cmd[11]=1;
	cmd[12]=0xc;	cmd[13]=0xa;	cmd[14]=0xd;	cmd[15]=0x5;
	//*/
//	unsigned char *cmd = "010300010001CAD5";
	int size = 16;
	
	int n_written = write( fd, cmd, 16 );
	int i;
	
	printf("%d sent\n",n_written);
	for(i=0;i<16;i++){
		printf("%d ",cmd[i]);
	}
	printf("\n");
	
	close(fd);
	
	printf("hello modbus!\n");
}

