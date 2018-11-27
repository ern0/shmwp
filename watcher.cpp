#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h> 
#include <sys/stat.h> 


int main(int argc, char* argv[]) {

	int id;
	char* ptr;

	id = shmget(0x2018, 0, 0); 
	if (id == -1) {
		fprintf(stderr, "shmget failed");
		exit(1);
	}

	ptr = (char*)shmat(id, 0, 0);

	while (true) {

		fprintf(stderr,"[%d]", *ptr);
		usleep(500);

	}

} // main()
