/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _BOX_3_H_RPCGEN
#define _BOX_3_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct dimensions {
	float length;
	float width;
	float height;
};
typedef struct dimensions dimensions;

struct box_results {
	float volume;
	float surface;
};
typedef struct box_results box_results;

struct mail_dims {
	float volume;
	float mass;
};
typedef struct mail_dims mail_dims;

#define RPC_BOX 0x40049453
#define BOXVERSION2 2

#if defined(__STDC__) || defined(__cplusplus)
#define BOX_CALC 1
extern  box_results * box_calc_2(dimensions *, CLIENT *);
extern  box_results * box_calc_2_svc(dimensions *, struct svc_req *);
#define MAIL_CALC 2
extern  float * mail_calc_2(mail_dims *, CLIENT *);
extern  float * mail_calc_2_svc(mail_dims *, struct svc_req *);
extern int rpc_box_2_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define BOX_CALC 1
extern  box_results * box_calc_2();
extern  box_results * box_calc_2_svc();
#define MAIL_CALC 2
extern  float * mail_calc_2();
extern  float * mail_calc_2_svc();
extern int rpc_box_2_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_dimensions (XDR *, dimensions*);
extern  bool_t xdr_box_results (XDR *, box_results*);
extern  bool_t xdr_mail_dims (XDR *, mail_dims*);

#else /* K&R C */
extern bool_t xdr_dimensions ();
extern bool_t xdr_box_results ();
extern bool_t xdr_mail_dims ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_BOX_3_H_RPCGEN */
