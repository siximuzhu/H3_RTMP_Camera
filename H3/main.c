#include "main.h"
#include <stdio.h>

void main(void)
{
	int fd_ads,fd_ir;
	int power;
	char ir_data[3] = {0x11,0x22,0x33};
	int ir_data_len;
	char nrf_data[64];
	int nrf_data_len;
	int counter = 0;// receive ir times;
	uint8_t serial[64];

	fd_ads = ads1013_init();
	fd_ir = ir_init();
	nrf24l01_init();

	memset(nrf_data,0x0,64);
	memset(serial,0x0,64);

	if(get_boad_serial(serial)){
		printf("get boad serial fail!\n");
		return -1;
	}else{
		printf("boad serial:%s\n",serial);
	}
	
	while(1)
	{
		ir_data_len = ir_recv(fd_ir,ir_data,3); 
		if(ir_data_len == 3){
			if(!memcmp(ir_data,"\x11\x22\x33",3)){
				counter += 1;
				power = get_power(fd_ads);
				printf("receive ir counter = %d,power = %d,ir recv data:0x%02x 0x%02x 0x%02x\n",counter,power,ir_data[0],ir_data[1],ir_data[2]);
				http_post_data(counter,power);
			}
		}
		if(ir_data_len == 1){
			printf("ir send success!\n");
		}

		nrf_data_len = NRF24L01_RxPacket(nrf_data);
		if(nrf_data_len > 0){
			printf("nrf data:%s\n",nrf_data);	
			ir_send(fd_ir,"\x11\x22\x33",3);
			printf("send cmd to anther H3 by ir\n");
		}
	}
	

}
