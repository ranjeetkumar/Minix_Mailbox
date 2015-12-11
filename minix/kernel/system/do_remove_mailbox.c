/*
 * do_remove_mailbox.c - Implements remove mailbox kernel call.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include "kernel/system.h"
#include "kernel/post_office.h"
#include <minix/endpoint.h>

int do_remove_mailbox(struct proc *caller_prt, message *m_ptr) {
	return(remove_mailbox(m_ptr->m_mailbox_remove.mboxd));
}
