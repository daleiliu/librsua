/**
 * @file auframe.h
 * @brief Audio frame
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAAUFRAME_H_INCLUDED
#define UAAUFRAME_H_INCLUDED

#include "rsua-re/re.h"

/**
 * Defines a frame of audio samples
 */
struct auframe {
	int fmt;             /**< Sample format (enum aufmt)        */
	void *sampv;         /**< Audio samples (must be mem_ref'd) */
	size_t sampc;        /**< Total number of audio samples     */
	uint64_t timestamp;  /**< Timestamp in AUDIO_TIMEBASE units */
};

void   auframe_init(struct auframe *af, int fmt, void *sampv, size_t sampc);
size_t auframe_size(const struct auframe *af);
void   auframe_mute(struct auframe *af);

#endif /* UAAUFRAME_H_INCLUDED */
