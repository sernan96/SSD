#include<stdio.h>
#include"ssd.h"
//#include "random_address.h"

int main(void) {
	/*
	while(1){
		uint32_t addr = make_random_address();
    	printf("랜덤 주소: 0x%08X\n", addr);
	}
	return 0;
	*/

	ssdInit();
	ssdWrite(1, 0x11111111);
	ssdWrite(2, 0x22222222);
	ssdWrite(3, 0x33333333);
	ssdWrite(4, 0x44444444);
	ssdWrite(5, 0x55555555);
	ssdWrite(6, 0x66666666);
	ssdWrite(7, 0x77777777);
	ssdWrite(8, 0x88888888);

	uint32_t arr[8];
	for (int i= 0; i < 8; i++) {
		arr[i] = ssdRead(i + 1);
		printf("%d: %X\n", i+1, arr[i]);
	}
	
}