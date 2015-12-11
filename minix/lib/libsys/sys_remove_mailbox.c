/*
 * sys_remove_mailbox.c - Implements remove mailbox system call.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include "syslib.h"

int sys_remove_mailbox(int mboxd) {
	message m;
	m.m_mailbox_remove.mboxd = mboxd;

	return(_kernel_call(SYS_REMOVE_MAILBOX, &m));
}
