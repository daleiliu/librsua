/**
 * @file timestamp.h
 * @brief Timestamp helpers
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UATIMESTAMP_H_INCLUDED
#define UATIMESTAMP_H_INCLUDED

#include "rsua-re/re.h"


#ifndef UAMODAPI_USE		/* Internal API */

/**
 * This struct is used to keep track of timestamps for
 * incoming RTP packets.
 */
struct timestamp_recv {
	uint32_t first;
	uint32_t last;
	bool is_set;
	unsigned num_wraps;
};


int      timestamp_wrap(uint32_t ts_new, uint32_t ts_old);
void     timestamp_set(struct timestamp_recv *ts, uint32_t rtp_ts);
uint64_t timestamp_duration(const struct timestamp_recv *ts);
uint64_t timestamp_calc_extended(uint32_t num_wraps, uint32_t ts);
double   timestamp_calc_seconds(uint64_t ts, uint32_t clock_rate);

#endif /* ifndef UAMODAPI_USE */

#endif /* UATIMESTAMP_H_INCLUDED */
