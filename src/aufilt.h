/**
 * @file aufilt.h
 * @brief Audio Filter
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAAUFILT_H_INCLUDED
#define UAAUFILT_H_INCLUDED

#include "rsua-re/re.h"

struct audio;
struct aufilt;
struct auframe;

/* Base class */
struct aufilt_enc_st {
	const struct aufilt *af;
	struct le le;
};

struct aufilt_dec_st {
	const struct aufilt *af;
	struct le le;
};

/** Audio Filter Parameters */
struct aufilt_prm {
	uint32_t srate;       /**< Sampling rate in [Hz]        */
	uint8_t  ch;          /**< Number of channels           */
	int      fmt;         /**< Sample format (enum aufmt)   */
};

typedef int (aufilt_encupd_h)(struct aufilt_enc_st **stp, void **ctx,
			      const struct aufilt *af, struct aufilt_prm *prm,
			      const struct audio *au);
typedef int (aufilt_encode_h)(struct aufilt_enc_st *st,
			      struct auframe *af);

typedef int (aufilt_decupd_h)(struct aufilt_dec_st **stp, void **ctx,
			      const struct aufilt *af, struct aufilt_prm *prm,
			      const struct audio *au);
typedef int (aufilt_decode_h)(struct aufilt_dec_st *st,
			      struct auframe *af);

struct aufilt {
	struct le le;
	const char *name;
	aufilt_encupd_h *encupdh;
	aufilt_encode_h *ench;
	aufilt_decupd_h *decupdh;
	aufilt_decode_h *dech;
};

void aufilt_register(struct list *aufiltl, struct aufilt *af);
void aufilt_unregister(struct aufilt *af);

#endif /* UAAUFILT_H_INCLUDED */
