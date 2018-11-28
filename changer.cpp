#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/shm.h> 
#include <sys/stat.h> 


int main(int argc, char* argv[]) {

	int id = shmget(0x2018, 0x1000, IPC_CREAT | 0666); 
	if (id == -1) {
		fprintf(stderr, "shmget failed \n");
		exit(1);
	}

	uint32_t* ptr = (uint32_t*)shmat(id, 0, 0);

	*ptr = 0;

	while (true) {
		fprintf(stderr,"%d.",*ptr);
		fflush(stderr);
		(*ptr)++;
		sleep(2);
	}

} // main()
