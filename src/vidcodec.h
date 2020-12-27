/**
 * @file vidcodec.h
 * @brief Video Codec
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAVIDCODEC_H_INCLUDED
#define UAVIDCODEC_H_INCLUDED

#include "rsua-re/re.h"

/** Video Codec parameters */
struct videnc_param {
	unsigned bitrate;  /**< Encoder bitrate in [bit/s] */
	unsigned pktsize;  /**< RTP packetsize in [bytes]  */
	double fps;        /**< Video framerate (max)      */
	uint32_t max_fs;
};

struct videnc_state;
struct viddec_state;
struct vidcodec;
struct vidframe;

typedef int (videnc_packet_h)(bool marker, uint64_t rtp_ts,
			      const uint8_t *hdr, size_t hdr_len,
			      const uint8_t *pld, size_t pld_len,
			      void *arg);

typedef int (videnc_update_h)(struct videnc_state **vesp,
			      const struct vidcodec *vc,
			      struct videnc_param *prm, const char *fmtp,
			      videnc_packet_h *pkth, void *arg);

typedef int (videnc_encode_h)(struct videnc_state *ves, bool update,
			      const struct vidframe *frame,
			      uint64_t timestamp);

typedef int (viddec_update_h)(struct viddec_state **vdsp,
			      const struct vidcodec *vc, const char *fmtp);
typedef int (viddec_decode_h)(struct viddec_state *vds, struct vidframe *frame,
                              bool *intra, bool marker, uint16_t seq,
                              struct mbuf *mb);

struct vidcodec {
	struct le le;
	const char *pt;
	const char *name;
	const char *variant;
	const char *fmtp;
	videnc_update_h *encupdh;
	videnc_encode_h *ench;
	viddec_update_h *decupdh;
	viddec_decode_h *dech;
	sdp_fmtp_enc_h *fmtp_ench;
	sdp_fmtp_cmp_h *fmtp_cmph;
};

void vidcodec_register(struct list *vidcodecl, struct vidcodec *vc);
void vidcodec_unregister(struct vidcodec *vc);
const struct vidcodec *vidcodec_find(const struct list *vidcodecl,
				     const char *name, const char *variant);
const struct vidcodec *vidcodec_find_encoder(const struct list *vidcodecl,
					     const char *name);
const struct vidcodec *vidcodec_find_decoder(const struct list *vidcodecl,
					     const char *name);

#endif /* UAVIDCODEC_H_INCLUDED */
