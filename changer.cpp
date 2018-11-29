#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <sys/shm.h> 
#include <sys/stat.h> 


void handler(int sig) {
	fprintf(stderr,"#");
}

int main(int argc, char* argv[]) {

	signal(SIGTRAP, handler);

	int id = shmget(0x2018, 0x1000, IPC_CREAT | 0666); 
	if (id == -1) {
		fprintf(stderr, "shmget failed \n");
		exit(1);
	}

	void* ptr = shmat(id, 0, 0);
	uint32_t* iptr = (uint32_t*)ptr;
	void** pptr = (void**)ptr;
	pptr[5] = ptr;

	fprintf(stderr,"PID=%d PTR=$%X - ",getpid(),ptr);

	*iptr = 0;

	while (true) {
		fprintf(stderr,"%d.",*iptr);
		fflush(stderr);
		(*iptr)++;
		sleep(2);
	}

} // main()
