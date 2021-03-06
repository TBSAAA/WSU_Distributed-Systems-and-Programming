/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "box_3.h"

box_results *
box_calc_2_svc(dimensions *argp, struct svc_req *rqstp) {
    static box_results result;
    int l = (int) argp->length, h = (int) argp->height, w = (int) argp->width;

    if (l % 3 != 0)
        l = l / 3 + 1;
    else
        l = l / 3;

    if (h % 3 != 0)
        h = h / 3 + 1;
    else
        h = h / 3;

    if (w % 3 != 0)
        w = w / 3 + 1;
    else
        w = w / 3;

    result.surface = 18 * (l * h + h * w + l * w);
    result.volume = 27 * l * h * w;

    return &result;
}

float *
mail_calc_2_svc(mail_dims *argp, struct svc_req *rqstp) {
    static float result;

    if (argp->volume == 0 || argp->mass == 0) {
        result = 0;
        return &result;
    }

    if (argp->volume < argp->mass * 9) {
        result = 3 * argp->volume;
    } else {
        result = 18 * argp->mass;
    }

    return &result;
}
