/*
 * do_create_mailbox.c - Implements create mailbox kernel call.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include "kernel/system.h"
#include "kernel/post_office.h"
#include <minix/endpoint.h>
#include <minix/sysutil.h>

int do_create_mailbox(struct proc *caller_prt, message *m_ptr) {
	return create_mailbox(&(m_ptr->m_mailbox_create.mboxd));
}
