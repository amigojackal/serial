//#include <unistd.h>
#include <stdlib.h>
#include <string.h> // memset
#include <stdio.h>
#include <fcntl.h> // defines O_RDWR|O_NONBLOCK|O_NDELAY
#include <termios.h>
#include <errno.h>

void parser(int type, unsigned char addr, unsigned char *data, int len){
	int error = 0;
	int value;
	char str[500];
	if(len != 7){
		printf("ERROR reading length!!\n");
		error = 1;
	}
	if(CalculateCRC(data,len-2) != data[len-2] + data[len-1]*256){
		printf("ERROR checking CRC!!\n");
		error = 2;
	}
	if(error){
		int delay = 10000000 + rand()%10000000;
		sleep(1);
		while(delay > 0) delay -= 1;
		return;
	}
	/*

	Note that rrd file has been generate by root in /var/rrd/ by the following scripts
	
	rrdtool create /var/rrd/co2.rrd --step 60 --no-overwrite DS:co2:GAUGE:5:U:U RRA:AVERAGE:0.5:1:1054080
	rrdtool create /var/rrd/temp.rrd --step 60 --no-overwrite DS:temp:GAUGE:5:U:U RRA:AVERAGE:0.5:1:1054080
	rrdtool create /var/rrd/humidity.rrd --step 60 --no-overwrite DS:humidity:GAUGE:5:U:U RRA:AVERAGE:0.5:1:1054080

	*/
	if(type == 4){
		value = data[3]*256+data[4];
		if(addr == 0){ // CO2
			printf("CO2 = %d\n",value);
			sprintf(str,"rrdtool update /var/rrd/co2.rrd N:%d",value);
			system(str);
		}
		if(addr == 1){ // Temperature
			printf("Temp = %.1f\n",value/10.0);
			sprintf(str,"rrdtool update /var/rrd/temp.rrd N:%.1f",value/10.0);
			system(str);
		}
		if(addr == 2){ // Humidity
			printf("Humidity = %.1f\n",value/10.0);
			sprintf(str,"rrdtool update /var/rrd/humidity.rrd N:%.1f",value/10.0);
			system(str);
		}
	}
}

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
	cfsetospeed (&tty, B9600);
	cfsetispeed (&tty, B9600);

	// 8 Bits, No Parity and 1 Stop bit settings
	tty.c_cflag     &=  ~PARENB;            // No Parity
	tty.c_cflag     &=  ~CSTOPB;            // 1 Stop Bit
	tty.c_cflag     &=  ~CSIZE;
	tty.c_cflag     |=  CS8;                // 8 Bits
	tty.c_cflag     &=  ~CRTSCTS;           // no flow control
	tty.c_cc[VMIN]   =  1;                  // read doesn't block
	tty.c_cc[VTIME]  =  10;                  // 0.5 seconds read timeout
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
	
	unsigned char cmd[8];
	int crc;
	int n_written, n;
	int i,j;
	unsigned char addr;
	char buf [16];
	
/* Modbus 40000 - 40009 (No use for now)
	cmd[0]=0x01;
	cmd[1]=0x03;
	cmd[2]=0x00;
	cmd[4]=0x00;
	cmd[5]=0x01;
	for(addr = 0; addr < 10 ; addr += 1){
		cmd[3]=addr;
		crc = CalculateCRC(cmd,6);
		cmd[6] = crc % 256;
		cmd[7] = (crc / 256)%256;
		memset (&buf, '\0', sizeof buf);
		
		n_written = write( fd, cmd, 8);
		printf("%d sent >> ",n_written);
		for(i=0;i<sizeof cmd;i++){
			printf("%d ",cmd[i]);
		}
		printf("\n");
		fflush(stdout); 

		n = read( fd, &buf , sizeof buf );
	// Error Handling 
		if(n<0){
			printf("Error reading: (%d) %s\n",errno,strerror(errno));
			break;
		}
		if(n>0){
	// Print what I read... 
			printf("%d read << ",n);
			for(i=0;i<n;i++){
				printf("%d ",buf[i]);
			}
			printf("\n");
			parser(3, addr, buf, n);
		}
	}
*/
	cmd[0]=0x01;
	cmd[1]=0x04;
	cmd[2]=0x00;
	cmd[4]=0x00;
	cmd[5]=0x01;
	for(addr = 0; addr < 3 ; addr += 1){
		cmd[3]=addr;
		crc = CalculateCRC(cmd,6);
		cmd[6] = crc % 256;
		cmd[7] = (crc / 256)%256;
		memset (&buf, '\0', sizeof buf);
		
		n_written = write( fd, cmd, 8);
		printf("%d sent >> ",n_written);
		for(i=0;i<sizeof cmd;i++){
			printf("%d ",cmd[i]);
		}
		printf("\n");
		fflush(stdout); 

		n = read( fd, &buf , sizeof buf );
	/* Error Handling */
		if(n<0){
			printf("Error reading: (%d) %s\n",errno,strerror(errno));
			break;
		}
		if(n>0){
	/* Print what I read... */
			printf("%d read << ",n);
			for(i=0;i<n;i++){
				printf("%d ",buf[i]);
			}
			printf("\n");
			parser(4, addr, buf, n);
		}
	}
	
	close(fd);
}

int CalculateCRC(unsigned char *cmd, int length)
{
	int x, y, SLSB;
	int CRC=65535; //0FFFFH
	for(x=0; x<length; x++){
		CRC=CRC ^ cmd[x];
		for(y=0; y<=7; y++){
			if( (CRC & 1)==1 ) SLSB=1;
			else SLSB=0;
			CRC=CRC >> 1;
			if( SLSB==1 ) CRC=CRC ^ 40961; //0A001H
		}
	}
	return CRC;
}


void main(){
	int count = 26;
	srand(time(NULL));
	while(count){
		system("date");
		go();
		sleep(2);
		count -= 1;
	}
}
