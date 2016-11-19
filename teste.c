#include <stdio.h>
void usarArgv(char **argv){
	printf("ARGV1: %s", argv[1]);
}

int main(int argc, char **argv){
	usarArgv(argv);
	return 0;
}
