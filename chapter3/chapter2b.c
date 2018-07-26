#include <stdio.h>

int main(int argc, char** argv) {
	int x = 34;
	if(x > 9 && x < 100) {
		puts("x is in the tens");
	}
	else {
		puts("x is not in the tens");
	}
	return 0;
}
