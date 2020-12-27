/**
 * @file mediadev.h
 * @brief Media Device
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAMEDIADEV_H_INCLUDED
#define UAMEDIADEV_H_INCLUDED

#include "rsua-re/re.h"

/** Defines a media device */
struct mediadev {
	struct le   le;
	char  *name;
};

int mediadev_add(struct list *dev_list, const char *name);
struct mediadev *mediadev_find(const struct list *dev_list, const char *name);
struct mediadev *mediadev_get_default(const struct list *dev_list);
int mediadev_print(struct re_printf *pf, const struct list *dev_list);

#endif /* UAMEDIADEV_H_INCLUDED */
