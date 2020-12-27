/**
 * @file vidsrc.h
 * @brief Video Source
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAVIDSRC_H_INCLUDED
#define UAVIDSRC_H_INCLUDED

#include "rsua-re/re.h"
#include "rsua-rem/rem.h"

struct media_ctx;
struct vidrect;
struct vidsrc;
struct vidsrc_st;
struct vidsz;

/** Video Source parameters */
struct vidsrc_prm {
	double fps;       /**< Wanted framerate                            */
	int fmt;          /**< Wanted pixel format (enum vidfmt)           */
};

/**
 * Provides video frames to the core
 *
 * @param frame     Video frame
 * @param timestamp Frame timestamp in VIDEO_TIMEBASE units
 * @param arg       Handler argument
 */
typedef void (vidsrc_frame_h)(struct vidframe *frame, uint64_t timestamp,
			      void *arg);
typedef void (vidsrc_error_h)(int err, void *arg);

typedef int  (vidsrc_alloc_h)(struct vidsrc_st **vsp, const struct vidsrc *vs,
			      struct media_ctx **ctx, struct vidsrc_prm *prm,
			      const struct vidsz *size,
			      const char *fmt, const char *dev,
			      vidsrc_frame_h *frameh,
			      vidsrc_error_h *errorh, void *arg);

typedef void (vidsrc_update_h)(struct vidsrc_st *st, struct vidsrc_prm *prm,
			       const char *dev);

/** Defines a video source */
struct vidsrc {
	struct le         le;
	const char       *name;
	struct list      dev_list;
	vidsrc_alloc_h   *alloch;
	vidsrc_update_h  *updateh;
};

int vidsrc_register(struct vidsrc **vp, struct list *vidsrcl, const char *name,
		    vidsrc_alloc_h *alloch, vidsrc_update_h *updateh);
const struct vidsrc *vidsrc_find(const struct list *vidsrcl, const char *name);
int vidsrc_alloc(struct vidsrc_st **stp, struct list *vidsrcl,
		 const char *name,
		 struct media_ctx **ctx, struct vidsrc_prm *prm,
		 const struct vidsz *size, const char *fmt, const char *dev,
		 vidsrc_frame_h *frameh, vidsrc_error_h *errorh, void *arg);
struct vidsrc *vidsrc_get(struct vidsrc_st *st);

#endif /* UAVIDSRC_H_INCLUDED */
