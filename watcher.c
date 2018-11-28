#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/shm.h> 
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/prctl.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>

extern int errno;
static pid_t extpid;


enum {
	BREAK_EXEC = 0x0,
	BREAK_WRITE = 0x1,
	BREAK_READWRITE = 0x3,
};

enum {
	BREAK_ONE = 0x0,
	BREAK_TWO = 0x1,
	BREAK_FOUR = 0x3,
	BREAK_EIGHT = 0x2,
};

#define ENABLE_BREAKPOINT(x) (0x1<<(x*2))
#define ENABLE_BREAK_EXEC(x) (BREAK_EXEC<<(16+(x*4)))
#define ENABLE_BREAK_WRITE(x) (BREAK_WRITE<<(16+(x*4)))
#define ENABLE_BREAK_READWRITE(x) (BREAK_READWRITE<<(16+(x*4)))

/*
 * This function fork()s a child that will use
 * ptrace to set a hardware breakpoint for 
 * memory r/w at _addr_. When the breakpoint is
 * hit, then _handler_ is invoked in a signal-
 * handling context.
 */
bool install_breakpoint(void *addr, int bpno, void (*handler)(int)) {

	pid_t child = 0;
	uint32_t enable_breakpoint = ENABLE_BREAKPOINT(bpno);
	uint32_t enable_breakwrite = ENABLE_BREAK_WRITE(bpno);
	pid_t parent = getpid();
	int child_status = 0;

	if (!(child = fork()))
	{
		int parent_status = 0;
		if (ptrace(PTRACE_ATTACH, extpid, NULL, NULL)) {
			perror("ptrace attach fail");
			printf("%d\n",extpid);
			_exit(1);
		}

		while (!WIFSTOPPED(parent_status)) {
			waitpid(extpid, &parent_status, 0);
		}
	
		/*
		 * set the breakpoint address.
		 */
		if (ptrace(PTRACE_POKEUSER,
		           extpid,
		           offsetof(struct user, u_debugreg[bpno]),
		           addr)) 
		{
			perror("wut1");
			_exit(1);
		}

		/*
		 * set parameters for when the breakpoint should be triggered.
		 */
		if (ptrace(PTRACE_POKEUSER,
		           extpid,
		           offsetof(struct user, u_debugreg[7]),
		           enable_breakwrite | enable_breakpoint)) 
		{
			perror("wut2");
			_exit(1);			
		}

		// itt kene nezni, hogy mi tortent az extpid-ben
		//while (true) sleep(1);

		if (ptrace(PTRACE_DETACH, extpid, NULL, NULL)) {
			perror("wut3");
			_exit(1);
		}

		_exit(0);
	}

	waitpid(child, &child_status, 0);

	signal(SIGTRAP, handler);

	if (WIFEXITED(child_status) && !WEXITSTATUS(child_status))
		return true;
	return false;
}

/*
 * This function will disable a breakpoint by
 * invoking install_breakpoint is a 0x0 _addr_
 * and no handler function. See comments above
 * for implementation details.
 */
bool disable_breakpoint(int bpno) 
{
	return install_breakpoint(0x0, bpno, NULL);
}


static int handled = 0;

void handle(int s) {
	handled++;
}


int main(int argc, char **argv) {

	if (argc < 2) {
		printf("specify PID \n");
		exit(1);
	}
	extpid = atoi(argv[1]);
	if (extpid == 0) {
		printf("invalid PID \n");
		exit(1);
	}
	fprintf(stderr,"PID=%d - ",extpid);

	static uint32_t* ptr;

	int id = shmget(0x2018, 0, 0); 
	if (id == -1) {
		fprintf(stderr, "shmget failed");
		exit(1);
	}

	ptr = (uint32_t*)shmat(id, 0, 0);

	if (!install_breakpoint(ptr, 0, handle)) {
		printf("failed to set the breakpoint!\n");
		exit(1);
	}

	(*ptr)++;

	while (true) {
		fprintf(stderr,"[%d:%d]",handled,*ptr);
		fflush(stderr);
		sleep(1);
		if (handled > 6) break;
	}

	if (!disable_breakpoint(0))
		printf("failed to disable the breakpoint!\n");

	return 0;
} // main()

