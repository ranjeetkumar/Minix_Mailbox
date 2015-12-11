/*
 * sys_pid_to_endpoint.c - Implements pid to endpoint library function.
 *
 *  Created on: Oct 13, 2015
 *	  Author: Aman Mishra, Ranjeet Kumar, Shruti Kasetty
 *
 */

#include <minix/procfs.h>
#include <string.h>
#include "syslib.h"

struct pstat { /* structure filled by pstat() */
	struct pstat *ps_next; /* next in process list */
	int ps_task; /* is this process a task or not? */
	int ps_endpt; /* process endpoint (NONE means unused slot) */
	dev_t ps_dev; /* major/minor of controlling tty */
	uid_t ps_ruid; /* real uid */
	uid_t ps_euid; /* effective uid */
	pid_t ps_pid; /* process id */
	pid_t ps_ppid; /* parent process id */
	int ps_pgrp; /* process group id */
	char ps_state; /* process state */
	char ps_pstate; /* sleep state */
	char ps_fstate; /* VFS block state */
	int ps_ftask; /* VFS suspend task (endpoint) */
	vir_bytes ps_memory; /* memory usage */
	int ps_recv; /* process number to receive from (endpoint) */
	unsigned int ps_utime; /* accumulated user time */
	unsigned int ps_stime; /* accumulated system time */
	char ps_name[PROC_NAME_LEN + 1];/* process name */
	char *ps_args; /* concatenated argument string */
};

int pstat(struct pstat *ps, pid_t pid);

int sys_pid_to_endpoint(pid_t pid, endpoint_t *endpoint) {
	struct pstat ps;
	int result = pstat(&ps, pid);
	if(result == OK) {
		*endpoint = ps.ps_endpt;
	}
	return result;
}

int pstat(struct pstat *ps, pid_t pid) {
	FILE *fp;
	int version, ruid, euid, dev;
	char type, path[PATH_MAX], name[256];

	ps->ps_pid = pid;
	ps->ps_next = NULL;

	sprintf(path, "/proc/%d/psinfo", pid);

	if ((fp = fopen(path, "r")) == NULL)
		return -1;

	if (fscanf(fp, "%d", &version) != 1) {
		fclose(fp);
		return -1;
	}

	/* The psinfo file's version must match what we expect. */
	if (version != PSINFO_VERSION) {
		fputs("procfs version mismatch!\n", stderr);
		exit(1);
	}

	if (fscanf(fp, " %c %d %255s %c %d %*d %u %u %*u %*u", &type, &ps->ps_endpt,
			name, &ps->ps_state, &ps->ps_recv, &ps->ps_utime, &ps->ps_stime)
			!= 7) {

		fclose(fp);
		return -1;
	}

	strncpy(ps->ps_name, name, sizeof(ps->ps_name) - 1);
	ps->ps_name[sizeof(ps->ps_name) - 1] = 0;

	ps->ps_task = type == TYPE_TASK;

	if (!ps->ps_task) {
		if (fscanf(fp, " %lu %*u %*u %c %d %u %u %u %*d %c %d %u",
				&ps->ps_memory, &ps->ps_pstate, &ps->ps_ppid, &ruid, &euid,
				&ps->ps_pgrp, &ps->ps_fstate, &ps->ps_ftask, &dev) != 9) {

			fclose(fp);
			return -1;
		}

		ps->ps_ruid = ruid;
		ps->ps_euid = euid;
		ps->ps_dev = dev;
	} else {
		ps->ps_memory = 0L;
		ps->ps_pstate = PSTATE_NONE;
		ps->ps_ppid = 0;
		ps->ps_ruid = 0;
		ps->ps_euid = 0;
		ps->ps_pgrp = 0;
		ps->ps_fstate = FSTATE_NONE;
		ps->ps_ftask = NONE;
		ps->ps_dev = NO_DEV;
	}

	fclose(fp);

	if (ps->ps_state == STATE_ZOMBIE)
		ps->ps_args = "<defunct>";
	else
		ps->ps_args = NULL;

	return OK;
}
