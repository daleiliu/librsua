/**
 * @file metric.h
 * @brief Metric
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAMETRIC_H_INCLUDED
#define UAMETRIC_H_INCLUDED

#include "rsua-re/re.h"


#ifndef UAMODAPI_USE		/* Internal API */

struct metric {
	/* internal stuff: */
	struct tmr tmr;
	struct lock *lock;
	uint64_t ts_start;
	bool started;

	/* counters: */
	uint32_t n_packets;
	uint32_t n_bytes;
	uint32_t n_err;

	/* bitrate calculation */
	uint32_t cur_bitrate;
	uint64_t ts_last;
	uint32_t n_bytes_last;
};

int      metric_init(struct metric *metric);
void     metric_reset(struct metric *metric);
void     metric_add_packet(struct metric *metric, size_t packetsize);
double   metric_avg_bitrate(const struct metric *metric);

#endif /* ifndef UAMODAPI_USE */

#endif /* UAMETRIC_H_INCLUDED */
