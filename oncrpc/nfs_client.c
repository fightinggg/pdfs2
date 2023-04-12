/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "nfs.h"


void
test_prog_2(char *host)
{
	CLIENT *clnt;
	struct TEST  *result_1;
	struct TEST  test_proc_2_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, TEST_PROG, TEST_VER, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = test_proc_2(&test_proc_2_arg, clnt);
	if (result_1 == (struct TEST *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	test_prog_2 (host);
exit (0);
}