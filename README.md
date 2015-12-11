
o Test program description and execution:
=========================================
- Present in this bundle three main test programs which help us execute several tests. Following is a brief description of each file:
	1. test_reader.c: A reader program that takes in the mailbox ID "mboxd" to read from and two other arguments - interval (in seconds) and number_of_iterations
	Upon invocation, it executes in a loop and retrieves a message from mailbox "mboxd" every "interval" seconds for "number_of_iterations" times
	2. test_writer.c: A writer program that takes in the mailbox ID "mboxd" to write to and three other arguments - interval (in seconds), number_of_iterations, number_of_pids, 
	"receivers" list_of_pids to which messages are to be sent
	Upon invocation, it executes in a loop and deposits a generated message (text) into mailbox "mboxd", to all the "receivers" every "interval" seconds for "number_of_iterations" times.  
	3. test_all.c: Main test program that invokes new system IPC calls that we have implemented. It has the following functionality:
		a. function testMailbox()
			Tests the creation of mailbox through the sys_create_mailbox() system call and the removal of mailbox through the sys_remove_mailbox() system calls. We have implemented the notion of a "post office"
			where a post office can hold a maximum of 16 mailboxes. In other words, only 16 mailboxes can operate at any given time. This function tests for normal working of the sys_create_mailbox() 
			and sys_remove_mailbox() system calls and the following exception scenarios:
			
			* Creating more than 16 mailboxes in the post office: POST_OFFICE_FULL error code returned to user process
			* Deleting a mailbox whose ID is not known: MAILBOX_NOT_FOUND error code returned to user process
			
		b. function testReader()
			Tests the retrieve mail IPC system call implemented as sys_retrieve_mail() system call. This function tests for normal working of the sys_retrieve_mail() system call and the following exception scenarios:
			
			* Retrieving mail from a mailbox whose ID does not exist: MAILBOX_NOT_FOUND error code returned to user process
			* Retrieving mail from a mailbox that has no messages: MAILBOX_EMPTY error code returned
			* Retrieving mail from a mailbox that is not empty but has no message present for the concerned reader/user process: MAILBOX_NO_MESSAGE error code returned
			
		c. function testWriter()
			Tests the deposit mail IPC system call implemented as sys_deposit_mail() system call. This function tests for normal working of the sys_deposit_mail() system call and the following exception scenarios:
			
			* Sending mail to a mailbox whose ID does not exist: MAILBOX_NOT_FOUND error code returned to user process
			* Sending mail to a mailbox that has reached its max limit of 16 messages: MAILBOX_FULL error code returned
			
		d. function testReaderWriter()
			Forks several child processes each executing reader/writer code to check for multiple readers and multiple writers scenario to the same mailbox or a different mailbox
			
- Executing test programs:
We need to first compile the test programs as shown below:

# clang -o test_reader test_reader.c -lsys -ltimers
# clang -o test_writer test_writer.c -lsys -ltimers
# clang -o test_all test_all.c -lsys -ltimers

Executing the test_all program would exercise all the above mentioned scenarios. It is done as follows:

# ./test_all

			



