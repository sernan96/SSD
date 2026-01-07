#include<stdio.h>
#include "random_address.h"
int main(void) {
	while(1){
		uint32_t addr = make_random_address();
    	printf("랜덤 주소: 0x%08X\n", addr);
	}
	return 0;
}