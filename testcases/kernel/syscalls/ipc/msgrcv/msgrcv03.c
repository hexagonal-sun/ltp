/*
 *
 *   Copyright (c) International Business Machines  Corp., 2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * NAME
 *	msgrcv03.c
 *
 * DESCRIPTION
 *	msgrcv03 - test for EINVAL error
 *
 * ALGORITHM
 *	create a message queue with read/write permissions
 *	loop if that option was specified
 *	call msgrcv() using two different invalid cases
 *	check the errno value
 *	  issue a PASS message if we get EINVAL
 *	otherwise, the tests fails
 *	  issue a FAIL message
 *	call cleanup
 *
 * USAGE:  <for command-line>
 *  msgrcv03 [-c n] [-e] [-i n] [-I x] [-P x] [-t]
 *     where,  -c n : Run n copies concurrently.
 *             -e   : Turn on errno logging.
 *	       -i n : Execute test n times.
 *	       -I x : Execute test for x seconds.
 *	       -P x : Pause for x seconds between iterations.
 *	       -t   : Turn on syscall timing.
 *
 * HISTORY
 *	03/2001 - Written by Wayne Boyer
 *
 * RESTRICTIONS
 *	none
 */

#include "test.h"

#include "ipcmsg.h"

void cleanup(void);
void setup(void);

char *TCID = "msgrcv03";
int TST_TOTAL = 2;

int msg_q_1 = -1;		/* The message queue id created in setup */
int bad_q = -1;			/* a value to use as a bad queue ID */
MSGBUF rcv_buf;

struct test_case_t {
	int *queue_id;
	int msize;
	int error;
} TC[] = {
	/* EINVAL - the queue ID is invalid */
	{
	&bad_q, MSGSIZE, EINVAL},
	    /* EINVAL - the message size is less than 0 */
	{
	&msg_q_1, -1, EINVAL}
};

int main(int ac, char **av)
{
	int lc;
	int i;

	tst_parse_opts(ac, av, NULL, NULL);

	setup();		/* global setup */

	/* The following loop checks looping state if -i option given */

	for (lc = 0; TEST_LOOPING(lc); lc++) {
		/* reset tst_count in case we are looping */
		tst_count = 0;

		for (i = 0; i < TST_TOTAL; i++) {

			/*
			 * Use the TEST macro to make the call
			 */

			TEST(msgrcv(*(TC[i].queue_id), &rcv_buf, TC[i].msize,
				    1, 0));

			if (TEST_RETURN != -1) {
				tst_resm(TFAIL, "call succeeded unexpectedly");
				continue;
			}

			if (TEST_ERRNO == TC[i].error) {
				tst_resm(TPASS, "expected failure - errno = "
					 "%d : %s", TEST_ERRNO,
					 strerror(TEST_ERRNO));
			} else {
				tst_resm(TFAIL, "call failed with an "
					 "unexpected error - %d : %s",
					 TEST_ERRNO, strerror(TEST_ERRNO));
			}
		}
	}

	cleanup();

	tst_exit();
}

/*
 * setup() - performs all the ONE TIME setup for this test.
 */
void setup(void)
{

	tst_sig(NOFORK, DEF_HANDLER, cleanup);

	TEST_PAUSE;

	/*
	 * Create a temporary directory and cd into it.
	 * This helps to ensure that a unique msgkey is created.
	 * See libs/libltpipc/libipc.c for more information.
	 */
	tst_tmpdir();

	msgkey = getipckey();

	/* create a message queue with read/write permission */
	if ((msg_q_1 = msgget(msgkey, IPC_CREAT | IPC_EXCL | MSG_RW)) == -1) {
		tst_brkm(TBROK, cleanup, "Can't create message queue");
	}
}

/*
 * cleanup() - performs all the ONE TIME cleanup for this test at completion
 * 	       or premature exit.
 */
void cleanup(void)
{
	/* if it exists, remove the message queue that was created */
	rm_queue(msg_q_1);

	tst_rmdir();

}
