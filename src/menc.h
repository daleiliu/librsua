/**
 * @file menc.h
 * @brief Media encryption (for RTP)
 *
 * Copyright (C) 2010 Creytiv.com
 * Copyright (C) 2020 Dalei Liu
 */

#ifndef UAMENC_H_INCLUDED
#define UAMENC_H_INCLUDED

#include "rsua-re/re.h"

struct menc;
struct menc_sess;
struct menc_media;
struct stream;

/** Defines a media encryption event */
enum menc_event {
	MENC_EVENT_SECURE,          /**< Media is secured               */
	MENC_EVENT_VERIFY_REQUEST,  /**< Request user to verify a code  */
	MENC_EVENT_PEER_VERIFIED,   /**< Peer was verified successfully */
};


typedef void (menc_event_h)(enum menc_event event, const char *prm,
			    struct stream *strm, void *arg);

typedef void (menc_error_h)(int err, void *arg);

typedef int  (menc_sess_h)(struct menc_sess **sessp, struct sdp_session *sdp,
			   bool offerer, menc_event_h *eventh,
			   menc_error_h *errorh, void *arg);

typedef int  (menc_media_h)(struct menc_media **mp, struct menc_sess *sess,
			   struct rtp_sock *rtp,
			   struct udp_sock *rtpsock, struct udp_sock *rtcpsock,
			   const struct sa *raddr_rtp,
			   const struct sa *raddr_rtcp,
			   struct sdp_media *sdpm,
			   const struct stream *strm);

struct menc {
	struct le le;
	const char *id;
	const char *sdp_proto;
	bool wait_secure;
	menc_sess_h *sessh;
	menc_media_h *mediah;
};

void menc_register(struct list *mencl, struct menc *menc);
void menc_unregister(struct menc *menc);
const struct menc *menc_find(const struct list *mencl, const char *id);
const char *menc_event_name(enum menc_event event);

#endif /* UAMENC_H_INCLUDED */
