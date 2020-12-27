/**
 * @file vidisp.h
 * @brief Video Display
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAVIDISP_H_INCLUDED
#define UAVIDISP_H_INCLUDED

#include "rsua-re/re.h"
#include "rsua-rem/rem.h"

struct vidisp;
struct vidisp_st;
struct vidrect;
struct vidsz;

/** Video Display parameters */
struct vidisp_prm {
	bool fullscreen;  /**< Enable fullscreen display                    */
};

typedef void (vidisp_resize_h)(const struct vidsz *size, void *arg);

typedef int  (vidisp_alloc_h)(struct vidisp_st **vp,
			      const struct vidisp *vd, struct vidisp_prm *prm,
			      const char *dev,
			      vidisp_resize_h *resizeh, void *arg);
typedef int  (vidisp_update_h)(struct vidisp_st *st, bool fullscreen,
			       int orient, const struct vidrect *window);
typedef int  (vidisp_disp_h)(struct vidisp_st *st, const char *title,
			     const struct vidframe *frame, uint64_t timestamp);
typedef void (vidisp_hide_h)(struct vidisp_st *st);

/** Defines a Video display */
struct vidisp {
	struct le        le;
	const char      *name;
	vidisp_alloc_h  *alloch;
	vidisp_update_h *updateh;
	vidisp_disp_h   *disph;
	vidisp_hide_h   *hideh;
};

int vidisp_register(struct vidisp **vp, struct list *vidispl, const char *name,
		    vidisp_alloc_h *alloch, vidisp_update_h *updateh,
		    vidisp_disp_h *disph, vidisp_hide_h *hideh);
int vidisp_alloc(struct vidisp_st **stp, struct list *vidispl,
		 const char *name,
		 struct vidisp_prm *prm, const char *dev,
		 vidisp_resize_h *resizeh, void *arg);
int vidisp_display(struct vidisp_st *st, const char *title,
		   const struct vidframe *frame, uint64_t timestamp);
const struct vidisp *vidisp_find(const struct list *vidispl, const char *name);
struct vidisp *vidisp_get(struct vidisp_st *st);

#endif /* UAVIDISP_H_INCLUDED */
