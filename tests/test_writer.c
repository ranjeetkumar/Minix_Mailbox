/*
 * test_writer.c - Implements a message writer
 *
 *  Created on: Oct 30, 2015
 *      Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <minix/syslib.h>

#define MAX_MAIL_DESTS 4
#define OK 0
#define EMPTY -1

int main(int argc, char *argv[]) {
	if (argc < 6) {
		printf("Usage: test_writer <mboxd> <interval> <num_iter> <num_pids> <list_of_pids>\n");
		exit(-1);
	}

	int num_pids = atoi(argv[4]);
	if (num_pids > 4) {
		printf("Error: Max number of pids is 4. You entered: %d\n", num_pids);
		exit(-1);
	}

	if (num_pids != argc - 5) {
		printf("Error: Number of pids in list_of_pids does not match num_pids. num_pids = %d, actual number of pids entered: %d\n",
				argc - 5, num_pids);
		exit(-1);
	}

	// Extract program arguments
	int mboxd = atoi(argv[1]);
	int interval = atoi(argv[2]);
	int num_iter = atoi(argv[3]);

	char msg[32];
	pid_t dest[4];

	int i = 0;
	for (i = 0; i < num_pids; i++) {
		dest[i] = atoi(argv[i + 5]);
	}

	// Filling rest of the dest array with 0 pid value
	for (; i < MAX_MAIL_DESTS; i++) {
		dest[i++] = EMPTY;
	}

	pid_t pid = getpid();

	for(int i = 0; i < num_iter; i++) {
		// Generate message
		sprintf(msg, "%05d_%05d", pid, i);

		printf("Writer %d: Writing message %s of length %d to mailbox %d\n",
				pid, msg, strlen(msg), mboxd);

		// Send message to mailbox
		int status = sys_deposit_mail(mboxd, dest, msg);

		if (status == OK) {
			printf("Writer %d: Successfully written message to mailbox %d\n", pid, mboxd);
		}

		sleep(interval);
	}
}
