#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h> 
#include <sys/stat.h> 


int id;
char* ptr;


int main(int argc, char* argv[]) {

	id = shmget(0x2018, 0x1000, IPC_CREAT | 0666); 
	if (id == -1) {
		fprintf(stderr, "shmget failed \n");
		exit(1);
	}

	ptr = (char*)shmat(id, 0, 0);

	while (true) {
		fprintf(stderr,".");
		fflush(stderr);
		ptr[0]++;
		sleep(1);
	}

} // main()
