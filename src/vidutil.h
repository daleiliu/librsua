/**
 * @file vidutil.h
 * @brief Video utilities
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAVIDUTIL_H_INCLUDED
#define UAVIDUTIL_H_INCLUDED

#include "rsua-re/re.h"

double video_calc_seconds(uint64_t rtp_ts);
double video_timestamp_to_seconds(uint64_t timestamp);
uint64_t video_calc_rtp_timestamp_fix(uint64_t timestamp);
uint64_t video_calc_timebase_timestamp(uint64_t rtp_ts);

#endif /* UAVIDUTIL_H_INCLUDED */
