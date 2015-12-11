/*
 * test_reader.c - Implements a message reader
 *
 *  Created on: Oct 30, 2015
 *      Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <minix/syslib.h>

#define OK 0

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("Usage: test_reader <mboxd> <interval> <num_iter>\n");
		exit(0);
	}

	// Extract program arguments
	int mboxd = atoi(argv[1]);
	int interval = atoi(argv[2]);
	int num_iter = atoi(argv[3]);

	char msg[32];

	pid_t pid = getpid();

	for(int i = 0; i < num_iter; i++) {
		printf ("Reader %d: Retrieving message from mailbox %d\n", pid, mboxd);

		// Read message from mailbox
		int status = sys_retrieve_mail(mboxd, msg);

		if(status == OK) {
			printf("Reader %d: Received message from mailbox %d: %s\n", pid, mboxd, msg);
		}

		sleep(interval);
	}
}
