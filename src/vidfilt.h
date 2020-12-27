/**
 * @file vidfilt.h
 * @brief Video Filter
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAVIDFILT_H_INCLUDED
#define UAVIDFILT_H_INCLUDED

#include "rsua-re/re.h"

struct video;
struct vidfilt;
struct vidframe;

/* Base class */
struct vidfilt_enc_st {
	const struct vidfilt *vf;
	struct le le;
};

struct vidfilt_dec_st {
	const struct vidfilt *vf;
	struct le le;
};

/** Video Filter Parameters */
struct vidfilt_prm {
	unsigned width;   /**< Picture width              */
	unsigned height;  /**< Picture height             */
	int fmt;          /**< Pixel format (enum vidfmt) */
	double fps;       /**< Video framerate            */
};

typedef int (vidfilt_encupd_h)(struct vidfilt_enc_st **stp, void **ctx,
			       const struct vidfilt *vf,
			       struct vidfilt_prm *prm,
			       const struct video *vid);
typedef int (vidfilt_encode_h)(struct vidfilt_enc_st *st,
			       struct vidframe *frame, uint64_t *timestamp);

typedef int (vidfilt_decupd_h)(struct vidfilt_dec_st **stp, void **ctx,
			       const struct vidfilt *vf,
			       struct vidfilt_prm *prm,
			       const struct video *vid);
typedef int (vidfilt_decode_h)(struct vidfilt_dec_st *st,
			       struct vidframe *frame, uint64_t *timestamp);

struct vidfilt {
	struct le le;
	const char *name;
	vidfilt_encupd_h *encupdh;
	vidfilt_encode_h *ench;
	vidfilt_decupd_h *decupdh;
	vidfilt_decode_h *dech;
};

void vidfilt_register(struct list *vidfiltl, struct vidfilt *vf);
void vidfilt_unregister(struct vidfilt *vf);
int vidfilt_enc_append(struct list *filtl, void **ctx,
		       const struct vidfilt *vf, struct vidfilt_prm *prm,
		       const struct video *vid);
int vidfilt_dec_append(struct list *filtl, void **ctx,
		       const struct vidfilt *vf, struct vidfilt_prm *prm,
		       const struct video *vid);

#endif /* UAVIDFILT_H_INCLUDED */
