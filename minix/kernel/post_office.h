/*
 * post_office.h - Declares various functions for mailbox.
 *
 *  Created on: Oct 13, 2015
 *      Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 */

#ifndef POST_OFFICE_H_
#define POST_OFFICE_H_

#include <minix/type.h>

#define MAX_MAILBOXES 16
#define MAX_MAILBOX_MESSAGES 16
#define MAX_MAIL_DESTS 4
#define MAX_MAIL_PAYLOAD_SIZE 32

// Status of mailbox and message slots
#define OK 0
#define FALSE 0
#define TRUE 1
#define INVALID -1
#define START 0
#define EMPTY -1

/*
 * Structure for message stored in mailbox
 */
typedef struct mail_message {
	int order_number;
	endpoint_t src; // Source process id
	endpoint_t dest[MAX_MAIL_DESTS]; // Destination process ids
	char msg[32]; // Message data defined in ipc.h. This is the message sent by user process.
} mail_message;

/*
 * Structure for mail-box
 */
typedef struct mailbox {
	int num_messages; // Number of messages queued in mailbox currently
	mail_message msg[MAX_MAILBOX_MESSAGES]; // Messages
	int message_empty[MAX_MAILBOX_MESSAGES]; 
	void *data; // Extra data for mailbox
} mailbox;

/*
 * Structure for post-office
 */
typedef struct post_office {
	int num_mailboxes; // Number of mailboxes in post office currently
	mailbox mbox[MAX_MAILBOXES]; // Mailboxes
	int mbox_availability[MAX_MAILBOXES]; // Array used to indicate whether mbox at corresponding index in the mbox array is free/in use
	void *data; // Extra data for post office
} post_office;

/**
 * Initializes post office. This can be invoked from kmain() in /usr/src/minix/kernel/main.c
 */
int init_post_office();

/**
 * Creates a new mailbox and stores a corresponding mailbox descriptor in mboxd.
 */
int create_mailbox(int *mboxd);

/**
 * Deposits a message msg into mailbox having given descriptor mboxd, which is sent by process
 * src_pid to processes dest_pid.
 */
int deposit_mail(int mboxd, endpoint_t src, endpoint_t dest[], char *msg);

/**
 * Retrieves a message msg from mailbox having given descriptor mboxd, which is sent to the given
 * process dest_pid.
 */
int retrieve_mail(int mboxd, endpoint_t dest, char *msg);

/**
 * Returns the number of messages in mailbox having given descriptor, into an integer pointed by
 * count.
 */
int get_mail_count(int mboxd, int *count);

/**
 * Removes the mailbox with given descriptor.
 */
int remove_mailbox(int mboxd);

/**
 * Destroy post office. Need to find where to invoke this from.
 */
int destroy_post_office();

#endif /* POST_OFFICE_H_ */
