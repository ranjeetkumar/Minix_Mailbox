/*
 * post_office.c - Implements various functions for mailbox.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include <sys/errno.h>
#include <minix/sysutil.h>
#include <string.h>
#include "post_office.h"

post_office po; // This will allocate memory for the post office, mailboxes, and messages

// Internal functions used by post office
int garbage_collector(int mboxd);
int message_index_minorder(int mboxd, int dest_pid);
int reordering(int order_number, int mboxd);
int min(int num1, int num2);

/**
 * Initializes post office.
 */
int init_post_office() {
	// Initialize number of used mail box slots in post office to 0
	po.num_mailboxes = 0;
	// Initialize number of used messages and availability flag in each mailbox of the post office
	for (int i = 0; i < MAX_MAILBOXES; i++) {
		po.mbox[i].num_messages = 0;
		po.mbox_availability[i] = TRUE; // set availability of mailbox to TRUE
	}
	printf("Post office initialized\n");
	return OK;
}

/**
 * Creates/allocates a new mailbox and stores/returns a corresponding mailbox descriptor in mboxd.
 */
int create_mailbox(int *mboxd) {
	// Check if a new mail box can be created i.e. post office is not full
	if(po.num_mailboxes == MAX_MAILBOXES) {
		*mboxd = -1;
		printf("Post office is full. Cannot create mailbox\n");
		return POST_OFFICE_FULL;
	}

	for (int i = 0; i < MAX_MAILBOXES; i++) {
		// See if mbox at index is available; return it as new mbox descriptor
		if (po.mbox_availability[i] == TRUE) {
			*mboxd = i;
			po.mbox_availability[i] = FALSE; // make mailbox slot unavailable
			po.num_mailboxes++; // Increment number of mailboxes in the post office

			// Initialize message slots in new mailbox
			for(int j=0; j < MAX_MAILBOX_MESSAGES; j++) {
				po.mbox[i].message_empty[j] = TRUE;
				po.mbox[i].msg[j].order_number = START;
				for (int k=0; k < MAX_MAIL_DESTS; k++) {
					po.mbox[i].msg[j].dest[k] = EMPTY;
				}
			}
			break;
		}
	}

	printf("Mailbox created: %d\n", *mboxd);

	return OK;
}

/**
 * Deposits a message msg into mailbox having given descriptor mboxd, which is sent by process
 * src to processes dest.
 */
int deposit_mail(int mboxd, endpoint_t src, endpoint_t dest[], char *msg) {
	// Check if mailbox is valid
	if(mboxd < 0 || mboxd >= MAX_MAILBOXES || po.mbox_availability[mboxd] == TRUE) {
		return MAILBOX_NOT_FOUND;
	}
	
	int num_messages = po.mbox[mboxd].num_messages;
	if (num_messages >= MAX_MAILBOX_MESSAGES) {
		return MAILBOX_FULL;	  
	} else {
		int vacant_position;
		for(int i = 0; i < MAX_MAILBOX_MESSAGES; i++) {
			if (po.mbox[mboxd].message_empty[i] == TRUE) {
				vacant_position = i;
				break;
			}
		}
		int max_order_number = START;
		for(int i= 0; i < MAX_MAILBOX_MESSAGES; i++) {  
			if(po.mbox[mboxd].msg[i].order_number > max_order_number)
			 max_order_number = po.mbox[mboxd].msg[i].order_number;
		}
		max_order_number = max_order_number + 1;	  	
		memcpy(po.mbox[mboxd].msg[vacant_position].msg, msg, MAX_MAIL_PAYLOAD_SIZE);
		po.mbox[mboxd].msg[vacant_position].src = src;
		po.mbox[mboxd].message_empty[vacant_position] = FALSE;
		po.mbox[mboxd].msg[vacant_position].order_number = max_order_number;
		for(int i = 0; i < MAX_MAIL_DESTS;i++) {
			po.mbox[mboxd].msg[vacant_position].dest[i] = dest[i];
		}	  
		num_messages = num_messages + 1;
		po.mbox[mboxd].num_messages = num_messages; 
		return OK;
	}
}

/**
 * Retrieves a message msg from mailbox having given descriptor mboxd, which is sent to the given
 * process dest.
 */
