/*
 * sys_deposit_mail.c - Implements deposit mail system call.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include <string.h>
#include "syslib.h"

#define MAX_MAIL_DESTS 4
#define MAX_MAIL_PAYLOAD_SIZE 32
#define EMPTY -1

int sys_deposit_mail(int mboxd, pid_t dest[], char *msg) {
	message m;
	m.m_mail_deposit.mboxd = mboxd;
	for(int i = 0; i < MAX_MAIL_DESTS; i++) {
		if(dest[i] == EMPTY) {
			m.m_mail_deposit.dest[i] = dest[i];
			break;
		}

		// Convert pid to endpoint
		endpoint_t endpoint;
		int status = sys_pid_to_endpoint(dest[i], &endpoint);
		if(status != OK) {
			endpoint = EMPTY;
		}
		m.m_mail_deposit.dest[i] = endpoint;
	}
	memcpy(m.m_mail_deposit.msg, msg, MAX_MAIL_PAYLOAD_SIZE);
	return(_kernel_call(SYS_DEPOSIT_MAIL, &m));
}