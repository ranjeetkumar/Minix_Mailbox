/*
 * sys_retrieve_mail.c - Implements retrieve mail system call.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include <string.h>
#include "syslib.h"

#define MAX_MAIL_PAYLOAD_SIZE 32

int sys_retrieve_mail(int mboxd, char *msg) {
	message m;
	m.m_mail_retrieve.mboxd = mboxd;

	int status = _kernel_call(SYS_RETRIEVE_MAIL, &m);
	if(status == OK) {
		memcpy(msg, m.m_mail_retrieve.msg, MAX_MAIL_PAYLOAD_SIZE);
	}

	return status;
}
