/**
 * @file ausrc.h
 * @brief Audio Source
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAAUSRC_H_INCLUDED
#define UAAUSRC_H_INCLUDED

#include "rsua-re/re.h"

struct auframe;
struct ausrc;
struct ausrc_st;
struct media_ctx;

/** Audio Source parameters */
struct ausrc_prm {
	uint32_t   srate;       /**< Sampling rate in [Hz]      */
	uint8_t    ch;          /**< Number of channels         */
	uint32_t   ptime;       /**< Wanted packet-time in [ms] */
	int        fmt;         /**< Sample format (enum aufmt) */
};

typedef void (ausrc_read_h)(struct auframe *af, void *arg);
typedef void (ausrc_error_h)(int err, const char *str, void *arg);

typedef int  (ausrc_alloc_h)(struct ausrc_st **stp, const struct ausrc *ausrc,
			     struct media_ctx **ctx,
			     struct ausrc_prm *prm, const char *device,
			     ausrc_read_h *rh, ausrc_error_h *errh, void *arg);

/** Defines an Audio Source */
struct ausrc {
	struct le        le;
	const char      *name;
	struct list      dev_list;
	ausrc_alloc_h   *alloch;
};

int ausrc_register(struct ausrc **asp, struct list *ausrcl, const char *name,
		   ausrc_alloc_h *alloch);
const struct ausrc *ausrc_find(const struct list *ausrcl, const char *name);
int ausrc_alloc(struct ausrc_st **stp, struct list *ausrcl,
		struct media_ctx **ctx,
		const char *name,
		struct ausrc_prm *prm, const char *device,
		ausrc_read_h *rh, ausrc_error_h *errh, void *arg);
struct ausrc *ausrc_get(struct ausrc_st *st);


#ifndef UAMODAPI_USE		/* Internal API */

struct ausrc_st {
	const struct ausrc *as;
};

#endif /* ifndef UAMODAPI_USE */

#endif /* UAAUSRC_H_INCLUDED */
