/*
 * sys_create_mailbox.c - Implements create mailbox system call.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include "syslib.h"

int sys_create_mailbox(int *mboxd) {
	message m;
	int status = _kernel_call(SYS_CREATE_MAILBOX, &m);
	if(status == OK) {
		*mboxd = m.m_mailbox_create.mboxd;
	}

	return status;
}
