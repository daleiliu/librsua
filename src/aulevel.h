/**
 * @file aulevel.h
 * @brief Audio level
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAAULEVEL_H_INCLUDED
#define UAAULEVEL_H_INCLUDED

#include "rsua-re/re.h"

#define AULEVEL_MIN  (-96.0)
#define AULEVEL_MAX    (0.0)


double aulevel_calc_dbov(int fmt, const void *sampv, size_t sampc);

#endif /* UAAULEVEL_H_INCLUDED */
