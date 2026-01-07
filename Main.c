#include<stdio.h>
#include"gen_test.h"
#include"ssd.h"
//#include "random_address.h"

int main(void) {
	ssdInit();

	gen_test_scenario("test.txt", 1000, 1);

	ssdExit();
}