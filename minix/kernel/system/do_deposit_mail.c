/*
 * do_deposit_mail.c - Implements deposit mail kernel call.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include "kernel/system.h"
#include "kernel/post_office.h"
#include <minix/endpoint.h>

int do_deposit_mail(struct proc *caller_prt, message *m_ptr) {
	return(deposit_mail(m_ptr->m_mail_deposit.mboxd, caller_prt->p_endpoint,
			m_ptr->m_mail_deposit.dest, m_ptr->m_mail_deposit.msg));
}