int retrieve_mail(int mboxd, endpoint_t dest, char *msg) {
	// Check if mailbox is valid
	if(mboxd < 0 || mboxd >= MAX_MAILBOXES || po.mbox_availability[mboxd] == TRUE) {
		return MAILBOX_NOT_FOUND;
	}
	
	if(po.mbox[mboxd].num_messages == 0) {
		return MAILBOX_EMPTY;		
	}

	int message_index = message_index_minorder(mboxd, dest);

	if (message_index == INVALID) {
		return MAILBOX_NO_MESSAGE;
	} else {
		for(int j = 0; j <MAX_MAIL_DESTS; j++) {
			if(po.mbox[mboxd].msg[message_index].dest[j] == dest) {
				memcpy(msg, po.mbox[mboxd].msg[message_index].msg, MAX_MAIL_PAYLOAD_SIZE);
				po.mbox[mboxd].msg[message_index].dest[j] = EMPTY;
				garbage_collector(mboxd);
			}
		}
	}
	return OK;
}

/**
 * Returns the number of messages in mailbox having given descriptor, into an integer pointed by
 * count.
 */
int get_mail_count(int mboxd, int *count) {
	// Check if mailbox is valid
	if(mboxd < 0 || mboxd >= MAX_MAILBOXES || po.mbox_availability[mboxd] == TRUE) {
		return MAILBOX_NOT_FOUND;
	}

	return po.mbox[mboxd].num_messages;
}


/**
 * Removes the mailbox with given descriptor.
 */
int remove_mailbox(int mboxd) {
	// Index of mailbox mentioned does not exist in the post office
	if (mboxd < 0 || mboxd >= MAX_MAILBOXES || po.mbox_availability[mboxd] == TRUE) {
		printf("Mailbox not found: %d\n", mboxd);
		return MAILBOX_NOT_FOUND;
	}

	// mailbox is valid and in use
	// Reset number of messages in that mailbox to be zero
	// Other fields will be overwritten by subsequent mailboxes that use this mailbox slot
	po.mbox[mboxd].num_messages = 0; 

	po.mbox_availability[mboxd] = TRUE;
	po.num_mailboxes--;

	printf("Mailbox removed: %d\n", mboxd);
	return OK;
}

/**
 * Destroy post office. Need to find where to invoke this from.
 */
int destroy_post_office() {
	// Set all variables to some invalid value to indicate destruction
	// Initialize number of used mail box slots in post office to invalid value
	po.num_mailboxes = INVALID;
	// Initialize number of message slots in each mailbox of the post office to invalid value
	for (int i = 0; i < MAX_MAILBOXES; i++) {
		po.mbox[i].num_messages = INVALID;
		po.mbox_availability[i] = INVALID; // Set availability of mailbox to invalid value
	}

	printf("Post office destroyed\n");
	return OK;
}

// Implementation of internal functions used by post office
int message_index_minorder(int mboxd, int dest) { 
	int order_number;
	int min_order_number = 100;
	int message_index = INVALID;
	for(int i = 0; i < MAX_MAILBOX_MESSAGES; i++) {
		order_number = po.mbox[mboxd].msg[i].order_number;
		if(po.mbox[mboxd].message_empty[i] == FALSE) {
			for(int j = 0; j<MAX_MAIL_DESTS; j++) {
				if(po.mbox[mboxd].msg[i].dest[j] == dest) {
					if(min_order_number > min(min_order_number, order_number)) { 
						message_index = i;
						min_order_number = min(min_order_number, order_number);
					}
				}
			}
		}
	}
	return message_index;
}

int min(int num1, int num2) {
	int min;
	if(num1 >= num2)
		min = num2;
	else
		min = num1;
	return min;
}

int garbage_collector(int mboxd) {
	int all_empty = TRUE;
	for(int i = 0; i < MAX_MAILBOX_MESSAGES; i++) {
		if(po.mbox[mboxd].message_empty[i] == FALSE) {
			for(int j = 0; j<MAX_MAIL_DESTS; j++) { 
				if (po.mbox[mboxd].msg[i].dest[j] != EMPTY) {
					all_empty = 0;		
				}
			}
			if(all_empty == TRUE) {
				po.mbox[mboxd].message_empty[i] = TRUE;
				po.mbox[mboxd].num_messages -= 1;
				reordering(po.mbox[mboxd].msg[i].order_number, mboxd);
			}
		}
	}
	return OK; 
}

int reordering(int order_number, int mboxd) {
	for(int i = 0; i < MAX_MAILBOX_MESSAGES; i++) {
		if(po.mbox[mboxd].message_empty[i] == FALSE) {
			if(po.mbox[mboxd].msg[i].order_number > order_number) {			  
				po.mbox[mboxd].msg[i].order_number -= 1;
			}
		}
	}
	return OK;
}