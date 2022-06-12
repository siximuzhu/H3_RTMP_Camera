#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int ir_init()
{
	int fd;

	if ((fd = serialOpen ("/dev/ttyS1", 9600)) < 0)
	{
		printf ("Unable to open serial device: %s\n", strerror (errno)) ;
		return fd ;
	}

	printf("open /dev/ttyS1 ok\n");
	if (wiringPiSetup () == -1)
	{
		printf ("Unable to start wiringPi: %s\n", strerror (errno)) ;
		return -1 ;
	}

	return fd;
}

void ir_send(int fd,unsigned char *data,unsigned int len)
{
	int i;

	serialPutchar (fd,0xa1 ) ;
	serialPutchar (fd,0xf1 ) ;

	for(i = 0;i < len;i++)
	{
		serialPutchar(fd,data[i]);
	}
}

int ir_recv(int fd,unsigned char *data,unsigned int len)
{
	int i;

	if (serialDataAvail (fd))
	{
		for(i = 0;i < len;i++)
		{
			data[i] = serialGetchar(fd);
			/*if recv 0xf1:send ok*/
			if(0xf1 == data[i]){
				return 1;
			}
		}
	}
	else{
		return 0;
	}

	return i;
}

int ir_test ()
{
	int fd;
	char data[3] = {0x44,0x55,0x66};


	fd = ir_init();
#if 0
	
	while(1)
	{
		if(ir_recv(fd,data,3)){
			printf("0x%02x 0x%02x 0x%02x\n",data[0],data[1],data[2]);	
		}
	}
#else
	ir_send(fd,data,3);
	printf("ir send over\n");


#endif



	return 0 ;
}
