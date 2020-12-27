/**
 * @file aucodec.h
 * @brief Audio codec
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAAUCODEC_H_INCLUDED
#define UAAUCODEC_H_INCLUDED

#include "rsua-re/re.h"

struct auenc_param {
	uint32_t ptime;    /**< Packet time in [ms]       */
	uint32_t bitrate;  /**< Wanted bitrate in [bit/s] */
};

struct auenc_state;
struct audec_state;
struct aucodec;

typedef int (auenc_update_h)(struct auenc_state **aesp,
			     const struct aucodec *ac,
			     struct auenc_param *prm, const char *fmtp);
typedef int (auenc_encode_h)(struct auenc_state *aes,
			     bool *marker, uint8_t *buf, size_t *len,
			     int fmt, const void *sampv, size_t sampc);

typedef int (audec_update_h)(struct audec_state **adsp,
			     const struct aucodec *ac, const char *fmtp);
typedef int (audec_decode_h)(struct audec_state *ads,
			     int fmt, void *sampv, size_t *sampc,
			     bool marker, const uint8_t *buf, size_t len);
typedef int (audec_plc_h)(struct audec_state *ads,
			  int fmt, void *sampv, size_t *sampc,
			  const uint8_t *buf, size_t len);

struct aucodec {
	struct le le;
	const char *pt;
	const char *name;
	uint32_t srate;             /* Audio samplerate */
	uint32_t crate;             /* RTP Clock rate   */
	uint8_t ch;
	uint8_t pch;                /* RTP packet channels */
	uint32_t ptime;             /* Packet time in [ms] (optional) */
	const char *fmtp;
	auenc_update_h *encupdh;
	auenc_encode_h *ench;
	audec_update_h *decupdh;
	audec_decode_h *dech;
	audec_plc_h    *plch;
	sdp_fmtp_enc_h *fmtp_ench;
	sdp_fmtp_cmp_h *fmtp_cmph;
};

void aucodec_register(struct list *aucodecl, struct aucodec *ac);
void aucodec_unregister(struct aucodec *ac);
const struct aucodec *aucodec_find(const struct list *aucodecl,
	const char *name, uint32_t srate, uint8_t ch);

#endif /* UAAUCODEC_H_INCLUDED */
