/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "nfs.h"

struct TEST *
test_proc_2_svc(struct TEST *argp, struct svc_req *rqstp)
{
	static struct TEST  result;

	/*
	 * insert server code here
	 */

	return &result;
}