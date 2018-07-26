#include <stdio.h>

int add_together(int x, int y) {
	int result = x + y;
	return result;
}

int main(int argc, char** argv) {
	int added = add_together(5, 8);
	printf("%d", added);
	return 0;
}



