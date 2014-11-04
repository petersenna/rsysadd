#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>

void whoami(void);

int main(int argc, char *argv[]) {
	int sysnum;
	
	if(argc != 2){
		fprintf(stderr, "Usage %s <syscall number>\n", argv[0]);
		exit(1);
	}

	sysnum = atoi(argv[1]);
	if(sysnum < 0){
		fprintf(stderr, "Invalid syscall number [%d]\n", sysnum);
		exit(1);
	}
	
	whoami();

	if(syscall(sysnum) == -1) {
		fprintf(stderr, "Error calling syscall: %s\n", strerror(errno));
		exit(1);
	}

	whoami();

	return 0;
}

void whoami(void) {
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);

	if(pw != NULL) {
		puts(pw->pw_name);
	}
}