/*
 * test_all.c - Test all system calls
 *
 *  Created on: Oct 30, 2015
 *      Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 */
#include <minix/syslib.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define OK 0
#define EMPTY -1

#define NUM_WRITERS 1
#define MAX_MAIL_DESTS 4
#define NUM_ITER 20

// Test functions
void testMailbox();
void testReader();
void testWriter();
void testReaderWriter();

// Util method
void cleanup();
void assertStatusCode();

int main() {
	cleanup();
    testMailbox();
    testReader();
    testWriter();
    testReaderWriter();
}

void testMailbox() {
    int mboxd[20];
    int result;
    // Create 16 mailboxes
    for(int i = 0; i < 16; i++) {
        result = sys_create_mailbox(&mboxd[i]);
        assertStatusCode("Create mailbox", OK, result);
    }

    // Post office is full. Create mailbox.
    result = sys_create_mailbox(&mboxd[17]);
    assertStatusCode("Create mailbox", POST_OFFICE_FULL, result);

    // Remove invalid mailbox.
    result = sys_remove_mailbox(17);
    assertStatusCode("Remove mailbox", MAILBOX_NOT_FOUND, result);

    // Remove valid mailboxes
    for(int i = 15; i >= 0; i--) {
        result = sys_remove_mailbox(mboxd[i]);
        assertStatusCode("Remove mailbox", OK, result);
    }
}

void testReader() {
	int mboxd;
	int result;
	char msg[32];
	// Create mailbox
	result = sys_create_mailbox(&mboxd);
	assertStatusCode("Create mailbox", OK, result);

	// Read from empty mailbox
	result = sys_retrieve_mail(mboxd, msg);
	assertStatusCode("Retrieve mail", MAILBOX_EMPTY, result);

	// Read from invalid mailbox
	result = sys_retrieve_mail(1, msg);
	assertStatusCode("Retrieve mail", MAILBOX_NOT_FOUND, result);

	// Remove mailbox
	result = sys_remove_mailbox(mboxd);
	assertStatusCode("Remove mailbox", OK, result);
}

void testWriter() {
	int mboxd;
	int result;
	char msg[32];
	sprintf(msg, "%s", "test");
	pid_t dest[4] = {1, EMPTY, EMPTY, EMPTY};
	// Create mailbox
	result = sys_create_mailbox(&mboxd);
	assertStatusCode("Create mailbox", OK, result);

	// Write to self
	pid_t pid = getpid();
	pid_t dest_self[] = {pid, EMPTY, EMPTY, EMPTY};
	result = sys_deposit_mail(mboxd, dest_self, msg);
	assertStatusCode("Deposit mail", OK, result);

	// Read from self
	result = sys_retrieve_mail(mboxd, msg);
	assertStatusCode("Retrieve mail", OK, result);
	printf("Message retrieved from self: %s\n", msg);

	// Write to valid mailbox and init process
	for(int i = 0; i < 16; i++) {
		result = sys_deposit_mail(mboxd, dest, msg);
		assertStatusCode("Deposit mail", OK, result);
	}

	// Read while no message for process
	result = sys_retrieve_mail(mboxd, msg);
	assertStatusCode("Retrieve mail", MAILBOX_NO_MESSAGE, result);


	// Write to full mailbox and init process
	result = sys_deposit_mail(mboxd, dest, msg);
	assertStatusCode("Deposit mail", MAILBOX_FULL, result);

	// Write to invalid mailbox
	result = sys_deposit_mail(1, dest, msg);
	assertStatusCode("Deposit mail", MAILBOX_NOT_FOUND, result);

	// Remove mailbox
	result = sys_remove_mailbox(mboxd);
	assertStatusCode("Remove mailbox", OK, result);
}

void testReaderWriter() {
	int mboxd;

	// Create mailbox
	int result = sys_create_mailbox(&mboxd);
	assertStatusCode("Create mailbox", OK, result);
	printf("Created mailbox: %d\n", mboxd);

	pid_t reader[MAX_MAIL_DESTS], reader2[MAX_MAIL_DESTS], writer[NUM_WRITERS];

	char mboxd_str[16];
	char num_iter_str_writer[16];
	char num_iter_str_reader[16];

	sprintf(mboxd_str, "%d", mboxd);
	sprintf(num_iter_str_writer, "%d", NUM_ITER);
	sprintf(num_iter_str_reader, "%d", NUM_ITER * 2);

	// Create readers
	for(int i = 0; i < MAX_MAIL_DESTS; i++) {
		reader[i] = fork();
		if(reader[i] == 0) {
			execl("test_reader", "test_reader", mboxd_str, "1", num_iter_str_reader, 0);
		}
	}

	char max_mail_dests1_str[16];
	char max_mail_dests2_str[16];
	char reader_0_str[16];
	char reader_1_str[16];
	char reader_2_str[16];
	char reader_3_str[16];

	sprintf(max_mail_dests1_str, "%d", MAX_MAIL_DESTS);
	sprintf(max_mail_dests2_str, "%d", MAX_MAIL_DESTS/2);
	sprintf(reader_0_str, "%d", reader[0]);
	sprintf(reader_1_str, "%d", reader[1]);
	sprintf(reader_2_str, "%d", reader[2]);
	sprintf(reader_3_str, "%d", reader[3]);

	// Create writers
	for(int i = 0; i < NUM_WRITERS; i++) {
		writer[i] = fork();
		if(writer[i] == 0) {
			if(i % 2 == 0) {
				execl("test_writer", "test_writer", mboxd_str, "1", num_iter_str_writer, max_mail_dests1_str,
						reader_0_str, reader_1_str, reader_2_str, reader_3_str, 0);
			} else {
				// Alternate writers send message to only half of the readers
				execl("test_writer", "test_writer", mboxd_str, "1", num_iter_str_writer, max_mail_dests2_str,
						reader_0_str, reader_1_str, 0);
			}
		}
	}

	int reader_status, writer_status;

	// Wait for writers to finish
	for(int i = 0; i < NUM_WRITERS; i++) {
		waitpid(writer[i], &writer_status, 0);
		printf("Writer finished executing: %d\n", writer[i]);
	}

	// Wait for readers to finish
	for(int i = 0; i < MAX_MAIL_DESTS; i++) {
		waitpid(reader[i], &reader_status, 0);
		printf("Reader finished executing: %d\n", reader[i]);
	}

	// Remove mailbox
	result = sys_remove_mailbox(mboxd);
	assertStatusCode("Remove mailbox", OK, result);
}

void cleanup() {
	// Remove all mailboxes
	int result;
	int mboxd = 0;
	do {
		result = sys_remove_mailbox(mboxd++);
	} while (result == OK && mboxd < 16);
}

void assertStatusCode(char *msg, int expected, int actual) {
	char *result;
	if(expected == actual) {
		printf("%s: expected (%d), actual (%d). [%s]\n", msg, expected, actual, "Success");
	} else {
		printf("%s: expected (%d), actual (%d). [%s]\n", msg, expected, actual, "Fail");
		exit(0);
	}
}
