/**
 * @file auplay.h
 * @brief Audio Player
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAAUPLAY_H_INCLUDED
#define UAAUPLAY_H_INCLUDED

#include "rsua-re/re.h"

struct auplay;
struct auplay_st;

/** Audio Player parameters */
struct auplay_prm {
	uint32_t   srate;       /**< Sampling rate in [Hz]      */
	uint8_t    ch;          /**< Number of channels         */
	uint32_t   ptime;       /**< Wanted packet-time in [ms] */
	int        fmt;         /**< Sample format (enum aufmt) */
};

typedef void (auplay_write_h)(void *sampv, size_t sampc, void *arg);

typedef int  (auplay_alloc_h)(struct auplay_st **stp, const struct auplay *ap,
			      struct auplay_prm *prm, const char *device,
			      auplay_write_h *wh, void *arg);

/** Defines an Audio Player */
struct auplay {
	struct le        le;
	const char      *name;
	struct list      dev_list;
	auplay_alloc_h  *alloch;
};

int auplay_register(struct auplay **pp, struct list *auplayl,
		    const char *name, auplay_alloc_h *alloch);
const struct auplay *auplay_find(const struct list *auplayl, const char *name);
int auplay_alloc(struct auplay_st **stp, struct list *auplayl,
		 const char *name,
		 struct auplay_prm *prm, const char *device,
		 auplay_write_h *wh, void *arg);
struct auplay *auplay_get(struct auplay_st *st);


#ifndef UAMODAPI_USE		/* Internal API */

struct auplay_st {
	struct auplay *ap;
};

#endif /* ifndef UAMODAPI_USE */

#endif /* UAAUPLAY_H_INCLUDED */
