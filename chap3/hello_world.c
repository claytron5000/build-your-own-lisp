#include <stdio.h>


void sayHello(int arg) {
	int i = 0;	
	while ( i<arg) {
		puts("Hello World");
		i++;
	}
}

int main(int arg, char** argv) {
	for (int i = 0; i < 5; i++) {
		puts("Hello World, from for loop");
	
	}
	int j = 0;
	while ( j < 5) {
		puts("Hello World, from while loop.");
		j++;
	}
	sayHello(3);
}
