/**
 * @file h264.h
 * @brief H.264
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAH264_H_INCLUDED
#define UAH264_H_INCLUDED

#include "rsua-re/re.h"
#include "rsua-rem/rem.h"
#include "rsua-mod/vidcodec.h"

/** Fragmentation Unit header */
struct h264_fu {
	unsigned s:1;      /**< Start bit                               */
	unsigned e:1;      /**< End bit                                 */
	unsigned r:1;      /**< The Reserved bit MUST be equal to 0     */
	unsigned type:5;   /**< The NAL unit payload type               */
};

int h264_fu_hdr_encode(const struct h264_fu *fu, struct mbuf *mb);
int h264_fu_hdr_decode(struct h264_fu *fu, struct mbuf *mb);

const uint8_t *h264_find_startcode(const uint8_t *p, const uint8_t *end);

int h264_packetize(uint64_t rtp_ts, const uint8_t *buf, size_t len,
		   size_t pktsize, videnc_packet_h *pkth, void *arg);
int h264_nal_send(bool first, bool last,
		  bool marker, uint32_t ihdr, uint64_t rtp_ts,
		  const uint8_t *buf, size_t size, size_t maxsz,
		  videnc_packet_h *pkth, void *arg);
bool h264_is_keyframe(int type);

#endif /* UAH264_H_INCLUDED */
