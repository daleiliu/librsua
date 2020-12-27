/**
 * @file custom_hdrs.h
 * @brief Custom headers
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UACUSTOM_HDRS_H_INCLUDED
#define UACUSTOM_HDRS_H_INCLUDED

#include "rsua-re/re.h"

typedef int (custom_hdrs_h)(const struct pl *name, const struct pl *val,
	void *arg);     /* returns error code if any */

int custom_hdrs_add(struct list *hdrs, const char *name, const char *fmt, ...);
int custom_hdrs_apply(const struct list *hdrs, custom_hdrs_h *h, void *arg);


#ifndef UAMODAPI_USE		/* Internal API */

int custom_hdrs_print(struct re_printf *pf,
		       const struct list *custom_hdrs);

#endif /* ifndef UAMODAPI_USE */

#endif /* UACUSTOM_HDRS_H_INCLUDED */
