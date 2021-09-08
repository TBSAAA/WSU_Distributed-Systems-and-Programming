/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "box_3.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
rpc_box_2(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		dimensions box_calc_2_arg;
		mail_dims mail_calc_2_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case BOX_CALC:
		_xdr_argument = (xdrproc_t) xdr_dimensions;
		_xdr_result = (xdrproc_t) xdr_box_results;
		local = (char *(*)(char *, struct svc_req *)) box_calc_2_svc;
		break;

	case MAIL_CALC:
		_xdr_argument = (xdrproc_t) xdr_mail_dims;
		_xdr_result = (xdrproc_t) xdr_float;
		local = (char *(*)(char *, struct svc_req *)) mail_calc_2_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (RPC_BOX, BOXVERSION2);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, RPC_BOX, BOXVERSION2, rpc_box_2, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (RPC_BOX, BOXVERSION2, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, RPC_BOX, BOXVERSION2, rpc_box_2, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (RPC_BOX, BOXVERSION2, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
